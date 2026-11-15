#include <GL/glut.h>
#include "camera.h"
#include "scene.h"

// ------------------------------------------------------------
// Projeto Museu 3D - Computacao Grafica
// Codigo-base fornecido pelo professor.
//
// ------------------------------------------------------------

int largura = 1000;
int altura = 700;

void desenho()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    configuraProjecao(largura, altura);
    aplicaCamera();

    desenhaCena();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    largura = (w > 0) ? w : 1;
    altura  = (h > 0) ? h : 1;
    glViewport(0, 0, largura, altura);
}

void idle()
{
    atualizaAnimacao();
    glutPostRedisplay();
}

// ------------------------------------------------------------
// INTERACAO INICIAL PELO TECLADO
// ------------------------------------------------------------

void teclado(unsigned char tecla, int x, int y)
{
    // x e y sao as coordenadas do mouse recebidas pelo GLUT.
    // Elas nao sao utilizadas por enquanto.
    switch (tecla)
    {
        case 'p':
        case 'P':
            usaPerspectiva();
            break;

        case 'o':
        case 'O':
            usaOrtografica();
            break;

        case ' ':
            alternaAnimacao();
            break;
    }

    glutPostRedisplay();
}

void inicializa()
{
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    inicializaCamera();
    inicializaCena();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(largura, altura);
    glutCreateWindow("Museu 3D - Computacao Grafica");

    inicializa();

    glutDisplayFunc(desenho);
    glutReshapeFunc(reshape);   
    glutIdleFunc(idle);
    glutKeyboardFunc(teclado);

    glutMainLoop();

    return 0;
}
