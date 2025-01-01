import GraphLayoutWASMModule from './wasm/GraphLayoutWASM.js';

const GraphLayoutWASM = await GraphLayoutWASMModule();

const _compareSVG = GraphLayoutWASM._compareSVG;
const FeedbackArcsMethod = GraphLayoutWASM.FeedbackArcsMethod;
const LayerAssignmentMethod = GraphLayoutWASM.LayerAssignmentMethod;
const CrossMinimizationMethod = GraphLayoutWASM.CrossMinimizationMethod;
const VertexPositioningMethod = GraphLayoutWASM.VertexPositioningMethod;
const SPDirectedGraph = GraphLayoutWASM.SPDirectedGraph;
const Attribute = GraphLayoutWASM.Attribute;
const AttributeRankDir = GraphLayoutWASM.AttributeRankDir;
const AttributeShape = GraphLayoutWASM.AttributeShape;
const Attributes = GraphLayoutWASM.Attributes;
const DirectedGraphHierarchicalLayout = GraphLayoutWASM.DirectedGraphHierarchicalLayout;

SPDirectedGraph.prototype.addEdges = function (edges) {
    for (const edge of edges) {
        this.addEdge(edge[0], edge[1]);
    }
}

export {
    _compareSVG,
    FeedbackArcsMethod,
    LayerAssignmentMethod,
    CrossMinimizationMethod,
    VertexPositioningMethod,
    SPDirectedGraph,
    Attribute,
    AttributeRankDir,
    AttributeShape,
    Attributes,
    DirectedGraphHierarchicalLayout,
};