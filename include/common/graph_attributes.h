#ifndef GRAPHLAYOUT_GRAPH_ATTRIBUTES_H
#define GRAPHLAYOUT_GRAPH_ATTRIBUTES_H

#include <string>
#include <unordered_map>

namespace graph_layout {

    constexpr std::string ATTRIBUTE_KEY_BGCOLOR = "bgcolor";
    constexpr std::string ATTRIBUTE_KEY_RANK = "rank";
    constexpr std::string ATTRIBUTE_KEY_LABEL = "label";
    constexpr std::string ATTRIBUTE_KEY_SHAPE = "shape";

    struct Color {
        std::string raw;

        void set(const std::string& color);
        [[nodiscard]] bool isNone() const;
        [[nodiscard]] std::tuple<double, double, double> toRGB() const;
    };

    struct GraphAttributes {
        Color bgcolor;
        enum class Rank {
            TopToBottom, BottomToTop, LeftToRight, RightToLeft,
        };
        Rank rank = Rank::TopToBottom;

        static GraphAttributes stringMappingToAttributes(const std::unordered_map<std::string, std::string>& mapping);
    };

    struct VertexAttributes {
        std::string label;
        enum class Shape {
            None, Circle, DoubleCircle
        };
        Shape shape = Shape::Circle;

        static VertexAttributes stringMappingToAttributes(const std::unordered_map<std::string, std::string>& mapping);
        static VertexAttributes stringMappingToAttributes(const std::unordered_map<std::string, std::string>& mapping, const VertexAttributes& defaultAttributes);
    };

    struct EdgeAttributes {
        std::string label;

        static EdgeAttributes stringMappingToAttributes(const std::unordered_map<std::string, std::string>& mapping);
        static EdgeAttributes stringMappingToAttributes(const std::unordered_map<std::string, std::string>& mapping, const EdgeAttributes& defaultAttributes);
    };

}

#endif //GRAPHLAYOUT_GRAPH_ATTRIBUTES_H