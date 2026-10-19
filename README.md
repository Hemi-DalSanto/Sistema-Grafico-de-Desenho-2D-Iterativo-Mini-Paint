# Sistema-Grafico-de-Desenho-2D-Iterativo-Mini-Paint
Projeto desenvolvimento para prática  do curso de Computação Gráfica

## Como compilar e executar

O projeto usa C++17 e GLUT (só para abrir a janela e copiar o framebuffer para a tela com `glDrawPixels`).

```bash
make        # compila (gera ./minipaint)
make run    # compila e executa
make clean  # apaga o executável e o desenho.ppm
```

- **macOS:** basta ter as Command Line Tools (`xcode-select --install`); o GLUT já vem no sistema.
- **Linux:** instale o freeglut (`sudo apt install freeglut3-dev`).
- **Windows (MinGW):** instale o freeglut e rode `mingw32-make`.

## Controles

| Ação | Mouse | Teclado |
|---|---|---|
| Lápis / Borracha | botões da barra | `p` / `b` |
| Linha | botão da barra | `l` |
| Retângulo vazado / cheio | botões da barra | `r` / `R` |
| Círculo vazado / cheio | botões da barra | `c` / `C` |
| Balde de tinta | botão da barra | `f` |
| Espessura fina / média / grossa | botões da barra | `1` / `2` / `3` |
| Cor | paleta de 8 cores | — |
| Novo (limpar) | botão NOVO | `n` |
| Salvar (`desenho.ppm`) | botão SALVAR | `s` |
| Cancelar forma em andamento | — | `ESC` |

Linha, retângulo e círculo são definidos clicando e arrastando: o primeiro clique é o canto (ou o centro, no círculo) e a forma acompanha o mouse até soltar o botão.

## Organização do código

| Arquivo | Conteúdo |
|---|---|
| `framebuffer.h/.cpp` | Matriz de pixels, `put_pixel`/`get_pixel`, limpar e exportar PPM |
| `rasterizacao.h/.cpp` | Bresenham (linha), ponto médio (círculo), retângulos e flood fill |
| `transformacoes.h/.cpp` | Translação, rotação e escala de pontos |
| `ferramentas.h/.cpp` | Ferramentas, espessuras e desenho com pincel espesso |
| `editor.h/.cpp` | Máquina de estados do mouse (clique, arrasto, soltar) |
| `interface.h/.cpp` | Barra de ferramentas, ícones, paleta e botões Novo/Salvar |
| `main.cpp` | Janela GLUT e loop de eventos |
