#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "circle.h"
#include "composite.h"
#include "rectangle.h"

static void printShape(const Shape& shape, std::ostream& os) {
    os << std::fixed << std::setprecision(2);

    const auto* comp = dynamic_cast<const CompositeShape*>(&shape);

    if (comp == nullptr) {
        os << "[" << shape.getName()
            << ", (" << shape.getCenter().x << ", " << shape.getCenter().y << "), "
            << shape.getArea() << "]";
        return;
    }

    os << "[" << shape.getName()
        << ", (" << shape.getCenter().x << ", " << shape.getCenter().y << "), "
        << shape.getArea() << ":\n";

    const auto& parts = comp->getShapes();
    for (std::size_t i = 0; i < parts.size(); ++i) {
        const auto& p = parts[i];
        os << p->getName()
            << ", (" << p->getCenter().x << ", " << p->getCenter().y << "), "
            << p->getArea();
        if (i + 1 < parts.size()) os << ",\n";
        else                      os << "\n";
    }
    os << "]";
}

int main() {
    std::vector<std::unique_ptr<Shape>> figures;

    figures.push_back(std::make_unique<Rectangle>(Point{ 1.0, 1.0 }, Point{ 3.0, 4.0 }));
    figures.push_back(std::make_unique<Rectangle>(Point{ 0.0, 0.0 }, Point{ 2.0, 2.0 }));
    figures.push_back(std::make_unique<Circle>(Point{ 0.0, 0.0 }, 2.5));
    figures.push_back(std::make_unique<Circle>(Point{ 5.0, 5.0 }, 1.0));

    auto composite = std::make_unique<CompositeShape>();
    composite->add(std::make_unique<Rectangle>(Point{ 10.0, 10.0 }, Point{ 12.0, 11.0 }));
    composite->add(std::make_unique<Circle>(Point{ 11.0, 11.0 }, 0.5));
    figures.push_back(std::move(composite));

    std::cout << "Before scaling:\n";
    for (const auto& f : figures) {
        printShape(*f, std::cout);
        std::cout << "\n";
    }

    for (auto& f : figures) f->scale(2.0);

    std::cout << "\nAfter scaling by 2:\n";
    for (const auto& f : figures) {
        printShape(*f, std::cout);
        std::cout << "\n";
    }

    return 0;
}