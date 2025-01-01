#ifndef GRAPHLAYOUT_GRAPH_ATTRIBUTES_H
#define GRAPHLAYOUT_GRAPH_ATTRIBUTES_H

#include <string>

namespace graph_layout {

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
    };

    struct VertexAttributes {
        enum class Shape {
            None, Circle, DoubleCircle
        };
        Shape shape = Shape::Circle;
    };

}

#endif //GRAPHLAYOUT_GRAPH_ATTRIBUTES_H