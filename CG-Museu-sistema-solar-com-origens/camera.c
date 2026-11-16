#include <GL/glut.h>
#include <math.h>
#include "camera.h"

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
// CAMERA DO VISITANTE
// ------------------------------------------------------------

// ORIGEM: [DESENVOLVIDO] Constantes e limites para a camera de visitante.
// As funcoes seno e cosseno trabalham com radianos.
#define PI 3.14159265f

// Os planetas e a Lua tem centro na altura 4.5.
// A camera fica nessa mesma altura enquanto caminhamos.
#define ALTURA_VISITANTE 4.5f

// Distancia percorrida por segundo (independente dos FPS).
#define VELOCIDADE 8.0f

// A sala vai de -65 ate +65 nos eixos X e Z.
// Deixamos uma margem de 2 unidades para nao atravessar as paredes.
#define LIMITE_HORIZONTAL 63.0f

// Limites de subida e descida quando o modo de voo esta ativo.
#define ALTURA_MINIMA 1.0f
#define ALTURA_MAXIMA 16.5f

// ------------------------------------------------------------
// ESTADO DA CAMERA
// ------------------------------------------------------------

// Inicio: visitante na entrada, em frente ao Sol.
static float camX = 0.0f;
static float camY = ALTURA_VISITANTE;
static float camZ = 25.0f;

// -90 graus corresponde a olhar para o fundo do museu (-Z).
// Zero na vertical corresponde a olhar na linha do horizonte.
static float anguloHorizontal = -90.0f;
static float anguloVertical = 0.0f;

static int perspectiva = 1;
static int modoVoo = 0;

// Mantem um numero dentro dos limites indicados.
// ORIGEM: [DESENVOLVIDO] Funcao auxiliar para restringir coordenadas e angulos.
static float limita(float valor, float minimo, float maximo)
{
    if (valor < minimo)
        return minimo;
    if (valor > maximo)
        return maximo;
    return valor;
}

// ORIGEM: [MISTO: BASE ORIGINAL + DESENVOLVIDO] A funcao de inicializacao existia no camera.c original; posicao de visitante, angulos e modo de voo sao adaptacoes novas.
void inicializaCamera(void)
{
    // A tecla R devolve a camera a essa configuracao.
    camX = 0.0f;
    camY = ALTURA_VISITANTE;
    camZ = 25.0f;
    anguloHorizontal = -90.0f;
    anguloVertical = 0.0f;
    perspectiva = 1;
    modoVoo = 0;
}

// ------------------------------------------------------------
// POSICAO E DIRECAO DO OLHAR
// ------------------------------------------------------------

// ORIGEM: [MISTO: BASE ORIGINAL + AULAS + DESENVOLVIDO] Uso de MODELVIEW, glLoadIdentity e gluLookAt ja existia no camera.c original e e abordado na Aula 07; direcao calculada com seno/cosseno e nova.
void aplicaCamera(void)
{
    float horizontal = anguloHorizontal * PI / 180.0f;
    float vertical = anguloVertical * PI / 180.0f;

    // Vetor que aponta para onde o visitante esta olhando.
    float dirX = cosf(vertical) * cosf(horizontal);
    float dirY = sinf(vertical);
    float dirZ = cosf(vertical) * sinf(horizontal);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // A origem da camera e (camX, camY, camZ).
    // Somamos a direcao para encontrar o ponto observado.
    // O ultimo vetor informa qual direcao representa "para cima".
    gluLookAt(camX, camY, camZ,
              camX + dirX, camY + dirY, camZ + dirZ,
              0.0f, 1.0f, 0.0f);
}

// ------------------------------------------------------------
// PERSPECTIVA E ORTOGRAFICA
// ------------------------------------------------------------

// ORIGEM: [MISTO: BASE ORIGINAL + AULAS + DESENVOLVIDO] Alternancia gluPerspective/glOrtho e matriz PROJECTION estavam no camera.c original; Aula 07 trata esses comandos. Limites e enquadramento foram adaptados ao museu.
void configuraProjecao(int largura, int altura)
{
    float aspecto = (altura == 0) ? 1.0f : (float)largura / (float)altura;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (perspectiva)
    {
        // Objetos mais distantes aparecem menores.
        // O plano distante cobre a sala de 130 x 130 unidades.
        gluPerspective(65.0, aspecto, 0.5, 200.0);
    }
    else
    {
        // Os tamanhos aparentes nao mudam com a distancia.
        // Os limites sao mais amplos que os da perspectiva.
        float metadeAltura = 42.0f;
        float metadeLargura = metadeAltura * aspecto;

        glOrtho(-metadeLargura, metadeLargura,
                -metadeAltura, metadeAltura,
                0.5, 300.0);
    }

    glMatrixMode(GL_MODELVIEW);
}

// ORIGEM: [MISTO: BASE ORIGINAL + DESENVOLVIDO] A funcao que ativa perspectiva existia; reposicionamento da camera foi acrescentado.
void usaPerspectiva()
{
    perspectiva = 1;

    // Camera de visitante.
    camX = 0.0f;
    camY = 4.5f;
    camZ = 55.0f;

    anguloHorizontal = -90.0f;
    anguloVertical = 0.0f;
}

// ORIGEM: [MISTO: BASE ORIGINAL + DESENVOLVIDO] A funcao que ativa ortografica existia; reposicionamento para a vista geral foi acrescentado.
void usaOrtografica()
{
    perspectiva = 0;

    // Camera de visao geral do museu.
    camX = 0.0f;
    camY = 14.0f;
    camZ = 95.0f;

    // Olhando para o centro da sala.
    anguloHorizontal = -90.0f;
    anguloVertical = -6.0f;
}

// ------------------------------------------------------------
// MOVIMENTACAO CONTINUA DO VISITANTE
// ------------------------------------------------------------

// ORIGEM: [DESENVOLVIDO] Navegacao em primeira pessoa, normalizacao de diagonal, movimento por tempo e limites da sala; usa conceitos geometricos, mas nao reproduz codigo dos slides.
void atualizaMovimentoCamera(float frente, float lado,
                            float vertical, float deltaTempo)
{
    float angulo = anguloHorizontal * PI / 180.0f;

    // Vetor de frente no plano do piso (eixos X e Z).
    // Olhar para cima ou para baixo nao altera a altura do andar.
    float frenteX = cosf(angulo);
    float frenteZ = sinf(angulo);

    // Vetor da direita: perpendicular a frente, no piso.
    float direitaX = -frenteZ;
    float direitaZ = frenteX;

    // Se W e D estiverem pressionados juntos, a diagonal
    // nao deve ser mais rapida do que caminhar em linha reta.
    float comprimento = sqrtf(frente * frente + lado * lado);
    if (comprimento > 1.0f)
    {
        frente /= comprimento;
        lado /= comprimento;
    }

    // Multiplicamos pelo tempo decorrido entre as atualizacoes.
    // Assim a velocidade nao depende da quantidade de FPS.
    float distancia = VELOCIDADE * deltaTempo;

    camX += (frente * frenteX + lado * direitaX) * distancia;
    camZ += (frente * frenteZ + lado * direitaZ) * distancia;

    // Impede atravessar as quatro paredes da sala.
    camX = limita(camX, -LIMITE_HORIZONTAL, LIMITE_HORIZONTAL);
    camZ = limita(camZ, -LIMITE_HORIZONTAL, LIMITE_HORIZONTAL);

    if (modoVoo)
    {
        // No voo, Q sobe e Z desce.
        camY += vertical * distancia;
        camY = limita(camY, ALTURA_MINIMA, ALTURA_MAXIMA);
    }
    else
    {
        // Caminhando, permanecemos sempre na mesma altura.
        camY = ALTURA_VISITANTE;
    }
}

// ORIGEM: [DESENVOLVIDO] Alternancia entre caminhada e voo.
void alternaModoVoo(void)
{
    // V muda entre andar no piso e voar pela sala.
    modoVoo = !modoVoo;

    // Ao voltar a andar, retorna imediatamente ao piso.
    if (!modoVoo)
        camY = ALTURA_VISITANTE;
}

// ------------------------------------------------------------
// DIRECAO DA CAMERA: MOUSE OU SETAS
// ------------------------------------------------------------

// ORIGEM: [DESENVOLVIDO] Atualiza os angulos de direcao e limita a inclinacao.
void giraCamera(float horizontal, float vertical)
{
    anguloHorizontal += horizontal;
    anguloVertical += vertical;

    // Nao permitimos virar completamente de cabeca para baixo.
    anguloVertical = limita(anguloVertical, -85.0f, 85.0f);
}

// Retorna 1 para perspectiva e 0 para ortografica.
// ORIGEM: [DESENVOLVIDO] Consulta usada para desenhar a sala de formas diferentes nas duas projecoes.
int cameraEstaEmPerspectiva(void)
{
    return perspectiva;
}
