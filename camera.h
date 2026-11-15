#ifndef CAMERA_H
#define CAMERA_H

// Posicao e orientacao iniciais da camera.
void inicializaCamera(void);

// Define o ponto de vista por meio de gluLookAt().
void aplicaCamera(void);

// Configura a projecao e permite alternar entre os dois modos.
void configuraProjecao(int largura, int altura);
void usaPerspectiva(void);
void usaOrtografica(void);

// Atualiza o deslocamento a partir das teclas que estao pressionadas.
// deltaTempo e medido em segundos.
void atualizaMovimentoCamera(float frente, float lado,
                            float vertical, float deltaTempo);

// V alterna entre caminhada no piso e voo livre na vertical.
void alternaModoVoo(void);

// Altera os angulos horizontal e vertical do olhar.
void giraCamera(float horizontal, float vertical);

// Informa se a camera esta usando a projecao perspectiva.
int cameraEstaEmPerspectiva(void);

#endif
