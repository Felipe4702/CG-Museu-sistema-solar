#include <GL/glut.h>
#include "scene.h"

// ------------------------------------------------------------
// ESTADO DA CENA
// ------------------------------------------------------------

// Controla se a iluminacao e a animacao estao ativadas.
static int iluminacaoLigada = 1;
static int animacaoLigada = 1;

// Angulo usado para fazer os planetas girarem sobre seus eixos.
static float anguloAnimacao = 0.0f;

// ------------------------------------------------------------
// ILUMINACAO BASICA (APROVEITADA DO CODIGO DO PROFESSOR)
// ------------------------------------------------------------

void configuraIluminacao()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambiente[]  = {0.20f, 0.20f, 0.20f, 1.0f};
    GLfloat difusa[]    = {0.80f, 0.80f, 0.80f, 1.0f};
    GLfloat especular[] = {1.00f, 1.00f, 1.00f, 1.0f};
    GLfloat posicao[]   = {0.0f, 8.0f, 5.0f, 1.0f};

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

// O museu tem um piso quadrado e tres paredes.
// A frente fica aberta para facilitar a visualizacao inicial.
// As paredes foram afastadas para acomodar todos os planetas.

void desenhaPiso()
{
    glColor3f(0.30f, 0.30f, 0.35f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-22.0f, 0.0f, -22.0f);
        glVertex3f( 22.0f, 0.0f, -22.0f);
        glVertex3f( 22.0f, 0.0f,  22.0f);
        glVertex3f(-22.0f, 0.0f,  22.0f);
    glEnd();
}

void desenhaParedeFundo()
{
    glColor3f(0.15f, 0.18f, 0.25f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-22.0f, 0.0f, -22.0f);
        glVertex3f( 22.0f, 0.0f, -22.0f);
        glVertex3f( 22.0f, 12.0f, -22.0f);
        glVertex3f(-22.0f, 12.0f, -22.0f);
    glEnd();
}

void desenhaParedeEsquerda()
{
    glColor3f(0.18f, 0.20f, 0.28f);

    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-22.0f, 0.0f, -22.0f);
        glVertex3f(-22.0f, 0.0f,  22.0f);
        glVertex3f(-22.0f, 12.0f,  22.0f);
        glVertex3f(-22.0f, 12.0f, -22.0f);
    glEnd();
}

void desenhaParedeDireita()
{
    glColor3f(0.18f, 0.20f, 0.28f);

    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(22.0f, 0.0f,  22.0f);
        glVertex3f(22.0f, 0.0f, -22.0f);
        glVertex3f(22.0f, 12.0f, -22.0f);
        glVertex3f(22.0f, 12.0f,  22.0f);
    glEnd();
}

// ------------------------------------------------------------
// OBJETOS DO SISTEMA SOLAR
// ------------------------------------------------------------

void desenhaSol()
{
    glPushMatrix();

        // Posiciona o Sol acima do centro do piso.
        glTranslatef(0.0f, 2.7f, 0.0f);
        glScalef(1.6f, 1.6f, 1.6f);

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
        glTranslatef(x, 2.7f, z);

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

void desenhaObjetosDoMuseu()
{
    // Centro do Sistema Solar.
    desenhaSol();

    // Planetas em ordem crescente de distancia ao Sol.
    // Os valores sao adaptados ao tamanho da sala.
    // Nao representam distancias ou diametros astronomicos reais.

    // Mercurio: distancia aproximada ao centro = 3.6 unidades.
    desenhaPlaneta(  3.5f, -1.0f, 0.27f, 0.55f, 0.53f, 0.50f);

    // Venus: distancia aproximada ao centro = 5.8 unidades.
    desenhaPlaneta(  5.0f,  3.0f, 0.45f, 0.86f, 0.72f, 0.43f);

    // Terra: distancia aproximada ao centro = 7.6 unidades.
    desenhaPlaneta(  1.0f,  7.5f, 0.51f, 0.15f, 0.39f, 0.90f);

    // Lua: fica proxima da Terra, mas nao orbita nesta etapa.
    desenhaPlaneta(  2.2f,  7.7f, 0.17f, 0.66f, 0.66f, 0.66f);

    // Marte: distancia aproximada ao centro = 8.8 unidades.
    desenhaPlaneta( -7.2f,  5.0f, 0.36f, 0.80f, 0.30f, 0.18f);

    // Jupiter: distancia aproximada ao centro = 11.0 unidades.
    desenhaPlaneta(-11.0f, -1.0f, 1.12f, 0.83f, 0.61f, 0.42f);

    // Saturno: distancia aproximada ao centro = 13.0 unidades.
    // Os aneis poderao ser adicionados em outra etapa.
    desenhaPlaneta( -7.0f,-11.0f, 0.95f, 0.85f, 0.75f, 0.48f);

    // Urano: distancia aproximada ao centro = 15.3 unidades.
    desenhaPlaneta(  8.0f,-13.0f, 0.69f, 0.47f, 0.82f, 0.85f);

    // Netuno: distancia aproximada ao centro = 17.1 unidades.
    desenhaPlaneta( 17.0f,  2.0f, 0.68f, 0.19f, 0.31f, 0.77f);
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

    desenhaPiso();
    desenhaParedeFundo();
    desenhaParedeEsquerda();
    desenhaParedeDireita();

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
