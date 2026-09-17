#include "rectangle.h"
#include <algorithm>

Rectangle::Rectangle(Point a, Point b)
    : bottomLeft_{ std::min(a.x, b.x), std::min(a.y, b.y) },
    topRight_{ std::max(a.x, b.x), std::max(a.y, b.y) } {
}

double Rectangle::getArea() const {
    return (topRight_.x - bottomLeft_.x) * (topRight_.y - bottomLeft_.y);
}

Point Rectangle::getCenter() const {
    return {
        (bottomLeft_.x + topRight_.x) / 2.0,
        (bottomLeft_.y + topRight_.y) / 2.0
    };
}

void Rectangle::move(double dx, double dy) {
    bottomLeft_.x += dx;
    bottomLeft_.y += dy;
    topRight_.x += dx;
    topRight_.y += dy;
}

void Rectangle::scale(double factor) {
    Point c = getCenter();
    bottomLeft_.x = c.x + (bottomLeft_.x - c.x) * factor;
    bottomLeft_.y = c.y + (bottomLeft_.y - c.y) * factor;
    topRight_.x = c.x + (topRight_.x - c.x) * factor;
    topRight_.y = c.y + (topRight_.y - c.y) * factor;
}

std::string Rectangle::getName() const {
    return "RECTANGLE";
}

std::pair<Point, Point> Rectangle::getBounds() const {
    return { bottomLeft_, topRight_ };
}
