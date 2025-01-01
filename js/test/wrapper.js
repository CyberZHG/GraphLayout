import { describe, it } from 'mocha';
import { strict as assert } from 'assert';
import { GraphLayout } from '../index.js';

describe('Wrapper', () => {
    it('special case 0', () => {
        const layout = new GraphLayout();
        layout.addVertex("1");
        layout.addVertex("2");
        layout.addEdge("1", "2");
        const coordinates = layout.layout();
        assert.deepEqual(coordinates, [[0.0, 0.0], [0.0, 60.0]]);
    });
});