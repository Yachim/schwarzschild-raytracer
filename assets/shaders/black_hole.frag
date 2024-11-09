#version 330 core
// right-handed y-up coordinates
// camera has left-handed y-up coordinates

precision highp float;
precision highp sampler2D;

#define PI 3.1415926535
#define MAX_SPHERES 3

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

uniform float u_f = 0.001;
uniform float radial_treshold = 0.999; // minimum value of v_0 . n when the trajectory is considered radial

struct Sphere {
    vec3 center;
    float radius;
    bool opaque;
    vec4 color;
    bool uv_colors;
};

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

// #region intersections
bool sphere_intersect(vec3 origin, vec3 dir, Sphere sphere, out vec3 intersection_point, float max_lambda) {
    float D = pow(dot(dir, origin - sphere.center), 2) - square_vector(origin - sphere.center) + sphere.radius * sphere.radius;
    if (D < 0) {
        return false;
    }

    float sqrt_D = sqrt(D);
    float first_term = -dot(dir, origin - sphere.center);
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
    return lambda >= 0 && (max_lambda < 0. || lambda < max_lambda);
}

bool intersect_closest_sphere(vec3 origin, vec3 dir, Sphere spheres[MAX_SPHERES], out vec3 closest_intersection_point, out Sphere closest_sphere, float max_lambda) {
    float min_dist = -1.;
    bool hit = false;
    for (int i = 0; i < MAX_SPHERES; i++) {
        Sphere sphere = spheres[i];
        vec3 intersection_point;
        bool current_hit = sphere_intersect(origin, dir, sphere, intersection_point, max_lambda);
        float dist = distance(intersection_point, origin);
        if (current_hit && (min_dist < 0. || dist < min_dist)) {
            closest_intersection_point = intersection_point;
            closest_sphere = sphere;
            min_dist = dist;
            hit = current_hit;
        }
    }

    return hit;
}

// returns true if opaque - return early
bool intersect(vec3 origin, vec3 dir, out vec4 color, Sphere spheres[MAX_SPHERES], float max_lambda) {
    color = vec4(0., 0., 0., 0.);
    bool opaque = false;

    vec3 intersection_point;
    Sphere sphere;
    if (intersect_closest_sphere(origin, dir, spheres, intersection_point, sphere, max_lambda)) {
        if (sphere.uv_colors) {
            vec2 tex_coords = sphere_map(normalize(intersection_point - sphere.center));
            color += vec4(tex_coords.x, tex_coords.y, 1., sphere.color.w);
        }
        else {
            color += sphere.color;
        }
        opaque = sphere.opaque;
    }

    return opaque;
}
bool intersect(vec3 origin, vec3 dir, out vec4 color, Sphere spheres[MAX_SPHERES]) {
    return intersect(origin, dir, color, spheres, -1.);
}

vec4 get_bg(vec3 dir) {
    vec2 tex_coords = sphere_map(dir);
    return texture(background_texture, tex_coords);
}
// #endregion

void main() {
    float ray_forward = 1. / tan(cam_fov / 360. * PI);
    float max_angle = 2. * float(max_revolutions) * PI;

    Sphere spheres[MAX_SPHERES] = Sphere[MAX_SPHERES](
        Sphere(vec3(0., 0., 0.), 1., true, vec4(0., 0., 0., 1.), false), // event horizon
        Sphere(vec3(0., 0., 10.), 1., true, vec4(0., 1., 0., 1.), false),
        Sphere(7.5 * normalize(vec3(1., 0., 1.)), 2., true, vec4(0., 0., 1., 1.), false)
    );

    vec3 ray = normalize(cam_right * uv.x + cam_up * uv.y * resolution.y / resolution.x + ray_forward * cam_forward);

    vec3 normal_vec = normalize(cam_pos);
    bool hit_opaque;
    FragColor = vec4(0., 0., 0., 0.);
    if (abs(dot(ray, normal_vec)) >= radial_treshold) {
        vec4 intersection_color;
        hit_opaque = intersect(cam_pos, ray, intersection_color, spheres);
        FragColor += intersection_color;
        if (!hit_opaque) FragColor += get_bg(ray);
        return;
    }

    vec3 tangent_vec = normalize(cross(cross(normal_vec, ray), normal_vec));

    vec3 prev_ray_pos;
    vec3 ray_pos = cam_pos;
    vec3 ray_dir;
    float u = 1. / length(cam_pos);
    float du = -u * dot(ray, normal_vec) / dot(ray, tangent_vec);

    float phi = 0.;
    for (int i = 0; i < max_steps; i++) {
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
        ray_dir = delta_ray / ray_length;

        vec4 intersection_color;
        hit_opaque = intersect(prev_ray_pos, ray_dir, intersection_color, spheres, ray_length);
        FragColor += intersection_color;
        if (hit_opaque) return;
    }

    FragColor += get_bg(ray_dir);
}