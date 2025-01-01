import { describe, it } from 'mocha';
import {DirectedGraphHierarchicalLayout, AttributeRankDir} from '../../index.js';
import { compareSVG } from './utils.js';

describe('DocsEdgeAttributes', () => {
    it('label', async () => {
        const layout = new DirectedGraphHierarchicalLayout();
        const graph = layout.createGraph(3);
        graph.addEdges([[0, 1], [1, 2], [2, 2]]);
        layout.initVertexLabelsWithNumericalValues(0);
        layout.setEdgeLabel(0, "A");
        layout.setEdgeLabel(1, "B");
        layout.setEdgeLabel(2, "C");
        layout.attributes().setRankDir(AttributeRankDir.LEFT_TO_RIGHT);
        layout.layoutGraph();
        const svg = layout.render();
        await compareSVG("edge_attr", "label", svg);
    });
});
