var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var bombaManual = false;
window.addEventListener('load', onload);

const settings = {
  timeOutInterval: '', 
  "arbusto": {
    "onoff": 2000,
    "interval": 5000
  },
  "arvore": {
    "onoff": 1000,
    "interval": 3000
  },
};

function salvarNovo(){

var nome = document.getElementById("nomeTipo").value;
var onoff = document.getElementById("tempoLigado").value*1000;
var interval = document.getElementById("tempoIntervalo").value*1000;

//add new item to settings object
settings[nome] = {
  "onoff": onoff,
  "interval": interval
};

var daySelect = document.getElementById('selectTipoIrrigacao');
daySelect.options[daySelect.options.length] = new Option(nome, nome);


console.log(nome);
}

function onload(event) {
initWebSocket(); 

}

function enableDisableDropDown(){
  const radioPersonalizadoElement = document.getElementById('radioPersonalizado');
  const enableDropdown = radioPersonalizadoElement.checked;

  document.getElementById('selectTipoIrrigacao').disabled = !enableDropdown;
}

function toggleBomba(zona)
{
  if(bombaManual==true){    
    websocket.send(zona+"/on");
    bombaManual=false;
  }else{
    websocket.send(zona+"/off");
    bombaManual=true;
  }
}

function salvar(zona){
  console.log(zona);
  var tiposelecionado;
  var ele = document.getElementsByName('radioTipo');
 
  for (i = 0; i < ele.length; i++) {
      if (ele[i].checked)
          tiposelecionado = ele[i].value;
  }
  
  switch(tiposelecionado){
    case "manual":
      bombaManual=true;
      websocket.send(zona+"/manual");      
      clearInterval(settings.timeOutInterval);
      break;
    case "automatico":
      bombaManual=false;
      websocket.send(zona+"/automatico");
      clearInterval(settings.timeOutInterval);
      break;
    case "personalizado":
      bombaManual=false;
      var e = document.getElementById("selectTipoIrrigacao");
      const tipo = e.options[e.selectedIndex].value;

      console.log(e.options[e.selectedIndex].value);
      document.getElementById('title').innerHTML = "Modo atual de irrigacao " + e.options[e.selectedIndex].text;

      if(settings.timeOutInterval){
        clearInterval(settings.timeOutInterval);
      }

      ligar(zona, tipo);
      break;

  }

}

function changeLed() {
    websocket.send("toggle");
} 

function ligar(zona, tipo){
  const primeiroIntervalo = settings[tipo].onoff;
  // MANDAR WEBSOCKET PARA LIGAR
  console.log("Ligando " + tipo);
  websocket.send(zona+"/on");
  settings.timeOutInterval =  setTimeout(desligar, primeiroIntervalo, zona, tipo);
}

function desligar(zona, tipo){
  // MANDAR WEBSOCKET PARA DESLIGAR
  console.log("Desligando " + tipo);
  websocket.send(zona+"/off");
  settings.timeOutInterval = setTimeout(ligar, settings[tipo].interval, zona, tipo);
}


function getReadings(){
  websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        var element = document.getElementById(key);
        if(element  && typeof element.innerHTML !== null){
          document.getElementById(key).innerHTML = myObj[key];
          if(key == "humidity"){
           // updateChart(myObj[key]);
          }
        }
    }
}