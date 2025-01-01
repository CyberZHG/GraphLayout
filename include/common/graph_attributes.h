#ifndef GRAPHLAYOUT_GRAPH_ATTRIBUTES_H
#define GRAPHLAYOUT_GRAPH_ATTRIBUTES_H

#include <string>
#include <unordered_map>

namespace graph_layout {

    constexpr std::string ATTRIBUTE_KEY_BG_COLOR = "bgcolor";
    constexpr std::string ATTRIBUTE_KEY_RANK_DIR = "rankdir";
    constexpr std::string ATTRIBUTE_KEY_LABEL = "label";
    constexpr std::string ATTRIBUTE_KEY_SHAPE = "shape";

    struct Color {
        std::string raw;

        void set(const std::string& color);
        [[nodiscard]] bool isNone() const;
        [[nodiscard]] std::tuple<double, double, double> toRGB() const;
    };

    struct GraphAttributes {
        Color backgroundColor;
        enum class RankDirection {
            TopToBottom, BottomToTop, LeftToRight, RightToLeft,
        };
        RankDirection rankDirection = RankDirection::TopToBottom;

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

    class Attributes {
    public:
        Attributes() = default;
        ~Attributes() = default;

        GraphAttributes& graphAttributes();
        [[nodiscard]] const GraphAttributes& graphAttributes() const;

        VertexAttributes& vertexAttributes();
        [[nodiscard]] VertexAttributes vertexAttributes(int u) const;
        void setVertexAttributes(int u, const std::string& key, const std::string& value);

        EdgeAttributes& edgeAttributes();
        [[nodiscard]] EdgeAttributes edgeAttributes(int u) const;
        void setEdgeAttributes(int u, const std::string& key, const std::string& value);
        void setEdgeAttributes(int u, const std::unordered_map<std::string, std::string>& mapping);
        void transferEdgeAttributes(int u, int v);

    private:
        GraphAttributes _graphAttributes;
        VertexAttributes _vertexGlobalAttributes;
        std::unordered_map<int, std::unordered_map<std::string, std::string>> _vertexAttributes;
        EdgeAttributes _edgeGlobalAttributes;
        std::unordered_map<int, std::unordered_map<std::string, std::string>> _edgeAttributes;
    };

}

#endif //GRAPHLAYOUT_GRAPH_ATTRIBUTES_H