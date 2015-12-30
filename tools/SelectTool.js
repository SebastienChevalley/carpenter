Qt.include("Tool.js")
Qt.include("qrc:/lib/lib/lodash.js");

var _ = lodash(this);

function SelectTool(context) {
    Tool.call(this, context);

    this.selectedItem = null;
    this.handlers = {};

    this.pointContextMenu = function() {
        return this.mouseArea.pointContextMenu;
    }

    this.itemClickHandler = function(item) {
        var that = this;
        return function() {
            console.log("item selected:", item);

            _.forEach(
                _.filter(that.getItems(), function(l) {
                    return l.identifier !== item.identifier && l.selected === true
                }),
                function(item) { item.selected = false; }
            );

            item.selected = true;
            that.selectedItem = item;

            that.enableEditFields.bind(that, item)();
        }
    }

    this.disableEditFields = function() {
        this.fields.widthEdit.enabled = false;
        this.fields.verticalConstraint.enabled = false;
        this.fields.horizontalConstraint.enabled = false;
        console.log('widthEdit', this.fields.widthEdit.enabled);

    }

    this.enableEditFields = function(item) {
        this.hidePointContextMenu();

        // line specific
        if(item.hasOwnProperty("distanceFixed")) {
            var line = item;
            this.fields.widthEdit.enabled = true;

            var fieldContent = line.distanceFixed ? line.desiredDistance : "";
            this.fields.widthEdit.text = fieldContent;

            this.fields.verticalConstraint.enabled = true;
            this.fields.verticalConstraint.checked = line.verticallyConstrained;

            this.fields.horizontalConstraint.enabled = true;
            this.fields.horizontalConstraint.checked = line.horizontallyConstrained;
        }
        // point specific
        else {
            /*
             * place the point context menu
             * In general, the context menu will be placed horizontally
             * centered and over the point. If the place is missing, the
             * following code tries to place it arround.
             */
            var canvas = { 'width': this.mouseArea.width, 'height': this.mouseArea.height };
            var menu = this.pointContextMenu();
            var width = menu.width;
            var height = menu.height;
            var pointRadius = context.Settings.pointSize / 2.0;
            var position = item.start;
            var margin = 20;

            var neededXSpace = Math.ceil(margin + width / 2.0);
            var isSpaceAvailableOnLeft = neededXSpace <= position.x;
            var isSpaceAvailableOnRight = neededXSpace <= (canvas.width - position.x);
            var centerOffset = width / 2.0;
            var horizontalOffset = (function() {
                if(isSpaceAvailableOnLeft)
                    if(isSpaceAvailableOnRight)
                        return -centerOffset;
                    else
                        return canvas.width - width - margin - position.x;
                else return margin - position.x;
            })();

            var spaceBetweenPointAndMenu = margin + pointRadius;
            var neededYSpaceOver = margin + spaceBetweenPointAndMenu + height;
            var isSpaceAvailableOver = neededYSpaceOver <= position.y;
            var isSpaceAvailableUnder = neededYSpaceOver <= (canvas.height - position.y);

            menu.x = position.x + horizontalOffset;
            menu.y = position.y + (isSpaceAvailableOver ? -(spaceBetweenPointAndMenu + height) : spaceBetweenPointAndMenu);

            menu.mx.checked = item.mx;
            menu.my.checked = item.my;
            menu.mz.checked = item.mz;

            menu.visible = true;
        }
    }

    this.hidePointContextMenu = function() {
        this.pointContextMenu().visible = false;
    }

    this.enableWidthFieldSubmit = function() {
        console.log("enableWidthFieldSubmit", this);

        if(this.sketch.isMmPerPixelScaleSet()) {
            this.sketch.setDesiredDistance(this.selectedItem.identifier, parseFloat(this.fields.widthEdit.text))
        }
        else {
            this.sketch.setInitialScale(this.selectedItem.pointer.length(), parseFloat(this.fields.widthEdit.text));
        }
    }

    this.verticalConstraintCheckboxHandler = function() {
        console.log(this.fields.verticalConstraint.checked)
        this.sketch.verticallyConstrainLine(this.selectedItem.identifier, this.fields.verticalConstraint.checked)
    }

    this.horizontalConstraintCheckboxHandler = function() {
        console.log(this.fields.horizontalConstraint.checked)
        this.sketch.horizontallyConstrainLine(this.selectedItem.identifier, this.fields.horizontalConstraint.checked)

    }

    this.getItems = function() {;
        return [].concat(_.values(this.mouseArea.points), _.values(this.mouseArea.lines));
    }

    this.itemLeaveHandler = function(item) {
        item.mouseArea.enabled = false;
        item.selected = false;
        item.mouseArea.clicked.disconnect(this.handlers[item.identifier]);
    }

    this.itemEnterHandler = function(item) {
        item.mouseArea.enabled = true;
        this.handlers[item.identifier] = this.itemClickHandler(item);
        item.mouseArea.clicked.connect(this.handlers[item.identifier]);
    }

    this.editWidthHandler = this.enableWidthFieldSubmit.bind(this);
    this.horizontalConstraintCheckboxHandler = this.horizontalConstraintCheckboxHandler.bind(this);
    this.verticalConstraintCheckboxHandler = this.verticalConstraintCheckboxHandler.bind(this);
}

SelectTool.prototype = Object.create(Tool.prototype);
SelectTool.prototype.constructor = SelectTool;

SelectTool.prototype.onPressed = function() {
    // fired when no lines has been clicked, deselect everything:
    _.forEach(this.getItems(), function(item) {
        item.selected = false;
        this.hidePointContextMenu();
    }, this);

    this.disableEditFields.bind(this)();
}

SelectTool.prototype.onPositionChanged = function() {

}

SelectTool.prototype.onReleased = function() {

}

SelectTool.prototype.onEnterTool = function() {
    _.forEach(this.getItems(), this.itemEnterHandler, this);

    this.disableEditFields();

    this.fields.widthEdit.accepted.connect(this.editWidthHandler);
    this.fields.verticalConstraint.clicked.connect(this.verticalConstraintCheckboxHandler);
    this.fields.horizontalConstraint.clicked.connect(this.horizontalConstraintCheckboxHandler);
}

SelectTool.prototype.onLeaveTool = function() {
    _.forEach(this.getItems(), this.itemLeaveHandler, this);

    this.disableEditFields();
    this.hidePointContextMenu();

    this.fields.widthEdit.accepted.disconnect(this.editWidthHandler);
    this.fields.verticalConstraint.clicked.disconnect(this.verticalConstraintCheckboxHandler);
    this.fields.horizontalConstraint.clicked.disconnect(this.horizontalConstraintCheckboxHandler);
}

SelectTool.prototype.toString = function(){
    return 'SelectTool';
}
