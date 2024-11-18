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

uniform int max_steps = 1000;
uniform int max_revolutions = 2;

uniform float u_f = 0.01;
uniform float parallel_treshold = 0.9999999; // minimum value of a dot product of two unit vectors a . b, when the vectors are considered parallel; perpendicular_treshold = 1 - parallel_treshold

// 0: false
// 1: full screen
// 2: right half flat
// 3: up half flat
uniform int flat_raytrace = 0;
uniform float flat_percentage = 0.5;

struct Transform {
    vec3 pos;
};

struct Camera {
    Transform transform;
    vec3 forward;
    vec3 right;
    vec3 up;
    float fov;
};

uniform Camera cam;

// Lighting parameters
struct Light {
    Transform transform;
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

struct Sphere {
    Transform transform; // pos - center
    Material material;
    float radius;
};

#define MAX_SPHERES 3
uniform int num_spheres;
uniform Sphere spheres[MAX_SPHERES];

const Material BLANK_MAT = Material(vec4(0.0, 0.0, 0.0, 1.0), 0.1, 0.0, 0.0, 32.0);
const Sphere BLACK_HOLE = Sphere(Transform(vec3(0., 0., 0.)), BLANK_MAT, 1.0);

struct Plane {
    Transform transform; // pos - some point
    Material material;
    vec3 normal; // normalized
};

#define MAX_PLANES 3
uniform int num_planes;
uniform Plane planes[MAX_PLANES];

struct Disk {
    Plane plane; // pos - center
    float radius;
};

#define MAX_DISKS 3
uniform int num_disks;
uniform Disk disks[MAX_DISKS];

struct HollowDisk {
    Plane plane; // pos - center
    float inner_radius;
    float outer_radius;
};

#define MAX_HOLLOW_DISKS 3
uniform int num_hollow_disks;
uniform HollowDisk hollow_disks[MAX_HOLLOW_DISKS];

// only lateral cylinders, bases have to be provided manually as disks
struct Cylinder {
    Transform transform; // pos - base center
    Material material;
    vec3 height;
    float radius;
};

#define MAX_CYLINDERS 3
uniform int num_cylinders;
uniform Cylinder cylinders[MAX_CYLINDERS];

// types:
// 0: sphere
// 1: plane
// 2: disk
// 3: hollow disk
// 4: cylinder
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
    if(u < 0.)
        u += 2.;
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

    return vec2(delta_phi / 6. * (k1 + 2. * k2 + 2. * k3 + k4), delta_phi / 6. * (l1 + 2. * l2 + 2. * l3 + l4));
}

float square_vector(vec3 v) {
    return dot(v, v);
}

vec4 calculate_lighting(vec3 point, vec3 normal, vec3 view_dir, Material material) {
    vec4 base_color = material.color;
    vec3 final_color = material.ambient * base_color.rgb; // Ambient component

    for(int i = 0; i < num_lights; i++) {
        Light light = lights[i];
        vec3 light_dir = normalize(light.transform.pos - point);
        float distance = length(light.transform.pos - point);

        // Attenuation
        float attenuation = 1.0 / (light.attenuation_constant +
            light.attenuation_linear * distance +
            light.attenuation_quadratic * distance * distance);

        // Diffuse
        float diff = max(dot(normal, light_dir), 0.0);
        vec3 diffuse = material.diffuse * diff * light.color * base_color.rgb;

        // Specular
        vec3 reflect_dir = reflect(-light_dir, normal);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
        vec3 specular = material.specular * spec * light.color;

        final_color += (diffuse + specular) * attenuation * light.intensity;
    }

    return vec4(final_color, base_color.a);
}

// return -1 if not neither positive
float min_positive(float n1, float n2) {
    float n = -1.;
    if(n1 > 0 && n2 > 0) {
        n = min(n1, n2);
    } else if(n1 > 0) {
        n = n1;
    } else if(n2 > 0) {
        n = n2;
    }

    return n;
}

// #region intersections
bool sphere_intersect(vec3 origin, vec3 dir, Sphere sphere, out vec3 intersection_point, float max_lambda) {
    float D = pow(dot(dir, origin - sphere.transform.pos), 2) - square_vector(origin - sphere.transform.pos) + sphere.radius * sphere.radius;
    if(D < 0) {
        return false;
    }

    float sqrt_D = sqrt(D);
    float first_term = -dot(dir, origin - sphere.transform.pos);
    float lambda1 = first_term - sqrt_D;
    float lambda2 = first_term + sqrt_D;

    float lambda = min_positive(lambda1, lambda2);

    intersection_point = origin + lambda * dir;
    return lambda >= 0 && (max_lambda < 0. || lambda <= max_lambda);
}
bool sphere_intersect(vec3 origin, vec3 dir, Sphere sphere, out vec3 intersection_point) {
    return sphere_intersect(origin, dir, sphere, intersection_point, -1.);
}

bool plane_intersect(vec3 origin, vec3 dir, Plane plane, out vec3 intersection_point, float max_lambda) {
    float denom = dot(plane.normal, dir);
    if(abs(denom) < 1. - parallel_treshold)
        return false;

    float lambda = dot(plane.normal, plane.transform.pos - origin) / denom;
    intersection_point = origin + dir * lambda;
    return lambda >= 0. && (max_lambda < 0. || lambda <= max_lambda);
}

bool disk_intersect(vec3 origin, vec3 dir, Disk disk, out vec3 intersection_point, float max_lambda) {
    bool hit = plane_intersect(origin, dir, disk.plane, intersection_point, max_lambda);
    return hit && square_vector(intersection_point - disk.plane.transform.pos) <= disk.radius * disk.radius;
}

bool hollow_disk_intersect(vec3 origin, vec3 dir, HollowDisk disk, out vec3 intersection_point, float max_lambda) {
    bool hit = plane_intersect(origin, dir, disk.plane, intersection_point, max_lambda);
    float squared_dist = square_vector(intersection_point - disk.plane.transform.pos);
    return hit && squared_dist >= disk.inner_radius * disk.inner_radius && squared_dist <= disk.outer_radius * disk.outer_radius;
}

bool isInRange(float n, float minimum, float maximum) {
    return n >= minimum && n <= maximum;
}

bool cylinder_intersect(vec3 origin, vec3 dir, Cylinder cylinder, out vec3 intersection_point, float max_lambda) {
    vec3 height = cylinder.height;
    float heightSquared = square_vector(height);

    vec3 dir_parallel = dot(dir, height) / heightSquared * height;
    vec3 dir_perp = dir - dir_parallel;

    float radiusSquared = cylinder.radius * cylinder.radius;

    vec3 l = cylinder.transform.pos - origin;
    float lambda_C = dot(l, dir_perp) / square_vector(dir_perp);
    vec3 P = origin + lambda_C * dir_perp;
    vec3 PC = P - cylinder.transform.pos;
    float dSquared = square_vector(PC - dot(PC, height) / heightSquared * height);
    if(dSquared > radiusSquared)
        return false;
    float lambda_0C = sqrt(radiusSquared - dSquared);

    float lambda1 = lambda_C - lambda_0C;
    float lambda2 = lambda_C + lambda_0C;
    vec3 intersection_point1 = origin + dir * lambda1;
    vec3 intersection_point2 = origin + dir * lambda2;
    bool inCylinder1 = isInRange(dot(intersection_point1 - cylinder.transform.pos, height) / heightSquared, 0., 1.);
    bool inCylinder2 = isInRange(dot(intersection_point2 - cylinder.transform.pos, height) / heightSquared, 0., 1.);

    if(!inCylinder1 && !inCylinder2)
        return false;
    float lambda;
    if(inCylinder1 && inCylinder2)
        lambda = min_positive(lambda1, lambda2);
    else if(inCylinder1)
        lambda = lambda1;
    else
        lambda = lambda2;

    intersection_point = origin + dir * lambda;
    return lambda >= 0. && (max_lambda < 0. || lambda <= max_lambda);
}

bool intersect(vec3 origin, vec3 dir, out vec4 color, float max_lambda) {
    float min_dist = -1.;
    bool hit = false;
    Material material;
    vec3 normal;

    // black hole check
    vec3 intersection_point = vec3(0., 0., 0.);
    if(sphere_intersect(origin, dir, BLACK_HOLE, intersection_point, max_lambda)) {
        material = BLACK_HOLE.material;
        normal = normalize(intersection_point - BLACK_HOLE.transform.pos);
        min_dist = distance(intersection_point, origin);
        hit = true;
    }

    for(int i = 0; i < num_objects; i++) {
        Object object = objects[i];
        int object_type = object.type;
        int object_index = object.index;

        bool current_hit = false;
        Material current_material;
        vec3 current_normal;
        switch(object_type) {
            case 0: // sphere
                Sphere sphere = spheres[object_index];
                current_hit = sphere_intersect(origin, dir, sphere, intersection_point, max_lambda);
                current_material = sphere.material;
                current_normal = normalize(intersection_point - sphere.transform.pos);
                break;
            case 1: // plane
                Plane plane = planes[object_index];
                current_hit = plane_intersect(origin, dir, plane, intersection_point, max_lambda);
                current_material = plane.material;
                current_normal = plane.normal;
                break;
            case 2: // disk
                Disk disk = disks[object_index];
                current_hit = disk_intersect(origin, dir, disk, intersection_point, max_lambda);
                current_material = disk.plane.material;
                current_normal = disk.plane.normal;
                break;
            case 3: // hollow disk
                HollowDisk hollow_disk = hollow_disks[object_index];
                current_hit = hollow_disk_intersect(origin, dir, hollow_disk, intersection_point, max_lambda);
                current_material = hollow_disk.plane.material;
                current_normal = hollow_disk.plane.normal;
                break;
            case 4: // cylinder
                Cylinder cylinder = cylinders[object_index];
                current_hit = cylinder_intersect(origin, dir, cylinder, intersection_point, max_lambda);
                current_material = cylinder.material;
                current_normal = normalize(intersection_point - dot(intersection_point, cylinder.height) / square_vector(cylinder.height) * cylinder.height);
                break;
        }

        if(current_hit) {
            float dist = distance(intersection_point, origin);
            if(!hit || dist < min_dist) {
                min_dist = dist;
                hit = true;
                material = current_material;
                normal = current_normal;
            }
        }
    }

    color = vec4(0., 0., 0., 0.);
    if(hit) {
        color = calculate_lighting(intersection_point, normal, -dir, material);
    }

    return color.a == 1.;
}

bool intersect(vec3 origin, vec3 dir, out vec4 color) {
    return intersect(origin, dir, color, -1.);
}
// #endregion

vec4 get_bg(vec3 dir) {
    vec2 tex_coords = sphere_map(dir);
    return texture(background_texture, tex_coords);
}

// renders only half with checkerboard pattern
uniform float checkerboard_detail;
void main() {
    if(checkerboard_detail > 0. && mod(floor(uv.x * checkerboard_detail) + floor(uv.y * checkerboard_detail * resolution.y / resolution.x), 2.0) != 0.0)
        return;

    float ray_forward = 1. / tan(cam.fov / 360. * PI);
    float max_angle = 2. * float(max_revolutions) * PI;

    vec3 ray = normalize(cam.right * uv.x + cam.up * uv.y * resolution.y / resolution.x + ray_forward * cam.forward);

    vec3 normal_vec = normalize(cam.transform.pos);
    bool hit_opaque;
    FragColor = vec4(0., 0., 0., 0.);
    if((flat_raytrace == 1 || (flat_raytrace == 2 && uv.x > 2. * flat_percentage + -1.) || (flat_raytrace == 3 && uv.y > 2. * flat_percentage + -1.)) ||
        abs(dot(ray, normal_vec)) >= parallel_treshold) { // if radial trajectory or flat space
        vec4 intersection_color;
        hit_opaque = intersect(cam.transform.pos, ray, intersection_color);
        FragColor += intersection_color;
        if(!hit_opaque)
            FragColor += get_bg(ray);
        return;
    }

    vec3 tangent_vec = normalize(cross(cross(normal_vec, ray), normal_vec));

    vec3 prev_ray_pos;
    vec3 ray_pos = cam.transform.pos;
    float u = 1. / length(cam.transform.pos);
    float du = -u * dot(ray, normal_vec) / dot(ray, tangent_vec);

    float phi = 0.;
    for(int i = 0; i < max_steps; i++) {
        if(u < u_f) {
            // flat space approximation
            vec3 u_f_intersection_point;
            if(!sphere_intersect(ray_pos, ray, Sphere(Transform(vec3(0., 0., 0.)), BLANK_MAT, 1. / u_f), u_f_intersection_point)) {
                vec4 intersection_color;
                hit_opaque = intersect(ray_pos, ray, intersection_color);
                FragColor += intersection_color;
                if(!hit_opaque)
                    FragColor += get_bg(ray);
                return;
            }

            normal_vec = normalize(u_f_intersection_point);
            if(abs(dot(ray, normal_vec)) >= parallel_treshold) { // if radial trajectory
                vec4 intersection_color;
                hit_opaque = intersect(cam.transform.pos, ray, intersection_color);
                FragColor += intersection_color;
                if(!hit_opaque)
                    FragColor += get_bg(ray);
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

        if(u < 0.)
            break;

        prev_ray_pos = ray_pos;
        ray_pos = (cos(phi) * normal_vec + sin(phi) * tangent_vec) / u;
        vec3 delta_ray = ray_pos - prev_ray_pos;
        float ray_length = length(delta_ray);
        ray = delta_ray / ray_length;

        vec4 intersection_color;
        hit_opaque = intersect(prev_ray_pos, ray, intersection_color, ray_length);
        FragColor += intersection_color;
        if(hit_opaque)
            return;
    }

    FragColor += get_bg(ray);
}
