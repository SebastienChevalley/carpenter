

function Tool(context) {
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

    this.fields = {
        widthEdit: context.widthEditField,
        verticalConstraint: context.verticalConstraint,
        horizontalConstraint: context.horizontalConstraint
    }

    this.sketch = context.sketch;

}

Tool.prototype.neededMethods = [
    'onPressed', 'onPositionChanged', 'onReleased', 'onLeaveTool', 'onEnterTool'
];

Tool.prototype.missingNeededMethods = function(that) {
    return this.neededMethods.filter(function(method) {
        return typeof that[method] !== 'function'
    });
}
