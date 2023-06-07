// --- production  ---
var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
// --- local testing ---
// var ws = new WebSocket('ws://' + window.location.hostname + ':5500/WSService'); 
// -----------------
var currentLapNumber;
var currentLapElapsedTime;
var currentTotalElapsedTime;
var FinalTotalTime;
var numberOfLaps = 2;

$(window).on("load", function () {
  // console.log("window loaded");
});

$(function () {
  console.log("Document ready!");
  $('#divStartYourRobotNow').hide();
  $('#divElapsedTimes').hide();
  $('#tblMenuTrDisqualify').hide();
  $('#divFinalTime').hide();
  $('#tblMenuTrStartNewRound').show();
});


ws.onmessage = function (event) {

  // console.log("ws_event");
  processJsonCommand(JSON.parse(event.data));
};


// NITRO COMPETITION TIMER SIMPLE JSON COMMUNICATION PROTOCOL:
//        Server side:
// command: "StartNewRound",
// command: "ShowFinalResult",
// command: "UpdateLapCounters",
// {
//   "command": "StartNewRound",
//   "numberOfLaps": 2,
//   "currentLapNumber": 1,
//   "lap1currentTime": "01:01:01:001",
//   "lap2currentTime": "02:02:02:002",
//   "totalElapsedTime": "11:11:11:110",
//   "bestLapTime": "11:11:11:110",
//   "totalFinalTime": "99:99:99:999"
// }
//
//        Client side:
// command: 'StartNewRound'
// command: 'CancelRound'
// command: 'SetCompetitionLine'
// command: 'SetCompetitionObstacleLine'
// command: 'SetCompetitionMaze'
// command: 'SetCompetitionTeleoperation'
//
// END JSON COMMUNICATION PROTOCOL ------------------

function processJsonCommand(jsonMsg) {

  // console.log("processJsonCommand");
  // console.log(jsonMsg);
  switch (jsonMsg.command) {

    case 'StartNewRound':
      console.log("processJsonCommand: StartNewRound");
      $('#divFinalTime').hide();
      $('#tblMenuTrStartNewRound').hide();
      $('#tblMenuTrDisqualify').show();
      $('#trStartYourRobotNow').show();
      $('#btnCopyFinalTimeToClipboard').hide();
      $('#divCurrentTimes').fadeIn("slow");
      break;
      case 'UpdateLapCounters':
        console.log("processJsonCommand: updateLapCounters");
        $('#divFinalTime').hide();
        $('#divStartYourRobotNow').hide();
        $('#tblMenuTrStartNewRound').hide();
        $('#btnCopyFinalTimeToClipboard').hide();
        $('#divElapsedTimes').show();
        $('#tblMenuTrDisqualify').show();
        // $('#totalElapsedTime').text("99:99.999");
        // $('#lap1ElapsedTime').text("Lap 1:  77.77.777");
        // $('#lap2ElapsedTime').text("Lap 2:  77.77.777");
        $('#spTotalElapsedTime').text(jsonMsg.totalElapsedTime);
        $('#lap1ElapsedTime').text("Lap 1:  " + jsonMsg.lap1currentTime);
        $('#lap2ElapsedTime').text("Lap 2:  " + jsonMsg.lap2currentTime);
  
        if (jsonMsg.numberOfLaps == 1) {
          $('#lap2ElapsedTime').hide();
        }else{
          $('#lap2ElapsedTime').show();
        }
  
        break;

        case 'ShowFinalResult':
          console.log("processJsonCommand: showFinalResult");
          $('#divStartYourRobotNow').hide();
          $('#divElapsedTimes').hide();
          $('#tblMenuTrDisqualify').hide();
          // $('#spanTotalTime').text("99:99.999");
          // $('#bestLapTime').text("BEST LAP TIME: 77.77.777");
          $('#spanTotalTime').text(jsonMsg.totalFinalTime);
          $('#bestLapTime').text("BEST LAP TIME: " + jsonMsg.bestLapTime);
          $('#tblMenuTrStartNewRound').show();
          $('#btnCopyFinalTimeToClipboard').show();
          clearPenaltyPointsTextbox(); 
          $('#divFinalTime').fadeIn("slow");
          break;
        case 'reset':
          console.log("processJsonCommand: reset");
          break;
        default:
          alert('Unrecognized command received! Reastart your NITROclubs counter device!');
      }
    };
    
$("#btnStartNewRound").click(function () {

  console.log("btnStartNewRound clicked!");
  var msg = {
    'actionRequest': 1 // 'StartNewRound'
  };
  ws.send(JSON.stringify(msg));

  console.log("btnStartNewRound clicked! - Setting UI");
  $('#tblMenuTrStartNewRound').hide();
  $('#btnCopyFinalTimeToClipboard').hide();
  $('#tblMenuTrDisqualify').show();
  $('#divStartYourRobotNow').show();
  $('#divFinalTime').hide();
  $('#divCurrentTimes').hide();
  });

$("#btnCancelRound").click(function () {

  console.log("btnCancelRound clicked!");
  var msg = {
    'actionRequest': 2 //'CancelRound'
  };
  ws.send(JSON.stringify(msg));
});

$("#liCompetitionType").change(function () {
  // actionRequest: 'SetCompetitionLine'
  // actionRequest: 'SetCompetitionObstacleLine'
  // actionRequest: 'SetCompetitionMaze'
  // actionRequest: 'SetCompetitionTeleoperation'
  var selectedCompetitionType = $(this).children("option:selected").val();
  console.log("Selection in competition type list is: - " + selectedCompetitionType);
  var msg;
  switch (selectedCompetitionType) {
    case 'SetCompetitionLine':
      msg = { actionRequest: 3 }; // actionRequest: 3 //'SetCompetitionLine'
      break;
    case 'SetCompetitionObstacleLine':
      msg = { actionRequest: 4 }; // actionRequest: 4 //'SetCompetitionObstacleLine'
      break;
    case 'SetCompetitionMaze':
      msg = { actionRequest: 5 }; // actionRequest: 5 //'SetCompetitionMaze'
      break;
    case 'SetCompetitionTeleoperation':
      msg = { actionRequest: 6 }; // actionRequest: 6 //'SetCompetitionTeleoperation'
      break;
  
    default:
      break;
  }
  ws.send(JSON.stringify(msg));
});
  
$("#btnCopyTimesToClipboard").click(function () {
  console.log("btnCopyTimesToClipboard clicked!");
  copyToClipboard();
});

function copyToClipboard() {
  var points = $('#tbPenaltyPoints').val();
  if (points == "") {
    points="0";
  }
  var text_to_copy =  $('#spanTotalTime').text() + "|" + $('#bestLapTime').text().replace('BEST LAP TIME: ', '') + "|" + points;

  if (!navigator.clipboard) {
    // the old way using execCommand
    var el = document.createElement('textarea');
    el.value = text_to_copy;
    el.setAttribute('readonly', '');
    el.style = {
      position: 'absolute',
      left: '-9999px'
    };
    document.body.appendChild(el);
    el.select();
    document.execCommand('copy');
    document.body.removeChild(el);
  } else {
    // the new way not using execCommand
    navigator.clipboard.writeText(text_to_copy).then(
      function () {
        console.log("Values copied to clipboard!"); // success 
      })
      .catch(
        function () {
          console.log("ERR, Could not copy to clipboard!"); // error
        });
  }
}


$('.onlyNumbers').keyup(function () {
  this.value = this.value.replace(/[^0-9\.]/g,'');
});

function clearPenaltyPointsTextbox() {
$('#tbPenaltyPoints').val("");
}
