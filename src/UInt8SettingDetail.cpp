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

#include "UInt8SettingDetail.h"

namespace RunCam {

  UInt8SettingDetail::UInt8SettingDetail(uint8_t settingId, uint8_t settingType, uint8_t value, uint8_t min, uint8_t max, uint8_t stepSize) : SettingDetail(settingId, settingType) {
    _value = value;
    _min = min;
    _max = max;
    _stepSize = stepSize;
  }

  uint8_t UInt8SettingDetail::getValue() {
    return _value;
  }

  uint8_t UInt8SettingDetail::getMin() {
    return _min;
  }

  uint8_t UInt8SettingDetail::getMax() {
    return _max;
  }

  uint8_t UInt8SettingDetail::getStepSize() {
    return _stepSize;
  }

}
