Pebble.addEventListener('appmessage', function(e) {
  if (e.payload['FETCH_COMMAND']) {
    fetchData();
  }
});

function fetchData() {
  var url = 'https://jsonplaceholder.typicode.com/todos/' + Math.floor(Math.random() * 20 + 1);

  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    var json = JSON.parse(this.responseText);
    
    // Enviamos el título y también un estado de éxito
    Pebble.sendAppMessage({
      'DATA_KEY': json.title
    });
  };
  
  xhr.onerror = function() {
    Pebble.sendAppMessage({
      'DATA_KEY': "Error de conexión"
    });
  };
  
  xhr.open('GET', url);
  xhr.send();
}