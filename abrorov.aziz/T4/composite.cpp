#include "composite.h"
#include <algorithm>
#include <limits>

void CompositeShape::add(std::unique_ptr<Shape> shape) {
    shapes_.push_back(std::move(shape));
}

double CompositeShape::getArea() const {
    double sum = 0.0;
    for (const auto& s : shapes_) sum += s->getArea();
    return sum;
}

std::pair<Point, Point> CompositeShape::getBounds() const {
    if (shapes_.empty()) return { {0.0, 0.0}, {0.0, 0.0} };

    Point min{ std::numeric_limits<double>::max(),
               std::numeric_limits<double>::max() };
    Point max{ std::numeric_limits<double>::lowest(),
               std::numeric_limits<double>::lowest() };

    for (const auto& s : shapes_) {
        auto b = s->getBounds();
        min.x = std::min(min.x, b.first.x);
        min.y = std::min(min.y, b.first.y);
        max.x = std::max(max.x, b.second.x);
        max.y = std::max(max.y, b.second.y);
    }
    return { min, max };
}

Point CompositeShape::getCenter() const {
    auto b = getBounds();
    return {
        (b.first.x + b.second.x) / 2.0,
        (b.first.y + b.second.y) / 2.0
    };
}

void CompositeShape::move(double dx, double dy) {
    for (auto& s : shapes_) s->move(dx, dy);
}

void CompositeShape::scale(double factor) {
    Point c = getCenter();
    for (auto& s : shapes_) {
        Point oldCenter = s->getCenter();
        s->scale(factor);
        Point newCenter{
            c.x + (oldCenter.x - c.x) * factor,
            c.y + (oldCenter.y - c.y) * factor
        };
        s->move(newCenter.x - oldCenter.x, newCenter.y - oldCenter.y);
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}

const std::vector<std::unique_ptr<Shape>>& CompositeShape::getShapes() const {
    return shapes_;
}