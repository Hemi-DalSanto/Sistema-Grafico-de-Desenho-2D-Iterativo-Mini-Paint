#include "Transformacoes.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

Point2D Transformacoes::translate(Point2D p, float dx, float dy) {
    return { p.x + dx, p.y + dy };
}

Point2D Transformacoes::rotate(Point2D p, float angle_degrees) {
    float rad = angle_degrees * (PI / 180.0f);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return {
        p.x * cosA - p.y * sinA,
        p.x * sinA + p.y * cosA
    };
}

Point2D Transformacoes::rotate_around_pivot(Point2D p, Point2D pivot, float angle_degrees) {
    Point2D translated = translate(p, -pivot.x, -pivot.y);
    Point2D rotated = rotate(translated, angle_degrees);
    return translate(rotated, pivot.x, pivot.y);
}

Point2D Transformacoes::scale(Point2D p, float sx, float sy) {
    return { p.x * sx, p.y * sy };
}

Point2D Transformacoes::scale_around_pivot(Point2D p, Point2D pivot, float sx, float sy) {
    Point2D translated = translate(p, -pivot.x, -pivot.y);
    Point2D scaled = scale(translated, sx, sy);
    return translate(scaled, pivot.x, pivot.y);
}
