#include "rasterizacao.h"
#include <cstdlib>
#include <algorithm>
#include <stack>
#include <utility>

// ==> Linha - Bresenham <==
void draw_line(Framebuffer& fb, int x0, int y0, int x1, int y1, Color cor) {
    
    //Distancia
    //Calculo da Distância Absoluta entre os pontos
    int dx = std::abs(x1 - x0); //Variação Total Horizontal
    int dy = std::abs(y1 - y0); // Variação Total Vertical
    // std:: abs = calcula a distancia absoluta de um número posito
   
    //Step
    //Se X/Y  inicial menor que  X final, move para Direita (Incrementa)
    //Caso contrário, move para Esquerda (-1 =Decrementa)
    int sx = (x0 < x1) ? 1 : -1; //Direçã do eixo X
    int sy = (y0 < y1) ? 1 : -1; // Direção do eixo Y
    
    int err = dx - dy; //Variável de Decisão Inicial

    // O Loop continuará interando pixel por pixel até que 
    //as  coordenadas iniciais (x0,y0) encontrem o ponto final (x1,y1)
    while (true) {

        //1. Desenha o pixel atual no FrameBuffer
        fb.put_pixel(x0, y0, cor); //put_pixel = calcula a posição exata de (x,y) e escreve a cor na matriz de pixels

        //2. Condição de parada
        if (x0 == x1 && y0 == y1) break;

        //3. Atualização do erro e das coordenadas
        int e2 = 2 * err; // Vai armazenar o dobro do erro atual para testar os limites de ajuste sem perder a precisão

        if (e2 > -dy) { // É o teste que determina se o traçado deve avançar no eixo X ou Y ou ambos
            err -= dy; //Atualiza o erro
            x0 += sx;   //Avança em direção ao X
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy; // Avança em direção ao Y
        }
    }
}

// ==> Retângulo vazado <==
void draw_rect(Framebuffer& fb, int x, int y, int width, int height, Color cor) {
    
    //Este IF impede que o programa tente desenhar retângulos com dimensões negativas ou zeradas
    if (width <= 0 || height <= 0) return;
    
    //A subt do 1 é necessária para não passar do ponto correto
    int x2 = x + width - 1;
    int y2 = y + height - 1;

    draw_line(fb, x, y, x2, y, cor); //Superior (mantém Y)
    draw_line(fb, x, y2, x2, y2, cor); //Inferior (mantém Y2)
    draw_line(fb, x, y, x, y2, cor); //Lateral  Direito (mantém X)
    draw_line(fb, x2, y, x2, y2, cor); // Laterla Esquerdo (mantém X2)
}

// ==> Retângulo preenchido (Varredura simples) <==
void draw_rect_filled(Framebuffer& fb, int x, int y, int width, int height, Color cor) {
    if (width <= 0 || height <= 0) return;

    //Loop responsável por percorrer as linhas do retangulo na vertical (Y)
    for (int j = y; j < y + height; ++j) {
        //Loop responsávl por percorrer as colunas na horizontal(X)
        for (int i = x; i < x + width; ++i) {
            fb.put_pixel(i, j, cor); //Pinta o pixel da coordenada i,j (xi,yj)
        }
    }
}

//  => Auxiliares do Círculo <=
//Aux1 -Desenha 8 pixels de uma só vez (Octante)
static void plot_circle_8_points(Framebuffer& fb, int xc, int yc, int x, int y, Color cor) {
    //xc,yc = coordenadas exatas do centro do círculo
    fb.put_pixel(xc + x, yc + y, cor); 
    fb.put_pixel(xc - x, yc + y, cor);
    fb.put_pixel(xc + x, yc - y, cor);
    fb.put_pixel(xc - x, yc - y, cor);
    fb.put_pixel(xc + y, yc + x, cor);
    fb.put_pixel(xc - y, yc + x, cor);
    fb.put_pixel(xc + y, yc - x, cor);
    fb.put_pixel(xc - y, yc - x, cor);
}

//Aux2 - Desenha uma linha reta horizontal na altura y
static void draw_horizontal_line(Framebuffer& fb, int x1, int x2, int y, Color cor) {
    
    //Este IF garante que o Ponto Inicial seja sempre menor (mais á esquerda) do que o ponto final
    //STD:: SWAP = Troca os valores de 2 var entre si
    if (x1 > x2) std::swap(x1, x2); // Garante que x1 seja sempre o ponto mais a esquerda e facilita o desenho
   
    //Percorre cada Pixel na Horizontal( mesma linha Y)
    for (int x = x1; x <= x2; ++x) {
        fb.put_pixel(x, y, cor);
    }
}


// ==> Círculo vazado - Ponto Médio <==
void draw_circle(Framebuffer& fb, int xc, int yc, int radius, Color cor) {
    if (radius <= 0) {
        //Se o raio for exatamente 0, desenha apenas o pixel central
        if (radius == 0) fb.put_pixel(xc, yc, cor);
        return;
    }

    int x = 0;
    int y = radius;
    //parametro de decisão do ponto médio
    //garante que todos os calculos usem apenas inteiros
    int d = 1 - radius;

    plot_circle_8_points(fb, xc, yc, x, y, cor);

    while (x < y) {
        x++;
        if (d < 0) { //d<0 significa que ponto médio está dentro do círculo
            d += 2 * x + 1;
        } else {
            y--; //Se d>0 o ponto Méd está fora do Circulo, então decrementa y
            d += 2 * (x - y) + 1;
        }
        plot_circle_8_points(fb, xc, yc, x, y, cor);
    }
}

// ==> Círculo preenchido <==
void draw_circle_filled(Framebuffer& fb, int xc, int yc, int radius, Color cor) {
    if (radius <= 0) {
        if (radius == 0) fb.put_pixel(xc, yc, cor);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    //Função Aux: Desenha Linhas Horizontais Contínuas  entre os pontos simétricos do circulo para preenche-lo
    auto fill_slices = [&](int cx, int cy, int px, int py) {
        //Preenche as partes Espelhadas
        draw_horizontal_line(fb, cx - px, cx + px, cy + py, cor); //Parte Inferior
        draw_horizontal_line(fb, cx - px, cx + px, cy - py, cor); // Parte Superior
        draw_horizontal_line(fb, cx - py, cx + py, cy + px, cor); //Lateral 
        draw_horizontal_line(fb, cx - py, cx + py, cy - px, cor); //Lateral
    };

    fill_slices(xc, yc, x, y);

    while (x < y) {
        x++;
        if (d < 0) { //(d<0) siginifica que está no ponto médio)
            d += 2 * x + 1;
        } else {
            y--; // (d>0) significa que não está nnno ponto médio
            d += 2 * (x - y) + 1;
        }
        fill_slices(xc, yc, x, y);
    }
}

// ==> Comparação direta de structs Color <==
// const Color& otimiza o código evitando estrutura inteira da cor copiada para a memória apenas para ser lida
static bool cores_iguais(const Color& a, const Color& b) {
    
    //Compara os componentes RGB
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}

//==> Flood Fill com std::stack (evita estouro de pilha de chamada) <==
void flood_fill(Framebuffer& fb, int start_x, int start_y, Color fill_color) {
    int w = fb.get_width();
    int h = fb.get_height();

    //Garante que o ponto de clique inicial está dentro da área visível da tela
    if (start_x < 0 || start_x >= w || start_y < 0 || start_y >= h) {
        return;
    }

    //Descobre a cor alvo onde o usuário clicou
    Color target_color = fb.get_pixel(start_x, start_y);

    //Compara se a cor clicada é igual a do preenchimento
    //Fill_Color é a cor que o usuário quer pintar
    if (cores_iguais(target_color, fill_color)) {
        return;
    }

    // ==>Uso da Pilha para evitar Stack Overflow
    //Armazena na memória do PC e não de chamada do sistema
    // std= biblioteca do c++, stack = pilha gerada, pair = Agrupa 2 numeros
    std::stack<std::pair<int, int>> pilha;

    //Push coloca o ponto inicial na pilha recém criada
    pilha.push({start_x, start_y});

    //Enquanto a pilha não estiver vazia
    while (!pilha.empty()) {

        // Lê o pixel do topo  da pilha
        auto [cx, cy] = pilha.top();
        // Remove o pixel da pilha para passar p/ próximo
        pilha.pop();

        //Verifica se está dentro do limite da tela
        if (cx < 0 || cx >= w || cy < 0 || cy >= h) {
            continue;
        }

        //Lê a cor do pixel atual e compara com o alvo
        Color atual = fb.get_pixel(cx, cy);
        
        //Se cor igual, colore o pixel atual
        //OBS: O Flood Fill se espalha enquanto encontra pixels com a mesma cor da área inicial
        if (cores_iguais(atual, target_color)) {
            fb.put_pixel(cx, cy, fill_color);

            //Adiciono a coordenada dos pixels vizinhos para que eles sejam pintados
            pilha.push({cx + 1, cy});
            pilha.push({cx - 1, cy});
            pilha.push({cx, cy + 1});
            pilha.push({cx, cy - 1});
        }
    }
}