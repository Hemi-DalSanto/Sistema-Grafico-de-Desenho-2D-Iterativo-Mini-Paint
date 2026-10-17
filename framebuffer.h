#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <vector>
#include <string>

struct Color {
    unsigned char r, g, b;
};

class Framebuffer {
private:
    int width;
    int height;
    std::vector<unsigned char> buffer;
    Color backgroundColor;

public:
    Framebuffer(int w, int h, Color bg);
    void put_pixel(int x, int y, Color cor);
    Color get_pixel(int x, int y);

    void clear_canvas();
    void export_ppm(const std::string& filename);
    const unsigned char* get_data() const;
    int get_width() const { return width; }
    int get_height() const { return height; }
};

#endif
