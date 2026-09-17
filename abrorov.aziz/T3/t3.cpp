#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <functional>
#include <map>

using namespace std::placeholders;

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

struct Polygon {
    std::vector<Point> points;

    size_t size() const { return points.size(); }

    double area() const {
        if (points.size() < 3) return 0.0;
        long long sum = std::accumulate(points.begin(), points.end(), 0LL,
            [this, i = 0](long long acc, const Point& a) mutable {
                const Point& b = points[(i + 1) % points.size()];
                i++;
                return acc + static_cast<long long>(a.x) * b.y - static_cast<long long>(a.y) * b.x;
            });
        return std::abs(sum) / 2.0;
    }
};

std::vector<Point> normalize(const Polygon& p) {
    if (p.points.empty()) return {};

    auto minx = std::min_element(p.points.begin(), p.points.end(),
        [](const Point& a, const Point& b) { return a.x < b.x; })->x;
    auto miny = std::min_element(p.points.begin(), p.points.end(),
        [](const Point& a, const Point& b) { return a.y < b.y; })->y;

    std::vector<Point> res(p.points.size());
    std::transform(p.points.begin(), p.points.end(), res.begin(),
        [minx, miny](const Point& pt) {
            return Point{ pt.x - minx, pt.y - miny };
        });
    std::sort(res.begin(), res.end());
    return res;
}

bool parsePolygon(const std::string& s, Polygon& poly) {
    std::istringstream iss(s);
    int n;
    if (!(iss >> n)) return false;
    if (n < 3) return false;

    std::vector<Point> pts;
    pts.reserve(n);

    for (int i = 0; i < n; ++i) {
        char c;
        if (!(iss >> c) || c != '(') return false;
        int x, y;
        if (!(iss >> x)) return false;
        if (!(iss >> c) || c != ';') return false;
        if (!(iss >> y)) return false;
        if (!(iss >> c) || c != ')') return false;
        pts.push_back({ x, y });
    }

    std::string extra;
    if (iss >> extra) return false;

    poly.points = std::move(pts);
    return true;
}


void cmdArea(const std::vector<Polygon>& polys, std::istringstream& iss) {
    std::string param;
    if (!(iss >> param)) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    double sum = 0.0;
    if (param == "EVEN") {
        sum = std::accumulate(polys.begin(), polys.end(), 0.0,
            std::bind(std::plus<double>(), _1,
                std::bind(std::multiplies<double>(),
                    std::bind(std::equal_to<size_t>(),
                        std::bind(std::modulus<size_t>(), std::bind(&Polygon::size, _2), 2), 0),
                    std::bind(&Polygon::area, _2)
                )
            )
        );
    }
    else if (param == "ODD") {
        sum = std::accumulate(polys.begin(), polys.end(), 0.0,
            std::bind(std::plus<double>(), _1,
                std::bind(std::multiplies<double>(),
                    std::bind(std::not_equal_to<size_t>(),
                        std::bind(std::modulus<size_t>(), std::bind(&Polygon::size, _2), 2), 0),
                    std::bind(&Polygon::area, _2)
                )
            )
        );
    }
    else if (param == "MEAN") {
        if (polys.empty()) {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
        sum = std::accumulate(polys.begin(), polys.end(), 0.0,
            std::bind(std::plus<double>(), _1, std::bind(&Polygon::area, _2)));
        sum /= polys.size();
    }
    else {
        try {
            size_t n = std::stoul(param);
            if (n < 3) {
                std::cout << "<INVALID COMMAND>\n";
                return;
            }
            sum = std::accumulate(polys.begin(), polys.end(), 0.0,
                std::bind(std::plus<double>(), _1,
                    std::bind(std::multiplies<double>(),
                        std::bind(std::equal_to<size_t>(), std::bind(&Polygon::size, _2), n),
                        std::bind(&Polygon::area, _2)
                    )
                )
            );
        }
        catch (...) {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
    }
    std::cout << std::fixed << std::setprecision(1) << sum << "\n";
}

void cmdMax(const std::vector<Polygon>& polys, std::istringstream& iss) {
    std::string param;
    if (!(iss >> param)) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    if (param == "AREA") {
        if (polys.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
        auto it = std::max_element(polys.begin(), polys.end(),
            std::bind(std::less<double>(), std::bind(&Polygon::area, _1), std::bind(&Polygon::area, _2)));
        std::cout << std::fixed << std::setprecision(1) << it->area() << "\n";
    }
    else if (param == "VERTEXES") {
        if (polys.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
        auto it = std::max_element(polys.begin(), polys.end(),
            std::bind(std::less<size_t>(), std::bind(&Polygon::size, _1), std::bind(&Polygon::size, _2)));
        std::cout << it->size() << "\n";
    }
    else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void cmdMin(const std::vector<Polygon>& polys, std::istringstream& iss) {
    std::string param;
    if (!(iss >> param)) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    if (param == "AREA") {
        if (polys.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
        auto it = std::min_element(polys.begin(), polys.end(),
            std::bind(std::less<double>(), std::bind(&Polygon::area, _1), std::bind(&Polygon::area, _2)));
        std::cout << std::fixed << std::setprecision(1) << it->area() << "\n";
    }
    else if (param == "VERTEXES") {
        if (polys.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
        auto it = std::min_element(polys.begin(), polys.end(),
            std::bind(std::less<size_t>(), std::bind(&Polygon::size, _1), std::bind(&Polygon::size, _2)));
        std::cout << it->size() << "\n";
    }
    else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void cmdCount(const std::vector<Polygon>& polys, std::istringstream& iss) {
    std::string param;
    if (!(iss >> param)) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    size_t cnt = 0;
    if (param == "EVEN") {
        cnt = std::count_if(polys.begin(), polys.end(),
            std::bind(std::equal_to<size_t>(),
                std::bind(std::modulus<size_t>(), std::bind(&Polygon::size, _1), 2), 0));
    }
    else if (param == "ODD") {
        cnt = std::count_if(polys.begin(), polys.end(),
            std::bind(std::not_equal_to<size_t>(),
                std::bind(std::modulus<size_t>(), std::bind(&Polygon::size, _1), 2), 0));
    }
    else {
        try {
            size_t n = std::stoul(param);
            if (n < 3) {
                std::cout << "<INVALID COMMAND>\n";
                return;
            }
            cnt = std::count_if(polys.begin(), polys.end(),
                std::bind(std::equal_to<size_t>(), std::bind(&Polygon::size, _1), n));
        }
        catch (...) {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
    }
    std::cout << cnt << "\n";
}

void cmdLessArea(const std::vector<Polygon>& polys, std::istringstream& iss) {
    std::string rest;
    std::getline(iss, rest);
    Polygon target;
    if (!parsePolygon(rest, target)) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }
    double ta = target.area();
    size_t cnt = std::count_if(polys.begin(), polys.end(),
        std::bind(std::less<double>(), std::bind(&Polygon::area, _1), ta));
    std::cout << cnt << "\n";
}

void cmdSame(const std::vector<Polygon>& polys, std::istringstream& iss) {
    std::string rest;
    std::getline(iss, rest);
    Polygon target;
    if (!parsePolygon(rest, target)) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }
    auto normTarget = normalize(target);
    size_t cnt = std::count_if(polys.begin(), polys.end(),
        std::bind(std::equal_to<std::vector<Point>>(),
            std::bind(normalize, _1), normTarget));
    std::cout << cnt << "\n";
}
void processCommands(const std::vector<Polygon>& polys) {
    // Карта команд: строка -> функция-обработчик
    std::map<std::string, std::function<void(const std::vector<Polygon>&, std::istringstream&)>> commands = {
        {"AREA", cmdArea},
        {"MAX", cmdMax},
        {"MIN", cmdMin},
        {"COUNT", cmdCount},
        {"LESSAREA", cmdLessArea},
        {"SAME", cmdSame}
    };

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        auto it = commands.find(cmd);
        if (it != commands.end()) {
            it->second(polys, iss);
        }
        else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: filename not provided\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: cannot open file\n";
        return 1;
    }

    std::vector<Polygon> polys;
    std::string line;
    while (std::getline(file, line)) {
        Polygon p;
        if (parsePolygon(line, p)) {
            polys.push_back(p);
        }
    }
    file.close();

    processCommands(polys);
    return 0;
}
