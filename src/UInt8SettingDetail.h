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

#ifndef __UINT8_SETTING_DETAIL_H__
#define __UINT8_SETTING_DETAIL_H__

#include <Arduino.h>
#include "SettingDetail.h"

namespace RunCam {

  class UInt8SettingDetail : public SettingDetail {
    public:
      UInt8SettingDetail(uint8_t settingId, uint8_t settingType, uint8_t value, uint8_t min, uint8_t max, uint8_t stepSize);

      uint8_t getValue();
      uint8_t getMin();
      uint8_t getMax();
      uint8_t getStepSize();

    private:
      uint8_t _value;
      uint8_t _min;
      uint8_t _max;
      uint8_t _stepSize;
  };

}

#endif // __UINT8_SETTING_DETAIL_H__