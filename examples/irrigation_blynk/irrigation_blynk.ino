#define BLYNK_PRINT Serial

// ---------- COPY YOUR FIVE WORKSHOP VALUES HERE ----------
#define BLYNK_TEMPLATE_ID   "PASTE_TEMPLATE_ID_HERE"
#define BLYNK_TEMPLATE_NAME "PASTE_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN    "PASTE_DEVICE_AUTH_TOKEN_HERE"

char wifiName[] = "PASTE_WORKSHOP_WIFI_NAME_HERE";
char wifiPassword[] = "PASTE_WORKSHOP_WIFI_PASSWORD_HERE";
// --------------------------------------------------------

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// ----------------------- Hardware -----------------------
const int dhtPin = 4;
const int ldrPin = 34;
const int soilPin = 35;
const int trigPin = 18;
const int echoPin = 19;

const int lampRelay = 25;
const int fanRelay = 26;
const int irrigationRelay = 27;
const int refillRelay = 14;

const int relayOn = LOW;    // The workshop relay board is active-low.
const int relayOff = HIGH;

DHT dht(dhtPin, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------------------- Sensor calibration ----------------
// Change these values after measuring the actual box.
const int lightRawDark = 2500;
const int lightRawBright = 4095;
const int soilRawDry = 4095;
const int soilRawWet = 2000;
const int tankHeightCm = 45;

// -------------------- Control thresholds ----------------
const int lightOnBelowPercent = 30;
const int lightOffAtPercent = 35;
const float fanOnAboveC = 30.0;
const float fanOffAtC = 29.0;
const int irrigationOnBelowPercent = 40;
const int irrigationOffAtPercent = 45;
const int refillOnBelowCm = 30;
const int refillOffAtCm = 32;

const unsigned long manualWateringTimeMs = 5000;
const unsigned long sensorIntervalMs = 2000;
const unsigned long cloudIntervalMs = 10000;
const unsigned long lcdIntervalMs = 2000;
const unsigned long blynkReconnectIntervalMs = 10000;

// ----------------------- Live data -----------------------
int lightPercent = 0;
int soilPercent = 0;
int waterLevelCm = 0;
float temperatureC = 0.0;
float humidityPercent = 0.0;
bool dhtValid = false;
bool tankValid = false;

bool lampOn = false;
bool fanOn = false;
bool automaticIrrigationOn = false;
bool manualIrrigationOn = false;
bool irrigationOn = false;
bool refillOn = false;

unsigned long manualWateringStartedAt = 0;
unsigned long lastSensorReadAt = 0;
unsigned long lastCloudPublishAt = 0;
unsigned long lastLcdChangeAt = 0;
unsigned long lastBlynkConnectAttemptAt = 0;
byte lcdPage = 0;

// --------------------- Helper functions ------------------
void setRelay(int pin, bool turnOn) {
  digitalWrite(pin, turnOn ? relayOn : relayOff);
}

int readTankLevelCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Stop waiting after 30 ms so a missing echo cannot freeze the program.
  unsigned long durationUs = pulseIn(echoPin, HIGH, 30000UL);
  if (durationUs == 0) {
    tankValid = false;
    return waterLevelCm;
  }

  float distanceCm = durationUs * 0.0343f / 2.0f;
  int level = tankHeightCm - (int)(distanceCm + 0.5f);
  tankValid = true;
  return constrain(level, 0, tankHeightCm);
}

void updateAutomaticOutputs() {
  if (!lampOn && lightPercent < lightOnBelowPercent) {
    lampOn = true;
  } else if (lampOn && lightPercent >= lightOffAtPercent) {
    lampOn = false;
  }

  if (!dhtValid) {
    fanOn = false;
  } else if (!fanOn && temperatureC > fanOnAboveC) {
    fanOn = true;
  } else if (fanOn && temperatureC <= fanOffAtC) {
    fanOn = false;
  }

  if (!automaticIrrigationOn && soilPercent < irrigationOnBelowPercent) {
    automaticIrrigationOn = true;
  } else if (automaticIrrigationOn && soilPercent >= irrigationOffAtPercent) {
    automaticIrrigationOn = false;
  }

  // A missing ultrasonic echo must leave the refill pump off.
  if (!tankValid) {
    refillOn = false;
  } else if (!refillOn && waterLevelCm < refillOnBelowCm) {
    refillOn = true;
  } else if (refillOn && waterLevelCm >= refillOffAtCm) {
    refillOn = false;
  }

  irrigationOn = automaticIrrigationOn || manualIrrigationOn;
  setRelay(lampRelay, lampOn);
  setRelay(fanRelay, fanOn);
  setRelay(irrigationRelay, irrigationOn);
  setRelay(refillRelay, refillOn);
}

void readSensors() {
  int lightRaw = analogRead(ldrPin);
  int soilRaw = analogRead(soilPin);

  lightPercent = map(lightRaw, lightRawDark, lightRawBright, 0, 100);
  lightPercent = constrain(lightPercent, 0, 100);
  soilPercent = map(soilRaw, soilRawDry, soilRawWet, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  float newTemperature = dht.readTemperature();
  float newHumidity = dht.readHumidity();
  dhtValid = !isnan(newTemperature) && !isnan(newHumidity);
  if (dhtValid) {
    temperatureC = newTemperature;
    humidityPercent = newHumidity;
  }

  waterLevelCm = readTankLevelCm();
  updateAutomaticOutputs();

  Serial.print("Light: ");
  Serial.print(lightPercent);
  Serial.print("%  Temp: ");
  if (dhtValid) Serial.print(temperatureC, 1); else Serial.print("ERROR");
  Serial.print(" C  Humidity: ");
  if (dhtValid) Serial.print(humidityPercent, 0); else Serial.print("ERROR");
  Serial.print("%  Soil: ");
  Serial.print(soilPercent);
  Serial.print("%  Tank: ");
  if (tankValid) Serial.print(waterLevelCm); else Serial.print("ERROR");
  Serial.print(" cm  Relays L/F/I/R: ");
  Serial.print(lampOn);
  Serial.print('/');
  Serial.print(fanOn);
  Serial.print('/');
  Serial.print(irrigationOn);
  Serial.print('/');
  Serial.println(refillOn);
}

const char* systemStatus() {
  if (!dhtValid) return "CHECK DHT22";
  if (!tankValid) return "CHECK TANK SENSOR";
  if (manualIrrigationOn && automaticIrrigationOn) return "AUTO + MANUAL WATER";
  if (manualIrrigationOn) return "MANUAL WATER 5 SEC";
  return "AUTOMATIC CONTROL";
}

void publishToBlynk() {
  if (!Blynk.connected()) {
    return;
  }

  Blynk.virtualWrite(V0, lightPercent);
  if (dhtValid) {
    Blynk.virtualWrite(V1, temperatureC);
    Blynk.virtualWrite(V2, humidityPercent);
  }
  Blynk.virtualWrite(V3, soilPercent);
  if (tankValid) {
    Blynk.virtualWrite(V4, waterLevelCm);
  }
  Blynk.virtualWrite(V6, lampOn ? 1 : 0);
  Blynk.virtualWrite(V7, fanOn ? 1 : 0);
  Blynk.virtualWrite(V8, irrigationOn ? 1 : 0);
  Blynk.virtualWrite(V9, refillOn ? 1 : 0);
  Blynk.virtualWrite(V10, systemStatus());
}

void showLcdPage() {
  lcd.setCursor(0, 0);
  if (lcdPage == 0) {
    lcd.print("Light:");
    lcd.print(lightPercent);
    lcd.print("%       ");
    lcd.setCursor(0, 1);
    lcd.print("Lamp:");
    lcd.print(lampOn ? "ON " : "OFF");
    lcd.print("         ");
  } else if (lcdPage == 1) {
    lcd.print("Temp:");
    if (dhtValid) lcd.print(temperatureC, 1); else lcd.print("ERR");
    lcd.print("C      ");
    lcd.setCursor(0, 1);
    lcd.print("Hum:");
    if (dhtValid) lcd.print(humidityPercent, 0); else lcd.print("ERR");
    lcd.print("% Fan:");
    lcd.print(fanOn ? "ON " : "OFF");
  } else if (lcdPage == 2) {
    lcd.print("Soil:");
    lcd.print(soilPercent);
    lcd.print("%       ");
    lcd.setCursor(0, 1);
    lcd.print("Water:");
    lcd.print(irrigationOn ? "ON " : "OFF");
    lcd.print("       ");
  } else {
    lcd.print("Tank:");
    if (tankValid) lcd.print(waterLevelCm); else lcd.print("ERR");
    lcd.print("cm       ");
    lcd.setCursor(0, 1);
    lcd.print("Refill:");
    lcd.print(refillOn ? "ON " : "OFF");
    lcd.print("      ");
  }
  lcdPage = (lcdPage + 1) % 4;
}

void startManualWatering() {
  manualIrrigationOn = true;
  manualWateringStartedAt = millis();
  updateAutomaticOutputs();
  publishToBlynk();
}

void updateManualWateringTimer() {
  if (manualIrrigationOn &&
      millis() - manualWateringStartedAt >= manualWateringTimeMs) {
    manualIrrigationOn = false;
    updateAutomaticOutputs();
    publishToBlynk();
  }
}

// The mobile button sends 1 on Virtual Pin V5.
BLYNK_WRITE(V5) {
  if (param.asInt() == 1) {
    startManualWatering();
    Blynk.virtualWrite(V5, 0);
  }
}

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V5, 0);
  publishToBlynk();
}

void maintainCloudConnection() {
  if (WiFi.status() != WL_CONNECTED || Blynk.connected()) {
    return;
  }

  unsigned long now = millis();
  if (now - lastBlynkConnectAttemptAt >= blynkReconnectIntervalMs) {
    lastBlynkConnectAttemptAt = now;
    Serial.println("Wi-Fi connected; trying Blynk.Cloud...");
    Blynk.connect(1000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Load the OFF level before each pin becomes an output.
  digitalWrite(lampRelay, relayOff);
  digitalWrite(fanRelay, relayOff);
  digitalWrite(irrigationRelay, relayOff);
  digitalWrite(refillRelay, relayOff);
  pinMode(lampRelay, OUTPUT);
  pinMode(fanRelay, OUTPUT);
  pinMode(irrigationRelay, OUTPUT);
  pinMode(refillRelay, OUTPUT);

  pinMode(ldrPin, INPUT);
  pinMode(soilPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  dht.begin();
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Farm IoT  ");
  lcd.setCursor(0, 1);
  lcd.print("Starting...     ");

  readSensors();
  lastSensorReadAt = millis();
  lastCloudPublishAt = millis();
  lastLcdChangeAt = millis();

  // Connect without blocking the local smart-farm control loop.
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiName, wifiPassword);
  Blynk.config(BLYNK_AUTH_TOKEN);
  lastBlynkConnectAttemptAt = millis() - blynkReconnectIntervalMs;
  Serial.println("Local automatic control is running; connecting to Wi-Fi...");
}

void loop() {
  maintainCloudConnection();
  if (Blynk.connected()) {
    Blynk.run();
  }
  updateManualWateringTimer();

  unsigned long now = millis();
  if (now - lastSensorReadAt >= sensorIntervalMs) {
    lastSensorReadAt = now;
    readSensors();
  }
  if (now - lastLcdChangeAt >= lcdIntervalMs) {
    lastLcdChangeAt = now;
    showLcdPage();
  }
  if (now - lastCloudPublishAt >= cloudIntervalMs) {
    lastCloudPublishAt = now;
    publishToBlynk();
  }
}
