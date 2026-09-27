# Beginner Blynk.Edgent Wi-Fi and Dashboard Tutorial

This tutorial connects the ESP32 irrigation controller to Blynk. It is written as a click-by-click class activity for learners who have not used Blynk before.

By the end, a learner can:

- open only the template and device assigned to their group;
- create four Virtual Pin datastreams;
- build a mobile dashboard one widget at a time;
- provision a 2.4 GHz Wi-Fi network to the ESP32 with Blynk.Edgent;
- create the group's Blynk edge device from the mobile app;
- read soil moisture on a phone;
- request one five-second watering cycle; and
- explain why the phone requests an action but the ESP32 enforces the safety timer.

Return to the [standalone ESP32 irrigation tutorial](README.md) if the soil sensor and GPIO27 relay have not already passed the local tests.

![Finished irrigation box prepared for a presentation slide](assets/images/irrigation-box-finished-transparent.png)

This beginner stage uses the soil sensor on GPIO35 and irrigation relay 3 on GPIO27. The other modules may remain mounted, but this sketch does not control them.

## The complete learning path

![Blynk setup process from account sign-in to safe relay testing](assets/images/blynk-setup-process.png)

Follow the six stages in order. Do not build the dashboard first and guess the datastreams later: every widget needs a datastream, every edge device needs a template, and the ESP32 firmware must contain the correct Template ID and Template Name.

## Shared training account and group names

Use the classroom Blynk account:

```text
Email: itetu.training@gmail.com
```

The instructor should sign in before the activity or enter the password privately. Learners must not write the password in this repository, a slide, a screenshot, or a group chat. Do not change the password, sign the account out of another station, or change account settings.

All groups can see the account's resources, so the names are the safety boundary. Use the row assigned by the instructor and copy its spelling exactly.

| Group | Colour | Blynk template name | Blynk device name |
| --- | --- | --- | --- |
| 1 | Red | `Irrigation G1` | `TU Box G1` |
| 2 | Blue | `Irrigation G2` | `TU Box G2` |
| 3 | Green | `Irrigation G3` | `TU Box G3` |
| 4 | Yellow | `Irrigation G4` | `TU Box G4` |

Each group has a **separate template** and a **separate edge device**. For example, Group 2 edits `Irrigation G2` and provisions `TU Box G2`. It must not provision the Group 2 box from `Irrigation G1`.

There is no template named `Student Irrigation Box` in this activity. The required template name is exactly `Irrigation G1` for Group 1, `Irrigation G2` for Group 2, `Irrigation G3` for Group 3, or `Irrigation G4` for Group 4.

**Do not type a dash or hyphen in a Blynk template name.** Blynk template names use letters, digits, and spaces. Copy the name exactly, including the single space before `G`.

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
4. Do not provision an ESP32 from another group's template.
5. Ask the instructor before signing out or changing account settings.
6. If the displayed name does not match the station record, stop and return to the resource list.

## Understand the four Blynk objects

| Object | Meaning in this lesson | Example for Group 1 |
| --- | --- | --- |
| Template | The reusable design: datastreams and dashboard layout | `Irrigation G1` |
| Edge device | The physical ESP32 plus its Blynk cloud record | `TU Box G1` |
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
- Provision only a trusted classroom 2.4 GHz Wi-Fi network. Do not use a personal hotspot unless the instructor approves it.
- Supervise the system. The five-second timer is a classroom safeguard, not a complete field safety system.
- Follow the voltage-domain rules in the [standalone hardware tutorial](README.md#important-safety-rules), especially if the shield voltage rail is set to 5 V.

## Before you begin

- ESP32 box with the soil sensor on GPIO35 and an active-low relay input on GPIO27
- USB data cable and Arduino IDE
- Blynk library for Arduino
- A 2.4 GHz Wi-Fi network the ESP32 may use; many ESP32 boards cannot join a 5 GHz-only network
- Wi-Fi network name and password available privately during provisioning
- Blynk mobile app on a phone or tablet
- Shared account already signed in, or the instructor present to sign it in
- Your completed group station record

Do not publish a Wi-Fi password, account password, or Blynk Auth Token. Blynk.Edgent sends the Wi-Fi credentials and a device token during provisioning, so they do not belong in the Arduino sketch. Use a temporary or isolated training network when possible.

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

Developer Mode is where a template's mobile layout is edited. The normal Devices view is where the live `TU Box G1`, `TU Box G2`, `TU Box G3`, or `TU Box G4` edge device is operated.

## Part 3: Open or create the assigned template

### Normal learner path: open the existing template

1. In the web console, click **Developer Zone > My Templates**.
2. Read your station record.
3. Find the exact assigned name: `Irrigation G1`, `Irrigation G2`, `Irrigation G3`, or `Irrigation G4`.
4. Click the template name once.
5. Confirm the name at the top of the page before editing.

Do not click **+ New Template** when your assigned template already exists. A name such as `Irrigation G1 copy` is not an acceptable replacement.

### If the instructor asks your group to create a missing template

1. Click **Developer Zone > My Templates**.
2. Click **+ New Template**.
3. In **Name**, type your assigned template name exactly. Use the space shown in `Irrigation G1`; do not type a dash.
4. In **Hardware**, choose **ESP32**.
5. In **Connection Type** or **Connectivity**, choose **WiFi**.
6. Click **Done** or **Create**.
7. Click **Save** if a Save button appears at the top-right.
8. Return to **My Templates** and verify that only one template has the assigned name.

If a template is missing and you were not instructed to create it, stop and tell the instructor.

## Part 4: Add the four datastreams

Repeat the following click path once for each row in the table.

1. Open `Irrigation G1`, `Irrigation G2`, `Irrigation G3`, or `Irrigation G4` according to your group number.
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

## Part 5: Prepare Blynk.Edgent

This lesson uses **Blynk.Edgent dynamic provisioning**. The ESP32 first broadcasts a temporary setup Wi-Fi network. The Blynk app connects to it, sends the classroom Wi-Fi details, requests a unique device token, and creates the edge device in the shared account.

Do not create the device manually in Blynk.Console and do not paste a Wi-Fi password or Auth Token into the sketch.

### Check for an older device record

1. In Blynk.Console, click **Search** or **Devices**.
2. Click **My Devices** if that submenu appears.
3. Look for the exact device name assigned to the group: `TU Box G1`, `TU Box G2`, `TU Box G3`, or `TU Box G4`.
4. If that device already exists, stop and tell the instructor. The instructor must decide whether to reconfigure that record or use a freshly reset ESP32. Do not delete it.

### Copy the template firmware values

1. Click **Developer Zone > My Templates**.
2. Open the exact group template: `Irrigation G1`, `Irrigation G2`, `Irrigation G3`, or `Irrigation G4`.
3. Open the **Home** or **Info** tab.
4. Find **Firmware Configuration**.
5. Copy the displayed `BLYNK_TEMPLATE_ID` into a temporary note.
6. Confirm that the displayed `BLYNK_TEMPLATE_NAME` is the exact group name below.

| Group | Required template-name line |
| --- | --- |
| 1 | `#define BLYNK_TEMPLATE_NAME "Irrigation G1"` |
| 2 | `#define BLYNK_TEMPLATE_NAME "Irrigation G2"` |
| 3 | `#define BLYNK_TEMPLATE_NAME "Irrigation G3"` |
| 4 | `#define BLYNK_TEMPLATE_NAME "Irrigation G4"` |

The Template ID is generated by Blynk, so the tutorial cannot predict it. The Template Name is fixed by the group table and contains spaces, not dashes. Blynk.Edgent assigns the Auth Token automatically during provisioning. See Blynk's official [Wi-Fi provisioning guide](https://docs.blynk.io/en/getting-started/activating-devices/blynk-edgent-wifi-provisioning).

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
7. Tap the exact template for your group: `Irrigation G1`, `Irrigation G2`, `Irrigation G3`, or `Irrigation G4`.
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
6. Do not look for the live device yet. Blynk.Edgent creates it during Wi-Fi provisioning in Part 8.

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
2. Open the exact template for your group: `Irrigation G1`, `Irrigation G2`, `Irrigation G3`, or `Irrigation G4`.
3. Click the **Web Dashboard** tab.
4. Click **Edit** at the top-right.
5. Drag a **Gauge** from the Widget Box to the dashboard.
6. Click its **gear/settings icon**, select `Soil Moisture (V0)`, and save the widget settings.
7. Add a **Switch** connected to `Water 5 Seconds (V1)`.
8. Add value/label widgets for `Pump State (V2)` and `Device Status (V3)`.
9. Click **Save**.
10. After Part 8 creates the edge device, click **Search > My Devices**, open the matching `TU Box G1`, `TU Box G2`, `TU Box G3`, or `TU Box G4` device, and click its **Dashboard** tab.

## Part 7: Build and upload the Blynk.Edgent sketch

Blynk.Edgent uses several supporting tabs. Do not start with an empty one-file sketch.

1. Open Arduino IDE.
2. Click **File > Examples > Blynk > Blynk.Edgent > Edgent_ESP32**.
3. Click **File > Save As** and save a working copy named `Irrigation_Edgent_G1`, `Irrigation_Edgent_G2`, `Irrigation_Edgent_G3`, or `Irrigation_Edgent_G4`.
4. Confirm that the Arduino editor shows the main `.ino` tab plus supporting tabs such as `BlynkEdgent.h` and `Settings.h`.
5. Open the main `.ino` tab. Replace its contents with the code below. Do not delete or rename the supporting tabs.
6. Change `CLASS_GROUP` to the station's group number.
7. Replace `YOUR_TEMPLATE_ID` with the Template ID copied from that group's template.
8. Leave `BLYNK_TEMPLATE_NAME` exactly as selected by the group block. Do not add a dash.

```cpp
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_ESP32_DEV_MODULE
#define BLYNK_FIRMWARE_VERSION "0.1.0"

#define CLASS_GROUP 1  // Change to 1, 2, 3, or 4 for this station

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"

#if CLASS_GROUP == 1
  #define BLYNK_TEMPLATE_NAME "Irrigation G1"
#elif CLASS_GROUP == 2
  #define BLYNK_TEMPLATE_NAME "Irrigation G2"
#elif CLASS_GROUP == 3
  #define BLYNK_TEMPLATE_NAME "Irrigation G3"
#elif CLASS_GROUP == 4
  #define BLYNK_TEMPLATE_NAME "Irrigation G4"
#else
  #error "CLASS_GROUP must be 1, 2, 3, or 4"
#endif

#include "BlynkEdgent.h"

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
  if (Blynk.connected()) {
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, "READY");
  }
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

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(soilPin, INPUT);
  pinMode(irrigationRelay, OUTPUT);
  digitalWrite(irrigationRelay, HIGH);  // Keep pump off during startup

  BlynkEdgent.begin();
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  BlynkEdgent.run();
  timer.run();

  // This local timer still stops the relay if the phone loses connection.
  if (watering && millis() - wateringStartedAt >= wateringTimeMs) {
    stopWatering();
  }
}
```

The sketch uses `BlynkTimer` to send sensor data once every two seconds. Do not put an unrestricted `Blynk.virtualWrite()` in `loop()`: Blynk warns that sending on every loop can flood the cloud connection. See [Send Data From Hardware to Blynk](https://docs.blynk.io/en/getting-started/how-to-display-any-sensor-data-in-blynk-app).

`BlynkEdgent.begin()` starts dynamic provisioning and `BlynkEdgent.run()` maintains provisioning, Wi-Fi, cloud connectivity, and the application connection. Do not add `BLYNK_AUTH_TOKEN`, `wifiName`, `wifiPassword`, or `Blynk.begin(...)` to this Edgent sketch.

Before uploading:

1. Confirm that `YOUR_TEMPLATE_ID` has been replaced.
2. Confirm that `CLASS_GROUP` matches the group number printed on the irrigation box.
3. Confirm that **Tools > Board** is the correct ESP32 board and **Tools > Port** is the box's USB port.
4. Keep the pump or valve disconnected from the relay contacts.
5. Click **Upload**.
6. Open **Serial Monitor** at 115200 baud after the upload finishes.

The example defines the ESP32 BOOT button on GPIO0 as the Edgent reset button. It does not use GPIO27 or GPIO35. Do not change `Settings.h` during this beginner activity.

## Part 8: Provision Wi-Fi and create the edge device

Provisioning creates the device and securely gives the ESP32 its network credentials and unique Auth Token. Keep Serial Monitor open so the instructor can see each state change.

### Prepare the phone and ESP32

1. Connect the phone to the classroom's **2.4 GHz Wi-Fi** network.
2. Confirm that the phone can reach the internet on that network.
3. Turn on the ESP32 and wait for its Edgent setup mode. Serial Monitor should indicate that it is waiting for configuration.
4. Open **Blynk IoT** and confirm that `itetu.training@gmail.com` is signed in.
5. Allow **Nearby devices**, **Local network**, or **Location** permission if the operating system requests it. Blynk needs the relevant permission to discover the ESP32 setup network.

### Add and connect the edge device

1. Open the app's **menu icon** at the top-right.
2. Tap **+ Add New Device** or **Add new device**.
3. Tap **Find Devices Nearby**.
4. Tap **Start** or **Ready**.
5. Wait for the device list. Select the setup device whose name begins with `Blynk` and contains your exact template name, such as `Irrigation G2`.
6. If iOS opens system Wi-Fi settings, select that Blynk setup network, return to the Blynk app, and tap **Already connected**.
7. On **Connect your device to WiFi**, tap **Choose Wi-Fi network**.
8. Select the approved classroom 2.4 GHz network. Do not choose a 5 GHz-only or browser-sign-in network.
9. Enter the Wi-Fi password privately.
10. Leave **Remember this network** off on a shared device unless the instructor specifically wants Blynk to retain it for the next group.
11. Tap **Continue**.
12. Keep the phone close to the ESP32 and wait. Do not close the app, unplug the ESP32, or switch networks while it shows connecting or configuring.
13. When Blynk reports that the device is connected, tap **Continue**.

### Name and verify the device

1. In **Device Name**, enter the exact space-separated name from the group table:

   - Group 1: `TU Box G1`
   - Group 2: `TU Box G2`
   - Group 3: `TU Box G3`
   - Group 4: `TU Box G4`

2. Do not type a dash or hyphen.
3. Review the device profile and confirm that its template and group number match.
4. Tap **Apply**.
5. Tap **Continue**.
6. Tap **Exit to app**.
7. Open **Devices** and tap the newly created `TU Box G1`, `TU Box G2`, `TU Box G3`, or `TU Box G4` tile.
8. Confirm that the device shows **Online** and that the four-widget mobile dashboard appears.

Under the hood, the ESP32 temporarily operates as a Wi-Fi access point. The phone sends the selected network credentials to it, Blynk supplies a unique Auth Token, the ESP32 stores the values in flash memory, and then it restarts and connects to Blynk.Cloud. The Wi-Fi password and token are not stored in this repository.

### If provisioning fails

- Read Serial Monitor before trying again. It normally reveals whether the ESP32 cannot join Wi-Fi or cannot reach Blynk.Cloud.
- Confirm that the Template ID and Template Name in the sketch exactly match the selected group template.
- Confirm that the Wi-Fi network is 2.4 GHz and does not require a browser login page.
- If an existing device record must keep its data, open that device in the app, open its action menu, and choose **Reconfigure**. Do not create a duplicate.
- With the instructor present, holding the ESP32 **BOOT** button for about 10 seconds while the Edgent firmware is running clears locally stored provisioning credentials and returns it to setup mode.
- Never delete another group's device while troubleshooting.

For the underlying sequence and current app screens, see Blynk's [Edgent Wi-Fi provisioning guide](https://docs.blynk.io/en/getting-started/activating-devices/blynk-edgent-wifi-provisioning) and [Add New Device guide](https://docs.blynk.io/en/blynk.apps/device-management/add-new-device).

## Part 9: Test without a pump

1. Confirm again that the pump or valve is disconnected from the relay contacts.
2. Open Serial Monitor at 115200 baud.
3. Wait for Blynk to report that the device is ready or online.
4. Check that the mobile dashboard shows a changing moisture value.
5. Tap **Water 5 Seconds** once.
6. Confirm that the GPIO27 relay indicator turns on, `Pump State` changes to 1, and `Device Status` shows `WATERING`.
7. Confirm that the relay turns off after about five seconds and the dashboard returns to `Pump State = 0` and `READY`.
8. Turn off the phone's Wi-Fi during a new test. Confirm that the relay still turns off after five seconds.
9. Repeat the test three times before connecting a real load.

Stop immediately if the relay turns on during ESP32 reset, remains on longer than five seconds, or behaves opposite to the comments in the code. The relay board may not match the expected active-low design.

## Part 10: Supervised water test

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
| Edgent setup begins | App discovers the correct group template | |
| Wi-Fi provisioning finishes | Correct `TU Box G1`–`TU Box G4` device is online | |
| Soil probe in dry sample | Moisture moves toward 0% | |
| Soil probe in moist sample | Moisture moves toward 100% | |
| Phone control pressed once | Relay turns on | |
| Five seconds pass | Relay turns off | |
| Phone loses connection during watering | Relay still turns off | |

Answer these questions:

1. What is the difference between GPIO27 and Virtual Pin V1?
2. Why does the program use a timer instead of leaving the phone switch in control of the relay?
3. What additional sensor should prevent a pump from running with an empty tank?
4. Why are the Wi-Fi password and Auth Token not written in the Edgent sketch?

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Sketch does not compile | Update the Blynk library, open the `Edgent_ESP32` example, keep all supporting tabs, and confirm the ESP32 board package is selected |
| ESP32 does not appear in **Find Devices Nearby** | Check Serial Monitor for waiting/configuration mode, allow nearby/local-network permissions, and confirm Template ID and Template Name are exact |
| App cannot send Wi-Fi settings | Keep the phone near the ESP32; on iOS, connect to the Blynk setup network in Settings and return to the app |
| ESP32 cannot join the network | Choose a 2.4 GHz network, re-enter its password, and avoid captive-portal or browser-sign-in networks |
| Device stays offline after provisioning | Read Serial Monitor, verify internet access to Blynk.Cloud, and use **Reconfigure** rather than creating a duplicate |
| Edgent reports an authentication/configuration error | Remove any manually defined `BLYNK_AUTH_TOKEN`; confirm the group Template ID and space-separated Template Name, then reset and provision again |
| Duplicate device name appears | Stop and tell the instructor; do not delete or rename another group's device |
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
