#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "scene.h"
#include "camera.h"

// ============================================================
// MUSEU 3D DO SISTEMA SOLAR - CENA
// ============================================================
// Reune as dimensoes da exposicao, os astros, a sala, as placas,
// a iluminacao, os materiais, a interface e as animacoes.
// Usa as primitivas e transformacoes de OpenGL/GLUT estudadas
// nas aulas. As secoes relacionadas a texturas e translacao
// orbital estao identificadas individualmente como feitas com IA.

// PI e usado nas posicoes das orbitas e nos aneis de Saturno.
#define PI 3.14159265f
// Medidas visuais do museu, sem proporcao astronomica real.
#define RAIO_SOL 6.4f
#define RAIO_PLANETA 1.6f
#define RAIO_LUA 0.8f
#define ALTURA_ASTROS 4.5f
#define ALTURA_SOL 6.5f
// Folga entre o Sol e Mercurio e entre orbitas consecutivas.
#define ESPACO_INICIAL 8.0f
#define ESPACO_BASE 2.0f
// A sala mede 130 x 130 unidades no plano XZ.
#define METADE_SALA 65.0f
#define ALTURA_PAREDES 18.0f

#define TOTAL_PLANETAS 8

// Cada planeta possui nome, classificacao, curiosidade, cor e
// propriedades visuais para o material da esfera.
// IA (translacao orbital): o campo velocidadeOrbital determina
// quantos graus por segundo cada planeta percorre na orbita.
// Essas velocidades sao ilustrativas; nao reproduzem o tempo real.
typedef struct
{
    const char *nome;
    const char *tipo;
    const char *informacao;
    float r, g, b;
    float velocidadeOrbital;
    float brilho;
} Planeta;

static const Planeta planetas[TOTAL_PLANETAS] = {
    {"MERCURIO", "Planeta rochoso", "Mais proximo do Sol", 0.55f, 0.53f, 0.50f, 13.0f, 24.0f},
    {"VENUS",    "Planeta rochoso", "Atmosfera muito densa", 0.86f, 0.72f, 0.43f, 10.5f, 32.0f},
    {"TERRA",    "Planeta rochoso", "Possui agua liquida",  0.15f, 0.39f, 0.90f, 8.5f,  65.0f},
    {"MARTE",    "Planeta rochoso", "Conhecido como vermelho",0.80f,0.30f,0.18f, 7.0f,  20.0f},
    {"JUPITER",  "Gigante gasoso",  "Maior planeta do Sistema",0.83f,0.61f,0.42f,5.0f,  35.0f},
    {"SATURNO",  "Gigante gasoso",  "Conhecido pelos aneis",   0.85f,0.75f,0.48f, 4.0f,  30.0f},
    {"URANO",    "Gigante gelado",  "Eixo muito inclinado",   0.47f, 0.82f, 0.85f, 3.0f,  50.0f},
    {"NETUNO",   "Gigante gelado",  "Ventos muito intensos",  0.19f, 0.31f, 0.77f, 2.4f,  60.0f}
};

// ============================================================
// TEXTURAS DOS ASTROS - FEITO COM AUXILIO DE IA
// ============================================================
// IA (texturas): leitura de BMP, envio da imagem a memoria de
// texturas do OpenGL e aplicacao nos astros.
// Tecnicas relacionadas: Aula 13 (glTexImage2D, glTexParameteri
// e coordenadas de mapeamento). O leitor BMP foi adicionado
// para nao exigir outra biblioteca de imagem no programa C.
// Cada BMP tem 24 bits, nao possui compressao e fica em texturas/.

// Indices dos dez mapas: 8 planetas, Sol e Lua.
#define TOTAL_TEXTURAS 10
#define TEX_SOL 8
#define TEX_LUA 9
// O zero indica que uma textura ainda nao foi carregada.
static GLuint texturas[TOTAL_TEXTURAS] = {0};
static GLUquadric *esferaTexturizada = NULL;

static const char *arquivosTexturas[TOTAL_TEXTURAS] = {
    "texturas/mercurio.bmp", "texturas/venus.bmp",
    "texturas/terra.bmp",    "texturas/marte.bmp",
    "texturas/jupiter.bmp", "texturas/saturno.bmp",
    "texturas/urano.bmp",   "texturas/netuno.bmp",
    "texturas/sol.bmp",     "texturas/lua.bmp"
};

// IA (texturas): converte dois bytes do cabecalho BMP para inteiro.
// O formato BMP guarda seus campos em little-endian.
static unsigned int ler16(const unsigned char *p)
{
    return (unsigned int)p[0] | ((unsigned int)p[1] << 8);
}

// IA (texturas): interpreta um campo de 4 bytes do cabecalho.
static unsigned int ler32(const unsigned char *p)
{
    return (unsigned int)p[0] | ((unsigned int)p[1] << 8) |
           ((unsigned int)p[2] << 16) | ((unsigned int)p[3] << 24);
}

// IA (texturas): carrega um arquivo BMP simples para um buffer RGB.
// Primeiro valida cabecalho, tamanho e formato; depois percorre as
// linhas de pixels, convertendo BGR para RGB.
// BMP com altura positiva armazena a primeira linha na parte inferior,
// o que combina com a ordem usada aqui para carregar no OpenGL.
// A funcao retorna NULL quando nao consegue ler o arquivo.
static unsigned char *carregaBMP(const char *arquivo, int *largura, int *altura)
{
    FILE *f = fopen(arquivo, "rb");
    unsigned char cabecalho[54];
    unsigned char *dados = NULL, *linha = NULL;
    unsigned int inicioPixels, w, h, bits, compressao, bytesLinha, x, y;

    if (!f) return NULL;
    if (fread(cabecalho, 1, sizeof(cabecalho), f) != sizeof(cabecalho))
        goto falha;

    inicioPixels = ler32(cabecalho + 10);
    w = ler32(cabecalho + 18);
    h = ler32(cabecalho + 22);
    bits = ler16(cabecalho + 28);
    compressao = ler32(cabecalho + 30);
    if (cabecalho[0] != 'B' || cabecalho[1] != 'M' ||
        ler32(cabecalho + 14) < 40 || bits != 24 || compressao != 0 ||
        w == 0 || h == 0 || w > 4096 || h > 4096)
        goto falha;

    bytesLinha = (w * 3 + 3) & ~3u; // BMP alinha as linhas em 4 bytes.
    dados = (unsigned char *)malloc((size_t)w * h * 3);
    linha = (unsigned char *)malloc(bytesLinha);
    if (!dados || !linha || fseek(f, (long)inicioPixels, SEEK_SET) != 0)
        goto falha;

    for (y = 0; y < h; ++y)
    {
        if (fread(linha, 1, bytesLinha, f) != bytesLinha)
            goto falha;
        for (x = 0; x < w; ++x)
        {
            size_t destino = ((size_t)y * w + x) * 3;
            dados[destino] = linha[3*x + 2];     // R
            dados[destino + 1] = linha[3*x + 1]; // G
            dados[destino + 2] = linha[3*x];     // B
        }
    }
    free(linha);
    fclose(f);
    *largura = (int)w;
    *altura = (int)h;
    return dados;

falha:
    free(dados);
    free(linha);
    fclose(f);
    return NULL;
}

// IA (texturas): transforma o buffer BMP em um identificador OpenGL.
// Se o arquivo nao existir, retorna zero; desenharemos uma esfera lisa.
static GLuint criaTextura(const char *arquivo)
{
    GLuint id;
    int w, h;
    unsigned char *pixels = carregaBMP(arquivo, &w, &h);
    if (!pixels)
    {
        fprintf(stderr, "Aviso: textura nao carregada: %s\n", arquivo);
        return 0;
    }

    // Reserva um identificador, seleciona a textura e envia os pixels.
    // GL_UNPACK_ALIGNMENT=1 evita assumir alinhamento de 4 bytes por linha.
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // GL_REPEAT permite repetir na longitude; GL_CLAMP limita a latitude.
    // GL_LINEAR suaviza os pixels quando a textura e ampliada ou reduzida.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(pixels);
    return id;
}

// IA (texturas): desenha uma esfera usando a imagem correspondente.
// A quadric GLU gera automaticamente as coordenadas (s,t).
// GL_MODULATE combina textura e iluminacao do material.
// Retorna 1 se desenhar a textura; 0 quando a esfera precisa do fallback.
static int desenhaEsferaComTextura(int indice)
{
    if (!esferaTexturizada || texturas[indice] == 0)
        return 0;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturas[indice]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.0f, 1.0f, 1.0f); // Evita tingir a imagem com a cor antiga.
    // IA (texturas): o GLU orienta os polos no eixo Z; o ajuste de
    // -90 graus no eixo X coloca os polos em cima/baixo (eixo Y).
    // Esta correcao tambem foi testada e aprovada no projeto.
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluSphere(esferaTexturizada, 1.0, 48, 32);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D); // Nao aplicar textura em pedestais, paredes ou aneis.
    return 1;
}

// Chaves de interacao e estado compartilhado pela cena.
static int iluminacaoLigada = 1;
static int animacaoLigada = 1;
// IA (translacao orbital): indica se os planetas devem orbitar.
static int translacaoLigada = 0;
static int sombreamentoSuave = 1;
static float anguloRotacao = 0.0f;
// IA (translacao orbital): cada planeta tem angulo de orbita proprio.
static float angulosOrbitais[TOTAL_PLANETAS] = {0};
// IA (translacao orbital): angulo da Lua ao redor da Terra.
static float anguloLua = 0.0f;
// IA (translacao orbital): descida dos pedestais antes da orbita.
static float descidaPedestais = 0.0f;
static int astroSelecionado = -1; // -1: nenhum; 0..7: planetas; 8: Sol.
static int mensagemAproximar = 0;

// IA (translacao orbital): calcula o raio de cada orbita.
// A distancia vai do centro do Sol ao centro do planeta e cresce
// por dois raios de planeta mais o espaco livre entre orbitas.
static float raioOrbita(int ordem)
{
    return RAIO_SOL + ESPACO_INICIAL + RAIO_PLANETA +
           (ordem - 1) * (2.0f * RAIO_PLANETA + ESPACO_BASE);
}

// IA (translacao orbital): encontra a posicao XZ de um planeta.
// cos() e sin() convertem o angulo orbital em ponto no circulo.
// Ordem 1,3,5,7 comeca a direita (+X); 2,4,6,8 a esquerda (-X).
static void posicaoPlaneta(int indice, float *x, float *z)
{
    float inicio = (indice % 2 == 0) ? 0.0f : 180.0f;
    float angulo = (inicio + angulosOrbitais[indice]) * PI / 180.0f;
    float distancia = raioOrbita(indice + 1);
    *x = distancia * cosf(angulo);
    *z = distancia * sinf(angulo);
}

// IA (translacao orbital): a Lua orbita a Terra, nao o Sol.
// Primeiro localiza a Terra; depois soma o deslocamento lunar.
static void posicaoLua(float *x, float *z)
{
    float tx, tz;
    float distancia = RAIO_PLANETA + RAIO_LUA + 0.8f;
    posicaoPlaneta(2, &tx, &tz);
    *x = tx + distancia * cosf(anguloLua * PI / 180.0f);
    *z = tz + distancia * sinf(anguloLua * PI / 180.0f);
}

// ============================================================
// ILUMINACAO E MATERIAIS
// ============================================================
// As aulas 09 e 9b apresentam componentes ambiente, difusa e
// especular. Usamos uma fonte pontual no Sol e outra na sala.
// Os materiais abaixo alteram a intensidade e a concentracao
// do brilho refletido sobre cada tipo de objeto.

// Prepara as duas luzes da cena e suas propriedades.
// A posicao usa w=1, indicando fonte pontual (e nao direcional).
void configuraIluminacao(void)
{
    GLfloat luzAmbienteGeral[] = {0.25f, 0.25f, 0.30f, 1.0f};
    GLfloat ambienteSol[] = {0.08f, 0.06f, 0.03f, 1.0f};
    GLfloat difusaSol[] = {1.00f, 0.85f, 0.65f, 1.0f};
    GLfloat especularSol[] = {1.00f, 0.92f, 0.75f, 1.0f};
    GLfloat posicaoSol[] = {0.0f, ALTURA_SOL, 0.0f, 1.0f};
    GLfloat ambienteSala[] = {0.04f, 0.06f, 0.09f, 1.0f};
    GLfloat difusaSala[] = {0.48f, 0.58f, 0.78f, 1.0f};
    GLfloat especularSala[] = {0.35f, 0.45f, 0.65f, 1.0f};
    GLfloat posicaoSala[] = {0.0f, 16.0f, 34.0f, 1.0f};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbienteGeral);

    // LIGHT0: fonte pontual exatamente no centro do Sol.
    // Essa luz ilumina os outros objetos; GL_EMISSION faz o
    // proprio Sol parecer brilhante, mas nao ilumina a cena sozinho.
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambienteSol);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusaSol);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especularSol);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoSol);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.006f);

    // LIGHT1: iluminacao mais fria, voltada para a exposicao.
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambienteSala);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, difusaSala);
    glLightfv(GL_LIGHT1, GL_SPECULAR, especularSala);
    glLightfv(GL_LIGHT1, GL_POSITION, posicaoSala);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.004f);
}

// Ajusta o reflexo especular e o brilho de um material.
// SHININESS maior concentra o reflexo em uma area menor.
static void material(float brilho, float reflexo)
{
    GLfloat especular[] = {reflexo, reflexo, reflexo, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, brilho);
}

// ============================================================
// SALA DO MUSEU
// ============================================================
// Piso, paredes e teto sao quadrilateros com vertices e normais.
// As normais informam ao OpenGL a direcao da superficie para
// que a iluminacao produza os resultados esperados.

// Desenha o piso horizontal em Y=0, cobrindo os eixos X e Z.
void desenhaPiso(void)
{
    material(8.0f, 0.09f);
    glColor3f(0.22f, 0.25f, 0.31f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, 0.0f,  METADE_SALA);
        glVertex3f(-METADE_SALA, 0.0f,  METADE_SALA);
    glEnd();
}

// Fecha a sala no plano Z negativo.
void desenhaParedeFundo(void)
{
    material(8.0f, 0.05f);
    glColor3f(0.13f, 0.16f, 0.23f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
    glEnd();
}

// Fecha a lateral da sala no plano X negativo.
void desenhaParedeEsquerda(void)
{
    material(8.0f, 0.05f);
    glColor3f(0.15f, 0.18f, 0.26f);
    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f(-METADE_SALA, 0.0f,  METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
    glEnd();
}

// Fecha a lateral da sala no plano X positivo.
void desenhaParedeDireita(void)
{
    material(8.0f, 0.05f);
    glColor3f(0.15f, 0.18f, 0.26f);
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(METADE_SALA, 0.0f,  METADE_SALA);
        glVertex3f(METADE_SALA, 0.0f, -METADE_SALA);
        glVertex3f(METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
        glVertex3f(METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
    glEnd();
}

// Fecha a entrada no plano Z positivo.
void desenhaParedeFrente(void)
{
    material(8.0f, 0.05f);
    glColor3f(0.13f, 0.16f, 0.23f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f( METADE_SALA, 0.0f, METADE_SALA);
        glVertex3f(-METADE_SALA, 0.0f, METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES, METADE_SALA);
    glEnd();
}

// Desenha a cobertura em Y=ALTURA_PAREDES.
void desenhaTeto(void)
{
    material(8.0f, 0.05f);
    glColor3f(0.14f, 0.16f, 0.23f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES,  METADE_SALA);
        glVertex3f( METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
        glVertex3f(-METADE_SALA, ALTURA_PAREDES, -METADE_SALA);
    glEnd();
}

// ============================================================
// ESFERAS E ANEIS
// ============================================================
// A pilha de matrizes isola as transformacoes de cada astro:
// posicionar (Translate), girar (Rotate) e ajustar raio (Scale).
// O desenho usa texturas quando disponiveis e cores como reserva.

// Desenha o Sol no centro da sala, em uma altura acima do piso.
// A emissao da superficie da aparencia luminosa ao objeto.
// A fonte GL_LIGHT0 e responsavel por iluminar os demais astros.
void desenhaSol(void)
{
    GLfloat emissao[] = {0.70f, 0.36f, 0.04f, 1.0f};
    GLfloat semEmissao[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glPushMatrix();
        glTranslatef(0.0f, ALTURA_SOL, 0.0f);
        glScalef(RAIO_SOL, RAIO_SOL, RAIO_SOL);
        material(18.0f, 0.12f);
        glColor3f(1.0f, 0.71f, 0.10f);
        // A emissao faz o Sol parecer luminoso; GL_LIGHT0 e a luz real.
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissao);
        if (!desenhaEsferaComTextura(TEX_SOL))
            glutSolidSphere(1.0, 36, 28); // Mantem a cor se faltar a imagem.
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, semEmissao);
    glPopMatrix();
}

// Forma o anel de Saturno com duas circunferencias ligadas por
// GL_QUAD_STRIP. Os 80 segmentos aproximam um circulo suave.
static void desenhaAneisSaturno(void)
{
    int i;
    const int segmentos = 80;
    glColor3f(0.79f, 0.72f, 0.56f);
    material(35.0f, 0.22f);
    glPushMatrix();
        glRotatef(24.0f, 1.0f, 0.0f, 0.0f);
        glBegin(GL_QUAD_STRIP);
        glNormal3f(0.0f, 1.0f, 0.0f);
        for (i = 0; i <= segmentos; i++)
        {
            float a = 2.0f * PI * i / segmentos;
            float c = cosf(a), s = sinf(a);
            glVertex3f(1.28f*c, 0.0f, 1.28f*s);
            glVertex3f(1.86f*c, 0.0f, 1.86f*s);
        }
        glEnd();
    glPopMatrix();
}

// Desenha o planeta indicado pelo indice na posicao XZ recebida.
// O angulo de rotacao proprio e aplicado antes de desenhar a esfera.
// IA (texturas): usa a textura do astro, mantendo a cor como reserva.
// IA (translacao orbital): recebe XZ calculados pela posicao orbital.
static void desenhaEsferaPlaneta(int indice, float x, float z)
{
    const Planeta *p = &planetas[indice];
    glPushMatrix();
        glTranslatef(x, ALTURA_ASTROS, z);
        glRotatef(anguloRotacao * (0.72f + indice * 0.045f), 0.0f, 1.0f, 0.0f);
        glScalef(RAIO_PLANETA, RAIO_PLANETA, RAIO_PLANETA);
        material(p->brilho, 0.42f);
        glColor3f(p->r, p->g, p->b);
        // A textura substitui a esfera de cor unica. A marca anterior
        // so aparece como reserva, caso o arquivo BMP esteja ausente.
        if (!desenhaEsferaComTextura(indice))
        {
            glutSolidSphere(1.0, 24, 16);
            glPushMatrix();
                glTranslatef(0.18f, 0.20f, 0.96f);
                glScalef(0.20f, 0.25f, 0.045f);
                material(10.0f, 0.09f);
                glColor3f(p->r * 0.55f, p->g * 0.55f, p->b * 0.55f);
                glutSolidSphere(1.0, 12, 8);
            glPopMatrix();
        }

        if (indice == 5)
            desenhaAneisSaturno();
    glPopMatrix();
}

// Desenha a Lua com metade do raio dos planetas.
// IA (translacao orbital): sua posicao acompanha o movimento da Terra.
// IA (texturas): aplica lua.bmp quando a imagem estiver disponivel.
static void desenhaLua(void)
{
    float x, z;
    posicaoLua(&x, &z);
    glPushMatrix();
        glTranslatef(x, ALTURA_ASTROS, z);
        glRotatef(anguloRotacao * 0.4f, 0.0f, 1.0f, 0.0f);
        glScalef(RAIO_LUA, RAIO_LUA, RAIO_LUA);
        material(6.0f, 0.12f);
        glColor3f(0.65f, 0.67f, 0.71f);
        if (!desenhaEsferaComTextura(TEX_LUA))
            glutSolidSphere(1.0, 22, 16);
    glPopMatrix();
}

// ============================================================
// PEDESTAIS E PLACAS
// ============================================================
// Os suportes sao blocos simples de GLUT, com uma placa inclinada.
// As letras sao desenhadas com glutStrokeCharacter() no espaco 3D:
// por isso acompanham a placa, e nao a orientacao da camera.
// IA (translacao orbital): na tecla T, os pedestais sao deslocados
// para baixo do piso, sem modificar a altura dos planetas.
// Escreve uma linha na superficie frontal da placa.
// Mede os caracteres, reduz a escala se necessario e centraliza.
static void escreveNaPlaca(const char *texto, float y, float escalaBase)
{
    const unsigned char *letra;
    float larguraTexto = 0.0f;
    float escala = escalaBase;

    // Calcula a largura do texto no sistema local da fonte.
    for (letra = (const unsigned char *)texto; *letra; letra++)
        larguraTexto += (float)glutStrokeWidth(GLUT_STROKE_ROMAN, *letra);

    // Impede que frases longas saiam do retangulo da placa.
    if (larguraTexto > 0.0f && larguraTexto * escala > 4.0f)
        escala = 4.0f / larguraTexto;

    glPushMatrix();
        // Centraliza o texto na horizontal e coloca na face frontal.
        // O tamanho e medido em unidades 3D, nao em pixels.
        glTranslatef(-0.5f * larguraTexto * escala, y, 0.170f);
        glScalef(escala, escala, escala);
        for (letra = (const unsigned char *)texto; *letra; letra++)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *letra);
    glPopMatrix();
}

// Desenha uma placa com moldura, centro escuro e duas linhas de texto.
// As transformacoes do texto ficam dentro das da propria placa.
static void desenhaPlaca(float x, float y, float z,
                         const char *titulo, const char *linha)
{
    glPushMatrix();
        glTranslatef(x, y, z);

        // Inclinacao FIXA em relacao ao pedestal.
        // Tanto a placa quanto suas letras recebem a mesma rotacao.
        // A placa nunca tenta acompanhar o movimento da camera.
        glRotatef(-12.0f, 1.0f, 0.0f, 0.0f);

        material(18.0f, 0.20f);
        glColor3f(0.72f, 0.60f, 0.33f);
        glPushMatrix();
            glScalef(4.50f, 1.85f, 0.22f);
            glutSolidCube(1.0);
        glPopMatrix();

        // Retangulo escuro sobre a face frontal da placa.
        glColor3f(0.07f, 0.11f, 0.19f);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.125f);
            glScalef(4.26f, 1.60f, 0.05f);
            glutSolidCube(1.0);
        glPopMatrix();

        // O texto e desenhado em 3D, imediatamente a frente
        // do retangulo. O teste de profundidade continua ativo.
        // Desligamos a iluminacao somente durante o desenho das letras.
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(2.0f);
        glColor3f(1.0f, 0.88f, 0.54f);
        escreveNaPlaca(titulo, 0.20f, 0.0050f);
        glColor3f(0.91f, 0.95f, 1.0f);
        escreveNaPlaca(linha, -0.44f, 0.0036f);
        glPopAttrib();
    glPopMatrix();
}

// Desenha o pedestal fixo do planeta na posicao inicial da exposicao.
// IA (translacao orbital): descidaPedestais desloca todo o suporte.
static void desenhaPedestal(int indice)
{
    float x = (indice % 2 == 0) ? raioOrbita(indice + 1) : -raioOrbita(indice + 1);
    const Planeta *p = &planetas[indice];
    glPushMatrix();
        glTranslatef(x, -descidaPedestais, 0.0f);
        material(18.0f, 0.13f);
        glColor3f(0.31f, 0.36f, 0.43f);
        glPushMatrix();
            glTranslatef(0.0f, 1.30f, 0.0f);
            glScalef(3.15f, 2.60f, 3.15f);
            glutSolidCube(1.0);
        glPopMatrix();
        glColor3f(0.55f, 0.59f, 0.65f);
        glPushMatrix();
            glTranslatef(0.0f, 2.69f, 0.0f);
            glScalef(3.38f, 0.18f, 3.38f);
            glutSolidCube(1.0);
        glPopMatrix();
        desenhaPlaca(0.0f, 1.62f, 2.25f, p->nome, p->informacao);
    glPopMatrix();
}

// Coloca uma placa de identificacao em frente ao Sol.
// IA (translacao orbital): oculta a placa quando a animacao orbital
// esta ativa para nao atrapalhar a visualizacao.
static void desenhaPlacaSol(void)
{
    // Painel central afastado da superficie do Sol.
    if (translacaoLigada)
        return;
    desenhaPlaca(0.0f, 1.55f - descidaPedestais, 9.0f,
                 "SOL", "Estrela do Sistema Solar");
}

// IA (translacao orbital): desenha as trajetorias circulares no piso.
// As linhas sao apenas guias visuais, nao sao objetos fisicos.
static void desenhaOrbitas(void)
{
    int i, passo;
    glDisable(GL_LIGHTING);
    glLineWidth(1.5f);
    for (i = 0; i < TOTAL_PLANETAS; i++)
    {
        float r = raioOrbita(i + 1);
        glColor3f(0.31f, 0.39f, 0.53f);
        glBegin(GL_LINE_LOOP);
        for (passo = 0; passo < 140; passo++)
        {
            float a = (2.0f * PI * passo) / 140.0f;
            glVertex3f(r * cosf(a), 0.07f, r * sinf(a));
        }
        glEnd();
    }
    if (iluminacaoLigada)
        glEnable(GL_LIGHTING);
}

// Reune todos os elementos da exposicao: Sol, suportes, planetas, Lua.
// IA (translacao orbital): decide quando exibir suportes e orbitas
// e obtem as posicoes atualizadas dos planetas.
void desenhaObjetosDoMuseu(void)
{
    int i;
    desenhaSol();
    if (translacaoLigada || cameraEstaEmVistaSuperior())
        desenhaOrbitas();

    // Pedestais permanecem nas posicoes iniciais, ate sumirem no piso.
    // Enquanto transladam, os planetas seguem as orbitas circulares.
    if (descidaPedestais < 3.2f && !cameraEstaEmVistaSuperior())
    {
        for (i = 0; i < TOTAL_PLANETAS; i++)
            desenhaPedestal(i);
        desenhaPlacaSol();
    }

    for (i = 0; i < TOTAL_PLANETAS; i++)
    {
        float x, z;
        posicaoPlaneta(i, &x, &z);
        desenhaEsferaPlaneta(i, x, z);
    }
    desenhaLua();
}

// ============================================================
// CENA PRINCIPAL
// ============================================================
// A cena adapta o ambiente ao ponto de vista selecionado.
// Em perspectiva ha sala completa; na ortografica, paredes
// ficam ocultas e piso/teto se afastam; na vista superior,
// a camera enxerga o Sistema Solar sem cobertura.

// Prepara o estado de iluminacao, desenha o ambiente e os astros.
void desenhaCena(void)
{
    glMatrixMode(GL_MODELVIEW);
    if (iluminacaoLigada)
        configuraIluminacao();
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }

    if (cameraEstaEmVistaSuperior())
    {
        // I: mostra o piso, mas nao teto e paredes.
        desenhaPiso();
    }
    else if (cameraEstaEmPerspectiva())
    {
        // P: sala fechada para a visita em primeira pessoa.
        desenhaPiso();
        desenhaParedeFundo();
        desenhaParedeEsquerda();
        desenhaParedeDireita();
        desenhaParedeFrente();
        desenhaTeto();
    }
    else
    {
        // O: os limites do ambiente saem do caminho da camera.
        // A geometria dos planetas nao e alterada.
        glPushMatrix();
            glTranslatef(0.0f, -50.0f, 0.0f);
            desenhaPiso();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.0f, 50.0f - ALTURA_PAREDES, 0.0f);
            desenhaTeto();
        glPopMatrix();
    }

    desenhaObjetosDoMuseu();
}

// ============================================================
// INTERACAO - TECLA E / INFORMACOES
// ============================================================

// Verifica qual astro esta a uma distancia de ate 5 unidades
// da superficie, usando a posicao atual do visitante.
// A tecla E alterna entre exibir e esconder o painel de informacoes.
void interageComAstro(void)
{
    int i, escolhido = -1;
    float px, py, pz, menorDistancia = 9999.0f;
    obtemPosicaoCamera(&px, &py, &pz);

    // O Sol possui raio muito maior do que os planetas.
    {
        float dx = px, dy = py - ALTURA_SOL, dz = pz;
        float distancia = sqrtf(dx*dx + dy*dy + dz*dz) - RAIO_SOL;
        if (distancia < 5.0f)
        {
            menorDistancia = distancia;
            escolhido = 8;
        }
    }
    for (i = 0; i < TOTAL_PLANETAS; i++)
    {
        float x, z, dx, dy, dz, distancia;
        posicaoPlaneta(i, &x, &z);
        dx = px - x; dy = py - ALTURA_ASTROS; dz = pz - z;
        distancia = sqrtf(dx*dx + dy*dy + dz*dz) - RAIO_PLANETA;
        if (distancia < 5.0f && distancia < menorDistancia)
        {
            menorDistancia = distancia;
            escolhido = i;
        }
    }
    if (escolhido == -1)
    {
        astroSelecionado = -1;
        mensagemAproximar = 1;
    }
    else
    {
        astroSelecionado = (astroSelecionado == escolhido) ? -1 : escolhido;
        mensagemAproximar = 0;
    }
}

// Desenha uma string de interface em coordenadas de tela.
// Este texto 2D e diferente das letras 3D das placas fisicas.
static void textoTela(float x, float y, const char *s)
{
    const char *c;
    glRasterPos2f(x, y);
    for (c = s; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

// Mostra controles, estado das luzes, sombreamento e informacoes.
// Isola matrizes de projecao/modelview para nao alterar a cena 3D.
void desenhaInterface(int largura, int altura)
{
    // A interface 2D nao deve depender da luz, da camera ou do Z-buffer.
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, largura, 0, altura, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.96f, 0.91f, 0.70f);
    textoTela(16.0f, altura - 27.0f,
              cameraEstaEmVistaSuperior() ? "VISTA SUPERIOR [I]" :
              (cameraEstaEmPerspectiva() ? "MUSEU - PERSPECTIVA [P]" : "MUSEU - ORTOGRAFICA [O]"));
    glColor3f(0.85f, 0.90f, 1.0f);
    textoTela(16.0f, altura - 53.0f,
              translacaoLigada ? "T: PARAR ORBITAS / PEDESTAIS OCULTOS" :
                                "T: INICIAR ORBITAS / PEDESTAIS EXPOSTOS");
    textoTela(16.0f, altura - 79.0f,
              animacaoLigada ? "ESPACO: PAUSAR ANIMACAO" : "ESPACO: CONTINUAR ANIMACAO");
    glColor3f(0.98f, 0.91f, 0.63f);
    textoTela(largura - 300.0f, altura - 27.0f,
              iluminacaoLigada ? "ILUMINACAO: LIGADA [L]" : "ILUMINACAO: DESLIGADA [L]");
    textoTela(largura - 300.0f, altura - 53.0f,
              sombreamentoSuave ? "SOMBREAMENTO: SMOOTH [G]" : "SOMBREAMENTO: FLAT [F]");
    glColor3f(0.85f, 0.90f, 1.0f);
    textoTela(16.0f, 21.0f,
              "WASD mover | Mouse olhar | E informacoes | I topo | F/G sombra | L luz | F11 tela cheia");

    if (astroSelecionado >= 0)
    {
        const char *nome = (astroSelecionado == 8) ? "SOL" : planetas[astroSelecionado].nome;
        const char *tipo = (astroSelecionado == 8) ? "Estrela central" : planetas[astroSelecionado].tipo;
        const char *fato = (astroSelecionado == 8) ? "Fonte principal de energia do Sistema Solar" :
                                              planetas[astroSelecionado].informacao;
        glColor3f(1.0f, 0.82f, 0.41f);
        textoTela(16.0f, altura - 122.0f, nome);
        glColor3f(0.96f, 0.96f, 0.98f);
        textoTela(16.0f, altura - 147.0f, tipo);
        textoTela(16.0f, altura - 172.0f, fato);
        textoTela(16.0f, altura - 196.0f, "Pressione E novamente para fechar");
    }
    else if (mensagemAproximar)
    {
        glColor3f(1.0f, 0.82f, 0.41f);
        textoTela(16.0f, altura - 122.0f, "Aproxime-se de um astro e pressione E.");
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

// ============================================================
// CONTROLES E ANIMACAO
// ============================================================
// Inicializacao de materiais, controle de luz e sombreamento.
// IA (translacao orbital): velocidades independentes, calculo por
// tempo real, recolhimento dos pedestais e retorno a exposicao.
// IA (texturas): criacao das quadricas e leitura dos arquivos BMP.

// Configura o sombreado inicial e os materiais, depois cria as
// texturas uma unica vez, quando ja existe contexto OpenGL.
void inicializaCena(void)
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
    material(30.0f, 0.28f);

    // Carrega as imagens uma unica vez, depois que a janela OpenGL existe.
    // Se faltar alguma, a esfera correspondente continua com a cor antiga.
    esferaTexturizada = gluNewQuadric();
    if (esferaTexturizada)
    {
        gluQuadricTexture(esferaTexturizada, GL_TRUE);
        gluQuadricNormals(esferaTexturizada, GLU_SMOOTH);
    }
    for (int i = 0; i < TOTAL_TEXTURAS; i++)
        texturas[i] = criaTextura(arquivosTexturas[i]);
}

// Alterna GL_LIGHTING a cada pressionamento da tecla L.
void alternaIluminacao(void)
{
    iluminacaoLigada = !iluminacaoLigada;
}

// Flat Shading: usa um valor de sombreamento por face.
void usaFlat(void)
{
    sombreamentoSuave = 0;
    glShadeModel(GL_FLAT);
}

// Smooth Shading: interpola o sombreamento entre vertices.
void usaSmooth(void)
{
    sombreamentoSuave = 1;
    glShadeModel(GL_SMOOTH);
}

// Pausa ou retoma os movimentos controlados pelo timer.
void alternaAnimacao(void)
{
    animacaoLigada = !animacaoLigada;
}

// IA (translacao orbital): T ativa/desativa as orbitas e os suportes.
// Ao desligar, zera os angulos para recolocar os planetas nas
// posicoes da exposicao, deixando os pedestais subir novamente.
void alternaTranslacao(void)
{
    int i;
    translacaoLigada = !translacaoLigada;
    // Ao desligar, retornamos ao alinhamento original dos pedestais.
    if (!translacaoLigada)
    {
        for (i = 0; i < TOTAL_PLANETAS; i++)
            angulosOrbitais[i] = 0.0f;
        anguloLua = 0.0f;
    }
    astroSelecionado = -1;
    mensagemAproximar = 0;
}

// IA (translacao orbital): atualiza angulos de planetas e da Lua.
// O delta de tempo garante velocidades por segundo e nao por quadro.
// Tambem atualiza suavemente a altura dos pedestais.
// O giro proprio dos astros continua com ou sem translacao.
void atualizaAnimacao(void)
{
    int i;
    static int tempoAnterior = 0;
    int agora = glutGet(GLUT_ELAPSED_TIME);
    float dt = (agora - tempoAnterior) / 1000.0f;
    tempoAnterior = agora;
    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.05f) dt = 0.05f;

    // A movimentacao dos pedestais termina mesmo que a animacao
    // esteja pausada, para nao deixar suportes no meio da sala.
    if (translacaoLigada && descidaPedestais < 4.0f)
    {
        descidaPedestais += 4.0f * dt;
        if (descidaPedestais > 4.0f) descidaPedestais = 4.0f;
    }
    else if (!translacaoLigada && descidaPedestais > 0.0f)
    {
        descidaPedestais -= 4.0f * dt;
        if (descidaPedestais < 0.0f) descidaPedestais = 0.0f;
    }

    if (!animacaoLigada) return;

    anguloRotacao = fmodf(anguloRotacao + 20.0f * dt, 360.0f);
    if (translacaoLigada && descidaPedestais >= 4.0f)
    {
        for (i = 0; i < TOTAL_PLANETAS; i++)
            angulosOrbitais[i] = fmodf(angulosOrbitais[i] + planetas[i].velocidadeOrbital * dt, 360.0f);
        anguloLua = fmodf(anguloLua + 45.0f * dt, 360.0f);
    }
}
