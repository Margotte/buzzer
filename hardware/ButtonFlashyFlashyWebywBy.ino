#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

// Use if you want to force non-accelerated pin access (hint: you really don't, it breaks lots of things)
#include <FastLED.h>


#define BUTTON_ID 1
#define SERVER_IP "192.168.0.40"


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

// How many leds are in the strip?
#define NUM_LEDS 15

// Data pin that led data will be written out over
#define DATA_PIN D3
#define BRIGHTNESS 20
#define BUTTON_PIN D2
#define BUTTON_LED_PIN D1

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// For the animation
unsigned long previousMillis = 0;
unsigned long delay_between_frames = 100;
int currentFrame = 0;

bool toggled = false;

// This function sets up the ledsand tells the controller about them
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LED_PIN, OUTPUT);

  Serial.begin(9600);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  randomSeed(millis());

  WiFiMulti.addAP("VOO-004253", "KKFFYHJB");

  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // IP, PORT, ADDRESS
  webSocket.begin(SERVER_IP, 8999, "/");

  // Event Handler
  webSocket.onEvent(webSocketEvent); // <---- cool comme fonction

  // use HTTP Basic Authorization this is optional remove if not needed
  //webSocket.setAuthorization("user", "Password"); // Commentaire très utile

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000); 
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);
}

void loop() {
  webSocket.loop();
  
  if (digitalRead(BUTTON_PIN) == LOW) {
    onButtonPressed();

    while (true) {
      int btnState = digitalRead(BUTTON_PIN);
      if (btnState == LOW) {
        delay(20);
        continue;
      } else {
        delay(20);
        if (digitalRead(BUTTON_PIN) == LOW)
          continue;
        else
          break;
      }
    }
  }


  if (toggled) {
    anim3();
  } else {
    anim1();
  }
}

void onButtonPressed() { // <- vrmt cool
  toggled = !toggled;

  if (toggled) {
    digitalWrite(BUTTON_LED_PIN, HIGH);
    webSocket.sendTXT(String(BUTTON_ID)+":1");
  }
  else {
    digitalWrite(BUTTON_LED_PIN, LOW);
    webSocket.sendTXT(String(BUTTON_ID)+":0");
  }
    
}

void anim1() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_between_frames) {
    previousMillis = currentMillis;

    if (currentFrame == 0) {
      for (int led = 0; led < NUM_LEDS; led++)
        leds[led] = CRGB(247, 166, 0);
    } else if (currentFrame < 16) {
      leds[currentFrame - 1] = CRGB(0, 79, 159);
    } else if (currentFrame < 31) {
      leds[15 - (currentFrame - 1 - 15)] = CRGB(247, 166, 0);
    }

    currentFrame++;
    if (currentFrame > 30)
      currentFrame = 0;

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();
  }
}

void anim2() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_between_frames) {
    previousMillis = currentMillis;

    for (int led = 0; led < NUM_LEDS; led++) {
      if (led % 2 == 0 && currentFrame == 0) {
        leds[led] = CRGB(247, 166, 0);
      } else if (led % 2 == 0 && currentFrame == 1) {
        leds[led] = CRGB(0, 79, 159);
      } else if (led % 2 != 0 && currentFrame == 1) {
        leds[led] = CRGB(247, 166, 0);
      } else if (led % 2 != 0 && currentFrame == 0) {
        leds[led] = CRGB(0, 79, 159);
      }
    }

    currentFrame++;
    if (currentFrame > 1)
      currentFrame = 0;

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();
  }
}

void anim3() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay_between_frames) {
    previousMillis = currentMillis;

    for (int led = 0; led < NUM_LEDS; led++) {
      if (random(2) == 1) {
        leds[led] = CRGB(247, 166, 0);
      } else {
        leds[led] = CRGB(0, 79, 159);
      }
    }

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();
  }
}



void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT("Connected");
    }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
        case WStype_PING:
            // pong will be send automatically
            Serial.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            Serial.printf("[WSc] get pong\n");
            break;
    }

}
