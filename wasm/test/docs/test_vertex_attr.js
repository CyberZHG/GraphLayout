import { describe, it } from 'mocha';
import {DirectedGraphHierarchicalLayout, AttributeRankDir, AttributeShape, buildVectorString} from '../../index.js';
import { compareSVG } from './utils.js';

describe('DocsVertexAttributes', () => {
    it('shape', async () => {
        const layout = new DirectedGraphHierarchicalLayout();
        const graph = layout.createGraph(3);
        graph.addEdges([[0, 1], [1, 2], [2, 0]]);
        layout.attributes().setVertexShape(0, AttributeShape.DOUBLE_CIRCLE);
        layout.attributes().setVertexShape(1, AttributeShape.RECT);
        layout.attributes().setVertexShape(2, AttributeShape.ELLIPSE);
        layout.attributes().setRankDir(AttributeRankDir.LEFT_TO_RIGHT);
        layout.initVertexLabelsWithNumericalValues(0);
        layout.layoutGraph();
        const svg = layout.render();
        await compareSVG("vertex_attr", "shape", svg);
    });
    it('label', async () => {
        const layout = new DirectedGraphHierarchicalLayout();
        const graph = layout.createGraph(3);
        graph.addEdges([[0, 1], [1, 2], [2, 0]]);
        layout.setVertexLabels(buildVectorString(["Eat", "Sleep", "Play"]));
        for (let u = 0; u < 3; ++u) {
            layout.attributes().setVertexShape(u, AttributeShape.ELLIPSE);
        }
        layout.attributes().setRankDir(AttributeRankDir.LEFT_TO_RIGHT);
        layout.layoutGraph();
        const svg = layout.render();
        await compareSVG("vertex_attr", "label", svg);
    });
});
