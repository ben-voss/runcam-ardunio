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

#ifndef __SETTING_H__
#define __SETTING_H__

#include <Arduino.h>

namespace RunCam {
  class Setting {
    public:
      Setting(uint8_t id, const String &name, const String &value);

      uint8_t getId();
      String getName();
      String getValue();

    private:
      uint8_t _id;
      String _name;
      String _value;
  };
}

#endif // __SETTING_H__