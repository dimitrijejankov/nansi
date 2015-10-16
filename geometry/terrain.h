#include "../math/gl_matrix.h"

typedef struct {

    GLuint vertex_buffer, element_buffer, normal_buffer, texture_buffer, texture[3];
    GLuint vertex_shader, fragment_shader, program;

    struct {
        GLint position;
        GLint normal;
        GLint texture_coords;
    } attributes;

    struct {
        GLint normal_matrix;
        GLint mvp_matrix;
        GLint mv_matrix;
        GLint color;
        GLint texture[3];
    } uniform;

    GLuint vertex_count;
    GLuint element_count;

} terrain;

terrain terrain_create(const char* filename, float cell_space, float height_factor);
terrain terrain_generate(int size, float roughness , float cell_space);

void terrain_render(terrain t, mat4_t model , mat4_t view, mat4_t projection);