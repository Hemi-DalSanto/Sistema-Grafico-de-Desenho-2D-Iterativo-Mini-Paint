#include "rasterizacao.h"
#include <cstdlib>
#include <algorithm>
#include <stack>
#include <utility>

// Linha - Bresenham
void draw_line(Framebuffer& fb, int x0, int y0, int x1, int y1, Color cor) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        fb.put_pixel(x0, y0, cor);
        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Retângulo vazado
void draw_rect(Framebuffer& fb, int x, int y, int width, int height, Color cor) {
    if (width <= 0 || height <= 0) return;
    int x2 = x + width - 1;
    int y2 = y + height - 1;

    draw_line(fb, x, y, x2, y, cor);
    draw_line(fb, x, y2, x2, y2, cor);
    draw_line(fb, x, y, x, y2, cor);
    draw_line(fb, x2, y, x2, y2, cor);
}

// Retângulo preenchido (Varredura simples)
void draw_rect_filled(Framebuffer& fb, int x, int y, int width, int height, Color cor) {
    if (width <= 0 || height <= 0) return;
    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            fb.put_pixel(i, j, cor);
        }
    }
}

// Auxiliares do Círculo
static void plot_circle_8_points(Framebuffer& fb, int xc, int yc, int x, int y, Color cor) {
    fb.put_pixel(xc + x, yc + y, cor);
    fb.put_pixel(xc - x, yc + y, cor);
    fb.put_pixel(xc + x, yc - y, cor);
    fb.put_pixel(xc - x, yc - y, cor);
    fb.put_pixel(xc + y, yc + x, cor);
    fb.put_pixel(xc - y, yc + x, cor);
    fb.put_pixel(xc + y, yc - x, cor);
    fb.put_pixel(xc - y, yc - x, cor);
}

static void draw_horizontal_line(Framebuffer& fb, int x1, int x2, int y, Color cor) {
    if (x1 > x2) std::swap(x1, x2);
    for (int x = x1; x <= x2; ++x) {
        fb.put_pixel(x, y, cor);
    }
}

// Círculo vazado - Ponto Médio
void draw_circle(Framebuffer& fb, int xc, int yc, int radius, Color cor) {
    if (radius <= 0) {
        if (radius == 0) fb.put_pixel(xc, yc, cor);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    plot_circle_8_points(fb, xc, yc, x, y, cor);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        plot_circle_8_points(fb, xc, yc, x, y, cor);
    }
}

// Círculo preenchido
void draw_circle_filled(Framebuffer& fb, int xc, int yc, int radius, Color cor) {
    if (radius <= 0) {
        if (radius == 0) fb.put_pixel(xc, yc, cor);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    auto fill_slices = [&](int cx, int cy, int px, int py) {
        draw_horizontal_line(fb, cx - px, cx + px, cy + py, cor);
        draw_horizontal_line(fb, cx - px, cx + px, cy - py, cor);
        draw_horizontal_line(fb, cx - py, cx + py, cy + px, cor);
        draw_horizontal_line(fb, cx - py, cx + py, cy - px, cor);
    };

    fill_slices(xc, yc, x, y);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        fill_slices(xc, yc, x, y);
    }
}

// Comparação direta de structs Color
static bool cores_iguais(const Color& a, const Color& b) {
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}

// Flood Fill com std::stack (evita estouro de pilha de chamada)
void flood_fill(Framebuffer& fb, int start_x, int start_y, Color fill_color) {
    int w = fb.get_width();
    int h = fb.get_height();

    if (start_x < 0 || start_x >= w || start_y < 0 || start_y >= h) {
        return;
    }

    Color target_color = fb.get_pixel(start_x, start_y);

    if (cores_iguais(target_color, fill_color)) {
        return;
    }

    std::stack<std::pair<int, int>> pilha;
    pilha.push({start_x, start_y});

    while (!pilha.empty()) {
        auto [cx, cy] = pilha.top();
        pilha.pop();

        if (cx < 0 || cx >= w || cy < 0 || cy >= h) {
            continue;
        }

        Color atual = fb.get_pixel(cx, cy);
        if (cores_iguais(atual, target_color)) {
            fb.put_pixel(cx, cy, fill_color);

            pilha.push({cx + 1, cy});
            pilha.push({cx - 1, cy});
            pilha.push({cx, cy + 1});
            pilha.push({cx, cy - 1});
        }
    }
}