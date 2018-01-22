$(document).ready(function () {

  // Device
  var address = "r1.sumo:8080";
  var device = new Device(address);

  // Buttons
  $('#forward').on('mousedown touchstart', function () {
    device.callFunction("forward");
  });
  $('#forward').on('mouseup touchend', function () {
    device.callFunction("stop");
  });

  $('#right').on('mousedown touchstart', function () {
    device.callFunction("right");
  });
  $('#right').on('mouseup touchend', function () {
    device.callFunction("stop");
  });

  $('#left').on('mousedown touchstart', function () {
    device.callFunction("left");
  });
  $('#left').on('mouseup touchend', function () {
    device.callFunction("stop");
  });

  $('#backward').on('mousedown touchstart', function () {
    device.callFunction("backw");
  });
  $('#backward').on('mouseup touchend', function () {
    device.callFunction("stop");
  });

  var $log = $("#log");

  var prevX = 0;
  var prevY = 0;

  function updateLog(x, y) {
    var deviceWidth = $(window).width();
    var deviceHeight = $(window).height();

    var valX, valY;
    valX = calcPercent(deviceWidth, x);
    valY = calcPercent(deviceHeight, y) * -1;

    if(Math.abs(prevX - valX) > 10 || Math.abs(prevY - valY) > 10)
    {
        prevX = valX;
        prevY = valY;

        $log.html('X: ' + valX + '; Y: ' + valY);

        device.callFunction("newxy", parseInt(valX) + ',' + parseInt(valY));
    }

  }

  function calcPercent(total, coord) {
    var half = (total / 2);
    return (coord - half) / half * 100;
    if (coord > half) {
      //right or down of center;
    }
    else {
      //left or up of center;
    }
    return 0;
  }

  document.addEventListener('touchstart', function (e) {
    updateLog(e.changedTouches[0].pageX, e.changedTouches[0].pageY);
  }, false);

  document.addEventListener('touchend', function (e) {
    updateLog($(window).width()/2, $(window).height()/2);
  }, false);

  document.addEventListener('touchmove', function (e) {
    e.preventDefault();
    updateLog(e.targetTouches[0].pageX, e.targetTouches[0].pageY);
  }, false);

});
