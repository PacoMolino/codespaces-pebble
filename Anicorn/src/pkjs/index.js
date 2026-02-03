Pebble.addEventListener('appmessage', function(e) {
  var dict = e.payload;

  if (dict['TRANSCRIPTION']) {
    var textReceived = dict['TRANSCRIPTION'];
    console.log('El usuario dijo: ' + textReceived);
    
    // Aquí es donde llamas a tu API externa pasando el texto
    callMyApi(textReceived);
  }
});

function callMyApi(query) {
  // Ejemplo: Enviar a una API que procesa lenguaje natural
  console.log("Enviando a la API: " + query);
  
  // fetch('https://tu-api.com/v1/process?q=' + encodeURIComponent(query)) ...
}