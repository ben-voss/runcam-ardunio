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

#ifndef __INFO_SETTING_DETAIL_H__
#define __INFO_SETTING_DETAIL_H__

#include <Arduino.h>
#include "SettingDetail.h"

namespace RunCam {

  class InfoSettingDetail : public SettingDetail {
    public:
      InfoSettingDetail(uint8_t settingId, uint8_t settingType, String value);

      String getValue();

    private:
      String _value;
  };

}

#endif // __INFO_SETTING_DETAIL_H__