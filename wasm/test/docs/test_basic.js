import { describe, it } from 'mocha';
import { DirectedGraphHierarchicalLayout } from '../../index.js';
import { compareSVG } from './utils.js';

describe('DocsBasic', () => {
    it('directed', async () => {
        const layout = new DirectedGraphHierarchicalLayout();
        const graph = layout.createGraph(8);
        graph.addEdges([
            [0, 1], [1, 2], [1, 3], [2, 5], [2, 6],
            [3, 4], [4, 5], [4, 6], [5, 7], [6, 7],
            [7, 0],
        ]);
        layout.initVertexLabelsWithNumericalValues(0);
        layout.layoutGraph();
        const svg = layout.render();
        await compareSVG("basic", "directed", svg);
    });
});
