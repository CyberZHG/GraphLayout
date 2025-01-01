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
    processors[ATTRIBUTE_KEY_BG_COLOR] = [&attributes](const string& raw) {
        attributes.backgroundColor.set(raw);
    };
    processors[ATTRIBUTE_KEY_RANK_DIR] = [&attributes](const string& raw) {
        attributes.rankDirection = RankDirection::TopToBottom;
        if (raw == "BT") {
            attributes.rankDirection = RankDirection::BottomToTop;
        } else if (raw == "LR") {
            attributes.rankDirection = RankDirection::LeftToRight;
        } else if (raw == "RL") {
            attributes.rankDirection = RankDirection::RightToLeft;
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

GraphAttributes& Attributes::graphAttributes() {
    return _graphAttributes;
}

const GraphAttributes & Attributes::graphAttributes() const {
    return _graphAttributes;
}

VertexAttributes& Attributes::vertexAttributes() {
    return _vertexGlobalAttributes;
}

VertexAttributes Attributes::vertexAttributes(const int u) const {
    if (const auto it = _vertexAttributes.find(u); it != _vertexAttributes.end()) {
        return VertexAttributes::stringMappingToAttributes(it->second, _vertexGlobalAttributes);
    }
    return _vertexGlobalAttributes;
}

void Attributes::setVertexAttributes(const int u, const std::string &key, const std::string &value) {
    _vertexAttributes[u][key] = value;
}

EdgeAttributes& Attributes::edgeAttributes() {
    return _edgeGlobalAttributes;
}

EdgeAttributes Attributes::edgeAttributes(const int u) const {
    if (const auto it = _edgeAttributes.find(u); it != _edgeAttributes.end()) {
        return EdgeAttributes::stringMappingToAttributes(it->second, _edgeGlobalAttributes);
    }
    return _edgeGlobalAttributes;
}

void Attributes::setEdgeAttributes(const int u, const std::string &key, const std::string &value) {
    _edgeAttributes[u][key] = value;
}

void Attributes::setEdgeAttributes(const int u, const unordered_map<string, string>& mapping) {
    _edgeAttributes[u] = mapping;
}

void Attributes::transferEdgeAttributes(const int u, const int v) {
    if (_edgeAttributes.contains(u)) {
        _edgeAttributes[v] = _edgeAttributes[u];
    }
}
