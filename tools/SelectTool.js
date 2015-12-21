Qt.include("Tool.js")
Qt.include("qrc:/lib/lib/lodash.js");

var _ = lodash(this);

function SelectTool(context) {
    Tool.call(this, context);

    this.lineClickHandler = function(line) {
        var that = this;
        return function() {
            console.log(line);

            _.forEach(
                _.filter(that.mouseArea.lines, function(l) {
                    return l.identifier !== line.identifier && l.selected === true
                }),
                function(line) { line.selected = false; }
            );

            line.selected = true;
            that.selectedLine = line;

            that.enableEditFields.bind(that, line)();
        }
    }

    this.disableEditFields = function() {
        this.fields.widthEdit.enabled = false;
        this.fields.verticalConstraint.enabled = false;
        this.fields.horizontalConstraint.enabled = false;
        console.log('widthEdit', this.fields.widthEdit.enabled);

    }

    this.enableEditFields = function(line) {
        console.log(line);
        this.fields.widthEdit.enabled = true;

        this.fields.verticalConstraint.enabled = true;
        this.fields.verticalConstraint.checked = line.verticallyConstrained;

        this.fields.horizontalConstraint.enabled = true;
        this.fields.horizontalConstraint.checked = line.horizontallyConstrained;
    }

    this.enableWidthFieldSubmit = function() {
        console.log("enableWidthFieldSubmit", this);

        if(this.sketch.isScaleSet()) {

        }
        else {
            this.sketch.setInitialScale(this.selectedLine.pointer.length(), parseFloat(this.fields.widthEdit.text));
        }
    }

    this.verticalConstraintCheckboxHandler = function() {
        console.log(this.fields.verticalConstraint.checked)
        this.sketch.verticallyConstrainLine(this.selectedLine.identifier, this.fields.verticalConstraint.checked)
    }

    this.horizontalConstraintCheckboxHandler = function() {
        console.log(this.fields.horizontalConstraint.checked)
        this.sketch.horizontallyConstrainLine(this.selectedLine.identifier, this.fields.horizontalConstraint.checked)

    }

    this.selectedLine = null;
    this.handlers = {};
    this.editWidthHandler = this.enableWidthFieldSubmit.bind(this);
    this.horizontalConstraintCheckboxHandler = this.horizontalConstraintCheckboxHandler.bind(this);
    this.verticalConstraintCheckboxHandler = this.verticalConstraintCheckboxHandler.bind(this);
}

SelectTool.prototype = Object.create(Tool.prototype);
SelectTool.prototype.constructor = SelectTool;

SelectTool.prototype.onPressed = function() {
    // fired when no lines has been clicked, deselect everything:
    _.forEach(this.mouseArea.lines, function(line) {
        line.selected = false;
    }, this);

    this.disableEditFields.bind(this)();
}

SelectTool.prototype.onPositionChanged = function() {

}

SelectTool.prototype.onReleased = function() {

}

SelectTool.prototype.onEnterTool = function() {
    _.forEach(this.mouseArea.lines, function(line) {
        line.mouseArea.enabled = true;
        this.handlers[line.identifier] = this.lineClickHandler(line);
        line.mouseArea.clicked.connect(this.handlers[line.identifier]);
    }, this);

    this.disableEditFields();

    this.fields.widthEdit.accepted.connect(this.editWidthHandler);
    this.fields.verticalConstraint.clicked.connect(this.verticalConstraintCheckboxHandler);
    this.fields.horizontalConstraint.clicked.connect(this.horizontalConstraintCheckboxHandler);
}

SelectTool.prototype.onLeaveTool = function() {
    _.forEach(this.mouseArea.lines, function(line) {
        line.mouseArea.enabled = false;
        line.selected = false;

        line.mouseArea.clicked.disconnect(this.handlers[line.identifier]);
    }, this);

    this.disableEditFields();

    this.fields.widthEdit.accepted.disconnect(this.editWidthHandler);
    this.fields.verticalConstraint.clicked.disconnect(this.verticalConstraintCheckboxHandler);
    this.fields.horizontalConstraint.clicked.disconnect(this.horizontalConstraintCheckboxHandler);
}

SelectTool.prototype.toString = function(){
    return 'SelectTool';
}
