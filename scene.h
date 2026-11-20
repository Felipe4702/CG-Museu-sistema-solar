#ifndef SCENE_H
#define SCENE_H

// Prepara materiais, sombreamento e imagens utilizadas na exposicao.
// IA (texturas): inicializa a leitura e a aplicacao das imagens BMP.
void inicializaCena(void);
// Desenha o ambiente e todos os astros a cada quadro.
void desenhaCena(void);
// Mostra os controles e informacoes em texto 2D na tela.
void desenhaInterface(int largura, int altura);

// Controles conforme o enunciado: L, F, G e espaco.
void alternaIluminacao(void);
void usaFlat(void);
void usaSmooth(void);
void alternaAnimacao(void);
// IA (translacao orbital): avanca orbitas/descida dos pedestais.
void atualizaAnimacao(void);

// IA (translacao orbital): inicia/encerra o movimento das orbitas.
// Interacao por proximidade: E abre dados do astro mais proximo.
void alternaTranslacao(void);
void interageComAstro(void);

#endif
