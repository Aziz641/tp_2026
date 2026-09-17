#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>

struct DataStruct {
    unsigned long long key1;  // ULL LIT
    double             key2;  // DBL LIT
    std::string        key3;
};

// -------- Вспомогательные обёртки для ввода/вывода --------
struct DelimiterIO { char exp; };
struct ULLIO { unsigned long long& ref; };
struct DBLIO { double& ref; };
struct StringIO { std::string& ref; };

class iofmtguard {
public:
    iofmtguard(std::basic_ios<char>& s)
        : s_(s), width_(s.width()), fill_(s.fill()),
        precision_(s.precision()), fmt_(s.flags()) {
    }
    ~iofmtguard() {
        s_.width(width_);
        s_.fill(fill_);
        s_.precision(precision_);
        s_.flags(fmt_);
    }
private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
};

// -------- Одиночный символ --------
std::istream& operator>>(std::istream& in, DelimiterIO&& d) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c = 0;
    if (in >> c && c != d.exp) in.setstate(std::ios::failbit);
    return in;
}

// -------- ULL LIT: 10u, 10l, 10ll, 10ull (суффикс необязателен) --------
std::istream& operator>>(std::istream& in, ULLIO&& d) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string tok;
    while (in && in.peek() != ':' && in.peek() != EOF) tok += in.get();

    size_t i = 0;
    if (i < tok.size() && tok[i] == '+') ++i;
    size_t ds = i;
    while (i < tok.size() && std::isdigit((unsigned char)tok[i])) ++i;
    if (i == ds) { in.setstate(std::ios::failbit); return in; }

    std::string digits = tok.substr(ds, i - ds);
    std::string suf = tok.substr(i);

    int u = 0, l = 0;
    for (char c : suf) {
        if (c == 'u' || c == 'U') ++u;
        else if (c == 'l' || c == 'L') ++l;
        else { in.setstate(std::ios::failbit); return in; }
    }
    if (u > 1 || l > 2) { in.setstate(std::ios::failbit); return in; }

    try { d.ref = std::stoull(digits); }
    catch (...) { in.setstate(std::ios::failbit); }
    return in;
}

// -------- DBL LIT: 50.0d / 50.0D --------
std::istream& operator>>(std::istream& in, DBLIO&& d) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string tok;
    while (in && in.peek() != ':' && in.peek() != EOF) tok += in.get();

    size_t i = 0;
    if (i < tok.size() && (tok[i] == '+' || tok[i] == '-')) ++i;
    size_t ds = i;
    while (i < tok.size() && std::isdigit((unsigned char)tok[i])) ++i;
    if (i == ds) { in.setstate(std::ios::failbit); return in; }

    if (i >= tok.size() || tok[i] != '.') { in.setstate(std::ios::failbit); return in; }
    ++i;
    size_t fs = i;
    while (i < tok.size() && std::isdigit((unsigned char)tok[i])) ++i;
    if (i == fs) { in.setstate(std::ios::failbit); return in; }

    if (i >= tok.size() || (tok[i] != 'd' && tok[i] != 'D')) { in.setstate(std::ios::failbit); return in; }
    ++i;
    if (i != tok.size()) { in.setstate(std::ios::failbit); return in; }

    try { d.ref = std::stod(tok.substr(0, tok.size() - 1)); }
    catch (...) { in.setstate(std::ios::failbit); }
    return in;
}

// -------- Строка в кавычках --------
std::istream& operator>>(std::istream& in, StringIO&& d) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    in >> DelimiterIO{ '"' };
    if (!in) return in;
    d.ref.clear();
    char c;
    while (in.get(c) && c != '"') d.ref += c;
    if (!in) in.setstate(std::ios::failbit);
    return in;
}

// -------- Разбор одной записи (порядок полей произвольный) --------
static bool parseRecord(std::istringstream& iss, DataStruct& dest) {
    DataStruct temp;
    bool has1 = false, has2 = false, has3 = false;

    iss >> DelimiterIO{ '(' };
    if (!iss) return false;

    while (iss) {
        iss >> DelimiterIO{ ':' };
        if (!iss) return false;

        if (iss.peek() == ')') {          // конец записи
            iss.get();
            if (has1 && has2 && has3) {
                dest = std::move(temp);
                return true;
            }
            return false;
        }

        // читаем имя поля до пробела
        std::string name;
        while (iss && iss.peek() != ' ' && iss.peek() != EOF)
            name += iss.get();
        iss >> DelimiterIO{ ' ' };
        if (!iss) return false;

        if (name == "key1") { iss >> ULLIO{ temp.key1 };    has1 = true; }
        else if (name == "key2") { iss >> DBLIO{ temp.key2 };    has2 = true; }
        else if (name == "key3") { iss >> StringIO{ temp.key3 }; has3 = true; }
        else return false;

        if (!iss) return false;
    }
    return false;
}

// -------- operator>> для DataStruct --------
// Читаем построчно; строки с неподдерживаемым форматом пропускаем.
std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        DataStruct temp;
        if (parseRecord(iss, temp)) {
            dest = std::move(temp);
            return in;
        }
        // строка не подошла — читаем следующую
    }
    in.setstate(std::ios::failbit);
    return in;
}

// -------- operator<< для DataStruct --------
std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    iofmtguard fmtguard(out);
    out << "(:key1 " << src.key1 << "ull"
        << ":key2 " << std::fixed << std::setprecision(1) << src.key2 << "d"
        << ":key3 \"" << src.key3 << "\":)";
    return out;
}

// -------- main --------
int main() {
    std::vector<DataStruct> data;

    std::copy(std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data));

    std::sort(data.begin(), data.end(),
        [](const DataStruct& a, const DataStruct& b) {
            if (a.key1 != b.key1) return a.key1 < b.key1;
            if (a.key2 != b.key2) return a.key2 < b.key2;
            return a.key3.length() < b.key3.length();
        });

    std::copy(data.begin(), data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
