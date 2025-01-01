import GraphLayoutWASMModule from './wasm/GraphLayoutWASM.js';

const GraphLayoutWASM = await GraphLayoutWASMModule();

export class GraphLayout {
    constructor() {
        this._layout = new GraphLayoutWASM.DirectedGraphLayout();
    }

    addVertex(id, attributes = {}) {
        this._layout.addVertex(id, attributes);
    }

    addEdge(id1, id2, attributes = {}) {
        this._layout.addEdge(id1, id2, attributes);
    }

    layout() {
        const coordinates = this._layout.layout();
        var xs = [], ys = [];
        for (var i = 0; i < coordinates.xs.size(); ++i) {
            xs.push(coordinates.xs.get(i));
            ys.push(coordinates.ys.get(i));
        }
        return [xs, ys];
    }
}
