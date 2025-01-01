import GraphLayoutWASMModule from './wasm/GraphLayoutWASM.js';

const GraphLayoutWASM = await GraphLayoutWASMModule();

const createDirectedGraph = GraphLayoutWASM.createDirectedGraph;
const SPDirectedGraph = GraphLayoutWASM.SPDirectedGraph;
const DirectedGraphHierarchicalLayout = GraphLayoutWASM.DirectedGraphHierarchicalLayout;

export {
    createDirectedGraph,
    SPDirectedGraph,
    DirectedGraphHierarchicalLayout,
};