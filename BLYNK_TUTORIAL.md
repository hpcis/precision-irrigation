# Beginner Blynk Dashboard Tutorial

This tutorial connects the ESP32 irrigation controller to Blynk. It is written as a click-by-click class activity for learners who have not used Blynk before.

By the end, a learner can:

- open only the template and device assigned to their group;
- create four Virtual Pin datastreams;
- build a mobile dashboard one widget at a time;
- read soil moisture on a phone;
- request one five-second watering cycle; and
- explain why the phone requests an action but the ESP32 enforces the safety timer.

Return to the [standalone ESP32 irrigation tutorial](README.md) if the soil sensor and GPIO27 relay have not already passed the local tests.

![Finished irrigation box prepared for a presentation slide](assets/images/irrigation-box-finished-transparent.png)

This beginner stage uses the soil sensor on GPIO35 and irrigation relay 3 on GPIO27. The other modules may remain mounted, but this sketch does not control them.

## The complete learning path

![Blynk setup process from account sign-in to safe relay testing](assets/images/blynk-setup-process.png)

Follow the six stages in order. Do not build the dashboard first and guess the datastreams later: every widget needs a datastream, every device needs a template, and the code needs credentials from the correct device.

## Shared training account and group names

Use the classroom Blynk account:

```text
Email: itetu.training@gmail.com
```

The instructor should sign in before the activity or enter the password privately. Learners must not write the password in this repository, a slide, a screenshot, or a group chat. Do not change the password, sign the account out of another station, or change account settings.

All groups can see the account's resources, so the names are the safety boundary. Use the row assigned by the instructor and copy its spelling exactly.

| Group | Colour | Blynk template name | Blynk device name |
| --- | --- | --- | --- |
| 1 | Red | `Irrigation-G1` | `TU-Box-G1` |
| 2 | Blue | `Irrigation-G2` | `TU-Box-G2` |
| 3 | Green | `Irrigation-G3` | `TU-Box-G3` |
| 4 | Yellow | `Irrigation-G4` | `TU-Box-G4` |

Each group has a **separate template** and a **separate device**. For example, Group 2 edits `Irrigation-G2`, opens `TU-Box-G2`, and uses the Auth Token from `TU-Box-G2`. It must not use `Irrigation-G1` with `TU-Box-G2`.

There is no template named `Student Irrigation Box` in this activity. The required template name is exactly `Irrigation-G1` for Group 1, `Irrigation-G2` for Group 2, `Irrigation-G3` for Group 3, or `Irrigation-G4` for Group 4. Capital letters, hyphens, and group numbers must match.

Before clicking anything, complete this station record:

```text
Our group number: ____________________
Our template name: ___________________
Our device name: _____________________
```

Shared-account rules:

1. Open only your group's template and device.
2. Never rename or delete any template, device, datastream, or dashboard owned by another group.
3. Never operate another group's watering control.
4. Do not reuse another device's Auth Token.
5. Ask the instructor before signing out or changing account settings.
6. If the displayed name does not match the station record, stop and return to the resource list.

## Understand the four Blynk objects

| Object | Meaning in this lesson | Example for Group 1 |
| --- | --- | --- |
| Template | The reusable design: datastreams and dashboard layout | `Irrigation-G1` |
| Device | The cloud record for one physical ESP32 box | `TU-Box-G1` |
| Datastream | A named data channel between Blynk and the ESP32 | `Soil Moisture (V0)` |
| Widget | A gauge, switch, or value tile on the dashboard | Gauge connected to V0 |

The phone does not drive GPIO27 directly. It sends a value through a Blynk datastream. The ESP32 receives the Virtual Pin value, checks its local rules, and then operates the physical relay.

![Concept map showing sensor data moving to Blynk and a watering request returning to the ESP32](assets/images/blynk-learning-map.png)

| Name | Virtual Pin | Direction | Purpose |
| --- | --- | --- | --- |
| Soil Moisture | V0 | ESP32 to dashboard | Moisture from 0 to 100 percent |
| Water 5 Seconds | V1 | Dashboard to ESP32 | A value of 1 requests one short watering cycle |
| Pump State | V2 | ESP32 to dashboard | Shows 0 for off or 1 for on |
| Device Status | V3 | ESP32 to dashboard | Shows `READY` or `WATERING` |

Virtual Pins are communication channels, not ESP32 pins. `V1` does **not** mean GPIO1. The physical soil input is GPIO35 and the physical irrigation relay output is GPIO27. See Blynk's [Virtual Pins documentation](https://docs.blynk.io/en/blynk-library-firmware-api/virtual-pins) and [device-control guide](https://docs.blynk.io/en/getting-started/using-virtual-pins-to-control-physical-devices).

## Safety before connecting Blynk

- Complete the local relay test before adding Wi-Fi.
- First test with the relay LED only. Leave the pump or valve disconnected from the relay screw terminals.
- The example expects an **active-low** relay on GPIO27: `HIGH` is off and `LOW` is on.
- Keep the ESP32, relay logic, and student wiring at safe low voltage.
- Use a separate fused supply for a pump or valve. Never power a load from an ESP32 pin or USB port.
- Keep mains-voltage wiring out of the student exercise. A qualified electrician must handle any mains load.
- The ESP32 stops a watering cycle after five seconds even if the phone disconnects.
- Supervise the system. The five-second timer is a classroom safeguard, not a complete field safety system.
- Follow the voltage-domain rules in the [standalone hardware tutorial](README.md#important-safety-rules), especially if the shield voltage rail is set to 5 V.

## Before you begin

- ESP32 box with the soil sensor on GPIO35 and an active-low relay input on GPIO27
- USB data cable and Arduino IDE
- Blynk library for Arduino
- Wi-Fi network the ESP32 may use
- Blynk mobile app on a phone or tablet
- Shared account already signed in, or the instructor present to sign it in
- Your completed group station record

Do not publish a Wi-Fi password, account password, or Blynk Auth Token. Use a temporary or isolated training network when possible.

## Part 1: Install the Blynk library

1. Open Arduino IDE.
2. Click **Tools > Manage Libraries**.
3. Click the search field and type `Blynk`.
4. Find the library published by Volodymyr Shymanskyy.
5. Click **Install**.
6. Wait for the installation to finish.
7. Click **File > Examples > Blynk** to confirm that the examples appear.

Keep the ESP32 board package, board selection, and USB port settings from the standalone tutorial.

## Part 2: Sign in and turn on Developer Mode

### In the web console

1. Open [Blynk.Console](https://blynk.cloud/) in the browser.
2. If a sign-in page appears, enter `itetu.training@gmail.com` in the email field.
3. Ask the instructor to enter the password. Do not ask the browser to display or save it on a public computer unless the instructor approves.
4. After sign-in, look at the top-right corner for the profile or account icon.
5. Click the profile icon.
6. Turn **Developer Mode** on if it is off.
7. Look at the left navigation. If it is hidden, click the **menu icon (☰)** to expand it.
8. Click **Developer Zone**.
9. Click **My Templates** or **Templates**. Blynk has used both labels in recent layouts.

You should now see the class templates. Stop if the shared-account email or organization is wrong.

### In the mobile app

1. Open the **Blynk IoT** app.
2. Sign in with `itetu.training@gmail.com` if the instructor has not already done so.
3. Tap the **profile/person icon**.
4. Find **Developer Mode** and turn its switch on.
5. Return to the main screen.
6. Tap **Developer Mode** or the **wrench/tool icon**, depending on the app version.

Developer Mode is where a template's mobile layout is edited. The normal Devices view is where the live `TU-Box-G1`, `TU-Box-G2`, `TU-Box-G3`, or `TU-Box-G4` device is operated.

## Part 3: Open or create the assigned template

### Normal learner path: open the existing template

1. In the web console, click **Developer Zone > My Templates**.
2. Read your station record.
3. Find the exact assigned name: `Irrigation-G1`, `Irrigation-G2`, `Irrigation-G3`, or `Irrigation-G4`.
4. Click the template name once.
5. Confirm the name at the top of the page before editing.

Do not click **+ New Template** when your assigned template already exists. A name such as `Irrigation-G1 copy` is not an acceptable replacement.

### If the instructor asks your group to create a missing template

1. Click **Developer Zone > My Templates**.
2. Click **+ New Template**.
3. In **Name**, type your assigned template name exactly.
4. In **Hardware**, choose **ESP32**.
5. In **Connection Type** or **Connectivity**, choose **WiFi**.
6. Click **Done** or **Create**.
7. Click **Save** if a Save button appears at the top-right.
8. Return to **My Templates** and verify that only one template has the assigned name.

If a template is missing and you were not instructed to create it, stop and tell the instructor.

## Part 4: Add the four datastreams

Repeat the following click path once for each row in the table.

1. Open `Irrigation-G1`, `Irrigation-G2`, `Irrigation-G3`, or `Irrigation-G4` according to your group number.
2. Click the **Datastreams** tab.
3. Click **Edit** if the page is read-only.
4. Click **+ New Datastream**.
5. Click **Virtual Pin**.
6. Enter the row's **Name**.
7. Open the **Pin** dropdown and choose the listed Virtual Pin.
8. Open **Data Type** and choose the listed type.
9. For integer values, enter the minimum and maximum.
10. Enter the unit only for V0.
11. Click **Create**.
12. Repeat for the next row.
13. When all four rows are visible, click **Save** or **Save and Apply** if shown.

| Datastream name | Pin | Data type | Minimum | Maximum | Unit |
| --- | ---: | --- | ---: | ---: | --- |
| Soil Moisture | V0 | Integer | 0 | 100 | `%` |
| Water 5 Seconds | V1 | Integer | 0 | 1 | leave blank |
| Pump State | V2 | Integer | 0 | 1 | leave blank |
| Device Status | V3 | String | not shown | not shown | leave blank |

Checkpoint: the template must show exactly four datastreams using V0, V1, V2, and V3 once each. If a pin is duplicated, edit the incorrect row before continuing. Blynk's official [datastream setup guide](https://docs.blynk.io/en/getting-started/template-quick-setup/set-up-datastreams) includes the current console workflow.

## Part 5: Create or open the assigned device

First check whether the device already exists:

1. Click **Search** or **Devices** in the left navigation.
2. Click **My Devices** if that submenu appears.
3. Find your assigned device name, such as `TU-Box-G2`.
4. If it exists, click it and skip to **Copy the device credentials** below.

Create it only if it is missing and the instructor tells you to:

1. Click **+ New Device**.
2. If Blynk asks for a creation method, click **From Template**.
3. Open the **Template** dropdown.
4. Choose your exact assigned template, such as `Irrigation-G2`.
5. In **Device Name**, type the matching assigned device name, such as `TU-Box-G2`.
6. Click **Create**.
7. Confirm that the template and device group numbers match.

### Copy the device credentials

1. Open `TU-Box-G1`, `TU-Box-G2`, `TU-Box-G3`, or `TU-Box-G4` according to your group number.
2. Click **Device Info** or **Developer Tools**. The label varies by console version.
3. Find the firmware configuration snippet.
4. Use the **copy icon** if available and copy these three definitions into a temporary private note:

```cpp
#define BLYNK_TEMPLATE_ID "..."
#define BLYNK_AUTH_TOKEN  "..."
```

5. Record the exact template-name line shown for your group:

| Group | Required template-name line |
| --- | --- |
| 1 | `#define BLYNK_TEMPLATE_NAME "Irrigation-G1"` |
| 2 | `#define BLYNK_TEMPLATE_NAME "Irrigation-G2"` |
| 3 | `#define BLYNK_TEMPLATE_NAME "Irrigation-G3"` |
| 4 | `#define BLYNK_TEMPLATE_NAME "Irrigation-G4"` |

6. Verify that the name exactly matches your station record.
7. Keep the Auth Token private. Do not commit it to Git or place it in a screenshot.

Each physical box needs its own Blynk device and Auth Token. Never copy the token from `TU-Box-G1`, `TU-Box-G2`, `TU-Box-G3`, or `TU-Box-G4` unless it is your assigned device. See Blynk's [manual activation guide](https://docs.blynk.io/en/getting-started/activating-devices/manual-device-activation).

## Part 6: Build the mobile dashboard

The Blynk mobile dashboard and web dashboard are separate layouts. Completing one does not automatically build the other. Build the mobile layout first.

![Illustrated click guide for adding and connecting the four mobile widgets](assets/images/blynk-dashboard-click-guide.png)

The image is a learning illustration, not a literal screenshot. If an icon has moved, follow the written label and click path below.

### Open the correct mobile template

1. Open **Blynk IoT** and confirm that the shared account is signed in.
2. Tap the **profile/person icon**.
3. Turn **Developer Mode** on.
4. Return to the main screen.
5. Tap **Developer Mode** or the **wrench/tool icon**.
6. Find your exact template name.
7. Tap the exact template for your group: `Irrigation-G1`, `Irrigation-G2`, `Irrigation-G3`, or `Irrigation-G4`.
8. Confirm the template name at the top before adding a widget.

### Add the Soil Moisture gauge

1. Tap **+** at the top-right. If there is no plus button, tap an empty area of the canvas.
2. In the widget list, tap **Gauge**.
3. Tap the new gauge to open its settings.
4. Tap **Datastream**.
5. Tap **Soil Moisture (V0)**.
6. Set the widget title to `Soil Moisture` if a title field is shown.
7. Confirm that the displayed range is 0 to 100 and the unit is `%`.
8. Tap **Back**, **Done**, or the **X** to return to the canvas; the exact close control depends on the phone.

### Add the Water 5 Seconds control

1. Tap **+**.
2. Tap **Switch**. If the app provides a Button widget with a **Push** mode, that is also acceptable.
3. Tap the new control to open its settings.
4. Tap **Datastream**.
5. Tap **Water 5 Seconds (V1)**.
6. Set the title to `Water 5 Seconds`.
7. Confirm that off is 0 and on is 1.
8. If a **Mode** setting appears, choose **Push**. If it does not, keep Switch mode; the ESP32 resets V1 to 0 after accepting a request.
9. Return to the canvas.

Do not test this control yet. The ESP32 code and no-load safety test must be ready first.

### Add the Pump State value

1. Tap **+**.
2. Tap **Labeled Value**.
3. Tap the new widget.
4. Tap **Datastream**.
5. Tap **Pump State (V2)**.
6. Set the title to `Pump State`.
7. Return to the canvas.

### Add the Device Status value

1. Tap **+**.
2. Tap **Labeled Value**.
3. Tap the new widget.
4. Tap **Datastream**.
5. Tap **Device Status (V3)**.
6. Set the title to `Device Status`.
7. Return to the canvas.

### Arrange and verify the layout

1. Long-press a widget and drag it to move it.
2. Select a widget and drag its green handles to resize it if handles appear.
3. Place the gauge at the top, the watering control below it, and the two status values at the bottom.
4. Open each widget once more and read its selected datastream aloud.
5. Leave Developer Mode.
6. Open **Devices**, then tap the exact device for your group: `TU-Box-G1`, `TU-Box-G2`, `TU-Box-G3`, or `TU-Box-G4`.

Expected live layout:

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

## Optional: build the web dashboard too

Do this only after the mobile layout works.

1. In Blynk.Console, click **Developer Zone > My Templates**.
2. Open the exact template for your group: `Irrigation-G1`, `Irrigation-G2`, `Irrigation-G3`, or `Irrigation-G4`.
3. Click the **Web Dashboard** tab.
4. Click **Edit** at the top-right.
5. Drag a **Gauge** from the Widget Box to the dashboard.
6. Click its **gear/settings icon**, select `Soil Moisture (V0)`, and save the widget settings.
7. Add a **Switch** connected to `Water 5 Seconds (V1)`.
8. Add value/label widgets for `Pump State (V2)` and `Device Status (V3)`.
9. Click **Save**.
10. To see live values, click **Search > My Devices**, open the matching `TU-Box-G1`, `TU-Box-G2`, `TU-Box-G3`, or `TU-Box-G4` device, and click its **Dashboard** tab.

## Part 7: Upload the beginner sketch

Create a new Arduino sketch and copy all of the code below. Change `CLASS_GROUP` to `1`, `2`, `3`, or `4`. The sketch then selects the exact required template name. Copy `BLYNK_TEMPLATE_ID` and `BLYNK_AUTH_TOKEN` from the matching device, then replace the Wi-Fi placeholders.

```cpp
#define BLYNK_PRINT Serial
#define CLASS_GROUP 1  // Change to 1, 2, 3, or 4 for this station

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN    "YOUR_DEVICE_AUTH_TOKEN"

#if CLASS_GROUP == 1
  #define BLYNK_TEMPLATE_NAME "Irrigation-G1"
#elif CLASS_GROUP == 2
  #define BLYNK_TEMPLATE_NAME "Irrigation-G2"
#elif CLASS_GROUP == 3
  #define BLYNK_TEMPLATE_NAME "Irrigation-G3"
#elif CLASS_GROUP == 4
  #define BLYNK_TEMPLATE_NAME "Irrigation-G4"
#else
  #error "CLASS_GROUP must be 1, 2, 3, or 4"
#endif

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

Do not upload while `YOUR_TEMPLATE_ID`, `YOUR_DEVICE_AUTH_TOKEN`, `YOUR_WIFI_NAME`, or `YOUR_WIFI_PASSWORD` remains in the sketch. Before uploading, confirm that `CLASS_GROUP` matches the group number printed on the irrigation box.

## Part 8: Test without a pump

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

## Part 9: Supervised water test

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
