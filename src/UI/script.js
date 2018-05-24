$(document).ready(function () {

  // Device
  var address = window.location.hostname + ":8080";
  var device = new Device(address);

  var authToken = 0;

  device.callFunction("auth", "", function(data){
    if(data.return_value != 0)
      authToken = data.return_value;
      else{
        $(".container").html('Device Already In Use. Reboot Device to connect.');
      }

  });

  var $log = $("#log");

  var prevX = 0;
  var prevY = 0;

  var awaitingCallback = false;

  function updateLog(x, y) {
    var deviceWidth = $(window).width();
    var deviceHeight = $(window).height();

    var valX, valY;
    valX = calcPercent(deviceWidth, x);
    valY = calcPercent(deviceHeight, y) * -1;

    if(Math.abs(prevX - valX) > 1 || Math.abs(prevY - valY) > 1)
    {
        prevX = valX;
        prevY = valY;

        $log.html('X: ' + valX + '; Y: ' + valY);

        if(authToken != 0)
        {
          if(!awaitingCallback || ( parseInt(valX) == 0 && parseInt(valY) == 0))
          {
            awaitingCallback = true;
            device.callFunction("newxy", parseInt(valX) + ',' + parseInt(valY) + ',' + authToken, function(){
              awaitingCallback = false;
            });
          }
        }
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
