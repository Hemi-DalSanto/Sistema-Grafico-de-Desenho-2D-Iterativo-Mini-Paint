#include "Framebuffer.h"
#include <fstream>
#include <iostream>

Framebuffer::Framebuffer(int w, int h, Color bg) : width(w), height(h), backgroundColor(bg) {
    buffer.resize(width * height * 3);
    clear_canvas();
}

void Framebuffer::put_pixel(int x, int y, Color cor) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        int index = (y * width + x) * 3;
        buffer[index] = cor.r;
        buffer[index + 1] = cor.g;
        buffer[index + 2] = cor.b;
    }
}

Color Framebuffer::get_pixel(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        int index = (y * width + x) * 3;
        return { buffer[index], buffer[index + 1], buffer[index + 2] };
    }
    return backgroundColor;
}

void Framebuffer::clear_canvas() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            buffer[index] = backgroundColor.r;
            buffer[index + 1] = backgroundColor.g;
            buffer[index + 2] = backgroundColor.b;
        }
    }
}

void Framebuffer::export_ppm(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para salvar." << std::endl;
        return;
    }
    file << "P6\n" << width << " " << height << "\n255\n";

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            file.write(reinterpret_cast<const char*>(&buffer[index]), 3);
        }
    }
    file.close();
    std::cout << "Arquivo salvo com sucesso em " << filename << std::endl;
}

const unsigned char* Framebuffer::get_data() const {
    return buffer.data();
}
