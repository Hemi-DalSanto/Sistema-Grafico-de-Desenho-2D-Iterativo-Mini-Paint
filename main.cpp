// Mini Paint 2D - janela gráfica e loop de eventos (GLUT)
//
// O OpenGL só é usado para abrir a janela e copiar os framebuffers para a tela
// com glDrawPixels. Todo o desenho (inclusive da barra) passa por put_pixel.

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <string>
#include "framebuffer.h"
#include "editor.h"
#include "interface.h"

const int LARGURA_JANELA = LARGURA_CANVAS;
const int ALTURA_JANELA = ALTURA_CANVAS + ALTURA_BARRA;

Framebuffer canvas(LARGURA_CANVAS, ALTURA_CANVAS, COR_FUNDO);
Framebuffer barra(LARGURA_CANVAS, ALTURA_BARRA, COR_BARRA);
Editor editor(canvas, COR_FUNDO);

// O GLUT mede y a partir do topo da janela; os framebuffers têm y = 0 embaixo
static int converter_y(int y_glut) {
    return ALTURA_JANELA - 1 - y_glut;
}

static void atualizar_titulo(const std::string& aviso = "") {
    int cor = indice_da_cor(editor.get_cor());
    std::string titulo = std::string("Mini Paint 2D - ") +
                         nome_ferramenta(editor.get_ferramenta()) + " | " +
                         nome_espessura(editor.get_espessura()) + " | " +
                         (cor >= 0 ? NOMES_CORES[cor] : "Cor personalizada");
    if (!aviso.empty()) {
        titulo += "  (" + aviso + ")";
    }
    glutSetWindowTitle(titulo.c_str());
}

static void salvar() {
    editor.salvar(ARQUIVO_SAIDA);
    atualizar_titulo(std::string("salvo em ") + ARQUIVO_SAIDA);
}

static void clicar_na_barra(int x, int y) {
    ElementoBarra e = elemento_em(x, y);
    switch (e.tipo) {
    case TipoElemento::FERRAMENTA:
        editor.selecionar_ferramenta(static_cast<Ferramenta>(e.indice));
        break;
    case TipoElemento::ESPESSURA:
        editor.selecionar_espessura(static_cast<Espessura>(e.indice));
        break;
    case TipoElemento::COR:
        editor.selecionar_cor(PALETA[e.indice]);
        break;
    case TipoElemento::NOVO:
        editor.novo();
        break;
    case TipoElemento::SALVAR:
        salvar();
        return;
    case TipoElemento::NENHUM:
        return;
    }
    atualizar_titulo();
}

// ---------------------------------------------------------------------------
// Callbacks do GLUT
// ---------------------------------------------------------------------------
static void display() {
    desenhar_barra(barra, editor);

    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glDrawPixels(canvas.get_width(), canvas.get_height(), GL_RGB, GL_UNSIGNED_BYTE, canvas.get_data());
    glRasterPos2i(0, ALTURA_CANVAS);
    glDrawPixels(barra.get_width(), barra.get_height(), GL_RGB, GL_UNSIGNED_BYTE, barra.get_data());
    glutSwapBuffers();
}

// A janela tem tamanho fixo: se o usuário redimensionar, volta ao tamanho original
static void reshape(int w, int h) {
    if (w != LARGURA_JANELA || h != ALTURA_JANELA) {
        glutReshapeWindow(LARGURA_JANELA, ALTURA_JANELA);
    }
    glViewport(0, 0, LARGURA_JANELA, ALTURA_JANELA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_JANELA, 0, ALTURA_JANELA, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Clique e soltar
static void mouse(int botao, int estado, int x, int y) {
    if (botao != GLUT_LEFT_BUTTON) return;

    y = converter_y(y);
    if (estado == GLUT_DOWN) {
        if (y >= ALTURA_CANVAS) {
            clicar_na_barra(x, y - ALTURA_CANVAS);
        } else {
            editor.pressionar(x, y);
        }
    } else {
        editor.soltar(x, y);
    }
    glutPostRedisplay();
}

// Arrasto (mouse em movimento com botão pressionado)
static void arrasto(int x, int y) {
    editor.arrastar(x, converter_y(y));
    glutPostRedisplay();
}

// Movimento sem botão: só troca o cursor entre canvas e barra
static void movimento_passivo(int x, int y) {
    (void)x;
    static int cursor_atual = -1;
    int cursor = (converter_y(y) < ALTURA_CANVAS) ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_LEFT_ARROW;
    if (cursor != cursor_atual) {
        glutSetCursor(cursor);
        cursor_atual = cursor;
    }
}

static void teclado(unsigned char tecla, int x, int y) {
    (void)x;
    (void)y;
    switch (tecla) {
    case 27:  editor.cancelar(); break;  // ESC
    case 'n': case 'N': editor.novo(); break;
    case 's': case 'S': salvar(); glutPostRedisplay(); return;
    case '1': editor.selecionar_espessura(Espessura::FINO); break;
    case '2': editor.selecionar_espessura(Espessura::MEDIO); break;
    case '3': editor.selecionar_espessura(Espessura::GROSSO); break;
    case 'p': editor.selecionar_ferramenta(Ferramenta::LAPIS); break;
    case 'b': editor.selecionar_ferramenta(Ferramenta::BORRACHA); break;
    case 'l': editor.selecionar_ferramenta(Ferramenta::LINHA); break;
    case 'r': editor.selecionar_ferramenta(Ferramenta::RETANGULO); break;
    case 'R': editor.selecionar_ferramenta(Ferramenta::RETANGULO_CHEIO); break;
    case 'c': editor.selecionar_ferramenta(Ferramenta::CIRCULO); break;
    case 'C': editor.selecionar_ferramenta(Ferramenta::CIRCULO_CHEIO); break;
    case 'f': editor.selecionar_ferramenta(Ferramenta::BALDE); break;
    default: return;
    }
    atualizar_titulo();
    glutPostRedisplay();
}

static void imprimir_ajuda() {
    std::printf(
        "Mini Paint 2D\n"
        "  Mouse: clique na barra para escolher ferramenta, espessura e cor.\n"
        "         Clique/arraste no canvas para desenhar.\n"
        "  Teclas: p lapis | b borracha | l linha | r/R retangulo vazado/cheio\n"
        "          c/C circulo vazado/cheio | f balde | 1/2/3 espessura\n"
        "          n novo | s salvar (%s) | ESC cancela a forma em andamento\n",
        ARQUIVO_SAIDA);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);
    glutCreateWindow("Mini Paint 2D");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // linhas do framebuffer sem preenchimento extra

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(arrasto);
    glutPassiveMotionFunc(movimento_passivo);
    glutKeyboardFunc(teclado);

    atualizar_titulo();
    imprimir_ajuda();
    glutMainLoop();
    return 0;
}
