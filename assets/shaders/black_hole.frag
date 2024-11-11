#version 330 core
// right-handed y-up coordinates
// camera has left-handed y-up coordinates
// TODO: raytrace towards light?

precision highp float;
precision highp sampler2D;

#define PI 3.1415926535

in vec2 uv;
out vec4 FragColor;

uniform vec2 resolution;
uniform float time;
uniform sampler2D background_texture;

uniform vec3 cam_pos;
uniform vec3 cam_forward;
uniform vec3 cam_right;
uniform vec3 cam_up;
uniform float cam_fov = 90.; // degrees, horizontal fov

uniform int max_steps = 1000;
uniform int max_revolutions = 2;

uniform float u_f = 0.01;
uniform float parallel_treshold = 0.999; // minimum value of a dot product of two unit vectors a . b, when the vectors are considered parallel; perpendicular_treshold = 1 - parallel_treshold

// Lighting parameters
struct Light {
    vec3 pos;
    vec3 color;
    float intensity;
    float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic;
};

#define MAX_LIGHTS 4
uniform int num_lights;
uniform Light lights[MAX_LIGHTS];

struct Material {
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct BaseObject {
    vec3 pos;
    bool opaque;
    Material material;
};

struct Sphere {
    BaseObject base; // pos - center
    float radius;
};

#define MAX_SPHERES 3
uniform int num_spheres;
uniform Sphere spheres[MAX_SPHERES];

const Material BLANK_MAT = Material(
    vec4(0.0, 0.0, 0.0, 1.0),   // color
    0.1,                        // ambient
    0.0,                        // diffuse (black hole absorbs all light)
    0.0,                        // specular
    32.0                        // shininess
);
const BaseObject BLACK_HOLE_BASE = BaseObject(
    vec3(0.0, 0.0, 0.0),  // center
    true,
    BLANK_MAT
);
const Sphere BLACK_HOLE = Sphere(
    BLACK_HOLE_BASE,
    1.0
);

struct Plane {
    BaseObject base; // pos - some point
    vec3 normal; // normalized
};

#define MAX_PLANES 3
uniform int num_planes;
uniform Plane planes[MAX_PLANES];

struct Disk {
    Plane plane; // base.pos - center
    float radius;
};

#define MAX_DISKS 3
uniform int num_disks;
uniform Disk disks[MAX_DISKS];

struct HollowDisk {
    Plane plane; // base.pos - center
    float inner_radius;
    float outer_radius;
};

#define MAX_HOLLOW_DISKS 3
uniform int num_hollow_disks;
uniform HollowDisk hollow_disks[MAX_HOLLOW_DISKS];

// types:
// 0: sphere
// 1: plane
// 2: disk
// 3: hollow disk
struct Object {
    int type;
    int index; // indexing respective arrays
};

#define MAX_OBJECTS MAX_SPHERES + MAX_PLANES + MAX_DISKS + MAX_HOLLOW_DISKS
uniform int num_objects;
uniform Object objects[MAX_OBJECTS];

// assuming normalized p
// returns (u, v), where u, v in [0, 1]
vec2 sphere_map(vec3 p) {
    float u = atan(p.z, p.x) / PI;
    if (u < 0.) u += 2.;
    return vec2(u * 0.5, asin(p.y) / PI + 0.5);
}

float ddu(float u) {
    return -u * (1. - 1.5 * u);
}

// returns (Δu_{i+1}, Δu'_{i+1})
vec2 rk4_step(float u_i, float du_i, float delta_phi) {
    float k1 = du_i;
    float l1 = ddu(u_i);

    float k2 = du_i + 0.5 * l1 * delta_phi;
    float l2 = ddu(u_i + 0.5 * k1 * delta_phi);

    float k3 = du_i + 0.5 * l2 * delta_phi;
    float l3 = ddu(u_i + 0.5 * k2 * delta_phi);

    float k4 = du_i + l3 * delta_phi;
    float l4 = ddu(u_i + k3 * delta_phi);

    return vec2(
        delta_phi / 6. * (k1 + 2. * k2 + 2. * k3 + k4),
        delta_phi / 6. * (l1 + 2. * l2 + 2. * l3 + l4)
    );
}

float square_vector(vec3 v) {
    return dot(v, v);
}

vec3 calculate_lighting(vec3 point, vec3 normal, vec3 view_dir, Material material) {
    vec3 base_color = material.color.rgb;
    vec3 final_color = material.ambient * base_color; // Ambient component
    
    for (int i = 0; i < num_lights; i++) {
        Light light = lights[i];
        vec3 light_dir = normalize(light.pos - point);
        float distance = length(light.pos - point);
        
        // Attenuation
        float attenuation = 1.0 / (
            light.attenuation_constant + 
            light.attenuation_linear * distance + 
            light.attenuation_quadratic * distance * distance
        );
        
        // Diffuse
        float diff = max(dot(normal, light_dir), 0.0);
        vec3 diffuse = material.diffuse * diff * light.color * base_color;
        
        // Specular
        vec3 reflect_dir = reflect(-light_dir, normal);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
        vec3 specular = material.specular * spec * light.color;
        
        final_color += (diffuse + specular) * attenuation * light.intensity;
    }
    
    return final_color;
}

// #region intersections
bool sphere_intersect(vec3 origin, vec3 dir, Sphere sphere, out vec3 intersection_point, float max_lambda) {
    float D = pow(dot(dir, origin - sphere.base.pos), 2) - square_vector(origin - sphere.base.pos) + sphere.radius * sphere.radius;
    if (D < 0) {
        return false;
    }

    float sqrt_D = sqrt(D);
    float first_term = -dot(dir, origin - sphere.base.pos);
    float lambda1 = first_term - sqrt_D;
    float lambda2 = first_term + sqrt_D;

    float lambda = -1.;
    if (lambda1 > 0 && lambda2 > 0) {
        lambda = min(lambda1, lambda2);
    }
    else if (lambda1 > 0) {
        lambda = lambda1;
    }
    else if (lambda2 > 0) {
        lambda = lambda2;
    }

    intersection_point = origin + lambda * dir;
    return lambda >= 0 && (max_lambda < 0. || lambda <= max_lambda);
}
bool sphere_intersect(vec3 origin, vec3 dir, Sphere sphere, out vec3 intersection_point) {
    return sphere_intersect(origin, dir, sphere, intersection_point, -1.);
}

bool plane_intersect(vec3 origin, vec3 dir, Plane plane, out vec3 intersection_point, float max_lambda) {
    float denom = dot(plane.normal, dir);
    if (abs(denom) < 1. - parallel_treshold) return false;

    float lambda = dot(plane.normal, plane.base.pos - origin) / denom;
    intersection_point = origin + dir * lambda;
    return lambda >= 0. && (max_lambda < 0. || lambda <= max_lambda);
}

bool disk_intersect(vec3 origin, vec3 dir, Disk disk, out vec3 intersection_point, float max_lambda) {
    bool hit = plane_intersect(origin, dir, disk.plane, intersection_point, max_lambda);
    return hit && square_vector(intersection_point - disk.plane.base.pos) <= disk.radius * disk.radius;
}

bool hollow_disk_intersect(vec3 origin, vec3 dir, HollowDisk disk, out vec3 intersection_point, float max_lambda) {
    bool hit = plane_intersect(origin, dir, disk.plane, intersection_point, max_lambda);
    float squared_dist = square_vector(intersection_point - disk.plane.base.pos);
    return hit && squared_dist >= disk.inner_radius * disk.inner_radius && squared_dist <= disk.outer_radius * disk.outer_radius;
}

bool intersect(vec3 origin, vec3 dir, out vec4 color, float max_lambda) {
    float min_dist = -1.;
    bool hit = false;
    Material material;
    vec3 normal;
    bool opaque = false;
    
    // black hole check
    vec3 intersection_point = vec3(0., 0., 0.);
    if (sphere_intersect(origin, dir, BLACK_HOLE, intersection_point, max_lambda)) {
        material = BLACK_HOLE.base.material;
        normal = normalize(intersection_point - BLACK_HOLE.base.pos);
        min_dist = distance(intersection_point, origin);
        hit = true;
        opaque = true;
    }
    
    for (int i = 0; i < num_objects; i++) {
        Object object = objects[i];
        int object_type = object.type;
        int object_index = object.index;

        bool current_hit = false;
        Material current_material;
        vec3 current_normal;
        bool current_opaque;
        switch (object_type) {
            case 0: // sphere
                Sphere sphere = spheres[object_index];
                current_hit = sphere_intersect(origin, dir, sphere, intersection_point, max_lambda);
                current_material = sphere.base.material;
                current_normal = normalize(intersection_point - sphere.base.pos);
                current_opaque = sphere.base.opaque;
                break;
            case 1: // plane
                Plane plane = planes[object_index];
                current_hit = plane_intersect(origin, dir, plane, intersection_point, max_lambda);
                current_material = plane.base.material;
                current_normal = plane.normal;
                current_opaque = plane.base.opaque;
                break;
            case 2: // disk
                Disk disk = disks[object_index];
                current_hit = disk_intersect(origin, dir, disk, intersection_point, max_lambda);
                current_material = disk.plane.base.material;
                current_normal = disk.plane.normal;
                current_opaque = disk.plane.base.opaque;
                break;
            case 3: // hollow disk
                HollowDisk hollow_disk = hollow_disks[object_index];
                current_hit = hollow_disk_intersect(origin, dir, hollow_disk, intersection_point, max_lambda);
                current_material = hollow_disk.plane.base.material;
                current_normal = hollow_disk.plane.normal;
                current_opaque = hollow_disk.plane.base.opaque;
                break;
        }

        if (current_hit) {
            float dist = distance(intersection_point, origin);
            if (!hit || dist < min_dist) {
                min_dist = dist;
                hit = true;
                material = current_material;
                normal = current_normal;
                opaque = current_opaque;
            }
        }
    }

    color = vec4(0., 0., 0., 0.);
    if (hit) {
        vec3 lit_color = calculate_lighting(
            intersection_point,
            normal,
            -dir,
            material
        );
        color = vec4(lit_color, material.color.a);
    }

    return opaque;
}

bool intersect(vec3 origin, vec3 dir, out vec4 color) {
    return intersect(origin, dir, color, -1.);
}

vec4 get_bg(vec3 dir) {
    vec2 tex_coords = sphere_map(dir);
    return texture(background_texture, tex_coords);
}

// renders only half with checkerboard pattern
uniform float checkerboard_detail;
void main() {
    if (checkerboard_detail > 0. && mod(floor(uv.x * checkerboard_detail) + floor(uv.y * checkerboard_detail * resolution.y / resolution.x), 2.0) != 0.0) return;

    float ray_forward = 1. / tan(cam_fov / 360. * PI);
    float max_angle = 2. * float(max_revolutions) * PI;

    vec3 ray = normalize(cam_right * uv.x + cam_up * uv.y * resolution.y / resolution.x + ray_forward * cam_forward);

    vec3 normal_vec = normalize(cam_pos);
    bool hit_opaque;
    FragColor = vec4(0., 0., 0., 0.);
    if (abs(dot(ray, normal_vec)) >= parallel_treshold) { // if radial trajectory
        vec4 intersection_color;
        hit_opaque = intersect(cam_pos, ray, intersection_color);
        FragColor += intersection_color;
        if (!hit_opaque) FragColor += get_bg(ray);
        return;
    }

    vec3 tangent_vec = normalize(cross(cross(normal_vec, ray), normal_vec));

    vec3 prev_ray_pos;
    vec3 ray_pos = cam_pos;
    float u = 1. / length(cam_pos);
    float du = -u * dot(ray, normal_vec) / dot(ray, tangent_vec);

    float phi = 0.;
    for (int i = 0; i < max_steps; i++) {
        if (u < u_f) {
            // flat space approximation
            vec3 u_f_intersection_point;
            if(!sphere_intersect(ray_pos, ray, Sphere(
                BaseObject(vec3(0., 0., 0.),
                true,
                BLANK_MAT
            ), 1./u_f), u_f_intersection_point)) {
                vec4 intersection_color;
                hit_opaque = intersect(ray_pos, ray, intersection_color);
                FragColor += intersection_color;
                if (!hit_opaque) FragColor += get_bg(ray);
                return;
            }

            normal_vec = normalize(u_f_intersection_point);
            if (abs(dot(ray, normal_vec)) >= parallel_treshold) { // if radial trajectory
                vec4 intersection_color;
                hit_opaque = intersect(cam_pos, ray, intersection_color);
                FragColor += intersection_color;
                if (!hit_opaque) FragColor += get_bg(ray);
                return;
            }

            tangent_vec = normalize(cross(cross(normal_vec, ray), normal_vec));
            u = 1. / length(u_f_intersection_point);
            du = -u * dot(ray, normal_vec) / dot(ray, tangent_vec);
        }

        float step_size = (max_angle - phi) / float(max_steps - i);
        phi += step_size;

        vec2 rk4_result = rk4_step(u, du, step_size);
        u += rk4_result.x;
        du += rk4_result.y;

        if (u < 0.) break;

        prev_ray_pos = ray_pos;
        ray_pos = (cos(phi) * normal_vec + sin(phi) * tangent_vec) / u;
        vec3 delta_ray = ray_pos - prev_ray_pos;
        float ray_length = length(delta_ray);
        ray = delta_ray / ray_length;

        vec4 intersection_color;
        hit_opaque = intersect(prev_ray_pos, ray, intersection_color, ray_length);
        FragColor += intersection_color;
        if (hit_opaque) return;
    }

    FragColor += get_bg(ray);
}
