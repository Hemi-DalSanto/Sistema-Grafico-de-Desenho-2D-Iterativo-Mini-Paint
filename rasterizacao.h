#ifndef RASTERIZACAO_H
#define RASTERIZACAO_H

#include "framebuffer.h"

// Reta (Bresenham)
void draw_line(Framebuffer& fb, int x0, int y0, int x1, int y1, Color cor);

// Retângulos
void draw_rect(Framebuffer& fb, int x, int y, int width, int height, Color cor);
void draw_rect_filled(Framebuffer& fb, int x, int y, int width, int height, Color cor);

// Círculos (Ponto Médio)
void draw_circle(Framebuffer& fb, int xc, int yc, int radius, Color cor);
void draw_circle_filled(Framebuffer& fb, int xc, int yc, int radius, Color cor);

// Preenchimento (Flood Fill iterativo com std::stack)
void flood_fill(Framebuffer& fb, int x, int y, Color fill_color);

#endif // RASTERIZACAO_H