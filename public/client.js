const connection = new WebSocket('ws://127.0.0.1:8080');

connection.onmessage = (message) => {
  console.log(message);
  alert(message.data);
  const parsed = JSON.parse(data);

}
