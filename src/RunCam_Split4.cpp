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

#include "RunCam_Split4.h"
#include "TextSelectionSettingDetail.h"

namespace RunCam {

  Split4::Split4(UART *uart) {
    _driver = new Protocol(uart);
  
    if (!_driver->readCameraInfo(&_version, &_features)) {
      _version = 0;
      _features = 0;
    }

    refreshSettings();
  }
  
  Split4::~Split4() {
    delete _driver;
  }

  uint8_t Split4::getVersion() {
    return _version;
  }

  void Split4::refreshSettings() {
    _settings.clear();
    _settingDetails.clear();

    if (_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS) {
      int remainingChunks;
      int i;
      do {
        remainingChunks = _driver->getSetting(i++, &_settings);
      } while (remainingChunks > 0);
      
      for (int i = 0; i < 7; i++) {
        remainingChunks = 0;
        do {
          remainingChunks = _driver->readSettingDetail(i, remainingChunks, &_settingDetails);
        } while (remainingChunks > 0);
      }
    }
  }

  bool Split4::pressWiFiButton() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_SIMULATE_WIFI_BUTTON)) {
      return false;
    }

    return _driver->cameraControl(RCDEVICE_PROTOCOL_SIMULATE_WIFI_BTN);
  }

  bool Split4::pressPowerButton() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_SIMULATE_POWER_BUTTON)) {
      return false;
    }

    return _driver->cameraControl(RCDEVICE_PROTOCOL_SIMULATE_POWER_BTN);
  }

  bool Split4::toggleMode() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_SIMULATE_POWER_BUTTON)) {
      return false;
    }

    return _driver->cameraControl(RCDEVICE_PROTOCOL_CHANGE_MODE);
  }

  String Split4::getCharset() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS)) {
      return "";
    }

    for (int i = 0; i < _settings.size(); i++){
      int value = _settings[SETTINGID_DISP_CHARSET]->getValue().toInt();

      TextSelectionSettingDetail* detail = (TextSelectionSettingDetail*)_settingDetails[SETTINGID_DISP_CHARSET];
      std::vector<arduino::String*>* selection = detail->getTextSelection();

      return *selection->at(value);
    }

    return "";
  }

  // v2.0.1 & v2.0.4 of the firmware reports these values.
  // 3840x2160P30 -> (0) 1080P60FPS
  // 2704x1520P60 -> (1) 1080P50FPS
  // 2704x1520P50 -> (2) 1080P30FPS
  // 1920x1080P60 -> (3) 720P60FPS
  // 1920x1080P50 -> (4) ?
  String Split4::getResolution() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS)) {
      return "";
    }

    for (int i = 0; i < _settings.size(); i++){
      TextSelectionSettingDetail* detail = (TextSelectionSettingDetail*)_settingDetails[SETTINGID_DISP_RESOLUTION];
      std::vector<arduino::String*>* selection = detail->getTextSelection();

      int value = detail->getValue();


      if (value >= selection->size()) {
        return "?";
      }

      return *selection->at(value);
    }

    return "";
  }

  bool Split4::setResolution(const String &resolution) {
    return _driver->writeSetting(SETTINGID_DISP_RESOLUTION, resolution);
  }

  int Split4::getDisplayColumns() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS)) {
      return -1;
    }

    for (size_t i = 0; i < _settings.size(); i++){
      return _settings[SETTINGID_DISP_COLUMNS]->getValue().toInt();
    }

    return -1;
  }

  String Split4::getDisplayMode() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS)) {
      return "";
    }

    for (int i = 0; i < _settings.size(); i++){
      int value = _settings[SETTINGID_DISP_TV_MODE]->getValue().toInt();

      TextSelectionSettingDetail* detail = (TextSelectionSettingDetail*)_settingDetails[SETTINGID_DISP_TV_MODE];
      std::vector<arduino::String*>* selection = detail->getTextSelection();

      value = detail->getValue();

      return *selection->at(value);
    }

    return "";
  }

  bool Split4::setDisplayMode(const String &displayMode) {    
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS)) {
      return false;
    }

    TextSelectionSettingDetail* detail = (TextSelectionSettingDetail*)_settingDetails.at(SETTINGID_DISP_TV_MODE);
    std::vector<arduino::String*>* selection = detail->getTextSelection();

    for (int i = 0; i < selection->size(); i++) {
      if (*(selection->at(i)) == displayMode) {
        return _driver->writeSetting(SETTINGID_DISP_TV_MODE, i);
      }
    }

    return false;
  }

  // Reports "0/3" if there is no SD card
  String Split4::getSdCapacity() {
    if (!(_features & RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS)) {
      return "";
    }

    for (int i = 0; i < _settings.size(); i++){
      return _settings[SETTINGID_DISP_SDCARD_CAPACITY]->getValue();
    }

    return "";
  }

  // Reports "0/3" if there is no SD card
  String Split4::getRemainingRecordingTime() {
    if (!(_features & SETTINGID_DISP_REMAIN_RECORDING_TIME)) {
      return "";
    }

    for (int i = 0; i < _settings.size(); i++){
      return _settings[SETTINGID_DISP_SDCARD_CAPACITY]->getValue();
    }

    return "";
  }

  bool Split4::hasSdCard() {
    return getSdCapacity() != "0/3";
  }

  String Split4::getCameraTime() {
    if (!(_features & SETTINGID_DISP_REMAIN_RECORDING_TIME)) {
      return "";
    }

    for (int i = 0; i < _settings.size(); i++){
      return _settings[SETTINGID_DISP_CAMERA_TIME]->getValue();
    }

    return "";
  }

  bool Split4::setCameraTime(const String &resolution) {
    return _driver->writeSetting(SETTINGID_DISP_CAMERA_TIME, resolution);
  }
}
