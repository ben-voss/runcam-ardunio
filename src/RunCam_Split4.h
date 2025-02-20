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

#ifndef __RUNCAM_SPLIT4_H__
#define __RUNCAM_SPLIT4_H__

#include "RunCam_Protocol.h"

namespace RunCam {

  class Split4 {
    private:
      RunCam::Protocol *_driver;
      uint8_t _version;
      uint16_t _features;
      std::vector<RunCam::Setting*> _settings = std::vector<RunCam::Setting*>();
      std::vector<RunCam::SettingDetail*> _settingDetails = std::vector<RunCam::SettingDetail*>();

    public:
      Split4(UART *uart);
      ~Split4();

      uint8_t getVersion();

      void refreshSettings();

      bool pressWiFiButton();

      bool pressPowerButton();

      bool toggleMode();

      String getCharset();

      String getResolution();
      bool setResolution(const String &resolution);

      int getDisplayColumns();

      String getDisplayMode();
      bool setDisplayMode(const String &displayMode);

      String getSdCapacity();

      bool hasSdCard();

      String getRemainingRecordingTime();

      String getCameraTime();
      bool setCameraTime(const String &time);
  };
}

#endif // __RUNCAM_SPLIT4_H__