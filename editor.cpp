#include "editor.h"
#include "rasterizacao.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

Editor::Editor(Framebuffer& canvas, Color cor_fundo)
    : canvas(canvas), copia(canvas), cor_fundo(cor_fundo) {}

// OCIOSO --clique--> DESENHANDO ou ARRASTANDO_FORMA (o balde pinta e continua OCIOSO)
void Editor::pressionar(int x, int y) {
    if (estado != EstadoMouse::OCIOSO) return;

    switch (ferramenta) {
    case Ferramenta::LAPIS:
    case Ferramenta::BORRACHA:
        estado = EstadoMouse::DESENHANDO;
        x_anterior = x;
        y_anterior = y;
        desenhar_traco(x, y, x, y);  // um clique sem arrastar já marca um ponto
        break;

    case Ferramenta::BALDE:
        flood_fill(canvas, x, y, cor);
        break;

    default:  // linha, retângulos e círculos
        estado = EstadoMouse::ARRASTANDO_FORMA;
        x_inicio = x;
        y_inicio = y;
        copia = canvas;  // guarda o desenho como estava antes da forma
        desenhar_forma(x, y);
        break;
    }
}

// DESENHANDO / ARRASTANDO_FORMA --arrasto--> mesmo estado
void Editor::arrastar(int x, int y) {
    if (estado == EstadoMouse::DESENHANDO) {
        // Liga o ponto anterior ao atual: se o mouse andar rápido o traço não fica falhado
        desenhar_traco(x_anterior, y_anterior, x, y);
        x_anterior = x;
        y_anterior = y;
    } else if (estado == EstadoMouse::ARRASTANDO_FORMA) {
        canvas = copia;  // apaga a pré-visualização anterior
        desenhar_forma(x, y);
    }
}

// DESENHANDO / ARRASTANDO_FORMA --soltar--> OCIOSO
void Editor::soltar(int x, int y) {
    arrastar(x, y);  // a posição final fica desenhada de vez no canvas
    estado = EstadoMouse::OCIOSO;
}

void Editor::cancelar() {
    if (estado == EstadoMouse::ARRASTANDO_FORMA) {
        canvas = copia;
    }
    estado = EstadoMouse::OCIOSO;
}

void Editor::novo() {
    cancelar();
    canvas.clear_canvas();
}

void Editor::salvar(const std::string& arquivo) {
    cancelar();  // não salva uma forma pela metade
    canvas.export_ppm(arquivo);
}

void Editor::selecionar_ferramenta(Ferramenta f) {
    if (f != ferramenta) {
        cancelar();
        ferramenta = f;
    }
}

void Editor::selecionar_espessura(Espessura e) {
    espessura = e;
}

void Editor::selecionar_cor(Color c) {
    cor = c;
}

// Lápis pinta com a cor atual; borracha pinta com a cor de fundo
void Editor::desenhar_traco(int x0, int y0, int x1, int y1) {
    Color c = (ferramenta == Ferramenta::BORRACHA) ? cor_fundo : cor;
    desenhar_linha_espessa(canvas, x0, y0, x1, y1, c, raio_do_pincel(ferramenta, espessura));
}

// Desenha a forma da ferramenta atual entre o ponto inicial e (x, y)
void Editor::desenhar_forma(int x, int y) {
    int raio = raio_do_pincel(ferramenta, espessura);

    switch (ferramenta) {
    case Ferramenta::LINHA:
        desenhar_linha_espessa(canvas, x_inicio, y_inicio, x, y, cor, raio);
        break;

    case Ferramenta::RETANGULO:
    case Ferramenta::RETANGULO_CHEIO: {
        // Os dois cantos opostos podem estar em qualquer direção do arrasto
        int rx = std::min(x_inicio, x);
        int ry = std::min(y_inicio, y);
        int largura = std::abs(x - x_inicio) + 1;
        int altura = std::abs(y - y_inicio) + 1;
        if (ferramenta == Ferramenta::RETANGULO_CHEIO) {
            draw_rect_filled(canvas, rx, ry, largura, altura, cor);
        } else {
            desenhar_retangulo_espesso(canvas, rx, ry, largura, altura, cor, raio);
        }
        break;
    }

    case Ferramenta::CIRCULO:
    case Ferramenta::CIRCULO_CHEIO: {
        // Centro no clique, raio = distância até o mouse
        int dx = x - x_inicio;
        int dy = y - y_inicio;
        int r = static_cast<int>(std::lround(std::sqrt(static_cast<double>(dx * dx + dy * dy))));
        if (ferramenta == Ferramenta::CIRCULO_CHEIO) {
            draw_circle_filled(canvas, x_inicio, y_inicio, r, cor);
        } else {
            desenhar_circulo_espesso(canvas, x_inicio, y_inicio, r, cor, raio);
        }
        break;
    }

    default:
        break;
    }
}
