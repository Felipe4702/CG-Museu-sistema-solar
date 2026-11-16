#include <GL/glut.h>
#include "camera.h"
#include "scene.h"

// ============================================================
// ORIGEM DO CODIGO - DOCUMENTACAO DE PROVENIENCIA
// ============================================================
// [BASE ORIGINAL]  Estrutura ou logica ja presente no Museu.zip
//                  fornecido para a atividade. Pode ter ajustes.
// [AULAS]          Recurso ou tecnica explicado nos slides da
//                  disciplina; nao significa que foi copiado
//                  literalmente dos slides.
// [DESENVOLVIDO]   Logica escrita/adaptada durante a criacao
//                  deste projeto, com apoio de inteligencia
//                  artificial; nao identificada no codigo-base
//                  nem como exemplo direto dos slides.
// [MISTO]          Combina mais de uma das origens acima.
//
// Todos os comentarios deste projeto, incluindo estas notas
// de origem, foram escritos/revisados com auxilio de IA.
// As marcacoes descrevem a origem dos trechos, nao a autoria
// dos comandos padrao da biblioteca OpenGL/GLUT.
// ============================================================


// ------------------------------------------------------------
// MUSEU 3D DO SISTEMA SOLAR - PROGRAMA PRINCIPAL
// ------------------------------------------------------------

// ORIGEM: [BASE ORIGINAL] Tamanho inicial de janela 1000 x 700 do main.c do professor.
// Dimensoes atuais da janela.
static int largura = 1000;
static int altura = 700;

// ORIGEM: [DESENVOLVIDO] Estado persistente do teclado, ausente no codigo-base.
// Guarda quais teclas de movimento estao pressionadas.
// Por exemplo, teclas['w'] = 1 enquanto W estiver apertado.
static int teclas[256] = {0};

// ORIGEM: [DESENVOLVIDO] Sensibilidade e captura de mouse em primeira pessoa.
// A sensibilidade determina quanto a camera gira ao mover o mouse.
#define SENSIBILIDADE_MOUSE 0.13f

// O mouse comeca capturado para controlar o olhar.
// ESC libera o cursor; clique esquerdo captura novamente.
static int mouseCapturado = 1;

// ------------------------------------------------------------
// DESENHO DA CENA
// ------------------------------------------------------------

// ORIGEM: [BASE ORIGINAL] Sequencia de limpar buffers, configurar camera, desenhar cena e trocar buffers, preservada do main.c do professor.
void desenho(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    configuraProjecao(largura, altura);
    aplicaCamera();
    desenhaCena();

    glutSwapBuffers();
}

// ------------------------------------------------------------
// AJUSTE DO TAMANHO DA JANELA
// ------------------------------------------------------------

// ORIGEM: [MISTO: BASE ORIGINAL + DESENVOLVIDO] glViewport e ajuste do tamanho ja existiam no main.c; centralizacao do cursor foi acrescentada. glViewport tambem e apresentado na Aula 07.
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

// Chamado aproximadamente a cada 16 ms (cerca de 60 FPS).
// O calculo usa o tempo real transcorrido entre chamadas.
// ORIGEM: [DESENVOLVIDO] Temporizador, deltaTempo e leitura simultanea de teclas para caminhar com movimento continuo; substitui o idle do codigo-base.
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

// ORIGEM: [DESENVOLVIDO] Tratamento de teclas e novos atalhos. O codigo original nao registrava teclado; corresponde a interacao acrescentada ao projeto.
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

// ORIGEM: [DESENVOLVIDO] Registra a liberacao das teclas para manter caminhada continua e diagonais.
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

// ORIGEM: [DESENVOLVIDO] Controle do olhar pelo deslocamento do mouse e recentralizacao do cursor (nao implementado no codigo-base).
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

// ESC solta o mouse; clicar com o botao esquerdo captura novamente.
// ORIGEM: [DESENVOLVIDO] Captura novamente o cursor pelo clique esquerdo.
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

// As setas continuam como alternativa para orientar a camera.
// ORIGEM: [DESENVOLVIDO] Setas como alternativa para girar a camera.
void teclasEspeciais(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla)
    {
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

// ORIGEM: [BASE ORIGINAL] Parametros iniciais OpenGL (depth test, normalize e color material) mantidos do main.c do professor.
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

// ORIGEM: [MISTO: BASE ORIGINAL + DESENVOLVIDO] Criacao da janela e loop GLUT vieram do main.c original; callbacks de mouse, teclado e temporizador foram adicionados.
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

    // Registro das funcoes de entrada de teclado e mouse.
    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(tecladoSolto); // Disponivel no FreeGLUT.
    glutSpecialFunc(teclasEspeciais);
    glutPassiveMotionFunc(movimentoMouse);
    glutMotionFunc(movimentoMouse);
    glutMouseFunc(botaoMouse);

    // Evita que o Windows repita pressionamentos de teclas mantidas.
    glutIgnoreKeyRepeat(1);

    // Esconde e centraliza o cursor para olhar como em um jogo 3D.
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(largura / 2, altura / 2);

    glutMainLoop();
    return 0;
}
