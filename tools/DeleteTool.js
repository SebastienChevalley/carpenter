Qt.include("Tool.js")
Qt.include("qrc:/lib/lib/lodash.js");

var _ = lodash(this);

function DeleteTool(context) {
    Tool.call(this, context);

    // enabled during a long pressed event
    this.isLongPressed = false
    // element under user cursor to be removed
    this.deleteTarget = null
    // timer used to fire long press event
    this.longPressTimer = null;

    /**
      * Used to compute if the mouse position is hover a line
      */
    this.isHoverALine = null

    // TODO move it elsewhere if it's needed
    this.timer = function(callback, delay, isRepeating) {
        var t = Qt.createQmlObject("import QtQuick 2.0; Timer {}", this.mouseArea);
        t.interval = delay
        t.repeat = isRepeating === undefined ? false : isRepeating

        var timerCallback = callback;

        // auto destroy non repeating callback
        /*if(!t.repeat) {
            timerCallback = function() {
                callback()
                t.destroy()
            }
        }*/
        t.triggered.connect(timerCallback);
        t.start()

        return t
    }

    //this.timer(function() { console.log("coucou") }, 1000, false)
}

DeleteTool.prototype = Object.create(Tool.prototype);
DeleteTool.prototype.constructor = DeleteTool;

DeleteTool.prototype.onPressed = function() {
    if(this.longPressTimer !== null) {
        this.longPressTimer.start()
    }
    else {
        console.log("will use a timer")
        var current = this;

        this.longPressTimer = this.timer(function() {
            current.isLongPressed = true;
        }, 1000, false);
    }
}

DeleteTool.prototype.onPositionChanged = function() {

}

DeleteTool.prototype.onReleased = function() {
    if(this.longPressTimer !== null) {
        this.longPressTimer.stop();
    }

    this.isLongPressed = false;
}

DeleteTool.prototype.onEnterTool = function() {

}

DeleteTool.prototype.onLeaveTool = function() {

}
