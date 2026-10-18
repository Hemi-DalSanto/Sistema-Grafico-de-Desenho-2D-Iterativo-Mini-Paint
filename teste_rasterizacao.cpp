#include "framebuffer.h"
#include "rasterizacao.h"
#include <iostream>

int main() {
    // Canvas 800x600 com fundo branco (255, 255, 255)
    Color fundo = {255, 255, 255};
    Framebuffer fb(800, 600, fundo);

    Color preto    = {0, 0, 0};
    Color vermelho = {255, 0, 0};
    Color verde    = {0, 255, 0};
    Color azul     = {0, 0, 255};

    // 1. Reta
    draw_line(fb, 20, 20, 200, 100, preto);

    // 2. Retângulos (vazado e preenchido)
    draw_rect(fb, 50, 150, 120, 80, vermelho);
    draw_rect_filled(fb, 220, 150, 120, 80, azul);

    // 3. Círculos (vazado e preenchido)
    draw_circle(fb, 450, 200, 60, verde);
    draw_circle_filled(fb, 620, 200, 60, vermelho);

    // 4. Balde de tinta dentro do retângulo vazado
    flood_fill(fb, 60, 160, verde);

    // Exporta imagem em formato PPM para conferência visual
    fb.export_ppm("teste_saida.ppm");

    std::cout << "Sucesso! Imagem 'teste_saida.ppm' gerada com todas as figuras." << std::endl;
    return 0;
}