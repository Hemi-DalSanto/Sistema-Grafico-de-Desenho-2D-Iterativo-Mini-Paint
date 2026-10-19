CXXFLAGS = -std=c++17 -Wall -Wextra -O2
ALVO     = minipaint
FONTES   = main.cpp editor.cpp ferramentas.cpp interface.cpp \
           framebuffer.cpp rasterizacao.cpp transformacoes.cpp

# Bibliotecas do GLUT/OpenGL em cada sistema
ifeq ($(OS),Windows_NT)
    LIBS = -lfreeglut -lopengl32
    ALVO := $(ALVO).exe
else ifeq ($(shell uname -s),Darwin)
    LIBS = -framework GLUT -framework OpenGL
    CXXFLAGS += -DGL_SILENCE_DEPRECATION
else
    LIBS = -lglut -lGL
endif

$(ALVO): $(FONTES) *.h
	$(CXX) $(CXXFLAGS) $(FONTES) -o $(ALVO) $(LIBS)

run: $(ALVO)
	./$(ALVO)

clean:
	rm -f $(ALVO) desenho.ppm

.PHONY: run clean
