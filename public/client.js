const connection = new WebSocket('ws://127.0.0.1:8080');

team_array = []

const TEAM_COUNT = 6;

function setCardFlipState(index, state) {
  var card = document.getElementById("card-"+index);
  if (card.classList.contains("flipped")) {
    if (state) {
      return;
    } else {
      card.classList.remove("flipped");
    }
  } else {
    if (state) {
      card.classList.add("flipped");
    } else {
      return;
    }
  }
}

function setCardImage(index, image) {
    var cardImg = document.querySelector("#card-"+index+" .team-img");
    cardImg.setAttribute("src","images/"+image+".jpg");
}

function updateFlipCards() {
  for (var i = 0; i<TEAM_COUNT; i++) {
    if (i<team_array.length) {
      setCardImage(i, team_array[i]);
      setCardFlipState(i, true);
    } else {
      setCardFlipState(i, false);
    }
  }
}

function handleParsifiedMessage(parsified) {
  if (parsified.toggled == true) {
    if (!team_array.includes(parsified.name)) {
      team_array.push(parsified.name);
      updateFlipCards();
    } else {
      console.log("Warning: OUT OF SYNC (mais ca va se resync pas de panique)");
    }
  } else {
    if (team_array.includes(parsified.name)) {
      team_array.splice( team_array.indexOf(parsified.name), 1);
      updateFlipCards();
    } else {
      console.log("Warning: OUT OF SYNC (mais ca va se resync pas de panique)");
    }
  }
} 

connection.onmessage = (message) => {
  console.log(message);
  alert(message.data);
  const parsified = JSON.parse(data);
  handleParsifiedMessage(parsified);
}
