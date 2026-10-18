#ifndef RASTERIZACAO_H
#define RASTERIZACAO_H

#include "framebuffer.h"

// Reta (Bresenham)
void draw_line(int x0, int y0, int x1, int y1, Color cor);

// Retângulos
void draw_rect(int x, int y, int width, int height, Color cor);
void draw_rect_filled(int x, int y, int width, int height, Color cor);

// Círculos (Ponto Médio)
void draw_circle(int xc, int yc, int radius, Color cor);
void draw_circle_filled(int xc, int yc, int radius, Color cor);

// Preenchimento (Flood Fill com pilha explícita)
void flood_fill(int x, int y, Color fill_color);

#endif // RASTERIZACAO_H