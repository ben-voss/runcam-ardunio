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

// https://note.youdao.com/coshare/index.html?token=9AD3F89F0B92488E8241F58CAEDF7939&gid=29699666&_time=1737308590149
// https://github.com/TeamSunride/RunCam-UART/blob/main/Runcam_recording_toggle_example.ino

#include <Arduino.h>
#include "RunCam_Protocol.h"

namespace RunCam {

  Protocol::Protocol(UART *uart) {
    _uart = uart;

    // Baud Rate Data Bits Stop Bits Patiry
    // 115200 8 1 none
    uart->begin(115200, SERIAL_8N1);

    // Give the runcam time to send whatever it does out of reset
    delay(3000);
  }

  uint8_t Protocol::calcCrc(const uint8_t *buf, const uint8_t numBytes) {
    uint8_t crc = 0;

    for (uint8_t i = 0; i < numBytes; i++) {
      crc = crc8Calc(crc, *(buf ++));
    }

    return crc;   
  }

  uint8_t Protocol::crc8Calc(uint8_t crc, unsigned char a) {
    crc ^= a;

    for (int i = 0; i < 8; ++i) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0xd5;
      } else {
        crc = crc << 1;
      }
    }
    
    return crc;
  }

  bool Protocol::checkCrc(const uint8_t *buf, const uint8_t numBytes) {
    uint8_t crc = calcCrc(buf, numBytes);

    return crc == 0; // buf[numBytes] == crc;
  }

  bool Protocol::checkCrcAndHeader(const uint8_t * buf, const uint8_t numBytes) {
    if (!checkCrc(buf, numBytes)) {
      Serial.println("Bad CRC");
      return false;
    }

    if (rxBuf[0] != 0xcc) {
      Serial.println("Bad header");
      return false;
    }

    return true;
  }

  // Flush the serial rx buffer before sending a command to clear any junk making response decoding more reliable
  void Protocol::flushRx() {
    while (_uart->available()) {
      _uart->read();
    }
  }

  void Protocol::send(size_t length) {
    txBuf[length - 1] = calcCrc(txBuf, length - 1);

    flushRx();
    _uart->write(txBuf, length);
  }

  int Protocol::fillRxBuffer(size_t count) {
    return fillRxBuffer(0, count);
  }

  int Protocol::fillRxBuffer(size_t offset, size_t count) {
    long now = millis();

    int numRead = 0;
    do {
      numRead += _uart->readBytes(rxBuf + offset + numRead, count);
    } while (count - numRead > 0 && millis() - now < 2000);

    return numRead;
  }

  // Read the basic information of the camera, such as firmware version, device type, protocol version
  bool Protocol::readCameraInfo(uint8_t *version, uint16_t *features) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_READ_CAMERA_INFO;

    send(3);
    fillRxBuffer(5);

    if (!checkCrcAndHeader(rxBuf, 5)) {
      return false;
    }

    *version = rxBuf[1];
    *features = rxBuf[2] | rxBuf[3] << 8;
    return true;
  }

  // Camera control，For example: through this instruction, send an instruction to simulate the actions of power button to the camera
  bool Protocol::cameraControl(uint8_t actionId) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_CAMERA_CONTROL;
    txBuf[2] = actionId;

    send(4);

    // Device does not produce a response
    return true;
  }

  bool Protocol::fiveKeySimulationPress(uint8_t actionId) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_FIVE_KEY_SIMULATION_PRESS;
    txBuf[2] = actionId;  

    send(4);

    fillRxBuffer(2);

    return checkCrcAndHeader(rxBuf, 2);
  }

  bool Protocol::fiveKeySimulationRelease() {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_FIVE_KEY_SIMULATION_RELEASE;
    
    send(3);

    fillRxBuffer(2);

    return checkCrcAndHeader(rxBuf, 2);
  }

  // Send handshake events and disconnected events to the camera
  bool Protocol::fiveKeySimulationConnection(uint8_t actionId) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_FIVE_KEY_SIMULATION_CONNECTION;
    txBuf[2] = actionId;  

    send(4);
    fillRxBuffer(3);

    if (!checkCrcAndHeader(rxBuf, 3)) {
      return false;
    }

    // [ (Action ID << 4) + Response result(1：Succes 0：Failure) ]
    return rxBuf[1] & 0x1 != 0;
  }

  size_t safe_strlen(const uint8_t *str, size_t max_len) {
    const uint8_t * end = (const uint8_t *)memchr(str, '\0', max_len);
    if (end == NULL) {
      return max_len;
    } else {
      return end - str;
    }
  }

  size_t safe_strlen(const uint8_t *start, const uint8_t* maxPtr) {
    const uint8_t *end = start;
    while (*end != 0 && end < maxPtr) {
      end++;
    }

    return end - start;
  }

  // get a setting
  // Enumerates the possible settings and their current values
  int Protocol::getSetting(uint8_t chunkIndex, std::vector<RunCam::Setting*> *settings) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_GET_SETTINGS;
    txBuf[2] = 0;   // setting Id - Retrieve the sub settings through the parent setting ID.  Changing the value doesnt seem to make a difference
    txBuf[3] = chunkIndex;

    send(5);

    fillRxBuffer(3);

    uint8_t remainingChunks = rxBuf[1];   // 3   <- Remaining chunks
    uint8_t dataLength = rxBuf[2];        // 24  <- data length

    // Read the payload + crc
    fillRxBuffer(3, dataLength + 1);

    // Check the CRC
    if (!checkCrcAndHeader(rxBuf, dataLength + 4)) {
      // Serial.println("Bad CRC");
      return -1;
    }

    const uint8_t* endPtr = rxBuf + dataLength + 4;
    uint8_t* chunkPtr = rxBuf + 3;

    do {
      uint8_t settingId = *(chunkPtr++);

      size_t nameLength = safe_strlen(chunkPtr, endPtr);
      String name = String(chunkPtr, nameLength);
      chunkPtr += nameLength + 1;

      size_t valueLength = safe_strlen(chunkPtr, endPtr);
      String value = String(chunkPtr, valueLength);
      chunkPtr += valueLength + 1;

      settings->push_back(new RunCam::Setting(settingId, name, value));

    } while (chunkPtr + 1 < rxBuf + dataLength + 4);

    return remainingChunks;
  }

  // Retrieve the detail of setting, e.g it's maybe including max value, min value and etc. This command can not be called for the setting type with Folder and Static
  int Protocol::readSettingDetail(uint8_t settingId, uint8_t chunkIndex, std::vector<RunCam::SettingDetail*> *settingDetails) {    
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_READ_SETTING_DETAIL;
    txBuf[2] = settingId;
    txBuf[3] = chunkIndex;

    send(5);

    // Read the header
    fillRxBuffer(3);

    uint8_t remainingChunkCount = rxBuf[1];
    uint8_t dataLength = rxBuf[2];

    // Read the payload + crc
    fillRxBuffer(3, dataLength + 1);

    // Check the CRC
    if (!checkCrcAndHeader(rxBuf, dataLength + 4)) {
      Serial.println("Bad CRC");
      return -1;
    }

    /*for (int i = 0; i < dataLength + 4; i++) {
      Serial.print(rxBuf[i]);
      Serial.print(' ');
    }
    Serial.println();*/

    int p = 3; 
    while (p <= dataLength + 2) {
      uint8_t settingType = rxBuf[p++];             // The type of setting，refer to 'setting type' section to know more

      switch (settingType) {
        case SETTING_TYPE_UINT8: {
          uint8_t value = rxBuf[p++];
          uint8_t min = rxBuf[p++];
          uint8_t max = rxBuf[p++];
          uint8_t stepSize = rxBuf[p++];

          settingDetails->push_back(new UInt8SettingDetail(settingId, settingType, value, min, max, stepSize));

          break;
        }

        case SETTING_TYPE_INT8: {
          int8_t value = rxBuf[p++];
          int8_t min = rxBuf[p++];
          int8_t max = rxBuf[p++];
          int8_t stepSize = rxBuf[p++];

          settingDetails->push_back(new Int8SettingDetail(settingId, settingType, value, min, max, stepSize));

          break;
        }

        case SETTING_TYPE_UINT16: {
          uint16_t value = rxBuf[p++] | (rxBuf[p++] << 8);
          uint16_t min = rxBuf[p++] | (rxBuf[p++] << 8);
          uint16_t max = rxBuf[p++] | (rxBuf[p++] << 8);
          uint16_t stepSize = rxBuf[p++] | (rxBuf[p++] << 8);

          settingDetails->push_back(new UInt16SettingDetail(settingId, settingType, value, min, max, stepSize));

          break;
        }

        case SETTING_TYPE_INT16: {
          int16_t value = rxBuf[p++] | (rxBuf[p++] << 8);
          int16_t min = rxBuf[p++] | (rxBuf[p++] << 8);
          int16_t max = rxBuf[p++] | (rxBuf[p++] << 8);
          int16_t stepSize = rxBuf[p++] | (rxBuf[p++] << 8);

          settingDetails->push_back(new Int16SettingDetail(settingId, settingType, value, min, max, stepSize));

          break;
        }

        case SETTING_TYPE_FLOAT: {
          float value = rxBuf[p++] | (rxBuf[p++] << 8) | (rxBuf[p++] << 16) | (rxBuf[p++] << 24);
          float min = rxBuf[p++] | (rxBuf[p++] << 8) | (rxBuf[p++] << 16) | (rxBuf[p++] << 24);
          float max = rxBuf[p++] | (rxBuf[p++] << 8) | (rxBuf[p++] << 16) | (rxBuf[p++] << 24);
          int16_t decimalPoint = rxBuf[p++] | (rxBuf[p++] << 8);            // Digit count after the decimal point
          int32_t stepSize = rxBuf[p++] | (rxBuf[p++] << 8) | (rxBuf[p++] << 16) | (rxBuf[p++] << 24);

          settingDetails->push_back(new FloatSettingDetail(settingId, settingType, value, min, max, decimalPoint, stepSize));

          break;
        }

        case SETTING_TYPE_TEXT_SELECTION: {
          uint8_t value = rxBuf[p++];

          std::vector<String*> *textSelection = new std::vector<String*>();

          size_t len = 0;
          while (p + len < dataLength + 3) {
            uint8_t c = rxBuf[p + len];

            if (c == ';' || c == 0) {
              textSelection->push_back(new String(&rxBuf[p], len));

              p += len + 1;
              len = 0;

              if (c == 0) {
                break;
              }
            } else {
              len++;
            }
          }

          settingDetails->push_back(new TextSelectionSettingDetail(settingId, settingType, value, textSelection));

          break;
        }

        case SETTING_TYPE_STRING: {
          size_t len = safe_strlen(&rxBuf[p], dataLength - p + 2);
          String value = String(&rxBuf[p], len);
          p += len + 1;

          uint8_t maxStringSize = rxBuf[p++];

          settingDetails->push_back(new StringSettingDetail(settingId, settingType, value, maxStringSize));

          break;
        }

        case SETTING_TYPE_FOLDER: {
          break;
        }

        case SETTING_TYPE_INFO: {
          size_t len = safe_strlen(&rxBuf[p], dataLength - p + 2);
          String value = String(&rxBuf[p], len);
          p += len + 1;
          
          settingDetails->push_back(new InfoSettingDetail(settingId, settingType, value));

          break;
        }

        default: {
          // Serial.print(settingType);
          // Serial.println(" - Unknown");
          break;
        }
      }
    }

    return remainingChunkCount;
  }

  // change the value of special setting，can't call this command with the setting type of FOLDER and INFO
  bool Protocol::writeSetting(uint8_t settingId, uint8_t value) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_WRITE_SETTING;
    txBuf[2] = settingId;
    txBuf[3] = value;

    send(5);

    // Read the response
    fillRxBuffer(4);

    for (int i = 0; i < 4; i++) {
      Serial.print(rxBuf[i]);
      Serial.print(' ');
    }
    Serial.println();

    if (!checkCrcAndHeader(rxBuf, 4)) {
      return false;
    }

    uint8_t resultCode = rxBuf[1];    // if value is 0, it means write operation succeed 
    uint8_t refresh = rxBuf[2];

    return resultCode == 0;
  }

  bool Protocol::writeSetting(uint8_t settingId, const String &value) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_WRITE_SETTING;
    txBuf[2] = settingId;

    int length = value.length();

    txBuf[3] = length;// + 1;

    for (int i = 0; i < length; i++) {
      txBuf[i + 4] = value.charAt(i);
    }

    //txBuf[length + 4] = 0;

    send(length + 4);

    // Read the response
    int numRead = fillRxBuffer(4);

    Serial.println(numRead);

    for (int i = 0; i < 4; i++) {
      Serial.print(rxBuf[i]);
      Serial.print(' ');
    }
    Serial.println();

    if (!checkCrcAndHeader(rxBuf, 4)) {
      return false;
    }

    uint8_t resultCode = rxBuf[1];    // if value is 0, it means write operation succeed 
    uint8_t refresh = rxBuf[2];

    return resultCode == 0;
  }

  // Fill an area with a specified char
  void Protocol::displayFillRegion(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t character) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_DISPLAY_FILL_REGION;
    txBuf[2] = x;
    txBuf[3] = y;
    txBuf[4] = width;
    txBuf[5] = height;
    txBuf[6] = character;

    send(8);
  }

  // Write a character at the specified position
  void Protocol::displayWriteChar(uint8_t x, uint8_t y, uint8_t character) {
    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_DISPLAY_WRITE_CHAR;
    txBuf[2] = x;
    txBuf[3] = y;
    txBuf[4] = character;

    send(6);
  }

  // Write a string horizonally at the specified position
  bool Protocol::displayWriteHorizontalString(uint8_t x, uint8_t y, const String &string) {
    int length = string.length();

    // Max buffer must not exceed 65 bytes.  Max string length is therefore 60
    if (length > 60) {
      return false;
    }

    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_DISPLAY_WRITE_HORIZONTAL_STRING;
    txBuf[2] = length;
    txBuf[3] = x;
    txBuf[4] = y;

    for (uint8_t i = 0; i < length; i++) {
      txBuf[i + 5] = string[i];    
    }

    send(length + 6);

    return true;
  }

  // Write a string verically at the specified position
  bool Protocol::displayWriteVerticalString(uint8_t x, uint8_t y, const String &string) {
    int length = string.length();

    // Max buffer must not exceed 65 bytes.  Max string length is therefore 59
    if (length > 59) {
      return false;
    }

    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_DISPLAY_WRITE_VERTICAL_STRING;
    txBuf[2] = length;
    txBuf[3] = x;
    txBuf[4] = y;

    for (uint8_t i = 0; i < length; i++) {
      txBuf[i + 5] = string[i];    
    }

    send(length + 6);

    return true;
  }

  // Write chars at the specified positions
  bool Protocol::displayWriteString(uint8_t x, uint8_t y, uint length, const CharAtPos *charAtPos) {  
    // Max buffer must not exceed 65 bytes.  Max length is therefore 20
    if (length > 20) {
      return false;
    }

    txBuf[0] = COMMAND_HEADER;
    txBuf[1] = COMMAND_DISPLAY_WRITE_STRING;
    txBuf[2] = length * 3;

    for (uint8_t i = 0; i < length; i++) {
      txBuf[i * 3 + 3] = charAtPos[i].x;
      txBuf[i * 3 + 4] = charAtPos[i].y;
      txBuf[i * 3 + 5] = charAtPos[i].c;
    }

    send(length * 3 + 4);

    return true;
  }
}