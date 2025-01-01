#include "graph_layout.h"

#include <map>
#include <unordered_map>

#include <emscripten/bind.h>
using namespace std;
using namespace emscripten;
using namespace graph_layout;

unordered_map<string, string> convert_js_object_to_unordered_map(val obj) {
    const auto keys = val::global("Object").call<val>("keys", obj);
    const int len = keys["length"].as<int>();
    unordered_map<string,string> m;
    for (int i = 0; i < len; ++i) {
        string key = keys[i].as<string>();
        string value = obj[key].as<string>();
        m[key] = value;
    }
    return m;
}

struct Coordinates {
    vector<double> xs;
    vector<double> ys;
};

class DirectedGraphLayoutJS {
public:
    DirectedGraphLayoutJS() {
        _graph = make_shared<SPDirectedGraph>(0);
    }

    void setGraphAttributes(val obj) {
        const auto attributes = convert_js_object_to_unordered_map(obj);
        _layout.attributes().setGraphAttributes(attributes);
    }

    void addVertex(const string& id, val obj) {
        const int u = static_cast<int>(_ids.size());
        _ids[id] = u;
        const auto attributes = convert_js_object_to_unordered_map(obj);
        _layout.attributes().setVertexAttributes(u, attributes);
        if (!attributes.contains(ATTRIBUTE_KEY_LABEL)) {
            _layout.attributes().setVertexAttributes(u, ATTRIBUTE_KEY_LABEL, id);
        }
        _graph->updateNumVertices(u + 1);
    }

    void addEdge(const string& id1, const string& id2, val obj) {
        const int u = _ids[id1];
        const int v = _ids[id2];
        const int id = _graph->addEdge(u, v);
        const auto attributes = convert_js_object_to_unordered_map(obj);
        for (const auto& [key, value] : attributes) {
            _layout.attributes().setEdgeAttributes(id, key, value);
        }
    }

    Coordinates layout() {
        _layout.setGraph(_graph);
        const auto [xs, ys] = _layout.layoutGraph();
        return {xs, ys};
    }
private:
    shared_ptr<SPDirectedGraph> _graph;
    DirectedGraphHierarchicalLayout _layout;
    unordered_map<string, int> _ids;
};

EMSCRIPTEN_BINDINGS(GraphLayoutWASM) {
    value_object<Coordinates>("Coordinates")
        .field("xs", &Coordinates::xs)
        .field("ys", &Coordinates::ys);
    register_vector<double>("VectorDouble");
    class_<DirectedGraphLayoutJS>("DirectedGraphLayout")
        .constructor<>()
        .function("setGraphAttributes", &DirectedGraphLayoutJS::setGraphAttributes)
        .function("addVertex", &DirectedGraphLayoutJS::addVertex)
        .function("addEdge", &DirectedGraphLayoutJS::addEdge)
        .function("layout", &DirectedGraphLayoutJS::layout)
        ;
}
