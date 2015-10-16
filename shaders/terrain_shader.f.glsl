uniform sampler2D texture;

uniform sampler2D grovel_texture;
uniform sampler2D stone_texture;
uniform sampler2D dirt_texture;

varying vec4 color;
varying vec2 texture_coords;
varying float height;

void main()
{
    const vec4 ambient_color = vec4(1.0, 1.0, 1.0, 1.0);
    const float ambient_intensity = 0.02;

    vec4 terrain_color;

    if(height < 120)
    {
        terrain_color = mix(texture2D(grovel_texture, texture_coords), texture2D(dirt_texture, texture_coords), height / 120.0);
    }
    else if(height < 300)
    {
        terrain_color = mix(texture2D(dirt_texture, texture_coords), texture2D(stone_texture, texture_coords), height / 300.0);
    }
    else
    {
        terrain_color = texture2D(stone_texture, texture_coords);
    }

    vec4 lighting_color = clamp(color + ambient_color * ambient_intensity, 0.0, 1.0);

    gl_FragColor = mix(terrain_color, lighting_color, 0.3);
}