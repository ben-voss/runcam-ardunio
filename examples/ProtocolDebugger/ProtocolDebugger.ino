/*
 * Copyright 2022-2025 Ben Voß
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
#include <RunCam_Protocol.h>

RunCam::Protocol* protocol;

void setup() {
  Serial.begin(1000000);
  while (!Serial);

  Serial.println("RunCam Control");

  protocol = new RunCam::Protocol(&Serial1);

  uint8_t version;
  uint16_t features;
  if (protocol->readCameraInfo(&version, &features)) {
    Serial.print("Version: ");
    Serial.println(version);
    Serial.println("Features:");
    if (features & RCDEVICE_PROTOCOL_FEATURE_SIMULATE_POWER_BUTTON) {
      Serial.println("  Simulation Click the power button");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_SIMULATE_WIFI_BUTTON) {
      Serial.println("  Simulation Click the wifi button");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_CHANGE_MODE) {
      Serial.println("  Switch the device operating mode");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_SIMULATE_5_KEY_OSD_CABLE) {
      Serial.println("  Simulation 5-key OSD remote control");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS) {
      Serial.println("  Support access to device settings");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_DISPLAYP_PORT) {
      Serial.println("  The device is identified as a DisplayPort device by flying controller and receives the OSD data display from the flight controller");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_START_RECORDING) {
      Serial.println("  Control the camera to start recording video");
    }
    if (features & RCDEVICE_PROTOCOL_FEATURE_STOP_RECORDING) {
      Serial.println("  Control the camera to stop recording video");
    }    
  } else {
    Serial.println("Get Features failed.");
  }


  auto settings = std::vector<RunCam::Setting*>();

  int remainingChunks;
  int i;
  do {
    remainingChunks = protocol->getSetting(i++, &settings);
  } while (remainingChunks > 0);

  Serial.println("Settings:");
  Serial.print("  Count: ");
  Serial.println(settings.size());
  
  for(int i=0; i < settings.size(); i++){
    Serial.print("  ");
    Serial.print(settings[i]->getId());
    Serial.print(": ");
    Serial.print(settings[i]->getName());
    Serial.print(" = ");
    Serial.print(settings[i]->getValue());
    Serial.println("");
  }

  auto settingDetails = std::vector<RunCam::SettingDetail*>();

  for (int i = 0; i < 7; i++) {
    remainingChunks = 0;
    do {
      remainingChunks = protocol->readSettingDetail(i, 0, &settingDetails);
    } while (remainingChunks > 0);
  }

  Serial.println("Setting Details:");
  Serial.print("  Count: ");
  Serial.println(settingDetails.size());

  for(int i=0; i < settingDetails.size(); i++){
    Serial.print("  Setting Id: ");
    Serial.print(settingDetails[i]->getSettingId());
    Serial.print(", Type: ");
    Serial.print(settingDetails[i]->getSettingType());
    Serial.print(", ");

    switch (settingDetails[i]->getSettingType()) {
      case SETTING_TYPE_UINT8: {
        RunCam::UInt8SettingDetail* setting = (RunCam::UInt8SettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Min: ");
        Serial.print(setting->getMin());
        Serial.print(", Max: ");
        Serial.print(setting->getMax());
        Serial.print(", Step Size: ");
        Serial.print(setting->getStepSize());

        break;
      }

      case SETTING_TYPE_INT8: {
        RunCam::Int8SettingDetail* setting = (RunCam::Int8SettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Min: ");
        Serial.print(setting->getMin());
        Serial.print(", Max: ");
        Serial.print(setting->getMax());
        Serial.print(", Step Size: ");
        Serial.print(setting->getStepSize());

        break;
      }

      case SETTING_TYPE_UINT16: {
        RunCam::UInt16SettingDetail* setting = (RunCam::UInt16SettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Min: ");
        Serial.print(setting->getMin());
        Serial.print(", Max: ");
        Serial.print(setting->getMax());
        Serial.print(", Step Size: ");
        Serial.print(setting->getStepSize());

        break;
      }

      case SETTING_TYPE_INT16: {
        RunCam::Int16SettingDetail* setting = (RunCam::Int16SettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Min: ");
        Serial.print(setting->getMin());
        Serial.print(", Max: ");
        Serial.print(setting->getMax());
        Serial.print(", Step Size: ");
        Serial.print(setting->getStepSize());

        break;
      }

      case SETTING_TYPE_FLOAT: {
        RunCam::FloatSettingDetail* setting = (RunCam::FloatSettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Min: ");
        Serial.print(setting->getMin());
        Serial.print(", Max: ");
        Serial.print(setting->getMax());
        Serial.print(", Decimal Point: ");
        Serial.print(setting->getDecimalPoint());
        Serial.print(", Step Size: ");
        Serial.print(setting->getStepSize());

        break;
      }

      case SETTING_TYPE_TEXT_SELECTION: {
        RunCam::TextSelectionSettingDetail* setting = (RunCam::TextSelectionSettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Text Selection: (");
        
        std::vector<String*>* value = setting->getTextSelection();
        Serial.print(value->size());
        Serial.print(") ");

        for (int i = 0; i < value->size(); i++) {
          if (i > 0) {
            Serial.print(", ");
          }
          Serial.print(*value->at(i));
        }

        break;
      }

      case SETTING_TYPE_STRING: {
        RunCam::StringSettingDetail* setting = (RunCam::StringSettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());
        Serial.print(", Max String Size: ");
        Serial.print(setting->getMaxStringSize());

        break;
      }

      case SETTING_TYPE_FOLDER: {
        break;
      }

      case SETTING_TYPE_INFO: {
        RunCam::InfoSettingDetail* setting = (RunCam::InfoSettingDetail*)settingDetails[i];

        Serial.print("Value: ");
        Serial.print(setting->getValue());

        break;
      }

      default: {
        Serial.println(" - Unknown");
        break;
      }
    }

    Serial.println("");
  }
}

void loop() {
  String line = Serial.readStringUntil('\n');
  
  if (line == "?") {
    Serial.println("'w' - Simulate WiFi button");
    Serial.println("'p' - Simulate Power button");
    Serial.println("'m' - Change Mode");
    Serial.println("'+' - Start Recording");
    Serial.println("'-' - Stop Recording");
    Serial.println("'c' - 5 Key - Connect");
    Serial.println("' ' - 5 Key - Set");
    Serial.println("'l' - 5 Key - Left");
    Serial.println("'r' - 5 Key - Right");
    Serial.println("'u' - 5 Key - Up");
    Serial.println("'d' - 5 Key - Down");
    Serial.println("'tv=' - set tv mode (PAL or NTSC)");
  } else if (line == "w") {
    Serial.println("WiFi");
    protocol->cameraControl(RCDEVICE_PROTOCOL_SIMULATE_WIFI_BTN);
  } else if (line == "p") {
    Serial.println("Power");
    protocol->cameraControl(RCDEVICE_PROTOCOL_SIMULATE_POWER_BTN);
  } else if (line == "m") {
    Serial.println("Mode");
    protocol->cameraControl(RCDEVICE_PROTOCOL_CHANGE_MODE);
  } else if (line == "+") {
    Serial.println("Start Recording");
    protocol->cameraControl(RCDEVICE_PROTOCOL_CHANGE_START_RECORDING);
  } else if (line == "-") {
    Serial.println("Stop Recording");
    protocol->cameraControl(RCDEVICE_PROTOCOL_CHANGE_STOP_RECORDING);
  } else if (line == "c") {
    Serial.println("Connect");
    protocol->fiveKeySimulationConnection(RCDEVICE_PROTOCOL_5KEY_FUNCTION_OPEN);
  } else if (line == " ") {
    Serial.println("Simulate: Set");
    protocol->fiveKeySimulationPress(RCDEVICE_PROTOCOL_5KEY_SIMULATION_SET);
    protocol->fiveKeySimulationRelease();
  } else if (line == "l") {
    Serial.println("Simulate: Left");
    protocol->fiveKeySimulationPress(RCDEVICE_PROTOCOL_5KEY_SIMULATION_LEFT);
    protocol->fiveKeySimulationRelease();
  } else if (line == "r") {
    Serial.println("Simulate: Right");
    protocol->fiveKeySimulationPress(RCDEVICE_PROTOCOL_5KEY_SIMULATION_RIGHT);
    protocol->fiveKeySimulationRelease();
  } else if (line == "u") {
    Serial.println("Simulate: Up");
    protocol->fiveKeySimulationPress(RCDEVICE_PROTOCOL_5KEY_SIMULATION_UP);
    protocol->fiveKeySimulationRelease();
  } else if (line == "d") {
    Serial.println("Simulate: Down");
    protocol->fiveKeySimulationPress(RCDEVICE_PROTOCOL_5KEY_SIMULATION_DOWN);
    protocol->fiveKeySimulationRelease();
  } else if (line.startsWith("tv=")) {
    int i = line.substring(5).toInt();

    if (protocol->writeSetting(SETTINGID_DISP_TV_MODE, i)) {
      Serial.println("Success");
    } else {
      Serial.println("Failed");
    }
  }
}
