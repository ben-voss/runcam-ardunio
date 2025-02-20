/*
 * Copyright 2024-2025 Ben Voß
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// https://note.youdao.com/coshare/index.html?token=9AD3F89F0B92488E8241F58CAEDF7939&gid=29699666&_time=1737308590149
// https://github.com/TeamSunride/RunCam-UART/blob/main/Runcam_recording_toggle_example.ino

#include <Arduino.h>
#include <RunCam_Split4.h>

RunCam::Split4* device;

void setup() {
  Serial.begin(1000000);
  while (!Serial);

  Serial.println("RunCam Split4 Control");

  device = new RunCam::Split4(&Serial1);

  Serial.print("Device Version: ");
  Serial.println(device->getVersion());
  logSettings();
  printHelp();
}

void loop() {
  String line = Serial.readStringUntil('\n');
  
  if (line == "?") {
    printHelp();
  } else if (line == "z") {
    Serial.println("OSD/Menu");
    device->pressWiFiButton();
  } else if (line == "x") {
    Serial.println("Power");
    device->pressPowerButton();
  } else if (line == "m") {
    Serial.println("Mode");
    device->toggleMode();
  } else if (line == "s") {
    device->refreshSettings();
    logSettings();
  } else if (line.startsWith("tv=")) {
    Serial.print("Setting TV Mode to ");
    Serial.println(line.substring(3));

    if (device->setDisplayMode(line.substring(3))) {
      Serial.println("Success");
    } else {
      Serial.println("Failed");
    }
  } else if (line.startsWith("time=")) {
    Serial.print("Setting time to ");
    Serial.println(line.substring(5));

    if (device->setCameraTime(line.substring(5))) {
      Serial.println("Success");
    } else {
      Serial.println("Failed");
    }
  }
}

void logSettings() {
  Serial.println("Settings:");

  Serial.print("  Charset: ");
  Serial.println(device->getCharset());

  Serial.print("  Display Columns: ");
  Serial.println(device->getDisplayColumns());

  Serial.print("  Display Mode: ");
  Serial.println(device->getDisplayMode());

  Serial.print("  Has SD Card: ");
  Serial.println(device->hasSdCard() ? "Yes" : "No");

  Serial.print("  SD Capacity: ");
  Serial.println(device->getSdCapacity());

  Serial.print("  Remaining Time: ");
  Serial.println(device->getRemainingRecordingTime());

  Serial.print("  Resolution: ");
  Serial.println(device->getResolution());

  Serial.print("  Camera Time: ");
  Serial.println(device->getCameraTime());  
}

void printHelp() {
  Serial.println("Help:");
  Serial.println("  'z' - Simulate OSD/Menu button press");
  Serial.println("  'x' - Simulate Power/Shutter button press");
  Serial.println("  'm' - Change Mode");
  Serial.println("  's' - Get Settings");
  Serial.println("  'tv=<mode>' - Set TV mode (PAL or NTSC)");
  Serial.println("  'time=<time>' - Set time (YYYYMMDDTHHmmSS.i)");
}
