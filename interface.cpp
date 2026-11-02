#include "interface.h"
#include "rasterizacao.h"
#include <cstring>

const Color PALETA[NUM_CORES] = {
    {0, 0, 0},       // preto
    {255, 255, 255}, // branco
    {255, 0, 0},     // vermelho
    {0, 255, 0},     // verde
    {0, 0, 255},     // azul
    {255, 255, 0},   // amarelo
    {0, 255, 255},   // ciano
    {255, 0, 255}    // magenta
};

const char* const NOMES_CORES[NUM_CORES] = {
    "Preto", "Branco", "Vermelho", "Verde", "Azul", "Amarelo", "Ciano", "Magenta"
};

// Cores da própria interface
static const Color COR_BOTAO = {245, 245, 245};
static const Color COR_BORDA = {150, 150, 150};
static const Color COR_BOTAO_SELECIONADO = {200, 220, 250};
static const Color COR_BORDA_SELECIONADA = {30, 90, 200};
static const Color COR_ICONE = {40, 40, 40};
static const Color COR_SEPARADOR = {170, 170, 170};

static bool cores_iguais(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

int indice_da_cor(Color c) {
    for (int i = 0; i < NUM_CORES; ++i) {
        if (cores_iguais(c, PALETA[i])) return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Layout da barra (coordenadas da barra: 800 x 56, y = 0 embaixo)
//
//  [8 ferramentas] | [3 espessuras] | [cor atual][paleta 4x2] | [NOVO][SALVAR]
// ---------------------------------------------------------------------------
struct Retangulo {
    int x, y, w, h;
    bool contem(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

static const int TAM_BOTAO = 36;
static const int PASSO_BOTAO = 40;  // botão + 4 px de espaço
static const int TAM_AMOSTRA = 16;

static Retangulo botao_ferramenta(int i) { return {8 + i * PASSO_BOTAO, 10, TAM_BOTAO, TAM_BOTAO}; }
static Retangulo botao_espessura(int i)  { return {340 + i * PASSO_BOTAO, 10, TAM_BOTAO, TAM_BOTAO}; }
static Retangulo amostra_cor_atual()     { return {472, 10, TAM_BOTAO, TAM_BOTAO}; }
static Retangulo amostra_paleta(int i)   { return {516 + (i % 4) * 20, 30 - (i / 4) * 20, TAM_AMOSTRA, TAM_AMOSTRA}; }
static Retangulo botao_novo()            { return {608, 10, 88, TAM_BOTAO}; }
static Retangulo botao_salvar()          { return {704, 10, 88, TAM_BOTAO}; }
static const int SEPARADORES[] = {332, 464, 600};

ElementoBarra elemento_em(int x, int y) {
    for (int i = 0; i < NUM_FERRAMENTAS; ++i) {
        if (botao_ferramenta(i).contem(x, y)) return {TipoElemento::FERRAMENTA, i};
    }
    for (int i = 0; i < NUM_ESPESSURAS; ++i) {
        if (botao_espessura(i).contem(x, y)) return {TipoElemento::ESPESSURA, i};
    }
    for (int i = 0; i < NUM_CORES; ++i) {
        if (amostra_paleta(i).contem(x, y)) return {TipoElemento::COR, i};
    }
    if (botao_novo().contem(x, y)) return {TipoElemento::NOVO, 0};
    if (botao_salvar().contem(x, y)) return {TipoElemento::SALVAR, 0};
    return {TipoElemento::NENHUM, 0};
}

// ---------------------------------------------------------------------------
// Fonte bitmap 5x7, só com as letras de "NOVO" e "SALVAR"
// (texto desenhado pixel a pixel, sem função de texto pronta)
// ---------------------------------------------------------------------------
struct Glifo {
    char letra;
    const char* linhas[7];  // de cima para baixo; '1' = pixel aceso
};

static const Glifo FONTE[] = {
    {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
    {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
    {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
    {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
    {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
    {'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
    {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
};

static const Glifo* buscar_glifo(char letra) {
    for (const Glifo& g : FONTE) {
        if (g.letra == letra) return &g;
    }
    return nullptr;
}

// Cada pixel da fonte vira um quadrado escala x escala
static void desenhar_texto(Framebuffer& fb, const char* texto, int x, int y, int escala, Color cor) {
    for (int i = 0; texto[i] != '\0'; ++i) {
        const Glifo* g = buscar_glifo(texto[i]);
        if (g) {
            for (int linha = 0; linha < 7; ++linha) {
                for (int col = 0; col < 5; ++col) {
                    if (g->linhas[linha][col] == '1') {
                        int px = x + col * escala;
                        int py = y + (6 - linha) * escala;  // linha 0 é a de cima
                        draw_rect_filled(fb, px, py, escala, escala, cor);
                    }
                }
            }
        }
        x += 6 * escala;  // 5 colunas + 1 de espaço
    }
}

static void desenhar_texto_centralizado(Framebuffer& fb, const char* texto, Retangulo r, int escala, Color cor) {
    int largura = static_cast<int>(std::strlen(texto)) * 6 * escala - escala;
    int altura = 7 * escala;
    desenhar_texto(fb, texto, r.x + (r.w - largura) / 2, r.y + (r.h - altura) / 2, escala, cor);
}

// ---------------------------------------------------------------------------
// Botões e ícones
// ---------------------------------------------------------------------------
static void desenhar_botao(Framebuffer& fb, Retangulo r, bool selecionado) {
    draw_rect_filled(fb, r.x, r.y, r.w, r.h, selecionado ? COR_BOTAO_SELECIONADO : COR_BOTAO);
    if (selecionado) {
        draw_rect(fb, r.x, r.y, r.w, r.h, COR_BORDA_SELECIONADA);
        draw_rect(fb, r.x + 1, r.y + 1, r.w - 2, r.h - 2, COR_BORDA_SELECIONADA);
    } else {
        draw_rect(fb, r.x, r.y, r.w, r.h, COR_BORDA);
    }
}

// Ícone de cada ferramenta, desenhado com as próprias primitivas do projeto.
// (x, y) é o canto inferior esquerdo do botão; o ícone ocupa o miolo 20x20.
static void desenhar_icone(Framebuffer& fb, Ferramenta f, int x, int y) {
    switch (f) {
    case Ferramenta::LAPIS:
        desenhar_linha_espessa(fb, x + 13, y + 13, x + 26, y + 26, {230, 160, 40}, 2);  // corpo
        desenhar_linha_espessa(fb, x + 9, y + 9, x + 12, y + 12, COR_ICONE, 1);         // ponta
        break;

    case Ferramenta::BORRACHA:
        draw_rect_filled(fb, x + 14, y + 12, 14, 12, {240, 150, 170});
        draw_rect_filled(fb, x + 8, y + 12, 6, 12, {255, 255, 255});
        draw_rect(fb, x + 8, y + 12, 20, 12, COR_ICONE);
        break;

    case Ferramenta::LINHA:
        draw_line(fb, x + 8, y + 8, x + 27, y + 27, COR_ICONE);
        break;

    case Ferramenta::RETANGULO:
        draw_rect(fb, x + 8, y + 11, 20, 14, COR_ICONE);
        break;

    case Ferramenta::RETANGULO_CHEIO:
        draw_rect_filled(fb, x + 8, y + 11, 20, 14, COR_ICONE);
        break;

    case Ferramenta::CIRCULO:
        draw_circle(fb, x + 18, y + 18, 9, COR_ICONE);
        break;

    case Ferramenta::CIRCULO_CHEIO:
        draw_circle_filled(fb, x + 18, y + 18, 9, COR_ICONE);
        break;

    case Ferramenta::BALDE:
        // Balde em forma de trapézio, pintado com o próprio flood fill
        draw_line(fb, x + 8, y + 24, x + 11, y + 8, COR_ICONE);
        draw_line(fb, x + 11, y + 8, x + 23, y + 8, COR_ICONE);
        draw_line(fb, x + 23, y + 8, x + 26, y + 24, COR_ICONE);
        draw_line(fb, x + 26, y + 24, x + 8, y + 24, COR_ICONE);
        flood_fill(fb, x + 17, y + 16, {60, 120, 220});
        draw_circle_filled(fb, x + 29, y + 13, 2, {60, 120, 220});  // gota
        break;
    }
}

void desenhar_barra(Framebuffer& barra, const Editor& editor) {
    barra.clear_canvas();

    // Linha que separa a barra do canvas
    draw_line(barra, 0, 0, barra.get_width() - 1, 0, COR_BORDA);

    for (int x : SEPARADORES) {
        draw_line(barra, x, 8, x, ALTURA_BARRA - 9, COR_SEPARADOR);
    }

    // Ferramentas
    for (int i = 0; i < NUM_FERRAMENTAS; ++i) {
        Ferramenta f = static_cast<Ferramenta>(i);
        Retangulo r = botao_ferramenta(i);
        desenhar_botao(barra, r, f == editor.get_ferramenta());
        desenhar_icone(barra, f, r.x, r.y);
    }

    // Espessuras: cada botão mostra um traço com a espessura real do lápis
    for (int i = 0; i < NUM_ESPESSURAS; ++i) {
        Espessura e = static_cast<Espessura>(i);
        Retangulo r = botao_espessura(i);
        int meio = r.y + r.h / 2;
        desenhar_botao(barra, r, e == editor.get_espessura());
        desenhar_linha_espessa(barra, r.x + 9, meio, r.x + r.w - 10, meio, COR_ICONE,
                               raio_do_pincel(Ferramenta::LAPIS, e));
    }

    // Cor atual (amostra grande)
    Retangulo atual = amostra_cor_atual();
    draw_rect_filled(barra, atual.x, atual.y, atual.w, atual.h, editor.get_cor());
    draw_rect(barra, atual.x, atual.y, atual.w, atual.h, COR_ICONE);

    // Paleta: a cor selecionada ganha uma moldura azul de 2 px
    int selecionada = indice_da_cor(editor.get_cor());
    for (int i = 0; i < NUM_CORES; ++i) {
        Retangulo r = amostra_paleta(i);
        draw_rect_filled(barra, r.x, r.y, r.w, r.h, PALETA[i]);
        draw_rect(barra, r.x, r.y, r.w, r.h, COR_BORDA);
        if (i == selecionada) {
            draw_rect(barra, r.x - 2, r.y - 2, r.w + 4, r.h + 4, COR_BORDA_SELECIONADA);
            draw_rect(barra, r.x - 3, r.y - 3, r.w + 6, r.h + 6, COR_BORDA_SELECIONADA);
        }
    }

    // Botões "Novo" e "Salvar"
    desenhar_botao(barra, botao_novo(), false);
    desenhar_texto_centralizado(barra, "NOVO", botao_novo(), 2, COR_ICONE);
    desenhar_botao(barra, botao_salvar(), false);
    desenhar_texto_centralizado(barra, "SALVAR", botao_salvar(), 2, COR_ICONE);
}
