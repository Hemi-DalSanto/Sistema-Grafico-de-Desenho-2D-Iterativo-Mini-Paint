#include "rasterizacao.h"
#include <cstdlib>
#include <stack>

void put_pixel(int x, int y, Color cor);
Color get_pixel(int x, int y);

void draw_line(int x0, int y0, int x1, int y1, Color cor) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        put_pixel(x0, y0, cor); // bounds checking é garantido pelo framebuffer

        if (x0 == x1 && y0 == y1) {
            break;
        }

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
void draw_rect(int x, int y, int width, int height, Color cor) {
    if (width <= 0 || height <= 0) return;

    int x2 = x + width - 1;
    int y2 = y + height - 1;

    // Segmentos horizontais
    draw_line(x, y, x2, y, cor);
    draw_line(x, y2, x2, y2, cor);

    // Segmentos verticais
    draw_line(x, y, x, y2, cor);
    draw_line(x2, y, x2, y2, cor);
}

void draw_rect_filled(int x, int y, int width, int height, Color cor) {
    if (width <= 0 || height <= 0) return;

    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            put_pixel(i, j, cor);
        }
    }
}

static void plot_circle_8_points(int xc, int yc, int x, int y, Color cor) {
    put_pixel(xc + x, yc + y, cor);
    put_pixel(xc - x, yc + y, cor);
    put_pixel(xc + x, yc - y, cor);
    put_pixel(xc - x, yc - y, cor);
    put_pixel(xc + y, yc + x, cor);
    put_pixel(xc - y, yc + x, cor);
    put_pixel(xc + y, yc - x, cor);
    put_pixel(xc - y, yc - x, cor);
}

void draw_circle(int xc, int yc, int radius, Color cor) {
    if (radius <= 0) {
        if (radius == 0) put_pixel(xc, yc, cor);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    plot_circle_8_points(xc, yc, x, y, cor);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        plot_circle_8_points(xc, yc, x, y, cor);
    }
}

// Função utilitária para comparação exata de cor
static bool cores_iguais(const Color& a, const Color& b) {
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}

void flood_fill(int start_x, int start_y, Color fill_color) {
    // Dimensões do canvas vindas do framebuffer (ex.: 800x600)[cite: 2]
    extern const int CANVAS_WIDTH;
    extern const int CANVAS_HEIGHT;

    if (start_x < 0 || start_x >= CANVAS_WIDTH || start_y < 0 || start_y >= CANVAS_HEIGHT) {
        return;
    }

    Color target_color = get_pixel(start_x, start_y); // Função da Pessoa 1[cite: 2]

    // Se já é a cor de preenchimento, nada a fazer
    if (cores_iguais(target_color, fill_color)) {
        return;
    }

    std::stack<std::pair<int, int>> pilha;
    pilha.push({start_x, start_y});

    while (!pilha.empty()) {
        auto [cx, cy] = pilha.top();
        pilha.pop();

        if (cx < 0 || cx >= CANVAS_WIDTH || cy < 0 || cy >= CANVAS_HEIGHT) {
            continue;
        }

        Color atual = get_pixel(cx, cy);
        if (cores_iguais(atual, target_color)) {
            put_pixel(cx, cy, fill_color);

            // Vizinhos 4-conectados
            pilha.push({cx + 1, cy});
            pilha.push({cx - 1, cy});
            pilha.push({cx, cy + 1});
            pilha.push({cx, cy - 1});
        }
    }
}