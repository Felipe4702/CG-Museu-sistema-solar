#include <GL/freeglut.h>
#include "camera.h"
#include "scene.h"

// ------------------------------------------------------------
// MUSEU 3D DO SISTEMA SOLAR - PROGRAMA PRINCIPAL
// ------------------------------------------------------------
// Cria a janela, registra eventos do FreeGLUT e organiza o loop.
// As rotinas especificas de navegacao do visitante e controle de
// camera estao indicadas como feitas com auxilio de IA.
// Usa o cabecalho freeglut.h para declarar o controle de F11.

// Dimensoes atuais da janela.
static int largura = 1000;
static int altura = 700;

// IA (movimentacao do visitante): registra teclas mantidas.
// Exemplo: teclas['w'] e 1 ate o evento de soltar W.
// Isso permite combinar W+A ou W+D para caminhar na diagonal.
static int teclas[256] = {0};

// IA (camera): graus de rotacao por pixel deslocado no mouse.
#define SENSIBILIDADE_MOUSE 0.13f

// IA (camera): mantem o mouse capturado ao iniciar.
// ESC libera; clique esquerdo volta ao controle do olhar.
static int mouseCapturado = 1;

// ------------------------------------------------------------
// DESENHO DA CENA
// ------------------------------------------------------------

// Desenha um quadro: limpa a imagem anterior, ajusta as matrizes,
// aplica a camera, desenha o museu e sobrepoe a interface 2D.
// O double buffer evita que o visitante veja a tela sendo redesenhada.
void desenho(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    configuraProjecao(largura, altura);
    aplicaCamera();
    desenhaCena();
    desenhaInterface(largura, altura);

    glutSwapBuffers();
}

// ------------------------------------------------------------
// AJUSTE DO TAMANHO DA JANELA
// ------------------------------------------------------------

// Reage a mudancas do tamanho da janela e ajusta o viewport.
// IA (camera): centraliza o cursor novamente, se estiver capturado.
void reshape(int w, int h)
{
    largura = (w > 0) ? w : 1;
    altura = (h > 0) ? h : 1;

    glViewport(0, 0, largura, altura);

    // Ao mudar o tamanho da janela, centraliza novamente o mouse.
    if (mouseCapturado)
        glutWarpPointer(largura / 2, altura / 2);
}

// ------------------------------------------------------------
// MOVIMENTACAO CONTINUA E ATUALIZACAO DA ANIMACAO
// ------------------------------------------------------------

// IA (movimentacao do visitante): rotina periodica do FreeGLUT.
// Aproximadamente a cada 16 ms, le as teclas mantidas, calcula o
// deslocamento com deltaTempo e pede o proximo quadro.
// O mesmo timer tambem atualiza a animacao dos astros na cena.
void atualiza(int valor)
{
    static int tempoAnterior = 0;
    int tempoAtual = glutGet(GLUT_ELAPSED_TIME);
    float deltaTempo = (tempoAtual - tempoAnterior) / 1000.0f;

    (void)valor;
    tempoAnterior = tempoAtual;

    // Evita saltos exagerados caso o programa fique temporariamente lento.
    if (deltaTempo > 0.05f)
        deltaTempo = 0.05f;

    // So movemos a camera se a tecla continuar pressionada.
    // Os sinais permitem ir e voltar nos mesmos eixos.
    float frente = (float)(teclas['w'] - teclas['s']);
    float lado = (float)(teclas['d'] - teclas['a']);
    float vertical = (float)(teclas['q'] - teclas['z']);

    atualizaMovimentoCamera(frente, lado, vertical, deltaTempo);
    atualizaAnimacao();

    glutPostRedisplay();

    // Agenda a proxima atualizacao.
    glutTimerFunc(16, atualiza, 0);
}

// ------------------------------------------------------------
// TECLADO: TECLA PRESSIONADA
// ------------------------------------------------------------

// Recebe teclas comuns e encaminha cada atalho para sua funcao.
// IA (movimentacao do visitante e camera): o movimento nao e feito
// aqui; armazenamos as teclas para processa-las no timer atualiza().
// IA (translacao orbital): T alterna o estado das orbitas.
void teclado(unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    // Converte letras maiusculas para minusculas.
    if (tecla >= 'A' && tecla <= 'Z')
        tecla = tecla + ('a' - 'A');

    switch (tecla)
    {
        // W, A, S, D caminham; Q e Z controlam a altura no voo.
        // Nao movemos aqui: apenas registramos a tecla apertada.
        case 'w':
        case 'a':
        case 's':
        case 'd':
        case 'q':
        case 'z':
            teclas[tecla] = 1;
            break;

        case 'v':
            alternaModoVoo();
            break;

        case 'p':
            usaPerspectiva();
            break;

        case 'o':
            usaOrtografica();
            break;

        // IA (camera): I alterna a vista superior das orbitas.
        case 'i':
            alternaVistaSuperior();
            break;

        // IA (translacao orbital): inicia/para a orbita dos planetas.
        case 't':
            alternaTranslacao();
            break;

        // F, G e L foram solicitados no enunciado.
        case 'f':
            usaFlat();
            break;

        case 'g':
            usaSmooth();
            break;

        case 'l':
            alternaIluminacao();
            break;

        // E: mostra os dados do astro ao se aproximar.
        case 'e':
            interageComAstro();
            break;

        case ' ':
            alternaAnimacao();
            break;

        case 'r':
            inicializaCamera();
            break;

        case 27: // ESC
            // Libera o mouse para poder usar o Windows normalmente.
            mouseCapturado = 0;
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            break;
    }

    glutPostRedisplay();
}

// ------------------------------------------------------------
// TECLADO: TECLA SOLTA
// ------------------------------------------------------------

// IA (movimentacao do visitante): limpa a tecla quando e solta.
// Sem esse evento, a camera continuaria andando indefinidamente.
void tecladoSolto(unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (tecla >= 'A' && tecla <= 'Z')
        tecla = tecla + ('a' - 'A');

    // Ao soltar a tecla, o visitante para de andar nessa direcao.
    teclas[tecla] = 0;
}

// ------------------------------------------------------------
// MOUSE: CONTROLA PARA ONDE O VISITANTE OLHA
// ------------------------------------------------------------
// IA (camera): a movimentacao relativa ao centro gira o olhar;
// o cursor e recentralizado para permitir giro continuo.
void movimentoMouse(int x, int y)
{
    if (!mouseCapturado)
        return;

    // Centro da janela em pixels.
    int centroX = largura / 2;
    int centroY = altura / 2;

    // O GLUT tambem chama essa funcao quando reposicionamos o mouse.
    // Nesse caso, ignoramos o evento para nao girar duas vezes.
    if (x == centroX && y == centroY)
        return;

    // Horizontal: para a direita = gira para a direita.
    // Vertical: ao subir o mouse (Y menor), olha para cima.
    float giroHorizontal = (x - centroX) * SENSIBILIDADE_MOUSE;
    float giroVertical = (centroY - y) * SENSIBILIDADE_MOUSE;

    giraCamera(giroHorizontal, giroVertical);

    // Recoloca o mouse no meio para permitir girar continuamente.
    glutWarpPointer(centroX, centroY);
    glutPostRedisplay();
}

// IA (camera): ESC solta o cursor para uso normal do Windows;
// clique esquerdo volta ao modo de olhar com mouse.
void botaoMouse(int botao, int estado, int x, int y)
{
    (void)x;
    (void)y;

    if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN && !mouseCapturado)
    {
        mouseCapturado = 1;
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(largura / 2, altura / 2);
    }
}

// IA (camera): as setas giram o olhar sem exigir mouse.
// F11 e um controle da janela do FreeGLUT, nao altera a camera.
void teclasEspeciais(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla)
    {
        // FreeGLUT: F11 alterna entre fullscreen e janela normal.
        // A funcao restaura a decoracao e a barra de tarefas
        // quando o usuario volta ao modo janela.
        case GLUT_KEY_F11:
            glutFullScreenToggle();
            break;

        case GLUT_KEY_LEFT:  giraCamera(-3.0f, 0.0f); break;
        case GLUT_KEY_RIGHT: giraCamera( 3.0f, 0.0f); break;
        case GLUT_KEY_UP:    giraCamera(0.0f,  3.0f); break;
        case GLUT_KEY_DOWN:  giraCamera(0.0f, -3.0f); break;
    }

    glutPostRedisplay();
}

// ------------------------------------------------------------
// INICIALIZACAO DO OPENGL
// ------------------------------------------------------------

// Prepara o estado global do OpenGL: cor de fundo, profundidade,
// normais, materiais e configuracoes iniciais da cena e camera.
void inicializa(void)
{
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    inicializaCamera();
    inicializaCena();
}

// Inicializa o FreeGLUT, cria a janela e registra os callbacks.
// glutMainLoop() mantem o programa ativo ate fechar a janela.
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(largura, altura);
    glutCreateWindow("Museu 3D - Sistema Solar");

    inicializa();

    // Funcao de desenho, janela e atualizacao periodica.
    glutDisplayFunc(desenho);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, atualiza, 0);

    // IA (movimentacao do visitante e camera): liga os eventos de
    // tecla apertada/solta e mouse aos controladores de navegacao.
    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(tecladoSolto);
    glutSpecialFunc(teclasEspeciais);
    glutPassiveMotionFunc(movimentoMouse);
    glutMotionFunc(movimentoMouse);
    glutMouseFunc(botaoMouse);

    // Evita que o Windows repita pressionamentos de teclas mantidas.
    glutIgnoreKeyRepeat(1);

    // Tela cheia real: remove a borda/titulo da janela e cobre
    // a barra de tarefas. F11 alterna para o modo janela.
    glutSetCursor(GLUT_CURSOR_NONE);
    glutFullScreen();

    glutMainLoop();
    return 0;
}
