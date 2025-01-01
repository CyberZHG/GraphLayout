declare module 'sp-graph-layout' {

    export function createDirectedGraph(numVertices: number): SPDirectedGraph

    export class SPDirectedGraph {
        constructor(numVertices: number)
        addEdge(from: number, to: number): number
        delete(): void
    }

    export class DirectedGraphHierarchicalLayout {
        constructor()
        setGraph(graph: SPDirectedGraph): void
        layoutGraph(): void
        render(): string
    }

}