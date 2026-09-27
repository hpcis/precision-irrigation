#define BLYNK_PRINT Serial

// ---------- COPY YOUR FIVE WORKSHOP VALUES HERE ----------
#define BLYNK_TEMPLATE_ID   "PASTE_TEMPLATE_ID_HERE"
#define BLYNK_TEMPLATE_NAME "PASTE_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN    "PASTE_DEVICE_AUTH_TOKEN_HERE"

char wifiName[] = "PASTE_WORKSHOP_WIFI_NAME_HERE";
char wifiPassword[] = "PASTE_WORKSHOP_WIFI_PASSWORD_HERE";
// --------------------------------------------------------

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Ticker.h>

const int soilPin = 35;
const int irrigationRelay = 27;

// Change these after measuring the sensor in dry and wet soil.
const int soilRawDry = 4095;
const int soilRawWet = 2000;

const unsigned long wateringTimeMs = 5000;

BlynkTimer dataTimer;
Ticker wateringStopTimer;
volatile bool watering = false;
volatile bool stoppedStateNeedsPublishing = false;

// Ticker runs this local safety action even if Wi-Fi or the phone disconnects.
void stopWateringHardware() {
  digitalWrite(irrigationRelay, HIGH);  // Active-low relay: HIGH is off
  watering = false;
  stoppedStateNeedsPublishing = true;
}

void startWatering() {
  if (watering) {
    return;
  }

  digitalWrite(irrigationRelay, LOW);   // Active-low relay: LOW is on
  watering = true;
  stoppedStateNeedsPublishing = false;
  wateringStopTimer.once_ms(wateringTimeMs, stopWateringHardware);

  Blynk.virtualWrite(V2, 1);
  Blynk.virtualWrite(V3, "WATERING");
}

// Blynk calls this function whenever the phone changes V1.
BLYNK_WRITE(V1) {
  int wateringRequest = param.asInt();

  if (wateringRequest == 1) {
    startWatering();
    Blynk.virtualWrite(V1, 0);  // Return the phone control to its off state
  }
}

// Update the phone at a controlled rate instead of on every loop.
void sendSensorData() {
  int soilRaw = analogRead(soilPin);
  int soilPercent = map(soilRaw, soilRawDry, soilRawWet, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, soilPercent);
  }

  Serial.print("Soil raw: ");
  Serial.print(soilRaw);
  Serial.print("  Moisture: ");
  Serial.print(soilPercent);
  Serial.println("%");
}

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, watering ? 1 : 0);
  Blynk.virtualWrite(V3, watering ? "WATERING" : "READY");
}

void publishStoppedState() {
  if (stoppedStateNeedsPublishing && Blynk.connected()) {
    stoppedStateNeedsPublishing = false;
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, "READY");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(soilPin, INPUT);
  digitalWrite(irrigationRelay, HIGH);  // Keep pump off during startup
  pinMode(irrigationRelay, OUTPUT);

  dataTimer.setInterval(2000L, sendSensorData);
  Blynk.begin(BLYNK_AUTH_TOKEN, wifiName, wifiPassword);
}

void loop() {
  Blynk.run();
  dataTimer.run();
  publishStoppedState();
}
