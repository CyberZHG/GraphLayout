import GraphLayoutWASMModule from './wasm/GraphLayoutWASM.js';

const GraphLayoutWASM = await GraphLayoutWASMModule();

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

export {
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