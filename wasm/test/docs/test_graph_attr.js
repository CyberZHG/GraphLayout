import { describe, it } from 'mocha';
import { DirectedGraphHierarchicalLayout, AttributeRankDir } from '../../index.js';
import { compareSVG } from './utils.js';

describe('DocsGraphAttributes', () => {
    it('rankDir', async () => {
        const rankDirs = [
            AttributeRankDir.TOP_TO_BOTTOM,
            AttributeRankDir.BOTTOM_TO_TOP,
            AttributeRankDir.LEFT_TO_RIGHT,
            AttributeRankDir.RIGHT_TO_LEFT,
        ];
        for (const rankDir of rankDirs) {
            const layout = new DirectedGraphHierarchicalLayout();
            const graph = layout.createGraph(13);
            graph.addEdges([
                [0, 1], [1, 2], [1, 3], [2, 5], [2, 6],
                [3, 4], [4, 5], [4, 6], [5, 7], [6, 7],
                [7, 0], [1, 0],
                [8, 8], [8, 9], [8, 10], [9, 12], [10, 12],
                [11, 12]
            ]);
            layout.initVertexLabelsWithNumericalValues(0);
            layout.attributes().setRankDir(rankDir);
            layout.layoutGraph();
            const svg = layout.render();
            await compareSVG("graph_attr", `rank_dir_${rankDir}`, svg);
        }
    });
});
