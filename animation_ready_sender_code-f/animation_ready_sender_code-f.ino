#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- BUTTON PINS ----------------
#define EMERGENCY_PIN 13
#define BRAKE_PIN     12
#define RIGHT_PIN     14
#define LEFT_PIN      27
#define OVERTAKE_PIN  26

// ---------------- RECEIVER MAC ----------------
uint8_t receiverAddress[] = {0xA4,0xF0,0x0F,0x77,0x47,0x6C};

uint8_t lastPriority = 255;

// Idle animation variables
unsigned long lastIdleAnim = 0;
int waveSize = 0;

// ---------------- STARTUP ----------------
void startupAnimation() {

  int centerX = -30;
  int centerY = 32;

  // Slide from left
  while(centerX < 64) {
    display.clearDisplay();
    display.drawCircle(centerX, centerY, 22, SSD1306_WHITE);
    display.drawCircle(centerX, centerY, 16, SSD1306_WHITE);
    display.display();
    centerX += 4;
    delay(30);
  }

  delay(300);

  // Rotation
  for(int angle = 0; angle < 360; angle += 12) {

    display.clearDisplay();

    display.drawCircle(64, 32, 22, SSD1306_WHITE);
    display.drawCircle(64, 32, 16, SSD1306_WHITE);

    for(int i = 0; i < 360; i += 90) {
      float rad = (i + angle) * 3.14159 / 180.0;
      int x = 64 + 10 * cos(rad);
      int y = 32 + 10 * sin(rad);
      display.fillCircle(x, y, 5, SSD1306_WHITE);
    }

    display.display();
    delay(35);
  }

  // Branding
  display.clearDisplay();
  display.drawCircle(64, 32, 22, SSD1306_WHITE);
  display.fillCircle(64, 32, 10, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(55, 29);
  display.println("TX");
  display.display();
  delay(800);

  // Message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 25);
  display.println("Transmitter Unit");
  display.display();
  delay(1200);
}

// ---------------- IDLE SIGNAL WAVE ANIMATION ----------------
void idleAnimation() {

  if (millis() - lastIdleAnim > 200) {

    lastIdleAnim = millis();
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(15, 5);
    display.println("IDLE -Trans._Ready");

    // Signal tower icon
    display.fillRect(62, 35, 4, 12, SSD1306_WHITE);

    // Expanding wave arcs
    display.drawCircle(64, 32, 8 + waveSize, SSD1306_WHITE);
    display.drawCircle(64, 32, 14 + waveSize, SSD1306_WHITE);

    waveSize += 2;
    if(waveSize > 6) waveSize = 0;

    display.display();
  }
}

// ---------------- OLED FUNCTION ----------------
void showOLED(String msg){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 25);
  display.println(msg);
  display.display();
}

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  pinMode(EMERGENCY_PIN, INPUT_PULLUP);
  pinMode(BRAKE_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(OVERTAKE_PIN, INPUT_PULLUP);

  Wire.begin(21,22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Startup Animation
  startupAnimation();

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_now_init();

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Sender Ready");
  showOLED("Sender Ready");
}

// ---------------- LOOP ----------------
void loop() {

  uint8_t currentPriority = 0;

  if(digitalRead(EMERGENCY_PIN) == LOW) currentPriority = 1;
  else if(digitalRead(BRAKE_PIN) == LOW) currentPriority = 2;
  else if(digitalRead(RIGHT_PIN) == LOW) currentPriority = 3;
  else if(digitalRead(LEFT_PIN) == LOW) currentPriority = 4;
  else if(digitalRead(OVERTAKE_PIN) == LOW) currentPriority = 5;

  if(currentPriority != lastPriority){

    esp_now_send(receiverAddress, &currentPriority, sizeof(currentPriority));

    Serial.print("Sending: ");
    Serial.println(currentPriority);

    if(currentPriority == 0){
      showOLED("Idle");
    }
    else{
      switch(currentPriority){
        case 1: showOLED("Sending EMERGENCY"); break;
        case 2: showOLED("Sending BRAKE"); break;
        case 3: showOLED("Sending RIGHT"); break;
        case 4: showOLED("Sending LEFT"); break;
        case 5: showOLED("Sending OVERTAKE"); break;
      }
    }

    lastPriority = currentPriority;
  }

  //idle animation when no action
  if(currentPriority == 0){
    idleAnimation();
  }

  delay(50);
}