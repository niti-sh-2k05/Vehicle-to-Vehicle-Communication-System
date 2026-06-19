'#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define BUZZER 25

// LED Pins
#define LED1 4
#define LED2 16
#define LED3 17
#define LED4 5
#define LED5 18
#define LED6 19

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t priority = 0;

String alertText = "";
String distanceText = "";

bool newMessage = false;

unsigned long lastIdleAnim = 0;
int idleFrame = 0;


// ---------------- OLED FUNCTION ----------------
void showMessage(String alertText, String distanceText) {

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // ALERT TEXT (BIG)
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println(alertText);

  // Label
  display.setTextSize(1);
  display.setCursor(0,30);
  display.println("Distance of vehicle");

  // Distance Value (BIG)
  display.setTextSize(2);
  display.setCursor(0,42);
  display.println(distanceText);

  display.display();
}


// ---------------- BUZZER FUNCTION ----------------
void buzzer3Seconds() {

  ledcAttach(BUZZER, 2000, 8);
  ledcWrite(BUZZER, 128);

  delay(1500);

  ledcWrite(BUZZER, 0);
}


// ---------------- LED CONTROL ----------------
void turnOffAllLEDs() {

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
  digitalWrite(LED6, LOW);
}


// ---------------- IDLE SEARCHING ANIMATION ----------------
void idleAnimation() {

  if (millis() - lastIdleAnim > 200) {

    lastIdleAnim = millis();

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 5);
    display.println("  IDLE - Response...");

    display.setTextSize(2);
    display.setCursor(50, 30);

    switch(idleFrame) {

      case 0: display.println(">"); break;
      case 1: display.println(">>"); break;
      case 2: display.println(">>>"); break;
      case 3: display.println(">>>>"); break;
    }

    idleFrame = (idleFrame + 1) % 4;

    display.display();
  }
}


// ---------------- STARTUP ANIMATION ----------------
void startupAnimation() {

  int centerX = -30;
  int centerY = 32;

  while(centerX < 64) {

    display.clearDisplay();

    display.drawCircle(centerX, centerY, 24, SSD1306_WHITE);
    display.drawCircle(centerX, centerY, 23, SSD1306_WHITE);
    display.drawCircle(centerX, centerY, 18, SSD1306_WHITE);

    display.display();

    centerX += 4;
    delay(30);
  }

  delay(300);

  for(int angle = 0; angle < 360; angle += 10) {

    display.clearDisplay();

    display.drawCircle(64, 32, 24, SSD1306_WHITE);
    display.drawCircle(64, 32, 23, SSD1306_WHITE);
    display.drawCircle(64, 32, 18, SSD1306_WHITE);

    for(int i = 0; i < 360; i += 90) {

      float rad = (i + angle) * 3.14159 / 180.0;

      int x = 64 + 12 * cos(rad);
      int y = 32 + 12 * sin(rad);

      display.fillCircle(x, y, 6, SSD1306_WHITE);
    }

    display.display();
    delay(40);
  }

  display.clearDisplay();

  display.drawCircle(64, 32, 24, SSD1306_WHITE);
  display.drawCircle(64, 32, 23, SSD1306_WHITE);
  display.drawCircle(64, 32, 18, SSD1306_WHITE);

  display.fillCircle(64, 20, 6, SSD1306_WHITE);
  display.fillCircle(76, 32, 6, SSD1306_WHITE);
  display.fillCircle(64, 44, 6, SSD1306_WHITE);
  display.fillCircle(52, 32, 6, SSD1306_WHITE);

  display.display();
  delay(800);

  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(56, 29);
  display.println("V2V");

  display.display();
  delay(800);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(18, 25);
  display.println("Fasten Your Belts");

  display.display();
  delay(1200);
}


// ---------------- RECEIVE CALLBACK ----------------
void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len) {

  memcpy(&priority, incomingData, sizeof(priority));

  int rssi = info->rx_ctrl->rssi;

  Serial.print("Received Priority: ");
  Serial.print(priority);
  Serial.print(" RSSI: ");
  Serial.println(rssi);


  if(priority == 0){
    turnOffAllLEDs();
    return;
  }


  // -------- Approximate Distance --------
  if (rssi > -45) {
      distanceText = "<5m";
  }
  else if (rssi > -55) {
      distanceText = "~10m";
  }
  else {
      distanceText = ">15m";
  }


  turnOffAllLEDs();

  switch(priority) {

    case 1:
      alertText = "EMERGENCY";
      digitalWrite(LED1, HIGH);
      newMessage = true;
      break;

    case 2:
      alertText = "BRAKE";
      digitalWrite(LED2, HIGH);
      newMessage = true;
      break;

    case 3:
      alertText = "RIGHT TURN";
      digitalWrite(LED3, HIGH);
      newMessage = true;
      break;

    case 4:
      alertText = "LEFT TURN";
      digitalWrite(LED4, HIGH);
      newMessage = true;
      break;

    case 5:
      alertText = "OVERTAKE";
      digitalWrite(LED5, HIGH);
      newMessage = true;
      break;
  }
}


// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  turnOffAllLEDs();

  Wire.begin(21,22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(true);
  }

  display.clearDisplay();
  display.display();

  startupAnimation();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW INIT FAILED");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver Ready");
  showMessage("Receiver", "READY");
}


// ---------------- LOOP ----------------
void loop() {

  if(newMessage){

    showMessage(alertText, distanceText);

    if(priority == 1){
      buzzer3Seconds();
    }

    newMessage = false;
  }

  if(priority == 0){
    idleAnimation();
  }
}'