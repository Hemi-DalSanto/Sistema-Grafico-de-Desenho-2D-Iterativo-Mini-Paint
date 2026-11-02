#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include "framebuffer.h"
#include "ferramentas.h"

// Estados da máquina de estados do mouse
enum class EstadoMouse {
    OCIOSO,           // nenhum botão pressionado
    DESENHANDO,       // arrastando com lápis/borracha (traço livre)
    ARRASTANDO_FORMA  // arrastando para definir linha/retângulo/círculo (com pré-visualização)
};

// Recebe os eventos do mouse (clique, arrasto, soltar) e aplica a ferramenta
// selecionada no canvas. Coordenadas em pixels do canvas, com y = 0 embaixo.
class Editor {
private:
    Framebuffer& canvas;
    Framebuffer copia;     // canvas salvo no início de uma forma (para a pré-visualização)
    Color cor_fundo;

    Ferramenta ferramenta = Ferramenta::LAPIS;
    Espessura espessura = Espessura::FINO;
    Color cor = {0, 0, 0};
    EstadoMouse estado = EstadoMouse::OCIOSO;

    int x_inicio = 0, y_inicio = 0;     // onde a forma começou
    int x_anterior = 0, y_anterior = 0; // último ponto do traço livre

    void desenhar_traco(int x0, int y0, int x1, int y1);
    void desenhar_forma(int x, int y);

public:
    Editor(Framebuffer& canvas, Color cor_fundo);

    // Eventos do mouse
    void pressionar(int x, int y);
    void arrastar(int x, int y);
    void soltar(int x, int y);
    void cancelar();  // desfaz a forma em andamento (tecla ESC)

    // Botões "Novo" e "Salvar"
    void novo();
    void salvar(const std::string& arquivo);

    void selecionar_ferramenta(Ferramenta f);
    void selecionar_espessura(Espessura e);
    void selecionar_cor(Color c);

    Ferramenta get_ferramenta() const { return ferramenta; }
    Espessura get_espessura() const { return espessura; }
    Color get_cor() const { return cor; }
    EstadoMouse get_estado() const { return estado; }
};

#endif
