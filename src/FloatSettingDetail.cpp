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
 
#include "FloatSettingDetail.h"

namespace RunCam {

  FloatSettingDetail::FloatSettingDetail(uint8_t settingId, uint8_t settingType, uint16_t value, uint16_t min, uint16_t max, int16_t decimalPoint, int32_t stepSize) : SettingDetail(settingId, settingType) {
    _value = value;
    _min = min;
    _max = max;
    _decimalPoint;
    _stepSize = stepSize;
  }

  uint16_t FloatSettingDetail::getValue() {
    return _value;
  }

  uint16_t FloatSettingDetail::getMin() {
    return _min;
  }

  uint16_t FloatSettingDetail::getMax() {
    return _max;
  }

  int16_t FloatSettingDetail::getDecimalPoint() {
    return _decimalPoint;
  }

  int32_t FloatSettingDetail::getStepSize() {
    return _stepSize;
  }

}
