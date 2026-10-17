#ifndef TRANSFORMACOES_H
#define TRANSFORMACOES_H

struct Point2D {
    float x, y;
};

class Transformacoes {
public:
    static Point2D translate(Point2D p, float dx, float dy);
    static Point2D rotate(Point2D p, float angle_degrees);
    static Point2D rotate_around_pivot(Point2D p, Point2D pivot, float angle_degrees);
    static Point2D scale(Point2D p, float sx, float sy);
    static Point2D scale_around_pivot(Point2D p, Point2D pivot, float sx, float sy);
};

#endif
