#ifndef INTERFACE_H
#define INTERFACE_H

#include "framebuffer.h"
#include "editor.h"

// Dimensões da janela: canvas embaixo, barra de ferramentas em cima
const int LARGURA_CANVAS = 800;
const int ALTURA_CANVAS = 600;
const int ALTURA_BARRA = 56;

const Color COR_FUNDO = {255, 255, 255};  // fundo do canvas (usado pela borracha e pelo "Novo")
const Color COR_BARRA = {222, 222, 222};

const char* const ARQUIVO_SAIDA = "desenho.ppm";

// Paleta com as 8 cores pedidas no enunciado
const int NUM_CORES = 8;
extern const Color PALETA[NUM_CORES];
extern const char* const NOMES_CORES[NUM_CORES];

// Índice da cor na paleta (-1 se não estiver nela)
int indice_da_cor(Color c);

// O que existe em cada posição da barra
enum class TipoElemento { NENHUM, FERRAMENTA, ESPESSURA, COR, NOVO, SALVAR };

struct ElementoBarra {
    TipoElemento tipo;
    int indice;  // qual ferramenta/espessura/cor (para os outros tipos vale 0)
};

// Desenha a barra (botões, ícones, paleta) usando as primitivas de rasterizacao.h
void desenhar_barra(Framebuffer& barra, const Editor& editor);

// Descobre em qual elemento da barra está o ponto (x, y), em coordenadas da barra
ElementoBarra elemento_em(int x, int y);

#endif
