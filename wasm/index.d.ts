declare module 'sp-graph-layout' {
    function _compareSVG(a: string, b: string): boolean

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
    }

    export class Attributes {
        constructor()
        setRankDir(rankDir: string): void
        setVertexShape(u: number, shape: string): void
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
        initVertexLabelsWithNumericalValues(start: number): void
        attributes(): Attributes
        layoutGraph(): void
        render(): string
    }

}