

function Tool(context) {
    //console.log('building a tool');
    console.log('context', context.nearestPoints);

    var missingMethods = this.missingNeededMethods(this)

    if(missingMethods.length > 0) {
        throw new TypeError("Missing methods : " + missingMethods.join(', '));
    }

    // useful wiring
    this.nearestPoints = context.nearestPoints;
    this.getMousePosition = context.getMousePosition;

    this.mouseArea = context;

    this.components = {
        insertPoint : context.insertPoint,
        lineUi : context.lineUiComponent,
        insertLine : context.insertLineComponent,
        intermediatePoint: context.intermediatePoint
    }

    // todo create a dedicated class
    this.sketch = {
        lines: context.lines,
        points: context.points
    }
}

Tool.prototype.neededMethods = [
    'onPressed', 'onPositionChanged', 'onReleased', 'onLeaveTool', 'onEnterTool'
];

Tool.prototype.missingNeededMethods = function(that) {
    console.log('checkImplementation')
    return this.neededMethods.filter(function(method) {
        return typeof that[method] !== 'function'
    });
}
