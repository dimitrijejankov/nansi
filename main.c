#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include "geometry/terrain.h"

terrain t;
mat4_t projection;
mat4_t view;
mat4_t model;

float eye[3] = {0.0f, 250.0, 0.0f};
float center[3] = {0.0f, 250.0f,10.0f};
float up[3] = {0.0f, 1.0f, 0.0f};

float position[3] = {0.0f, 0.0f, 0.0f};

static int init(void) {

    glEnable(GL_DEPTH_TEST);

    //t = terrain_create("terrain.tga", 30.0f, 5.0f);
    t = terrain_generate(5, 400.0f , 10.0f);

    projection = mat4_create(projection);
    view = mat4_create(view);
    model = mat4_create(model);

    model = mat4_identity(model);
    model = mat4_translate(model, position, model);

    return 1;
}

void changeSize(int w, int h) {

    if (h == 0)
        h = 1;

    float ratio =  w * 1.0f / h;

    glViewport(0, 0, w, h);

    mat4_identity(projection);
    projection = mat4_perspective(45.0f, ratio, 1.0f, 100000.0f , projection);

    mat4_identity(view);
    view = mat4_lookAt(eye, center, up, view);
}

static void update(void) {
    glutPostRedisplay();
}

static void render(void) {

    glClearColor(0.2, 0.2 , 0.6, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    terrain_render(t, model, view, projection);

    glutSwapBuffers();
}

void shutdown()
{
    free(view);
    free(projection);
    free(model);
}

void pressKey(int key, int xx, int yy) {

    switch (key) {
        case GLUT_KEY_UP : position[2] += -10.5f; break;
        case GLUT_KEY_DOWN : position[2] += 10.5f; break;
        case GLUT_KEY_LEFT : position[0] += -10.5f; break;
        case GLUT_KEY_RIGHT : position[0] += 10.5f; break;
        case GLUT_KEY_PAGE_UP : position[1] += 10.5f; break;
        case GLUT_KEY_PAGE_DOWN : position[1] += -10.5f; break;

        default:break;
    }

    model = mat4_identity(model);
    model = mat4_translate(model, position, model);

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    changeSize(400, 300);
    glutReshapeFunc(changeSize);
    glutCreateWindow("NANSI project");
    glutIdleFunc(&update);
    glutDisplayFunc(&render);
    glutSpecialFunc(pressKey);

    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        return 1;
    }

    if (!init()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }

    glutMainLoop();

    shutdown();
    return 0;
}

