# Beginner Blynk Mobile-Control Tutorial

This second tutorial connects the ESP32 irrigation controller to the Blynk mobile app. It is designed for students with little programming experience and builds only one small, testable feature at a time.

By the end, a student can:

- read soil moisture on a phone;
- press a phone control to request watering;
- see whether the irrigation relay is on; and
- explain how a Blynk Virtual Pin connects a phone widget to ESP32 code.

Return to the [standalone ESP32 irrigation tutorial](README.md) if the soil sensor and GPIO27 relay have not already been tested locally.

![Finished irrigation box prepared for a presentation slide](assets/images/irrigation-box-finished-transparent.png)

This stage uses only the soil sensor on GPIO35 and the irrigation relay on GPIO27. The other modules may remain mounted, but the beginner Blynk sketch does not read or control them.

## What the system will do

```text
Soil sensor -> ESP32 -> Wi-Fi -> Blynk Cloud -> phone gauge
Phone button -> Blynk Cloud -> ESP32 -> GPIO27 relay -> pump or valve
```

The phone does not drive GPIO27 directly. It sends a value through a Blynk **datastream**. The ESP32 receives that value on a **Virtual Pin**, checks the safety rules, and then operates the relay.

| Name | Virtual Pin | Direction | Purpose |
| --- | --- | --- | --- |
| Soil Moisture | V0 | ESP32 to phone | Moisture from 0 to 100 percent |
| Water 5 Seconds | V1 | Phone to ESP32 | A value of 1 requests one short watering cycle |
| Pump State | V2 | ESP32 to phone | Shows 0 for off or 1 for on |
| Device Status | V3 | ESP32 to phone | Shows a short status message |

Virtual Pins are communication channels, not ESP32 GPIO numbers. `V1` therefore does **not** mean GPIO1. Blynk recommends Virtual Pin datastreams when app data must be processed by the microcontroller. See Blynk's [Virtual Pins documentation](https://docs.blynk.io/en/blynk-library-firmware-api/virtual-pins) and [device-control guide](https://docs.blynk.io/en/getting-started/using-virtual-pins-to-control-physical-devices).

## Safety before connecting Blynk

- Complete the local relay test before adding Wi-Fi.
- First test with the relay LED only. Leave the pump or valve disconnected from the relay screw terminals.
- The example expects an **active-low** relay on GPIO27: `HIGH` is off and `LOW` is on.
- Keep the ESP32, relay logic, and student wiring at safe low voltage.
- Use a separate fused supply for a pump or valve. Never power a load from an ESP32 pin or USB port.
- Keep mains-voltage wiring out of the student exercise. A qualified electrician must handle any mains load.
- The example stops a watering cycle after five seconds even if the phone disconnects.
- Supervise the system. The five-second timer is a classroom safeguard, not a complete field safety system.
- Follow the voltage-domain rules in the [standalone hardware tutorial](README.md#important-safety-rules), especially if the shield voltage rail is set to 5 V.

## Before you begin

- An ESP32 with the soil sensor connected to GPIO35
- A 3.3 V-input-compatible active-low relay module connected to GPIO27
- A USB data cable and Arduino IDE
- A Wi-Fi network the ESP32 may use
- A Blynk account and the Blynk mobile app
- Optionally, a Blynk template that your teacher prepared for the class

Do not publish a real Wi-Fi password or Blynk Auth Token in screenshots, shared documents, or a public repository. For a classroom, use a temporary or isolated training network when possible.

## Part 1: Install the Blynk library

1. Open Arduino IDE.
2. Select **Tools > Manage Libraries**.
3. Search for `Blynk`.
4. Install the library published by Volodymyr Shymanskyy.
5. Confirm that **File > Examples > Blynk** now appears.

Keep the ESP32 board package and USB port settings from the first tutorial.

## Part 2: Create a Blynk template

1. Sign in to [Blynk.Console](https://blynk.cloud/).
2. Open **Developer Zone > Templates**.
3. Create a new template with these settings:
   - Name: `Student Irrigation Box`
   - Hardware: `ESP32`
   - Connection type: `WiFi`
4. Open the template's **Datastreams** tab.
5. Add the four Virtual Pin datastreams in the following table.

| Datastream name | Pin | Data type | Minimum | Maximum | Unit |
| --- | ---: | --- | ---: | ---: | --- |
| Soil Moisture | V0 | Integer | 0 | 100 | % |
| Water 5 Seconds | V1 | Integer | 0 | 1 | none |
| Pump State | V2 | Integer | 0 | 1 | none |
| Device Status | V3 | String | N/A | N/A | none |

A datastream describes the value travelling between one device and Blynk. Blynk's current setup guide shows how to [create Virtual Pin datastreams](https://docs.blynk.io/en/getting-started/template-quick-setup/set-up-datastreams).

## Part 3: Create a device and copy its credentials

1. In Blynk.Console, create a device **from the template** named `Student Irrigation Box`.
2. Open the new device.
3. Open **Device Info** or **Developer Tools**, depending on the console layout.
4. Copy these three values into a temporary private note:
   - `BLYNK_TEMPLATE_ID`
   - `BLYNK_TEMPLATE_NAME`
   - `BLYNK_AUTH_TOKEN`

Each physical ESP32 should have its own device and Auth Token. Do not give several boards the same token. Blynk documents this prototype workflow under [Manual Device Activation](https://docs.blynk.io/en/getting-started/activating-devices/manual-device-activation).

## Part 4: Build the phone dashboard

Install and sign in to the Blynk mobile app. Enable Developer Mode if the app asks for it, then open the `Student Irrigation Box` template and create its mobile dashboard.

Add these widgets:

1. A **Gauge** or **Labeled Value** connected to `Soil Moisture (V0)`.
2. A **Button** or **Switch** connected to `Water 5 Seconds (V1)`. Set its off/on values to 0 and 1. If a push-button mode is available, use it; otherwise the ESP32 code resets the value to 0 after accepting a request.
3. A **Labeled Value** or indicator connected to `Pump State (V2)`.
4. A **Labeled Value** connected to `Device Status (V3)`.

The mobile layout and web layout are separate in Blynk. This exercise uses the mobile layout. See Blynk's [mobile dashboard guide](https://docs.blynk.io/en/getting-started/template-quick-setup/set-up-mobile-app-dashboard) if the widget editor is unfamiliar.

Suggested layout:

```text
+---------------------------+
|     Soil Moisture  52%    |
|          (gauge)          |
+---------------------------+
|    [ WATER 5 SECONDS ]    |
+---------------------------+
| Pump: 0     Status: READY |
+---------------------------+
```

## Part 5: Upload the beginner sketch

Create a new Arduino sketch. Copy all of the code below, then replace the credential and Wi-Fi placeholder values near the top. If the template has a different name, replace `Student Irrigation Box` as well.

```cpp
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Student Irrigation Box"
#define BLYNK_AUTH_TOKEN    "YOUR_DEVICE_AUTH_TOKEN"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char wifiName[] = "YOUR_WIFI_NAME";
char wifiPassword[] = "YOUR_WIFI_PASSWORD";

const int soilPin = 35;
const int irrigationRelay = 27;

// Change these after measuring the sensor in dry and wet soil.
const int soilRawDry = 4095;
const int soilRawWet = 2000;

const unsigned long wateringTimeMs = 5000;

BlynkTimer timer;
bool watering = false;
unsigned long wateringStartedAt = 0;

void stopWatering() {
  digitalWrite(irrigationRelay, HIGH);  // Active-low relay: HIGH is off
  watering = false;
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, "READY");
}

void startWatering() {
  if (watering) {
    return;
  }

  digitalWrite(irrigationRelay, LOW);   // Active-low relay: LOW is on
  watering = true;
  wateringStartedAt = millis();
  Blynk.virtualWrite(V2, 1);
  Blynk.virtualWrite(V3, "WATERING");
}

// Blynk calls this function whenever the phone changes V1.
BLYNK_WRITE(V1) {
  int request = param.asInt();

  if (request == 1) {
    startWatering();
    Blynk.virtualWrite(V1, 0);  // Return the phone control to its off state
  }
}

// Update the phone at a controlled rate instead of on every loop.
void sendSensorData() {
  int soilRaw = analogRead(soilPin);
  int soilPercent = map(soilRaw, soilRawDry, soilRawWet, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  Blynk.virtualWrite(V0, soilPercent);

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

void setup() {
  Serial.begin(115200);

  pinMode(soilPin, INPUT);
  pinMode(irrigationRelay, OUTPUT);
  digitalWrite(irrigationRelay, HIGH);  // Keep pump off during startup

  Blynk.begin(BLYNK_AUTH_TOKEN, wifiName, wifiPassword);
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();

  // This local timer still stops the relay if the phone loses connection.
  if (watering && millis() - wateringStartedAt >= wateringTimeMs) {
    stopWatering();
  }
}
```

The sketch uses `BlynkTimer` to send sensor data once every two seconds. Do not put an unrestricted `Blynk.virtualWrite()` in `loop()`: Blynk warns that sending on every loop can flood the cloud connection. See [Send Data From Hardware to Blynk](https://docs.blynk.io/en/getting-started/how-to-display-any-sensor-data-in-blynk-app).

## Part 6: Test without a pump

1. Disconnect the pump or valve from the relay contacts.
2. Upload the sketch.
3. Open Serial Monitor at 115200 baud.
4. Wait for Blynk to report that the device is ready or online.
5. Check that the mobile dashboard shows a changing moisture value.
6. Tap **Water 5 Seconds** once.
7. Confirm that the GPIO27 relay indicator turns on, `Pump State` changes to 1, and `Device Status` shows `WATERING`.
8. Confirm that the relay turns off after about five seconds and the dashboard returns to `Pump State = 0` and `READY`.
9. Turn off the phone's Wi-Fi during a new test. Confirm that the relay still turns off after five seconds.
10. Repeat the test three times before connecting a real load.

Stop immediately if the relay turns on during ESP32 reset, remains on longer than five seconds, or behaves opposite to the comments in the code. The relay board may not match the expected active-low design.

## Part 7: Supervised water test

Only continue after the no-load test passes.

1. Disconnect all power.
2. Connect a low-voltage pump or valve to the relay's `COM` and `NO` terminals using its separate fused supply.
3. Check tubing, polarity, insulation, and the water path.
4. Restore power while a teacher or another responsible person watches the system.
5. Tap the watering control once.
6. Verify that water flows to the correct area and stops after five seconds.
7. Check for leaks and confirm that the ESP32 does not reset when the pump starts.

Do not repeatedly press the button to defeat the short-run limit. A later version should also include a tank-empty input, a longer lockout between runs, flow confirmation, and a physical emergency stop.

## Record your test results

Complete this table during the test.

| Observation | Expected result | Actual result |
| --- | --- | --- |
| ESP32 starts | Relay remains off | |
| Soil probe in dry sample | Moisture moves toward 0% | |
| Soil probe in moist sample | Moisture moves toward 100% | |
| Phone control pressed once | Relay turns on | |
| Five seconds pass | Relay turns off | |
| Phone loses connection during watering | Relay still turns off | |

Answer these questions:

1. What is the difference between GPIO27 and Virtual Pin V1?
2. Why does the program use a timer instead of leaving the phone switch in control of the relay?
3. What additional sensor should prevent a pump from running with an empty tank?

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Sketch does not compile | Install the Blynk library and confirm the ESP32 board package is selected |
| Device stays offline | Check the network name, password, Auth Token, and that a compatible 2.4 GHz network without a browser sign-in page is reachable by the ESP32 |
| Authentication error | Copy the Auth Token from the correct Blynk device, not from another student's device |
| Dashboard has no values | Confirm that widgets use V0, V1, V2, and V3 exactly as listed |
| Moisture is always 0% or 100% | Record the raw serial readings and replace `soilRawDry` and `soilRawWet` with measured values |
| Button changes but relay does not | Check the V1 datastream, GPIO27 wiring, common ground, and relay input-voltage compatibility |
| Relay logic is reversed | Stop the test and verify whether the relay module is active low or active high before changing code |
| ESP32 resets when pump starts | Disconnect the pump; correct load power, grounding, suppression, and wiring before retesting |
| Dashboard control stays on | Confirm V1 accepts device updates and that `Blynk.virtualWrite(V1, 0)` is present |

## Optional challenge: automatic mode

Do not add automatic watering until every group can explain and test the five-second manual cycle. A good next exercise is to add a `Manual/Automatic` datastream and start a short watering cycle only when all of these are true:

- automatic mode is enabled;
- soil moisture is below the calibrated threshold;
- the tank is not empty;
- the minimum time between watering cycles has passed; and
- no sensor-failure condition is active.

Keep the maximum runtime enforced locally on the ESP32. Cloud connectivity should provide monitoring and requests, but it should never be the only mechanism capable of stopping a pump.
