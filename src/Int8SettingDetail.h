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
 
#ifndef __INT8_SETTING_DETAIL_H__
#define __INT8_SETTING_DETAIL_H__

#include <Arduino.h>
#include "SettingDetail.h"

namespace RunCam {

  class Int8SettingDetail : public SettingDetail {
    public:
      Int8SettingDetail(uint8_t settingId, uint8_t settingType, int8_t value, int8_t min, int8_t max, int8_t stepSize);

      int8_t getValue();
      int8_t getMin();
      int8_t getMax();
      int8_t getStepSize();

    private:
      int8_t _value;
      int8_t _min;
      int8_t _max;
      int8_t _stepSize;
  };

}

#endif // __INT8_SETTING_DETAIL_H__