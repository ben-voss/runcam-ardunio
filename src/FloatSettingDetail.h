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
 
#ifndef __FLOAT_SETTING_DETAIL_H__
#define __FLOAT_SETTING_DETAIL_H__

#include <Arduino.h>
#include "SettingDetail.h"

namespace RunCam {

  class FloatSettingDetail : public SettingDetail {
    public:
      FloatSettingDetail(uint8_t settingId, uint8_t settingType, uint16_t value, uint16_t min, uint16_t max, int16_t decimalPoint, int32_t stepSize);

      uint16_t getValue();
      uint16_t getMin();
      uint16_t getMax();
      int16_t getDecimalPoint();
      int32_t getStepSize();

    private:
      uint16_t _value;
      uint16_t _min;
      uint16_t _max;
      int16_t _decimalPoint;
      int32_t _stepSize;
  };

}

#endif // __FLOAT_SETTING_DETAIL_H__