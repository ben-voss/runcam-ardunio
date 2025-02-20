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
 
#ifndef __INT16_SETTING_DETAIL_H__
#define __INT16_SETTING_DETAIL_H__

#include <Arduino.h>
#include "SettingDetail.h"

namespace RunCam {

  class Int16SettingDetail : public SettingDetail {
    public:
      Int16SettingDetail(uint8_t settingId, uint8_t settingType, int16_t value, int16_t min, int16_t max, int16_t stepSize);

      int16_t getValue();
      int16_t getMin();
      int16_t getMax();
      int16_t getStepSize();

    private:
      int16_t _value;
      int16_t _min;
      int16_t _max;
      int16_t _stepSize;
  };

}

#endif // __INT16_SETTING_DETAIL_H__