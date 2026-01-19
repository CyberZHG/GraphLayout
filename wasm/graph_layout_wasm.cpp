#include "graph_layout.h"
#include "common/compare_svg.h"
#include <memory>
#include <emscripten/bind.h>
using namespace std;
using namespace emscripten;
using namespace graph_layout;

EMSCRIPTEN_BINDINGS(GraphLayoutWASM) {
    register_vector<string>("VectorString");
    emscripten::function("_compareSVG", &_compareSVG);
    enum_<FeedbackArcsMethod>("FeedbackArcsMethod")
        .value("EADES_93", FeedbackArcsMethod::EADES_93)
        .value("MIN_ID", FeedbackArcsMethod::MIN_ID)
    ;
    enum_<LayerAssignmentMethod>("LayerAssignmentMethod")
        .value("TOPOLOGICAL", LayerAssignmentMethod::TOPOLOGICAL)
        .value("MIN_NUM_OF_LAYERS", LayerAssignmentMethod::MIN_NUM_OF_LAYERS)
        .value("GANSNER_93", LayerAssignmentMethod::GANSNER_93)
        .value("MIN_TOTAL_EDGE_LENGTH", LayerAssignmentMethod::MIN_TOTAL_EDGE_LENGTH)
    ;
    enum_<CrossMinimizationMethod>("CrossMinimizationMethod")
        .value("BARYCENTER", CrossMinimizationMethod::BARYCENTER)
        .value("MEDIAN", CrossMinimizationMethod::MEDIAN)
        .value("PAIRWISE_SWITCH", CrossMinimizationMethod::PAIRWISE_SWITCH)
    ;
    enum_<VertexPositioningMethod>("VertexPositioningMethod")
        .value("BRANDES_KOPF", VertexPositioningMethod::BRANDES_KOPF)
    ;
    class_<SPDirectedGraph>("SPDirectedGraph")
        .constructor<size_t>()
        .smart_ptr<shared_ptr<SPDirectedGraph>>("SPDirectedGraph")
        .function("addEdge", select_overload<int(int, int)>(&SPDirectedGraph::addEdge))
    ;
    class_<Attribute>("Attribute")
        .constructor<>()
        .function("set", &Attribute::set)
        .function("value", &Attribute::value)
    ;
    class_<AttributeRankDir, base<Attribute>>("AttributeRankDir")
        .constructor<>()
        .class_property("TOP_TO_BOTTOM", &AttributeRankDir::TOP_TO_BOTTOM)
        .class_property("BOTTOM_TO_TOP", &AttributeRankDir::BOTTOM_TO_TOP)
        .class_property("LEFT_TO_RIGHT", &AttributeRankDir::LEFT_TO_RIGHT)
        .class_property("RIGHT_TO_LEFT", &AttributeRankDir::RIGHT_TO_LEFT)
    ;
    class_<AttributeShape, base<Attribute>>("AttributeShape")
        .constructor<>()
        .class_property("NONE", &AttributeShape::NONE)
        .class_property("CIRCLE", &AttributeShape::CIRCLE)
        .class_property("DOUBLE_CIRCLE", &AttributeShape::DOUBLE_CIRCLE)
        .class_property("ELLIPSE", &AttributeShape::ELLIPSE)
        .class_property("RECT", &AttributeShape::RECT)
        .class_property("RECORD", &AttributeShape::RECORD)
    ;
    class_<AttributeArrowShape, base<Attribute>>("AttributeArrowShape")
        .constructor<>()
        .class_property("NONE", &AttributeArrowShape::NONE)
        .class_property("NORMAL", &AttributeArrowShape::NORMAL)
        .class_property("EMPTY", &AttributeArrowShape::EMPTY)
    ;
    class_<Attributes>("Attributes")
        .constructor<>()
        .function("setRankDir", &Attributes::setRankDir)
        .function("setVertexShape", &Attributes::setVertexShape)
        .function("setEdgeTailLabel", &Attributes::setEdgeTailLabel)
        .function("setEdgeHeadLabel", &Attributes::setEdgeHeadLabel)
        .function("setEdgeLabelDistance", &Attributes::setEdgeLabelDistance)
        .function("setVertexDefaultShape", &Attributes::setVertexDefaultShape)
        .function("setVertexDefaultMonospace", &Attributes::setVertexDefaultMonospace)
        .function("setEdgeDefaultMonospace", &Attributes::setEdgeDefaultMonospace)
        .function("setEdgeDefaultArrowHead", &Attributes::setEdgeDefaultArrowHead)
        .function("setEdgeDefaultArrowTail", &Attributes::setEdgeDefaultArrowTail)
        .function("setEdgeArrowHead", &Attributes::setEdgeArrowHead)
        .function("setEdgeArrowTail", &Attributes::setEdgeArrowTail)
        .function("setVertexDefaultColor", &Attributes::setVertexDefaultColor)
        .function("setVertexDefaultFillColor", &Attributes::setVertexDefaultFillColor)
        .function("setVertexDefaultFontColor", &Attributes::setVertexDefaultFontColor)
        .function("setEdgeDefaultColor", &Attributes::setEdgeDefaultColor)
        .function("setEdgeDefaultFontColor", &Attributes::setEdgeDefaultFontColor)
        .function("setVertexColor", &Attributes::setVertexColor)
        .function("setVertexFillColor", &Attributes::setVertexFillColor)
        .function("setVertexFontColor", &Attributes::setVertexFontColor)
        .function("setEdgeColor", &Attributes::setEdgeColor)
        .function("setEdgeFontColor", &Attributes::setEdgeFontColor)
    ;
    class_<DirectedGraphHierarchicalLayout>("DirectedGraphHierarchicalLayout")
        .constructor<>()
        .function("createGraph", &DirectedGraphHierarchicalLayout::createGraph)
        .function("setGraph", &DirectedGraphHierarchicalLayout::setGraph)
        .function("setFeedbackArcsMethod", &DirectedGraphHierarchicalLayout::setFeedbackArcsMethod)
        .function("setLayerAssignmentMethod", &DirectedGraphHierarchicalLayout::setLayerAssignmentMethod)
        .function("setCrossMinimizationMethod", &DirectedGraphHierarchicalLayout::setCrossMinimizationMethod)
        .function("setVertexPositioningMethod", &DirectedGraphHierarchicalLayout::setVertexPositioningMethod)
        .function("setVertexLabels", &DirectedGraphHierarchicalLayout::setVertexLabels)
        .function("setEdgeLabel", &DirectedGraphHierarchicalLayout::setEdgeLabel)
        .function("initVertexLabelsWithNumericalValues", select_overload<void(int)>(&DirectedGraphHierarchicalLayout::initVertexLabelsWithNumericalValues))
        .function("attributes", &DirectedGraphHierarchicalLayout::attributes, return_value_policy::reference())
        .function("layoutGraph", &DirectedGraphHierarchicalLayout::layoutGraph)
        .function("render", select_overload<string()const>(&DirectedGraphHierarchicalLayout::render))
    ;
}
