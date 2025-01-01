#include "common/graph_attributes.h"

#include <functional>
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

GraphAttributes GraphAttributes::stringMappingToAttributes(const unordered_map<string, string>& mapping) {
    GraphAttributes attributes;
    std::unordered_map<std::string, function<void(string)>> processors;
    processors[ATTRIBUTE_KEY_BGCOLOR] = [&attributes](const string& raw) {
        attributes.bgcolor.set(raw);
    };
    processors[ATTRIBUTE_KEY_RANK] = [&attributes](const string& raw) {
        attributes.rank = Rank::TopToBottom;
        if (raw == "BT") {
            attributes.rank = Rank::BottomToTop;
        } else if (raw == "LR") {
            attributes.rank = Rank::LeftToRight;
        } else if (raw == "RL") {
            attributes.rank = Rank::RightToLeft;
        }
    };
    for (const auto& [key, value] : mapping) {
        processors[key](value);
    }
    return attributes;
}

VertexAttributes VertexAttributes::stringMappingToAttributes(const unordered_map<string, string>& mapping) {
    const VertexAttributes attributes;
    return stringMappingToAttributes(mapping, attributes);
}

VertexAttributes VertexAttributes::stringMappingToAttributes(const unordered_map<string, string>& mapping, const VertexAttributes& defaultAttributes) {
    VertexAttributes attributes = defaultAttributes;
    std::unordered_map<std::string, function<void(string)>> processors;
    processors[ATTRIBUTE_KEY_LABEL] = [&attributes](const string& raw) {
        attributes.label = raw;
    };
    processors[ATTRIBUTE_KEY_SHAPE] = [&attributes](const string& raw) {
        attributes.shape = Shape::Circle;
        if (raw == "none") {
            attributes.shape = Shape::None;
        } else if (raw == "doublecircle") {
            attributes.shape = Shape::DoubleCircle;
        }
    };
    for (const auto& [key, value] : mapping) {
        processors[key](value);
    }
    return attributes;
}

EdgeAttributes EdgeAttributes::stringMappingToAttributes(const unordered_map<string, string>& mapping) {
    const EdgeAttributes attributes;
    return stringMappingToAttributes(mapping, attributes);
}

EdgeAttributes EdgeAttributes::stringMappingToAttributes(const unordered_map<string, string> &mapping, const EdgeAttributes& defaultAttributes) {
    EdgeAttributes attributes = defaultAttributes;
    std::unordered_map<std::string, function<void(string)>> processors;
    processors[ATTRIBUTE_KEY_LABEL] = [&attributes](const string& raw) {
        attributes.label = raw;
    };
    for (const auto& [key, value] : mapping) {
        processors[key](value);
    }
    return attributes;
}
