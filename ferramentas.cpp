#include "ferramentas.h"
#include "rasterizacao.h"
#include <utility>
#include <vector>

int raio_do_pincel(Ferramenta f, Espessura e) {
    static const int RAIO_PADRAO[NUM_ESPESSURAS]   = {0, 2, 4};  // 1, 5 e 9 pixels
    static const int RAIO_BORRACHA[NUM_ESPESSURAS] = {3, 6, 10}; // 7, 13 e 21 pixels

    int i = static_cast<int>(e);
    return (f == Ferramenta::BORRACHA) ? RAIO_BORRACHA[i] : RAIO_PADRAO[i];
}

const char* nome_ferramenta(Ferramenta f) {
    static const char* NOMES[NUM_FERRAMENTAS] = {
        "Lapis", "Borracha", "Linha", "Retangulo", "Retangulo cheio",
        "Circulo", "Circulo cheio", "Balde de tinta"
    };
    return NOMES[static_cast<int>(f)];
}

const char* nome_espessura(Espessura e) {
    static const char* NOMES[NUM_ESPESSURAS] = {"Fino", "Medio", "Grosso"};
    return NOMES[static_cast<int>(e)];
}

// Deslocamentos (dx, dy) que formam um disco de raio "raio": a ponta do pincel
static std::vector<std::pair<int, int>> ponta_do_pincel(int raio) {
    std::vector<std::pair<int, int>> pontos;
    for (int dy = -raio; dy <= raio; ++dy) {
        for (int dx = -raio; dx <= raio; ++dx) {
            // "+ raio" arredonda o disco (sem isso as bordas ficam pontudas)
            if (dx * dx + dy * dy <= raio * raio + raio) {
                pontos.push_back({dx, dy});
            }
        }
    }
    return pontos;
}

void desenhar_linha_espessa(Framebuffer& fb, int x0, int y0, int x1, int y1, Color cor, int raio) {
    for (auto [dx, dy] : ponta_do_pincel(raio)) {
        draw_line(fb, x0 + dx, y0 + dy, x1 + dx, y1 + dy, cor);
    }
}

void desenhar_retangulo_espesso(Framebuffer& fb, int x, int y, int width, int height, Color cor, int raio) {
    for (auto [dx, dy] : ponta_do_pincel(raio)) {
        draw_rect(fb, x + dx, y + dy, width, height, cor);
    }
}

void desenhar_circulo_espesso(Framebuffer& fb, int xc, int yc, int radius, Color cor, int raio) {
    for (auto [dx, dy] : ponta_do_pincel(raio)) {
        draw_circle(fb, xc + dx, yc + dy, radius, cor);
    }
}
