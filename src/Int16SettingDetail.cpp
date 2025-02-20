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
 
#include "Int16SettingDetail.h"

namespace RunCam {

  Int16SettingDetail::Int16SettingDetail(uint8_t settingId, uint8_t settingType, int16_t value, int16_t min, int16_t max, int16_t stepSize) : SettingDetail(settingId, settingType) {
    _value = value;
    _min = min;
    _max = max;
    _stepSize = stepSize;
  }

  int16_t Int16SettingDetail::getValue() {
    return _value;
  }

  int16_t Int16SettingDetail::getMin() {
    return _min;
  }

  int16_t Int16SettingDetail::getMax() {
    return _max;
  }

  int16_t Int16SettingDetail::getStepSize() {
    return _stepSize;
  }

}