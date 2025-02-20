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

#ifndef __SETTING_TYPE_H__
#define __SETTING_TYPE_H__

#include <Arduino.h>

namespace RunCam {

  class SettingDetail {
    public:
      SettingDetail(uint8_t settingId, uint8_t settingType);

      uint8_t getSettingId();
      uint8_t getSettingType();

    private:
      uint8_t _settingId;
      uint8_t _settingType;
  };

}

#endif // __SETTING_TYPE_H__