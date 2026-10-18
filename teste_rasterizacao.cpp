#include "framebuffer.h"
#include "rasterizacao.h"
#include <iostream>

int main() {
Framebuffer fb(800, 600, {255, 255, 255});

    Color vermelho = {255, 0, 0};
    Color verde    = {0, 255, 0};
    Color azul     = {0, 0, 255};

    // Testa reta
    draw_line(10, 10, 100, 80, vermelho);

    // Testa retângulos
    draw_rect(120, 20, 50, 40, verde);
    draw_rect_filled(180, 20, 50, 40, azul);

    // Testa círculos
    draw_circle(300, 150, 40, vermelho);
    draw_circle_filled(400, 150, 40, verde);

    // Testa balde de tinta em região fechada
    flood_fill(125, 25, azul);

    // Exporta imagem para validar visualmente
    // framebuffer.exportar_imagem("teste_saida.ppm");

    std::cout << "Testes de rasterizacao executados com sucesso!" << std::endl;
    return 0;
}