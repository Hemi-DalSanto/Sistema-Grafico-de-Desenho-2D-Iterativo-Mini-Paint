#ifndef FERRAMENTAS_H
#define FERRAMENTAS_H

#include "framebuffer.h"

// Ferramentas de desenho (a ordem é a mesma dos botões da barra)
enum class Ferramenta {
    LAPIS,
    BORRACHA,
    LINHA,
    RETANGULO,
    RETANGULO_CHEIO,
    CIRCULO,
    CIRCULO_CHEIO,
    BALDE
};
const int NUM_FERRAMENTAS = 8;

// Espessuras do pincel/ferramentas
enum class Espessura { FINO, MEDIO, GROSSO };
const int NUM_ESPESSURAS = 3;

// Raio da ponta do pincel para a ferramenta e espessura escolhidas
// (raio 0 = 1 pixel; a borracha é sempre maior que o lápis)
int raio_do_pincel(Ferramenta f, Espessura e);

const char* nome_ferramenta(Ferramenta f);
const char* nome_espessura(Espessura e);

// Desenho com espessura: repete a primitiva 1px de rasterizacao.h
// em cada ponto de um disco de raio "raio" (a ponta do pincel)
void desenhar_linha_espessa(Framebuffer& fb, int x0, int y0, int x1, int y1, Color cor, int raio);
void desenhar_retangulo_espesso(Framebuffer& fb, int x, int y, int width, int height, Color cor, int raio);
void desenhar_circulo_espesso(Framebuffer& fb, int xc, int yc, int radius, Color cor, int raio);

#endif
