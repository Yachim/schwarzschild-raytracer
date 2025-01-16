#version 330 core
// right-handed y-up coordinates
// camera has left-handed y-up coordinates
// matrix columns represent the x, y, z axes respectively (right, up, forward)
// TODO: raytrace towards light?
// FIXME: continue raytracing in flat space if hit transparent object

precision highp float;
precision highp sampler2D;

#define PI 3.1415926535

in vec2 uv;
out vec4 FragColor;

uniform vec2 resolution;
uniform float time;
uniform sampler2D background_texture;

uniform int max_steps = 100;
uniform int max_revolutions = 2;

uniform float u_f = 0.01;

uniform bool crosshair = false;
const float crosshair_length = 10.;
const float crosshair_width = 2.;
const float crosshair_space = 5.;
const vec4 crosshair_color = vec4(0.5, 0.5, 0.5, 0.5);

const float epsilon = 0.0000001;

const int RAYTRACE_TYPE_CURVED      = 0;
const int RAYTRACE_TYPE_FLAT        = 1;
const int RAYTRACE_TYPE_HALF_WIDTH  = 2;
const int RAYTRACE_TYPE_HALF_HEIGHT = 3;
uniform int raytrace_type = RAYTRACE_TYPE_CURVED;
uniform float curved_percentage = 0.5;

uniform float percent_black = .75;

struct Transform {
    vec3 pos;
    mat3 axes;
};

struct Camera {
    Transform transform;
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

#define MAX_TEXTURES 10
uniform vec2 texture_sizes[MAX_TEXTURES];
uniform vec2 max_texture_size;
uniform sampler2DArray textures;

struct Material {
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float texture_opacity;
    int texture_index; // < 0 to disable
    int normal_map_index; // < 0 to disable
    // first swapped, then inverted
    bool invert_uv_x;
    bool invert_uv_y;
    bool swap_uvs;
    bool double_sided_normals;
    bool flip_normals;
};

#define MAX_MATERIALS 10
uniform Material materials[MAX_MATERIALS];

struct Sphere {
    Transform transform; // pos - center
    float radius;
};

#define MAX_SPHERES 3
uniform Sphere spheres[MAX_SPHERES];

const mat3 DEFAULT_AXES = mat3(
    vec3(1., 0., 0.),
    vec3(0., 1., 0.),
    vec3(0., 0., 1.)
);
const Sphere BLACK_HOLE = Sphere(Transform(vec3(0., 0., 0.), DEFAULT_AXES), 1.0);

struct Plane {
    Transform transform; // pos - some point
    vec2 texture_offset;
    bool repeat_texture;
    vec2 texture_size;
};

#define MAX_PLANES 3
uniform Plane planes[MAX_PLANES];

struct Disk {
    Plane plane; // pos - center
    float radius;
};

#define MAX_DISKS 3
uniform Disk disks[MAX_DISKS];

struct HollowDisk {
    Plane plane; // pos - center
    float inner_radius;
    float outer_radius;
};

#define MAX_HOLLOW_DISKS 3
uniform HollowDisk hollow_disks[MAX_HOLLOW_DISKS];

// only lateral cylinders, bases have to be provided manually as disks
struct Cylinder {
    Transform transform; // pos - base center
    float height;
    float radius;
};

#define MAX_CYLINDERS 3
uniform Cylinder cylinders[MAX_CYLINDERS];

struct Rectangle {
    Plane plane; // pos - top left corner
    float width;
    float height;
};

#define MAX_RECTANGLES 3
uniform Rectangle rectangles[MAX_RECTANGLES];

struct Box {
    Transform transform; // pos - left bottom back corner
    float width;
    float depth;
    float height;
};

#define MAX_BOXES 3
uniform Box boxes[MAX_BOXES];

const int OBJECT_TYPE_TEST_RAY_CURVED = -99;
const int OBJECT_TYPE_TEST_RAY_FLAT   = -98;
const int OBJECT_TYPE_SPECIAL         = -42;
const int OBJECT_TYPE_SPHERE          = 0;
const int OBJECT_TYPE_PLANE           = 1;
const int OBJECT_TYPE_DISK            = 2;
const int OBJECT_TYPE_HOLLOW_DISK     = 3;
const int OBJECT_TYPE_CYLINDER        = 4;
const int OBJECT_TYPE_RECTANGLE       = 5;
const int OBJECT_TYPE_BOX             = 6;
struct Object {
    int type;
    int index; // indexing respective arrays
    int material_index;
};

#define MAX_OBJECTS MAX_SPHERES + MAX_PLANES + MAX_DISKS + MAX_HOLLOW_DISKS + MAX_CYLINDERS + MAX_RECTANGLES + MAX_BOXES
uniform int num_objects;
uniform Object objects[MAX_OBJECTS];

#define MAX_POINTS 1000
uniform int num_test_ray_curved_points;
uniform vec3[MAX_POINTS] test_ray_curved_points;
uniform vec3 test_ray_flat_origin;
uniform vec3 test_ray_flat_dir;
uniform bool test_ray_visible = false;

uniform float test_ray_radius = 0.025;
uniform float test_ray_extended_length = 1000.;
uniform vec4 test_ray_curved_color = vec4(1., 0., 0., 1.);
uniform vec4 test_ray_flat_color = vec4(0., 1., 0., 1.);

struct Ray {
    vec3 origin;
    vec3 dir;
};

struct HitInfo {
  bool is_hit;
  float dist;
  vec3 intersection_point;
  mat3 tangent_space; // [tangent, bitangent, normal]
  vec2 tangent_coordinates; // [tangent direction, bitangent direction]; normalized except for plane
  Object object;
};

// #region tangent_space
void sphere_tangent_space(inout HitInfo hit_info, Sphere sphere) {
    Transform transform = sphere.transform;
    vec3 displacement = hit_info.intersection_point - transform.pos;
    vec3 normal = normalize(displacement);

    vec3 local_displacement = transpose(transform.axes) * displacement;
    float phi = atan(local_displacement.x, local_displacement.z);
    if (phi < 0.) phi += 2.* PI;
    float theta = asin(local_displacement.y / sphere.radius);

    hit_info.tangent_coordinates = vec2(phi / (2. * PI), theta / PI + 0.5);

    vec3 tangent = vec3(cos(phi), 0.0, -sin(phi));
    vec3 bitangent = vec3(sin(phi) * cos(theta), sin(theta), cos(phi) * cos(theta));

    tangent = transform.axes * tangent;
    bitangent = transform.axes * bitangent;

    hit_info.tangent_space = mat3(
        tangent,
        bitangent,
        normal
    );
}

void plane_tangent_space(inout HitInfo hit_info, Plane plane) {
    Transform transform = plane.transform;
    vec3 displacement = hit_info.intersection_point - transform.pos;

    vec3 local_displacement = transpose(transform.axes) * displacement;
    hit_info.tangent_coordinates = local_displacement.xz;
    hit_info.tangent_coordinates.y = 1. - hit_info.tangent_coordinates.y;

    hit_info.tangent_space = mat3(
        transform.axes[0],
        -transform.axes[2],
        transform.axes[1]
    );
}

void disk_tangent_space(inout HitInfo hit_info, Disk disk) {
    Transform transform = disk.plane.transform;
    vec3 displacement = hit_info.intersection_point - transform.pos;

    vec3 local_displacement = transpose(transform.axes) * displacement;
    float phi = atan(local_displacement.x, local_displacement.z);
    if (phi < 0.) phi += 2.* PI;

    hit_info.tangent_coordinates = vec2(
        length(local_displacement) / disk.radius,
        phi / (2. * PI)
    );

    vec3 tangent = normalize(displacement);
    vec3 bitangent = vec3(cos(phi), 0., -sin(phi));
    bitangent = transform.axes * bitangent;

    hit_info.tangent_space = mat3(
        tangent,
        bitangent,
        transform.axes[1]
    );
}

void hollow_disk_tangent_space(inout HitInfo hit_info, HollowDisk disk) {
    Transform transform = disk.plane.transform;
    vec3 displacement = hit_info.intersection_point - transform.pos;

    vec3 local_displacement = transpose(transform.axes) * displacement;
    float phi = atan(local_displacement.x, local_displacement.z);
    if (phi < 0.) phi += 2.* PI;

    hit_info.tangent_coordinates = vec2(
        (length(local_displacement) - disk.inner_radius) / (disk.outer_radius - disk.inner_radius),
        phi / (2. * PI)
    );

    vec3 tangent = normalize(displacement);
    vec3 bitangent = vec3(cos(phi), 0., -sin(phi));
    bitangent = transform.axes * bitangent;

    hit_info.tangent_space = mat3(
        tangent,
        bitangent,
        transform.axes[1]
    );
}

void cylinder_tangent_space(inout HitInfo hit_info, Cylinder cylinder) {
    Transform transform = cylinder.transform;
    vec3 displacement = hit_info.intersection_point - transform.pos;
    vec3 normal = normalize(displacement);
    vec3 bitangent = transform.axes[1];

    vec3 local_displacement = transpose(transform.axes) * displacement;
    float phi = atan(local_displacement.x, local_displacement.z);
    if (phi < 0.) phi += 2.* PI;

    hit_info.tangent_coordinates = vec2(
        phi / (2. * PI),
        local_displacement.y / cylinder.height + 0.5
    );

    vec3 tangent = cylinder.transform.axes * vec3(cos(phi), 0.0, -sin(phi));
    hit_info.tangent_space = mat3(
        tangent,
        bitangent,
        normal
    );
}

void rectangle_tangent_space(inout HitInfo hit_info, Rectangle rectangle) {
    Transform transform = rectangle.plane.transform;
    vec3 displacement = hit_info.intersection_point - transform.pos;

    vec3 local_displacement = transpose(transform.axes) * displacement;
    hit_info.tangent_coordinates = local_displacement.xz / vec2(rectangle.width, rectangle.height) + 0.5;
    hit_info.tangent_coordinates.y = 1. - hit_info.tangent_coordinates.y;

    hit_info.tangent_space = mat3(
        transform.axes[0],
        -transform.axes[2],
        transform.axes[1]
    );
}
// #endregion

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

float square_vector(vec2 v) {
    return square_vector(vec3(v, 0.));
}

vec4 calculate_lighting(HitInfo hit_info, vec3 view_dir) {
    if (hit_info.object.type == OBJECT_TYPE_SPECIAL) return vec4(0., 0., 0., 1.);
    if (hit_info.object.type == OBJECT_TYPE_TEST_RAY_CURVED) return test_ray_curved_color;
    if (hit_info.object.type == OBJECT_TYPE_TEST_RAY_FLAT) return test_ray_flat_color;

    Material material = materials[hit_info.object.material_index];
    if (material.flip_normals) hit_info.tangent_space[2] *= -1.;
    if (!material.double_sided_normals && dot(hit_info.tangent_space[2], view_dir) < 0.) return vec4(0., 0., 0., 0.);
    vec2 object_uv = hit_info.tangent_coordinates;

    if (material.swap_uvs)
        object_uv = vec2(object_uv.y, object_uv.x);
    if (material.invert_uv_x)
        object_uv.x = (hit_info.object.type == OBJECT_TYPE_PLANE ? planes[hit_info.object.index].texture_size.x : 1.) - object_uv.x;
    if (material.invert_uv_y)
        object_uv.y = (hit_info.object.type == OBJECT_TYPE_PLANE ? planes[hit_info.object.index].texture_size.y : 1.) - object_uv.y;

    vec4 base_color = material.color;
    if (material.texture_index >= 0) {
        vec2 rescaled_uv = object_uv * texture_sizes[material.texture_index] / max_texture_size;

        bool render_texture = true;
        if (hit_info.object.type == OBJECT_TYPE_PLANE) {
            Plane plane = planes[hit_info.object.index];
            rescaled_uv -= plane.texture_offset;
            vec2 plane_uv = rescaled_uv / plane.texture_size;

            rescaled_uv.x = mod(rescaled_uv.x, plane.texture_size.x);
            rescaled_uv.y = mod(rescaled_uv.y, plane.texture_size.y);
            rescaled_uv = rescaled_uv / plane.texture_size;

            render_texture = plane.repeat_texture || (
                (plane_uv.x >= 0. && plane_uv.x <= 1.) &&
                (plane_uv.y >= 0. && plane_uv.y <= 1.)
            );
        }

        if (render_texture) {
            base_color = texture(textures, vec3(rescaled_uv, material.texture_index));
            base_color.a *= material.texture_opacity;
        }
    }
    vec3 final_color = material.ambient * base_color.rgb; // Ambient component

    vec3 normal = hit_info.tangent_space[2];
    if (material.normal_map_index >= 0) {
        vec2 rescaled_uv = object_uv * texture_sizes[material.normal_map_index] / max_texture_size;
        vec3 normal_map = texture(textures, vec3(rescaled_uv, material.normal_map_index)).rgb;
        normal = normalize(hit_info.tangent_space * normal_map);
    }

    for(int i = 0; i < num_lights; i++) {
        Light light = lights[i];
        vec3 light_dir = normalize(light.transform.pos - hit_info.intersection_point);
        float distance = length(light.transform.pos - hit_info.intersection_point);

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
    }
    else if(n1 > 0) {
        n = n1;
    }
    else if(n2 > 0) {
        n = n2;
    }

    return n;
}

// #region intersections
HitInfo sphere_intersect(Ray ray, Sphere sphere, float max_lambda) {
    HitInfo res;
    float D = pow(dot(ray.dir, ray.origin - sphere.transform.pos), 2) - square_vector(ray.origin - sphere.transform.pos) + sphere.radius * sphere.radius;
    if(D < 0) {
        res.is_hit = false;
        return res;
    }

    float sqrt_D = sqrt(D);
    float first_term = -dot(ray.dir, ray.origin - sphere.transform.pos);
    float lambda1 = first_term - sqrt_D;
    float lambda2 = first_term + sqrt_D;

    float lambda = min_positive(lambda1, lambda2);

    res.is_hit = lambda >= 0 && (max_lambda < 0. || lambda <= max_lambda);
    if (!res.is_hit) return res;
    res.intersection_point = ray.origin + lambda * ray.dir;
    res.dist = distance(res.intersection_point, ray.origin);
    sphere_tangent_space(res, sphere);
    return res;
}
HitInfo sphere_intersect(Ray ray, Sphere sphere) {
    return sphere_intersect(ray, sphere, -1.);
}

HitInfo plane_intersect(Ray ray, Plane plane, float max_lambda) {
    HitInfo res;

    vec3 normal = plane.transform.axes[1];
    float denom = dot(normal, ray.dir);
    if(abs(denom) < epsilon) {
        res.is_hit = false;
        return res;
    }

    float lambda = dot(normal, plane.transform.pos - ray.origin) / denom;
    res.is_hit = lambda >= 0. && (max_lambda < 0. || lambda <= max_lambda);
    if (!res.is_hit) return res;
    res.intersection_point = ray.origin + ray.dir * lambda;
    res.dist = distance(res.intersection_point, ray.origin);
    plane_tangent_space(res, plane);
    return res;
}

HitInfo disk_intersect(Ray ray, Disk disk, float max_lambda) {
    HitInfo res = plane_intersect(ray, disk.plane, max_lambda);
    res.is_hit = res.is_hit && square_vector(res.intersection_point - disk.plane.transform.pos) <= disk.radius * disk.radius;
    if (!res.is_hit) return res;
    disk_tangent_space(res, disk);
    return res;
}

HitInfo hollow_disk_intersect(Ray ray, HollowDisk disk, float max_lambda) {
    HitInfo res = plane_intersect(ray, disk.plane, max_lambda);
    float squared_dist = square_vector(res.intersection_point - disk.plane.transform.pos);
    res.is_hit = res.is_hit && squared_dist >= disk.inner_radius * disk.inner_radius && squared_dist <= disk.outer_radius * disk.outer_radius;
    if (!res.is_hit) return res;
    hollow_disk_tangent_space(res, disk);
    return res;
}

HitInfo cylinder_intersect(Ray ray, Cylinder cylinder, float max_lambda) {
    vec3 pos = cylinder.transform.pos;
    mat3 axes = cylinder.transform.axes;
    mat3 axesT = transpose(axes);
    vec3 axis = axes[1];
    float height = cylinder.height;
    float radius = cylinder.radius;

    vec3 local_origin = axesT * (ray.origin - pos);
    vec3 local_dir = axesT * ray.dir;

    // parallel to the base
    float origin_parallel_sq = square_vector(local_origin.xz);
    float dir_parallel_sq = square_vector(local_dir.xz);
    float a = local_origin.x * local_dir.x + local_origin.z * local_dir.z;

    float D = a*a + dir_parallel_sq * (radius * radius - origin_parallel_sq);
    HitInfo res;
    if (D < 0.) {
        res.is_hit = false;
        return res;
    }

    float lambda1 = - (a + sqrt(D)) / dir_parallel_sq;
    float lambda2 = - (a - sqrt(D)) / dir_parallel_sq;
    vec3 intersection_point1 = ray.origin + ray.dir * lambda1;
    vec3 intersection_point2 = ray.origin + ray.dir * lambda2;
    bool inCylinder1 = abs(dot(intersection_point1 - pos, axis)) <= height / 2.;
    bool inCylinder2 = abs(dot(intersection_point2 - pos, axis)) <= height / 2.;

    if (!inCylinder1 && !inCylinder2) {
        res.is_hit = false;
        return res;
    }
    float lambda = -1.;
    if (inCylinder1 && inCylinder2)
        lambda = min_positive(lambda1, lambda2);
    else if (inCylinder1)
        lambda = lambda1;
    else if (inCylinder2)
        lambda = lambda2;

    res.intersection_point = ray.origin + ray.dir * lambda;
    res.is_hit = lambda >= 0. && (max_lambda < 0. || lambda <= max_lambda);
    if (!res.is_hit) return res;
    res.dist = distance(res.intersection_point, ray.origin);
    cylinder_tangent_space(res, cylinder);
    return res;
}

HitInfo rectangle_intersect(Ray ray, Rectangle rectangle, float max_lambda) {
    HitInfo res = plane_intersect(ray, rectangle.plane, max_lambda);
    if (!res.is_hit) return res;

    Transform transform = rectangle.plane.transform;
    float alpha = dot(res.intersection_point - transform.pos, transform.axes[0]);
    float beta = dot(res.intersection_point - transform.pos, transform.axes[2]);
    res.is_hit = abs(alpha) <= rectangle.width / 2. && abs(beta) <= rectangle.height / 2.;
    if (!res.is_hit) return res;
    rectangle_tangent_space(res, rectangle);
    return res;
}

HitInfo box_intersect(Ray ray, Box box, float max_lambda) {
    Rectangle bot_rect = Rectangle(
        Plane(Transform(
            box.transform.pos - box.transform.axes[1] * box.height / 2.,
            mat3(
                box.transform.axes[0],
                -box.transform.axes[1],
                -box.transform.axes[2]
            )
        ), vec2(0.), false, vec2(0.)),
        box.width,
        box.depth
    );
    Rectangle top_rect = bot_rect;
    top_rect.plane.transform = Transform(
            box.transform.pos + box.transform.axes[1] * box.height / 2.,
        box.transform.axes
    );

    Rectangle back_rect = Rectangle(
        Plane(Transform(
            box.transform.pos - box.transform.axes[2] * box.depth / 2.,
            mat3(
                -box.transform.axes[0],
                -box.transform.axes[2],
                -box.transform.axes[1]
            )
        ), vec2(0.), false, vec2(0.)),
        box.width,
        box.height
    );
    Rectangle front_rect = back_rect;
    front_rect.plane.transform = Transform(
            box.transform.pos + box.transform.axes[2] * box.depth / 2.,
        mat3(
            box.transform.axes[0],
            box.transform.axes[2],
            -box.transform.axes[1]
        )
    );

    Rectangle left_rect = Rectangle(
        Plane(Transform(
            box.transform.pos - box.transform.axes[0] * box.width / 2.,
            mat3(
                box.transform.axes[2],
                -box.transform.axes[0],
                -box.transform.axes[1]
            )
        ), vec2(0.), false, vec2(0.)),
        box.depth,
        box.height
    );
    Rectangle right_rect = left_rect;
    right_rect.plane.transform = Transform(
            box.transform.pos + box.transform.axes[0] * box.width / 2.,
        mat3(
            -box.transform.axes[2],
            box.transform.axes[0],
            -box.transform.axes[1]
        )
    );

    Rectangle rects[6] = Rectangle[](bot_rect, top_rect, front_rect, back_rect, left_rect, right_rect);

    HitInfo res;
    res.is_hit = false;
    int closest_index = -1;
    for (int i = 0; i < 6; i++) {
        HitInfo hit_info = rectangle_intersect(ray, rects[i], max_lambda);
        if (!hit_info.is_hit) continue;

        if (closest_index < 0 || hit_info.dist < res.dist) {
            res = hit_info;
            closest_index = i;
        }
    }
    if (!res.is_hit) return res;

    rectangle_tangent_space(res, rects[closest_index]);
    
    switch (closest_index) {
        case 0: // bot
            res.tangent_coordinates.x += 1.;
            break;
        case 1: // top
            res.tangent_coordinates.x += 1.;
            res.tangent_coordinates.y += 2.;
            break;
        case 2: // front
            res.tangent_coordinates.x += 1.;
            res.tangent_coordinates.y += 1.;
            break;
        case 3: // back
            res.tangent_coordinates.x += 3.;
            res.tangent_coordinates.y += 1.;
            break;
        case 4: // left
            res.tangent_coordinates.y += 1.;
            break;
        case 5: // right
            res.tangent_coordinates.x += 2.;
            res.tangent_coordinates.y += 1.;
            break;
    }
    res.tangent_coordinates.x /= 4.;
    res.tangent_coordinates.y /= 3.;

    return res;
}

HitInfo intersect_object(Ray ray, Object object, float max_lambda) {
    int object_type = object.type;
    int object_index = object.index;

    HitInfo res;
    res.is_hit = false;
    switch(object_type) {
        case OBJECT_TYPE_SPHERE:
            Sphere sphere = spheres[object_index];
            res = sphere_intersect(ray, sphere, max_lambda);
            break;
        case OBJECT_TYPE_PLANE:
            Plane plane = planes[object_index];
            res = plane_intersect(ray, plane, max_lambda);
            break;
        case OBJECT_TYPE_DISK:
            Disk disk = disks[object_index];
            res = disk_intersect(ray, disk, max_lambda);
            break;
        case OBJECT_TYPE_HOLLOW_DISK:
            HollowDisk hollow_disk = hollow_disks[object_index];
            res = hollow_disk_intersect(ray, hollow_disk, max_lambda);
            break;
        case OBJECT_TYPE_CYLINDER:
            Cylinder cylinder = cylinders[object_index];
            res = cylinder_intersect(ray, cylinder, max_lambda);
            break;
        case OBJECT_TYPE_RECTANGLE:
            Rectangle rectangle = rectangles[object_index];
            res = rectangle_intersect(ray, rectangle, max_lambda);
            break;
        case OBJECT_TYPE_BOX:
            Box box = boxes[object_index];
            res = box_intersect(ray, box, max_lambda);
            break;
    }
    res.object = object;

    return res;
}

// project v onto target
vec3 project(vec3 v, vec3 target) {
    return dot(v, target) / square_vector(target) * target;
}

// keeps the direction of y constant
mat3 gram_schmidt(mat3 m) {
    m[0] = m[0] - project(m[0], m[1]);
    m[2] = m[2] - project(m[2], m[1]) - project(m[2], m[0]);

    m[0] = normalize(m[0]);
    m[1] = normalize(m[1]);
    m[2] = normalize(m[2]);

    return m;
}

vec4 intersect(Ray ray, float max_lambda) {
    // black hole check
    HitInfo closest_hit = sphere_intersect(ray, BLACK_HOLE, max_lambda);
    closest_hit.object.type = OBJECT_TYPE_SPECIAL;

    if (test_ray_visible) {
        HitInfo hit = cylinder_intersect(ray, Cylinder(
            Transform(
                test_ray_flat_origin,
                gram_schmidt(mat3(test_ray_flat_dir.xzy, test_ray_flat_dir, test_ray_flat_dir.zxy))
            ),
            test_ray_extended_length,
            test_ray_radius
        ), max_lambda);

        hit.object.type = OBJECT_TYPE_TEST_RAY_FLAT;

        if(hit.is_hit && (!closest_hit.is_hit || hit.dist < closest_hit.dist)) {
            closest_hit = hit;
        }

        // test ray curved
        for (int i = 0; i < num_test_ray_curved_points - 1; i++) {
            if (num_test_ray_curved_points < 2) break;

            vec3 diff = test_ray_curved_points[i + 1] - test_ray_curved_points[i];
            float test_ray_length = length(diff);
            if (i == num_test_ray_curved_points - 2 && length(test_ray_curved_points[num_test_ray_curved_points - 1]) < 1.) {
                test_ray_length = test_ray_extended_length;
            }

            HitInfo hit = cylinder_intersect(ray, Cylinder(
                Transform(
                    test_ray_curved_points[i],
                    gram_schmidt(mat3(diff.xzy, diff, diff.zxy))
                ),
                test_ray_length,
                test_ray_radius
            ), max_lambda);

            hit.object.type = OBJECT_TYPE_TEST_RAY_CURVED;

            if (!hit.is_hit) continue;

            if(!closest_hit.is_hit || hit.dist < closest_hit.dist) {
                closest_hit = hit;
            }
        }
    }

    for (int i = 0; i < num_objects; i++) {
        Object current_object = objects[i];

        HitInfo hit = intersect_object(ray, current_object, max_lambda);
        if (!hit.is_hit) continue;

        if(!closest_hit.is_hit || hit.dist < closest_hit.dist) {
            closest_hit = hit;
        }
    }

    vec4 color = vec4(0., 0., 0., 0.);
    if (closest_hit.is_hit) {
        color = calculate_lighting(closest_hit, -ray.dir);
    }

    return color;
}

vec4 intersect(Ray ray) {
    return intersect(ray, -1.);
}
// #endregion

vec4 get_bg(vec3 dir) {
    float u = atan(dir.z, dir.x) / PI;
    if(u < 0.) u += 2.;
    u *= 0.5;

    float v = asin(dir.y) / PI + 0.5;

    return texture(background_texture, vec2(u, v));
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    FragColor = vec4(0.);
    if (crosshair && ( // dividing by two because uv is in [-1, 1]
        (
            abs(uv.x * resolution.x / 2.) < crosshair_width / 2. && 
            abs(uv.y * resolution.y / 2.) > crosshair_space && 
            abs(uv.y * resolution.y / 2.) < crosshair_length + crosshair_space
        ) || (
            abs(uv.y * resolution.y / 2.) < crosshair_width / 2. && 
            abs(uv.x * resolution.x / 2.) > crosshair_space && 
            abs(uv.x * resolution.x / 2.) < crosshair_length + crosshair_space
        )
    )) {
        FragColor = crosshair_color;
    }

    float ray_forward = 1. / tan(cam.fov / 360. * PI);
    float max_angle = 2. * float(max_revolutions) * PI;

    vec2 uv_vec = vec2(uv.x, uv.y * resolution.y / resolution.x);
    Ray ray = Ray(cam.transform.pos, normalize(cam.transform.axes * vec3(uv_vec, ray_forward)));

    if (
        (raytrace_type == RAYTRACE_TYPE_HALF_WIDTH && abs(uv.x - (2. * curved_percentage - 1.)) * resolution.x < 1.) ||
        (raytrace_type == RAYTRACE_TYPE_HALF_HEIGHT && abs(uv.y - (2. * curved_percentage - 1.)) * resolution.y < 1.)
    ) {
        FragColor = vec4(1., 1., 1., 1.);
        return;
    }

    vec3 normal_vec = normalize(ray.origin);
    if (
        (
            raytrace_type == RAYTRACE_TYPE_FLAT ||
            (raytrace_type == RAYTRACE_TYPE_HALF_WIDTH && uv.x > 2. * curved_percentage - 1.) ||
            (raytrace_type == RAYTRACE_TYPE_HALF_HEIGHT && uv.y > 2. * curved_percentage - 1.)
        ) || // flat space
        abs(dot(ray.dir, normal_vec)) >= 1. - epsilon // radial trajectory
    ) {
        vec4 intersection_color = intersect(ray);
        FragColor += intersection_color;
        if (intersection_color.a != 1.) FragColor += get_bg(ray.dir);
        return;
    }
    else if (rand(uv_vec) <= percent_black) { // if black due to noise optimization
        return;
    }

    vec3 tangent_vec = normalize(cross(cross(normal_vec, ray.dir), normal_vec));

    vec3 prev_ray_pos;
    float u = 1. / length(ray.origin);
    float du = -u * dot(ray.dir, normal_vec) / dot(ray.dir, tangent_vec);

    float phi = 0.;
    for(int i = 0; i < max_steps; i++) {
        if(u < u_f) {
            // flat space approximation
            HitInfo u_f_hit = sphere_intersect(ray, Sphere(Transform(vec3(0., 0., 0.), DEFAULT_AXES), 1. / u_f));
            if (!u_f_hit.is_hit) {
                vec4 intersection_color = intersect(ray);
                FragColor += intersection_color;
                if(intersection_color.a != 1.) FragColor += get_bg(ray.dir);
                return;
            }

            normal_vec = normalize(u_f_hit.intersection_point);
            if(abs(dot(ray.dir, normal_vec)) >= 1. - epsilon) { // if radial trajectory
                vec4 intersection_color = intersect(ray);
                FragColor += intersection_color;
                if(intersection_color.a != 1.) FragColor += get_bg(ray.dir);
                return;
            }

            tangent_vec = normalize(cross(cross(normal_vec, ray.dir), normal_vec));
            u = 1. / length(u_f_hit.intersection_point);
            du = -u * dot(ray.dir, normal_vec) / dot(ray.dir, tangent_vec);
        }

        float step_size = (max_angle - phi) / float(max_steps - i);
        phi += step_size;

        vec2 rk4_result = rk4_step(u, du, step_size);
        u += rk4_result.x;
        du += rk4_result.y;

        if(u < 0.)
            break;

        prev_ray_pos = ray.origin;
        ray.origin = (cos(phi) * normal_vec + sin(phi) * tangent_vec) / u;
        vec3 delta_ray = ray.origin - prev_ray_pos;
        float ray_length = length(delta_ray);
        ray.dir = delta_ray / ray_length;

        vec4 intersection_color = intersect(Ray(prev_ray_pos, ray.dir), ray_length);
        FragColor += intersection_color;
        if(intersection_color.a == 1.) return;
    }

    FragColor += get_bg(ray.dir);
}
