#include <GL/glut.h>
#include "scene.h"
#include <GL/glut.h>
#include "scene.h"
#include "camera.h"

// ------------------------------------------------------------
// TAMANHOS E DIMENSOES DO MUSEU
// ------------------------------------------------------------

// O Sol e o dobro do tamanho que tinha na etapa anterior.
// Todos os planetas continuam com o mesmo raio, e a Lua e menor.
#define RAIO_SOL 6.4f
#define RAIO_PLANETA 1.6f
#define RAIO_LUA 0.8f

// Planetas e Lua permanecem na altura da visao do visitante.
#define ALTURA_ASTROS 4.5f

// O Sol fica um pouco mais alto para nao atravessar o piso.
// Seu raio e 6.4 e seu centro esta em 6.5: sobra 0.1 ate o piso.
#define ALTURA_SOL 6.5f

// Folga entre a superficie do Sol e a superficie de Mercurio.
// Ela e maior para deixar o Sol em destaque no centro da sala.
#define ESPACO_INICIAL 8.0f

// Folga entre as orbitas de planetas consecutivos.
// Esta distancia continua igual a da etapa anterior.
#define ESPACO_BASE 2.0f

// Sala ampliada para a futura orbita de Netuno caber por inteiro.
#define METADE_SALA 65.0f
#define ALTURA_PAREDES 18.0f

// ------------------------------------------------------------
// ESTADO DA CENA
// ------------------------------------------------------------

// Controla se a iluminacao e a animacao estao ativadas.
static int iluminacaoLigada = 1;
static int animacaoLigada = 1;

// Angulo usado para fazer os planetas girarem sobre seus eixos.
static float anguloAnimacao = 0.0f;

// ------------------------------------------------------------
// ILUMINACAO BASICA
// ------------------------------------------------------------

void configuraIluminacao()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambiente[]  = {0.20f, 0.20f, 0.20f, 1.0f};
    GLfloat difusa[]    = {0.80f, 0.80f, 0.80f, 1.0f};
    GLfloat especular[] = {1.00f, 1.00f, 1.00f, 1.0f};
    GLfloat posicao[]   = {0.0f, 11.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao);
}

void configuraMaterialExemplo()
{
    GLfloat especular[] = {0.8f, 0.8f, 0.8f, 1.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
}

// Essa funcao sera completada no encontro sobre iluminacao.
void configuraLuzesDaCena()
{
    // Por enquanto, usamos apenas a luz original do professor.
}

// ------------------------------------------------------------
// AMBIENTE DO MUSEU
// ------------------------------------------------------------

// O museu tem piso, quatro paredes e teto.
// Todas as superficies utilizam as dimensoes definidas acima.

void desenhaPiso()
{
    glColor3f(0.30f, 0.30f, 0.35f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, 0.0f,  METADE_SALA);
        glVertex3f(-METADE_SALA, 0.0f,  METADE_SALA);
    glEnd();
}

void desenhaParedeFundo()
{
    glColor3f(0.15f, 0.18f, 0.25f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
    glEnd();
}

void desenhaParedeEsquerda()
{
    glColor3f(0.18f, 0.20f, 0.28f);

    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f(-METADE_SALA, 0.0f,  METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
    glEnd();
}

void desenhaParedeDireita()
{
    glColor3f(0.18f, 0.20f, 0.28f);

    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(METADE_SALA, 0.0f,  METADE_SALA);
        glVertex3f(METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f(METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
        glVertex3f(METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
    glEnd();
}


// Fecha a frente do museu (lado positivo do eixo Z).
void desenhaParedeFrente()
{
    glColor3f(0.15f, 0.18f, 0.25f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f( METADE_SALA, 0.0f, METADE_SALA);
        glVertex3f(-METADE_SALA, 0.0f, METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES, METADE_SALA);
    glEnd();
}

// Teto simples para o ambiente parecer uma sala de verdade.
void desenhaTeto()
{
    glColor3f(0.17f, 0.19f, 0.25f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
    glEnd();
}

// ------------------------------------------------------------
// OBJETOS DO SISTEMA SOLAR
// ------------------------------------------------------------

void desenhaSol()
{
    glPushMatrix();

        // O Sol permanece no centro da sala, mais alto para nao
        // atravessar o piso depois de dobrar de tamanho.
        glTranslatef(0.0f, ALTURA_SOL, 0.0f);
        glScalef(RAIO_SOL, RAIO_SOL, RAIO_SOL);

        // Emissao deixa o Sol visivelmente iluminado.
        // A configuracao de iluminacao completa ficara para depois.
        GLfloat emissao[] = {0.65f, 0.30f, 0.02f, 1.0f};
        GLfloat semEmissao[] = {0.0f, 0.0f, 0.0f, 1.0f};

        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissao);
        glColor3f(1.0f, 0.72f, 0.10f);
        glutSolidSphere(1.0, 32, 24);

        // Retira a emissao para nao afetar os outros objetos.
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, semEmissao);

    glPopMatrix();
}

// Desenha um planeta utilizando apenas uma esfera.
// x e z: posicao no piso; raio: tamanho da esfera;
// vermelho, verde e azul: componentes da cor (de 0 a 1).
void desenhaPlaneta(float x, float z, float raio,
                    float vermelho, float verde, float azul)
{
    glPushMatrix();

        // 1. Translada: coloca o planeta em sua posicao fixa.
        glTranslatef(x, ALTURA_ASTROS, z);

        // 2. Rotaciona: gira o planeta sobre seu proprio eixo Y.
        // Nao ha translacao orbital nesta primeira entrega.
        glRotatef(anguloAnimacao, 0.0f, 1.0f, 0.0f);

        // 3. Escala: define o tamanho do planeta.
        glScalef(raio, raio, raio);

        // Desenha a esfera usando a cor escolhida.
        glColor3f(vermelho, verde, azul);
        glutSolidSphere(1.0, 32, 24);

        // Pequena marca na superficie para tornar a rotacao visivel.
        // Sem textura ou marca, uma esfera de cor uniforme parece parada.
        glPushMatrix();
            glTranslatef(0.25f, 0.20f, 0.94f);
            glScalef(0.20f, 0.24f, 0.06f);
            glColor3f(vermelho * 0.50f, verde * 0.50f, azul * 0.50f);
            glutSolidSphere(1.0, 12, 8);
        glPopMatrix();

    glPopMatrix();
}

// Calcula a distancia entre o Sol e o centro de cada planeta.
// ordem = 1 para Mercurio, 2 para Venus, ... 8 para Netuno.
//
// Mercurio: folga de ESPACO_INICIAL entre a borda do Sol
// e a borda do planeta.
// Cada nova orbita acrescenta o diametro de um planeta
// mais ESPACO_BASE. A progressao anterior e preservada.
static float raioOrbita(int ordem)
{
    float diametroPlaneta = 2.0f * RAIO_PLANETA;
    float primeiraOrbita = RAIO_SOL + ESPACO_INICIAL + RAIO_PLANETA;

    return primeiraOrbita + (ordem - 1) * (diametroPlaneta + ESPACO_BASE);
}

void desenhaObjetosDoMuseu()
{
    // Todos os astros estao no mesmo plano Z = 0.
    // Planetas e Lua ficam em Y = ALTURA_ASTROS; o Sol, que e maior,
    // tem o centro um pouco mais alto para nao atravessar o piso.
    // O visitante ve impares a direita (+X) e pares a esquerda (-X).

    desenhaSol();

    // 1. Mercurio: direita, primeira orbita (X = +16.0).
    desenhaPlaneta( raioOrbita(1), 0.0f, RAIO_PLANETA,
                    0.55f, 0.53f, 0.50f);

    // 2. Venus: esquerda, segunda orbita (X = -21.2).
    desenhaPlaneta(-raioOrbita(2), 0.0f, RAIO_PLANETA,
                    0.86f, 0.72f, 0.43f);

    // 3. Terra: direita, terceira orbita (X = +26.4).
    desenhaPlaneta( raioOrbita(3), 0.0f, RAIO_PLANETA,
                    0.15f, 0.39f, 0.90f);

    // Lua: logo depois da Terra, na mesma altura.
    // Distancia entre centros = raio da Terra + raio da Lua + 0.8.
    // Seu movimento orbital em torno da Terra sera feito no futuro.
    desenhaPlaneta( raioOrbita(3) + RAIO_PLANETA + RAIO_LUA + 0.8f,
                    0.0f, RAIO_LUA, 0.66f, 0.66f, 0.66f);

    // 4. Marte: esquerda, quarta orbita (X = -31.6).
    desenhaPlaneta(-raioOrbita(4), 0.0f, RAIO_PLANETA,
                    0.80f, 0.30f, 0.18f);

    // 5. Jupiter: direita, quinta orbita (X = +36.8).
    desenhaPlaneta( raioOrbita(5), 0.0f, RAIO_PLANETA,
                    0.83f, 0.61f, 0.42f);

    // 6. Saturno: esquerda, sexta orbita (X = -42.0).
    // Aneis de Saturno serao acrescentados futuramente.
    desenhaPlaneta(-raioOrbita(6), 0.0f, RAIO_PLANETA,
                    0.85f, 0.75f, 0.48f);

    // 7. Urano: direita, setima orbita (X = +47.2).
    desenhaPlaneta( raioOrbita(7), 0.0f, RAIO_PLANETA,
                    0.47f, 0.82f, 0.85f);

    // 8. Netuno: esquerda, oitava orbita (X = -52.4).
    desenhaPlaneta(-raioOrbita(8), 0.0f, RAIO_PLANETA,
                    0.19f, 0.31f, 0.77f);
}

// ------------------------------------------------------------
// CENA PRINCIPAL
// ------------------------------------------------------------

void desenhaCena()
{
    glMatrixMode(GL_MODELVIEW);

    if (iluminacaoLigada)
    {
        configuraIluminacao();
        configuraLuzesDaCena();
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    // Define como o ambiente sera desenhado
    // dependendo da projecao selecionada.
    if (cameraEstaEmPerspectiva())
    {
        // Modo perspectiva: museu completo.
        desenhaPiso();
        desenhaParedeFundo();
        desenhaParedeEsquerda();
        desenhaParedeDireita();
        desenhaParedeFrente();
        desenhaTeto();
    }
    else
    {
        // Modo ortografico: afastamos o piso
        // para a altura Y = -50.
        glPushMatrix();
            glTranslatef(0.0f, -50.0f, 0.0f);
            desenhaPiso();
        glPopMatrix();

        // Afastamos o teto para a altura Y = 50.
        // Subtraimos a altura original do teto,
        // pois ele ja foi criado em Y = 18.
        glPushMatrix();
            glTranslatef(0.0f, 50.0f - ALTURA_PAREDES, 0.0f);
            desenhaTeto();
        glPopMatrix();

        // As quatro paredes nao sao desenhadas
        // enquanto a projecao ortografica estiver ativa.
    }

    // Os astros aparecem normalmente nas duas projecoes.
    desenhaObjetosDoMuseu();
}

// ------------------------------------------------------------
// CONTROLES E ANIMACAO
// ------------------------------------------------------------

void inicializaCena()
{
    glShadeModel(GL_SMOOTH);
    configuraMaterialExemplo();
}

// Estas funcoes serao conectadas ao teclado na etapa de iluminacao.
void alternaIluminacao()
{
    iluminacaoLigada = !iluminacaoLigada;
}

void usaFlat()
{
    glShadeModel(GL_FLAT);
}

void usaSmooth()
{
    glShadeModel(GL_SMOOTH);
}

// Alterna entre rotacao ativa e pausada.
void alternaAnimacao()
{
    animacaoLigada = !animacaoLigada;
}

void atualizaAnimacao()
{
    // O tempo e medido em milissegundos desde o inicio do GLUT.
    // Assim a velocidade nao depende de quantas vezes a tela e desenhada.
    static int tempoAnterior = 0;
    int tempoAtual = glutGet(GLUT_ELAPSED_TIME);
    int intervalo = tempoAtual - tempoAnterior;
    tempoAnterior = tempoAtual;

    if (animacaoLigada)
    {
        // 0.02 grau por milissegundo = 20 graus por segundo.
        anguloAnimacao += intervalo * 0.02f;

        // Mantem o angulo no intervalo de 0 a 360 graus.
        if (anguloAnimacao >= 360.0f)
            anguloAnimacao -= 360.0f;
    }
}
