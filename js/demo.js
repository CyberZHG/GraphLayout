var layout = new Module.DirectedGraphLayout();
layout.addVertex("1", {});
layout.addVertex("2", {});
layout.addEdge("1", "2", {});
var coordinates = layout.layout()
var xs = [], ys = [];
for (var i = 0; i < coordinates.xs.size(); ++i) {
    xs.push(coordinates.xs.get(i));
    ys.push(coordinates.ys.get(i));
}
console.log(xs);
console.log(ys);
