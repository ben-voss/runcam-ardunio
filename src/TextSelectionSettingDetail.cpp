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
 
#include "TextSelectionSettingDetail.h"
#include <Arduino.h>
#include <vector>

namespace RunCam {

  TextSelectionSettingDetail::TextSelectionSettingDetail(uint8_t settingId, uint8_t settingType, uint16_t value, std::vector<String*>* textSelection) : SettingDetail(settingId, settingType) {
    _value = value;
    _textSelection = textSelection;
  }

  uint16_t TextSelectionSettingDetail::getValue() {
    return _value;
  }

  std::vector<String*>* TextSelectionSettingDetail::getTextSelection() {
    return _textSelection;
  }

}