#ifndef COMPOSITE_H
#define COMPOSITE_H

#include "shape.h"
#include <memory>
#include <vector>

class CompositeShape : public Shape {
    std::vector<std::unique_ptr<Shape>> shapes_;

public:
    void add(std::unique_ptr<Shape> shape);

    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
    std::pair<Point, Point> getBounds() const override;

    const std::vector<std::unique_ptr<Shape>>& getShapes() const;
};

#endif
