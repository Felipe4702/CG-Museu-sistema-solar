#ifndef CAMERA_H
#define CAMERA_H

// IA (camera): reinicia a posicao e a orientacao do visitante.
void inicializaCamera(void);

// IA (camera): aplica posicao e direcao na matriz de visualizacao.
void aplicaCamera(void);

// IA (camera): escolhe perspectiva, ortografica ou visao de cima.
void configuraProjecao(int largura, int altura);
void usaPerspectiva(void);
void usaOrtografica(void);

// IA (camera): I alterna a vista superior sem perder a vista anterior.
void alternaVistaSuperior(void);
int cameraEstaEmVistaSuperior(void);
void obtemPosicaoCamera(float *x, float *y, float *z);

// IA (movimentacao): caminha/voa conforme as teclas pressionadas.
// deltaTempo e medido em segundos.
void atualizaMovimentoCamera(float frente, float lado,
                            float vertical, float deltaTempo);

// IA (movimentacao): V alterna caminhada e voo vertical.
void alternaModoVoo(void);

// IA (camera): altera os angulos horizontal e vertical do olhar.
void giraCamera(float horizontal, float vertical);

// Informa se a camera esta usando a projecao perspectiva.
int cameraEstaEmPerspectiva(void);

#endif
