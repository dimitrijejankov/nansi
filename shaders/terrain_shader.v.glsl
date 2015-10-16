attribute vec3 position;
attribute vec3 normal;
attribute vec2 tex_coords;

uniform mat4 mvp_matrix;
uniform mat4 normal_matrix;
uniform mat4 mv_matrix;

const vec3 diffuse_light_direction = vec3(0.0, 1.0 , 1.0);
const vec4 diffuse_color = vec4(1, 1, 1, 1);

float DiffuseIntensity = 0.6;

varying vec4 color;
varying vec2 texture_coords;
varying float height;

void main()
{
    texture_coords = tex_coords;
    height = position.y;

    gl_Position = mvp_matrix * vec4(position, 1.0);

    normal = vec3(normal_matrix * vec4(normal, 1.0));
    float lightIntensity = dot(normal, diffuse_light_direction);

    color = clamp(diffuse_color * DiffuseIntensity * lightIntensity, 0.0, 1.0);
}