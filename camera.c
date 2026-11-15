#include <GL/glut.h>
#include <math.h>
#include "camera.h"

// ------------------------------------------------------------
// Camera inicial com uma vista elevada do Sistema Solar.
// A navegacao pelo teclado sera implementada na proxima etapa.
// ------------------------------------------------------------

static float camX = 0.0f;
static float camY = 13.0f;
static float camZ = 32.0f;

static float alvoX = 0.0f;
static float alvoY = 2.7f;
static float alvoZ = 0.0f;

static int perspectiva = 1;

void inicializaCamera()
{
    camX = 0.0f;
    camY = 13.0f;
    camZ = 32.0f;

    alvoX = 0.0f;
    alvoY = 2.7f;
    alvoZ = 0.0f;

    perspectiva = 1;
}

void aplicaCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        camX, camY, camZ,
        alvoX, alvoY, alvoZ,
        0.0f, 1.0f, 0.0f
    );
}

void configuraProjecao(int largura, int altura)
{
    float aspecto = (altura == 0) ? 1.0f: (float)largura / (float)altura;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (perspectiva)
    {
        gluPerspective(
            60.0,
            aspecto,
            0.5,
            100.0
        );
    }
    else
    {
        // Aumentamos o volume ortografico para mostrar toda a exposicao.
        float metadeAltura = 23.0f;
        float metadeLargura = metadeAltura * aspecto;

        glOrtho(
            -metadeLargura, metadeLargura,
            -metadeAltura, metadeAltura,
            -100.0, 100.0
        );
    }

    glMatrixMode(GL_MODELVIEW);
}

void usaPerspectiva()
{
    perspectiva = 1;
}

void usaOrtografica()
{
    perspectiva = 0;
}

// ------------------------------------------------------------
// Movimento simples.
// ------------------------------------------------------------

