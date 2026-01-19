declare module 'sp-graph-layout' {
    function _compareSVG(a: string, b: string): boolean

    export class VectorString {
        constructor();

        size(): number;
        get(index: number): string;

        push_back(value: string): void;
        resize(size: number): void;
        clear(): void;

        delete(): void;
    }

    export function buildVectorString(strings: string[]): VectorString

    export const enum FeedbackArcsMethod {
        EADES_93,
        MIN_ID,
    }

    export const enum LayerAssignmentMethod {
        TOPOLOGICAL,
        MIN_NUM_OF_LAYERS,
        GANSNER_93,
        MIN_TOTAL_EDGE_LENGTH,
    }

    export const enum CrossMinimizationMethod {
        BARYCENTER,
        MEDIAN,
        PAIRWISE_SWITCH,
    }

    export const enum VertexPositioningMethod {
         BRANDES_KOPF,
    }

    export class SPDirectedGraph {
        constructor(numVertices: number)
        addEdge(from: number, to: number): number
        addEdges(edges: number[][]): void
        delete(): void
    }

    export class Attribute {
        constructor()
        set(value: string): void
        value(): string
    }

    export class AttributeRankDir extends Attribute {
        constructor()
        static readonly TOP_TO_BOTTOM: string
        static readonly BOTTOM_TO_TOP: string
        static readonly LEFT_TO_RIGHT: string
        static readonly RIGHT_TO_LEFT: string
    }

    export class AttributeShape extends Attribute {
        constructor()
        static readonly NONE: string
        static readonly CIRCLE: string
        static readonly DOUBLE_CIRCLE: string
        static readonly ELLIPSE: string
        static readonly RECT: string
        static readonly RECORD: string
    }

    export class AttributeArrowShape extends Attribute {
        constructor()
        static readonly NONE: string
        static readonly NORMAL: string
        static readonly EMPTY: string
    }

    export class Attributes {
        constructor()
        setRankDir(rankDir: string): void
        setVertexShape(u: number, shape: string): void
        setEdgeTailLabel(edgeId: number, label: string): void
        setEdgeHeadLabel(edgeId: number, label: string): void
        setEdgeLabelDistance(edgeId: number, scale: number): void
        setVertexDefaultShape(shape: string): void
        setVertexDefaultMonospace(): void
        setEdgeDefaultMonospace(): void
        setEdgeDefaultArrowHead(value: string): void
        setEdgeDefaultArrowTail(value: string): void
        setEdgeArrowHead(edgeId: number, value: string): void
        setEdgeArrowTail(edgeId: number, value: string): void
        setVertexDefaultColor(value: string): void
        setVertexDefaultFillColor(value: string): void
        setVertexDefaultFontColor(value: string): void
        setEdgeDefaultColor(value: string): void
        setEdgeDefaultFontColor(value: string): void
        setVertexColor(u: number, value: string): void
        setVertexFillColor(u: number, value: string): void
        setVertexFontColor(u: number, value: string): void
        setEdgeColor(edgeId: number, value: string): void
        setEdgeFontColor(edgeId: number, value: string): void
    }

    export class DirectedGraphHierarchicalLayout {
        constructor()
        createGraph(numVertices: number): SPDirectedGraph
        setGraph(graph: SPDirectedGraph): void
        setFeedbackArcsMethod(method: FeedbackArcsMethod): void
        setLayerAssignmentMethod(method: LayerAssignmentMethod): void
        setCrossMinimizationMethod(method: CrossMinimizationMethod): void
        setVertexPositioningMethod(method: VertexPositioningMethod): void
        setVertexLabels(labels: string[]): void
        setEdgeLabel(edgeId: number, label: string): void
        initVertexLabelsWithNumericalValues(start: number): void
        attributes(): Attributes
        layoutGraph(): void
        render(): string
    }

}