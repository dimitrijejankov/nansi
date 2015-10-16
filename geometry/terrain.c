#include <malloc.h>
#include <GL/glew.h>
#include <time.h>
#include "terrain.h"
#include "../utilities/glutil.h"
#include "../utilities/util.h"


void diamond_square(int size, int chunk_size, float roughness, GLfloat *vertex_data) {

    chunk_size = chunk_size / 2;
    roughness /= 2;

    if (chunk_size < 1)
        return;

    //square
    for (int y = chunk_size; y < size; y += chunk_size * 2)
    {
        for (int x = chunk_size; x < size; x += chunk_size * 2)
        {
            float rnd = ((((float)(rand())) / RAND_MAX) - 0.5f) * 2 * roughness;
            float up_left = 0;
            float up_right = 0;
            float down_left = 0;
            float down_right = 0;
            int count = 0;

            if(y + chunk_size < size && x - chunk_size >= 0)
            { up_left = vertex_data[(((y + chunk_size) * size) + x - chunk_size)*3 + 1]; ++count;}

            if(y + chunk_size < size && x + chunk_size < size)
            { up_left = vertex_data[(((y + chunk_size) * size) + x + chunk_size)*3 + 1]; ++count;}

            if(y - chunk_size >= 0 && x + chunk_size < size)
            { down_right = vertex_data[(((y - chunk_size) * size) + x + chunk_size)*3 + 1]; ++count;}

            if(y - chunk_size >= 0 && x - chunk_size >= 0)
            { down_right = vertex_data[(((y - chunk_size) * size) + x - chunk_size)*3 + 1]; ++count;}

            float avg = 0;

            if (count != 0)
                avg = (up_left + up_right + down_left + down_right) / count;

            vertex_data[((y * size) + x)*3 + 1] = rnd + avg;
        }
    }

    // diamond
    for (int y = 0; y <= size; y += chunk_size)
    {
        for (int x = (y / chunk_size) % 2 ? 0 : chunk_size; x <= size; x += chunk_size * 2)
        {
            float rnd = ((((float)(rand())) / RAND_MAX) - 0.5f) * 2 * roughness;
            float up = 0;
            float down = 0;
            float left = 0;
            float right = 0;
            int count = 0;

            if(y + chunk_size < size)
            { up = vertex_data[(((y + chunk_size) * size) + x)*3 + 1]; ++count;}

            if(y - chunk_size >= 0)
            { down = vertex_data[(((y - chunk_size) * size) + x)*3 + 1]; ++count;}

            if(x - chunk_size >= 0)
            { left = vertex_data[((y * size) + x - chunk_size)*3 + 1]; ++count;}

            if(x + chunk_size < size)
            { right = vertex_data[((y * size) + x + chunk_size)*3 + 1]; ++count;}

            float avg = 0;

            if (count != 0)
                avg = (up + down + left + right) / count;
            vertex_data[((y * size) + x)*3 + 1] = rnd + avg;
        }
    }

    diamond_square(size, chunk_size, roughness, vertex_data);
}

terrain terrain_generate(int size, float roughness , float cell_space) {
    terrain t;

    int width = (2 << size) + 1;
    int height = (2 << size) + 1;

    GLfloat *vertex_data = calloc(sizeof(GLfloat) * 3, (size_t) (width * height));
    GLfloat *texture_data = calloc(sizeof(GLfloat) * 2, (size_t) (width * height));

    srand((unsigned int) time(NULL));
    diamond_square(width, width, roughness, vertex_data);


    float lowest = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            vertex_data[((y * width) + x) * 3] = x * cell_space;
            vertex_data[((y * width) + x) * 3 + 2] = y * cell_space;

            if(vertex_data[((y * width) + x) * 3 + 1] < lowest)
                lowest = vertex_data[((y * width) + x) * 3 + 1];

            texture_data[((y * width) + x) * 2] = (float) x / 60.0f;
            texture_data[((y * width) + x) * 2 + 1] = (float) y / 60.0f;
        }
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            vertex_data[((y * width) + x) * 3 + 1] += abs(lowest);
        }
    }


    GLuint * element_data = calloc((size_t) ((width-1) * (height-1) * 6), sizeof(GLuint));

    GLuint counter = 0;
    for(GLuint i = 0; i < width-1; ++i)
    {
        for(GLuint j = 0; j < height-1; ++j)
        {
            int lowerLeft = i + j*width;
            int lowerRight = (i + 1) + j*width;
            int topLeft = i + (j + 1) * width;
            int topRight = (i + 1) + (j + 1) * width;

            element_data[counter++] = (GLuint) topLeft;
            element_data[counter++] = (GLuint) lowerRight;
            element_data[counter++] = (GLuint) lowerLeft;

            element_data[counter++] = (GLuint) topLeft;
            element_data[counter++] = (GLuint) topRight;
            element_data[counter++] = (GLuint) lowerRight;
        }
    }

    GLfloat* normal_data = calloc((size_t) (width * height * 3), sizeof(GLfloat));

    float side1[3];
    float side2[3];
    float normal[3];

    for (int i = 0; i < counter / 3; i++)
    {
        int index1 = element_data[i * 3];
        int index2 = element_data[i * 3 + 1];
        int index3 = element_data[i * 3 + 2];

        vec3_subtract(&vertex_data[index1*3], &vertex_data[index3*3], side1);
        vec3_subtract(&vertex_data[index1*3], &vertex_data[index2*3], side2);

        vec3_cross(side2, side1, normal);

        vec3_add(&normal_data[index1*3], normal, &normal_data[index1*3]);
        vec3_add(&normal_data[index2*3], normal, &normal_data[index2*3]);
        vec3_add(&normal_data[index3*3], normal, &normal_data[index3*3]);
    }

    for (int i = 0; i < width * height; i++)
    {
        vec3_normalize(&normal_data[i*3], &normal_data[i*3]);
    }

    t.texture[0] = make_texture("gravel.tga");
    t.texture[1] = make_texture("stone.tga");
    t.texture[2] = make_texture("dirt.tga");

    t.element_count = (GLuint) ((width-1) * (height-1) * 6);
    t.vertex_count = (GLuint) (width * height);

    t.vertex_buffer = make_buffer(GL_ARRAY_BUFFER, vertex_data, (GLsizei) (t.vertex_count * 3 * sizeof(GLfloat)));
    t.texture_buffer = make_buffer(GL_ARRAY_BUFFER, texture_data, (GLsizei) (t.vertex_count * 2 * sizeof(GLfloat)));
    t.normal_buffer = make_buffer(GL_ARRAY_BUFFER, normal_data, (GLsizei) (t.vertex_count * 3 * sizeof(GLfloat)));
    t.element_buffer = make_buffer(GL_ELEMENT_ARRAY_BUFFER, element_data, (GLsizei) (t.element_count * sizeof(GLuint)));

    t.vertex_shader = make_shader(GL_VERTEX_SHADER, "shaders/terrain_shader.v.glsl");
    t.fragment_shader = make_shader(GL_FRAGMENT_SHADER, "shaders/terrain_shader.f.glsl");

    t.program = make_program(t.vertex_shader, t.fragment_shader);

    t.attributes.position = glGetAttribLocation(t.program, "position");
    t.attributes.normal = glGetAttribLocation(t.program, "normal");
    t.attributes.texture_coords = glGetAttribLocation(t.program, "tex_coords");

    t.uniform.mvp_matrix = glGetUniformLocation(t.program, "mvp_matrix");
    t.uniform.mv_matrix = glGetUniformLocation(t.program, "mv_matrix");
    t.uniform.normal_matrix = glGetUniformLocation(t.program, "normal_matrix");
    t.uniform.color = glGetUniformLocation(t.program, "color");

    t.uniform.texture[0] = glGetUniformLocation(t.program, "grovel_texture");
    t.uniform.texture[1] = glGetUniformLocation(t.program, "stone_texture");
    t.uniform.texture[2] = glGetUniformLocation(t.program, "dirt_texture");

    free(vertex_data);
    free(texture_data);
    free(element_data);
    free(normal_data);

    return t;
}

terrain terrain_create(const char* filename, float cell_space, float height_factor)
{
    terrain t;

    int width,height;

    unsigned char* pixels = read_tga(filename, &width, &height);

    GLfloat* vertex_data = calloc(sizeof(GLfloat) * 3, (size_t) (width * height));
    GLfloat* texture_data = calloc(sizeof(GLfloat) * 2, (size_t) (width * height));

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            vertex_data[((y * width) + x)*3] = x * cell_space;
            vertex_data[((y * width) + x)*3 + 1] = pixels[(y*width + x)*3]*height_factor;
            vertex_data[((y * width) + x)*3 + 2] = y*cell_space;

            texture_data[((y * width) + x)*2] = (float)x / 60.0f;
            texture_data[((y * width) + x)*2 + 1] = (float)y / 60.0f;
        }
    }

    GLuint * element_data = calloc((size_t) ((width-1) * (height-1) * 6), sizeof(GLuint));

    GLuint counter = 0;
    for(GLuint i = 0; i < width-1; ++i)
    {
        for(GLuint j = 0; j < height-1; ++j)
        {
            int lowerLeft = i + j*width;
            int lowerRight = (i + 1) + j*width;
            int topLeft = i + (j + 1) * width;
            int topRight = (i + 1) + (j + 1) * width;

            element_data[counter++] = (GLuint) topLeft;
            element_data[counter++] = (GLuint) lowerRight;
            element_data[counter++] = (GLuint) lowerLeft;

            element_data[counter++] = (GLuint) topLeft;
            element_data[counter++] = (GLuint) topRight;
            element_data[counter++] = (GLuint) lowerRight;
        }
    }

    GLfloat* normal_data = calloc((size_t) (width * height * 3), sizeof(GLfloat));

    float side1[3];
    float side2[3];
    float normal[3];

    for (int i = 0; i < counter / 3; i++)
    {
        int index1 = element_data[i * 3];
        int index2 = element_data[i * 3 + 1];
        int index3 = element_data[i * 3 + 2];

        vec3_subtract(&vertex_data[index1*3], &vertex_data[index3*3], side1);
        vec3_subtract(&vertex_data[index1*3], &vertex_data[index2*3], side2);

        vec3_cross(side2, side1, normal);

        vec3_add(&normal_data[index1*3], normal, &normal_data[index1*3]);
        vec3_add(&normal_data[index2*3], normal, &normal_data[index2*3]);
        vec3_add(&normal_data[index3*3], normal, &normal_data[index3*3]);
    }

    for (int i = 0; i < width * height; i++)
    {
        vec3_normalize(&normal_data[i*3], &normal_data[i*3]);
    }

    t.texture[0] = make_texture("grovel.tga");
    t.texture[1] = make_texture("stone.tga");
    t.texture[2] = make_texture("dirt.tga");

    t.element_count = (GLuint) ((width-1) * (height-1) * 6);
    t.vertex_count = (GLuint) (width * height);

    t.vertex_buffer = make_buffer(GL_ARRAY_BUFFER, vertex_data, (GLsizei) (t.vertex_count * 3 * sizeof(GLfloat)));
    t.texture_buffer = make_buffer(GL_ARRAY_BUFFER, texture_data, (GLsizei) (t.vertex_count * 2 * sizeof(GLfloat)));
    t.normal_buffer = make_buffer(GL_ARRAY_BUFFER, normal_data, (GLsizei) (t.vertex_count * 3 * sizeof(GLfloat)));
    t.element_buffer = make_buffer(GL_ELEMENT_ARRAY_BUFFER, element_data, (GLsizei) (t.element_count * sizeof(GLuint)));

    t.vertex_shader = make_shader(GL_VERTEX_SHADER, "shaders/terrain_shader.v.glsl");
    t.fragment_shader = make_shader(GL_FRAGMENT_SHADER, "shaders/terrain_shader.f.glsl");

    t.program = make_program(t.vertex_shader, t.fragment_shader);

    t.attributes.position = glGetAttribLocation(t.program, "position");
    t.attributes.normal = glGetAttribLocation(t.program, "normal");
    t.attributes.texture_coords = glGetAttribLocation(t.program, "tex_coords");

    t.uniform.mvp_matrix = glGetUniformLocation(t.program, "mvp_matrix");
    t.uniform.mv_matrix = glGetUniformLocation(t.program, "mv_matrix");
    t.uniform.normal_matrix = glGetUniformLocation(t.program, "normal_matrix");
    t.uniform.color = glGetUniformLocation(t.program, "color");

    t.uniform.texture[0] = glGetUniformLocation(t.program, "grovel_texture");
    t.uniform.texture[1] = glGetUniformLocation(t.program, "stone_texture");
    t.uniform.texture[2] = glGetUniformLocation(t.program, "dirt_texture");

    free(pixels);
    free(vertex_data);
    free(texture_data);
    free(element_data);
    free(normal_data);

    return t;
}

void terrain_render(terrain t, mat4_t model, mat4_t view, mat4_t projection)
{

    glUseProgram(t.program);

    float mvp[16];
    mat4_multiply(projection, view, mvp);
    mat4_multiply(mvp, model , mvp);

    float normal_matrix[16];
    mat4_multiply(view, model , normal_matrix);

    mat4_inverse(normal_matrix, normal_matrix);
    mat4_transpose(normal_matrix, normal_matrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (GLuint) t.texture[0]);
    glUniform1i((GLuint) t.uniform.texture[0], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, (GLuint) t.texture[1]);
    glUniform1i((GLuint) t.uniform.texture[1], 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, (GLuint) t.texture[2]);
    glUniform1i((GLuint) t.uniform.texture[2], 2);

    glBindBuffer(GL_ARRAY_BUFFER, t.vertex_buffer);
    glVertexAttribPointer(
            (GLuint) t.attributes.position,  /* attribute */
            3,                               /* size */
            GL_FLOAT,                        /* type */
            GL_FALSE,                        /* normalized? */
            sizeof(GLfloat) * 3,             /* stride */
            (void *) 0                       /* array buffer offset */
    );

    glEnableVertexAttribArray((GLuint) t.attributes.position);

    glBindBuffer(GL_ARRAY_BUFFER, t.texture_buffer);
    glVertexAttribPointer(
            (GLuint) t.attributes.texture_coords,  /* attribute */
            2,                                     /* size */
            GL_FLOAT,                              /* type */
            GL_FALSE,                              /* normalized? */
            sizeof(GLfloat) * 2,                   /* stride */
            (void *) 0                             /* array buffer offset */
    );

    glEnableVertexAttribArray((GLuint) t.attributes.texture_coords);

    glBindBuffer(GL_ARRAY_BUFFER, t.normal_buffer);
    glVertexAttribPointer(
            (GLuint) t.attributes.normal,  /* attribute */
            3,                               /* size */
            GL_FLOAT,                         /* type */
            GL_FALSE,                         /* normalized? */
            sizeof(GLfloat) * 3,                /* stride */
            (void *) 0                          /* array buffer offset */
    );

    glEnableVertexAttribArray((GLuint) t.attributes.normal);

    glUniformMatrix4fv(t.uniform.mvp_matrix, 1, GL_FALSE, mvp);
    glUniformMatrix4fv(t.uniform.mv_matrix, 1, GL_FALSE, view);
    glUniformMatrix4fv(t.uniform.normal_matrix, 1, GL_FALSE, normal_matrix);
    glUniform4f(t.uniform.color, 0.0f ,0.0f, 1.0f, 0.0f);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t.element_buffer);

    glDrawElements(
            GL_TRIANGLES,  /* mode */
            t.element_count,                  /* count */
            GL_UNSIGNED_INT,  /* type */
            (void *) 0            /* element array buffer offset */
    );


    glDisableVertexAttribArray((GLuint) t.attributes.position);
}

void terrain_free(terrain t)
{

}
