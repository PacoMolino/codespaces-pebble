// Escuchar cuando el JS está listo
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS listo!');
});

// Escuchar mensajes desde el reloj (C)
Pebble.addEventListener('appmessage', function(e) {
  var dict = e.payload;
  console.log('Mensaje recibido en JS: ' + JSON.stringify(dict));

  if (dict['FETCH_COMMAND']) {
    fetchData();
  }
});

function fetchData() {
  // Ejemplo con una API pública (JSONPlaceholder)
  var url = 'https://jsonplaceholder.typicode.com/todos/1';

  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    // JSON recibido del API
    var json = JSON.parse(this.responseText);
    
    // Construimos el diccionario para enviar al reloj
    // Usamos el nombre de la llave definido en package.json
    var dictionary = {
      'DATA_KEY': json.title // Enviamos el título de la tarea
    };

    // Enviar al Pebble
    Pebble.sendAppMessage(dictionary, function(e) {
      console.log('Info enviada al reloj con éxito');
    }, function(e) {
      console.log('Error enviando info al reloj');
    });
  };
  xhr.open('GET', url);
  xhr.send();
}