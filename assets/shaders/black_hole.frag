#version 330 core
// right-handed y-up coordinates
// camera has left-handed y-up coordinates

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

void main() {
    float ray_forward = 1. / tan(cam_fov / 360. * PI);
    float max_angle = 2. * float(max_revolutions) * PI;

    vec3 ray = normalize(cam_right * uv.x + cam_up * uv.y * resolution.y / resolution.x + ray_forward * cam_forward);

    vec3 normal_vec = normalize(cam_pos);
    vec3 tangent_vec = normalize(cross(cross(normal_vec, ray), normal_vec));

    vec3 prev_ray_pos = cam_pos;
    vec3 ray_pos;
    float u = 1. / length(cam_pos);
    float du = -u * dot(ray, normal_vec) / dot(ray, tangent_vec);

    float phi = 0.;
    for (int i = 0; i < max_steps; i++) {
        float step_size = (max_angle - phi) / float(max_steps - i);
        phi += step_size;
        vec2 rk4_result = rk4_step(u, du, step_size);

        u += rk4_result.x;
        du += rk4_result.y;

        if (u < 0.) {
            break;
        }

        prev_ray_pos = ray_pos;
        ray_pos = (cos(phi) * normal_vec + sin(phi) * tangent_vec) / u;

        if (u >= 1.) {
            FragColor = vec4(0., 0., 0., 1.);
            return;
        }
    }

    vec2 tex_coords = sphere_map(normalize(ray_pos - prev_ray_pos));
    FragColor = texture(background_texture, tex_coords);
}