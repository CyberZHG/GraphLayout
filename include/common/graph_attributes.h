#ifndef GRAPHLAYOUT_GRAPH_ATTRIBUTES_H
#define GRAPHLAYOUT_GRAPH_ATTRIBUTES_H

#include <string>
#include <unordered_map>

namespace graph_layout {

    constexpr std::string ATTRIBUTE_KEY_BG_COLOR = "bgcolor";
    constexpr std::string ATTRIBUTE_KEY_RANK_DIR = "rankdir";
    constexpr std::string ATTRIBUTE_KEY_LABEL = "label";
    constexpr std::string ATTRIBUTE_KEY_SHAPE = "shape";

    class Attribute {
    public:
        Attribute();
        explicit Attribute(const std::string& value);
        ~Attribute() = default;

        void set(const std::string& value);
        [[nodiscard]] const std::string& value() const;

    protected:
        std::string _raw;
    };

    class AttributeColor : public Attribute {
    public:
        using Attribute::Attribute;

        [[nodiscard]] static std::tuple<double, double, double> toRGB(const std::string& raw);
    };

    class AttributeRankDir : public Attribute {
    public:
        using Attribute::Attribute;

        static constexpr std::string TOP_TO_BOTTOM = "TB";
        static constexpr std::string BOTTOM_TO_TOP = "BT";
        static constexpr std::string LEFT_TO_RIGHT = "LR";
        static constexpr std::string RIGHT_TO_LEFT = "RL";
    };

    class AttributeShape : public Attribute {
    public:
        using Attribute::Attribute;

        static constexpr std::string NONE = "none";
        static constexpr std::string CIRCLE = "circle";
        static constexpr std::string DOUBLE_CIRCLE = "doublecircle";
    };

    class Attributes {
    public:
        Attributes() = default;
        ~Attributes() = default;

        [[nodiscard]] std::string graphAttributes(const std::string& key) const;
        void setGraphAttributes(const std::string& key, const std::string& value);

        [[nodiscard]] std::string vertexAttributes(int u, const std::string& key) const;
        void setVertexAttributes(int u, const std::string& key, const std::string& value);

        [[nodiscard]] std::string edgeAttributes(int u, const std::string& key) const;
        void setEdgeAttributes(int u, const std::string& key, const std::string& value);
        void setEdgeAttributes(int u, const std::unordered_map<std::string, std::string>& mapping);
        void transferEdgeAttributes(int u, int v);

        [[nodiscard]] std::string rankDir() const;
        void setRankDir(const std::string& value);

    private:
        static std::unordered_map<std::string, std::string> DEFAULT_GRAPH_ATTRIBUTE_VALUES;
        static std::unordered_map<std::string, std::string> DEFAULT_VERTEX_ATTRIBUTE_VALUES;
        static std::unordered_map<std::string, std::string> DEFAULT_EDGE_ATTRIBUTE_VALUES;

        std::unordered_map<std::string, std::string> _graphAttributes;
        std::unordered_map<std::string, std::string> _vertexGlobalAttributes;
        std::unordered_map<int, std::unordered_map<std::string, std::string>> _vertexAttributes;
        std::unordered_map<std::string, std::string> _edgeGlobalAttributes;
        std::unordered_map<int, std::unordered_map<std::string, std::string>> _edgeAttributes;
    };

}

#endif //GRAPHLAYOUT_GRAPH_ATTRIBUTES_H