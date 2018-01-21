$( document ).ready(function() {

    // Device
    var address = "r1.sumo:8080";
    var device = new Device(address);

    // Buttons
    $('#forward').on('mousedown touchstart', function() {
      device.callFunction("forward");
    });
    $('#forward').on('mouseup touchend', function() {
      device.callFunction("stop");
    });

    $('#right').on('mousedown touchstart', function() {
      device.callFunction("right");
    });
    $('#right').on('mouseup touchend', function() {
      device.callFunction("stop");
    });

    $('#left').on('mousedown touchstart', function() {
      device.callFunction("left");
    });
    $('#left').on('mouseup touchend', function() {
      device.callFunction("stop");
    });

    $('#backward').on('mousedown touchstart', function() {
      device.callFunction("backw");
    });
    $('#backward').on('mouseup touchend', function() {
      device.callFunction("stop");
    });

});
