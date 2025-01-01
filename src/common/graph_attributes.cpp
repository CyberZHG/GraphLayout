#include "common/graph_attributes.h"
using namespace std;
using namespace graph_layout;

void Color::set(const std::string& color) {
    raw = color;
}

bool Color::isNone() const {
    return raw.empty();
}

std::tuple<double, double, double> Color::toRGB() const {
    if (raw == "white") {
        return make_tuple(1.0, 1.0, 1.0);
    }
    return make_tuple(0.0, 0.0, 0.0);
}
