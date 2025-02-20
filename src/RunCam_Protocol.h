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

#ifndef __RUNCAMDRIVER_H__
#define __RUNCAMDRIVER_H__

#include <Arduino.h>
#include <vector>
#include "Setting.h"
#include "SettingDetail.h"
#include "UInt8SettingDetail.h"
#include "Int8SettingDetail.h"
#include "UInt16SettingDetail.h"
#include "Int16SettingDetail.h"
#include "FloatSettingDetail.h"
#include "TextSelectionSettingDetail.h"
#include "StringSettingDetail.h"
#include "InfoSettingDetail.h"

#define COMMAND_HEADER 0xcc

#define COMMAND_READ_CAMERA_INFO 0x00
#define COMMAND_CAMERA_CONTROL 0x01
#define COMMAND_FIVE_KEY_SIMULATION_PRESS 0x02
#define COMMAND_FIVE_KEY_SIMULATION_RELEASE 0x03
#define COMMAND_FIVE_KEY_SIMULATION_CONNECTION 0x04

// For FOLDER SETTINGS: When you call Get Detail Command(0x7) with it, it will return a empty response and the error code won't be zero.
#define COMMAND_GET_DETAIL 0x07 //

#define COMMAND_GET_SETTINGS 0x10 // Get sub settings with special setting ID
#define COMMAND_READ_SETTING_DETAIL 0x11 // Read a setting detail
#define COMMAND_WRITE_SETTING 0x13 // Write a setting

#define COMMAND_DISPLAY_FILL_REGION 0x20
#define COMMAND_DISPLAY_WRITE_CHAR 0x21
#define COMMAND_DISPLAY_WRITE_HORIZONTAL_STRING 0x22
#define COMMAND_DISPLAY_WRITE_VERTICAL_STRING 0x23
#define COMMAND_DISPLAY_WRITE_STRING 0x24

#define RCDEVICE_PROTOCOL_FEATURE_SIMULATE_POWER_BUTTON     (1 << 0)	// Simulation Click the power button
#define RCDEVICE_PROTOCOL_FEATURE_SIMULATE_WIFI_BUTTON	    (1 << 1)	// Simulation Click the Wi-Fi button
#define RCDEVICE_PROTOCOL_FEATURE_CHANGE_MODE	              (1 << 2)	// Switch the device operating mode
#define RCDEVICE_PROTOCOL_FEATURE_SIMULATE_5_KEY_OSD_CABLE  (1 << 3)	// Simulation 5-key OSD remote control
#define RCDEVICE_PROTOCOL_FEATURE_DEVICE_SETTINGS_ACCESS	  (1 << 4)	// Support access to device settings
#define RCDEVICE_PROTOCOL_FEATURE_DISPLAYP_PORT	            (1 << 5)	// The device is identified as a DisplayPort device by flying controller and receives the OSD data display from the flight controller
#define RCDEVICE_PROTOCOL_FEATURE_START_RECORDING	          (1 << 6)	// Control the camera to start recording video
#define RCDEVICE_PROTOCOL_FEATURE_STOP_RECORDING	          (1 << 7)	// Control the camera to stop recording video


#define RCDEVICE_PROTOCOL_SIMULATE_WIFI_BTN       0x00  // Simulation Click the Wi-Fi button
#define RCDEVICE_PROTOCOL_SIMULATE_POWER_BTN	    0x01	// Simulation Click the Power button
#define RCDEVICE_PROTOCOL_CHANGE_MODE	            0x02	// Switch the camera mode
#define RCDEVICE_PROTOCOL_CHANGE_START_RECORDING  0x03	// Control the camera to start recording
#define RCDEVICE_PROTOCOL_CHANGE_STOP_RECORDING	  0x04	// Control the camera to stop recording


#define RCDEVICE_PROTOCOL_5KEY_SIMULATION_SET  	0x01	// Simulate the confirmation key of the 5 key remote control
#define RCDEVICE_PROTOCOL_5KEY_SIMULATION_LEFT	0x02	// Simulate the left key of the 5 key remote control
#define RCDEVICE_PROTOCOL_5KEY_SIMULATION_RIGHT	0x03	// Simulate the right key of the 5 key remote control
#define RCDEVICE_PROTOCOL_5KEY_SIMULATION_UP	  0x04	// Simulate the up key of the 5 key remote control
#define RCDEVICE_PROTOCOL_5KEY_SIMULATION_DOWN	0x05	// Simulate the down key of the 5 key remote control


#define RCDEVICE_PROTOCOL_5KEY_FUNCTION_OPEN	0x01	// Initiate a handshake action to the camera
#define RCDEVICE_PROTOCOL_5KEY_FUNCTION_CLOSE	0x02	// Initiate a disconnection action to the camera


#define SETTINGID_DISP_CHARSET	              0 // TEXT_SELECTION	This setting is store current charset of the device	Read & Write
#define SETTINGID_DISP_COLUMNS	              1	// UINT8	Read the number of columns displayed on the screen line	Read only
#define SETTINGID_DISP_TV_MODE	              2	// TEXT_SELECTION	Read and set the camera's TV mode(NTSC,PAL)	Read & Write
#define SETTINGID_DISP_SDCARD_CAPACITY        3	// STRING	Read the camera's memory card capacity	Read only
#define SETTINGID_DISP_REMAIN_RECORDING_TIME	4	// STRING	Read the remaining recording time of the camera	Read only
#define SETTINGID_DISP_RESOLUTION	            5	// TEXT_SELECTION	Read and set the camera's resolution	Read & Write
#define SETTINGID_DISP_CAMERA_TIME	          6	// STRING	Read and set the camera's time	Read & Write


#define SETTING_TYPE_UINT8	         0
#define SETTING_TYPE_INT8	           1
#define SETTING_TYPE_UINT16	         2
#define SETTING_TYPE_INT16	         3
#define SETTING_TYPE_FLOAT	         8
#define SETTING_TYPE_TEXT_SELECTION  9
#define SETTING_TYPE_STRING	        10
#define SETTING_TYPE_FOLDER	        11
#define SETTING_TYPE_INFO	          12

#define BUFF_SIZE 65

namespace RunCam {

  struct CharAtPos {
    uint8_t x;
    uint8_t y;
    char c;
  };

  class Protocol {

    private:
      UART* _uart;
      uint8_t txBuf[BUFF_SIZE];
      uint8_t rxBuf[BUFF_SIZE];

      bool checkCrc(const uint8_t *buf, const uint8_t numBytes);
      bool checkCrcAndHeader(const uint8_t * buf, const uint8_t numBytes);
      void flushRx();
      int fillRxBuffer(size_t count);
      int fillRxBuffer(size_t offset, size_t count);
      void send(size_t length);

    public:
      Protocol(UART* uart);

      uint8_t calcCrc(const uint8_t *buf, const uint8_t numBytes);
      uint8_t crc8Calc(uint8_t crc, unsigned char a);

      typedef void (*GetSettingsCallbackFuncPtr)(uint8_t id, const String &name, const String &value);

      bool readCameraInfo(uint8_t *version, uint16_t *features);
      bool cameraControl(uint8_t actionId);
      bool fiveKeySimulationPress(uint8_t actionId);
      bool fiveKeySimulationRelease();
      bool fiveKeySimulationConnection(uint8_t actionId);

      String getSetting(uint8_t chunkIndex);
      int getSetting(uint8_t chunkIndex, std::vector<RunCam::Setting*> *settings);
      int readSettingDetail(uint8_t settingId, uint8_t chunkIndex, std::vector<SettingDetail*> *settingDetails);
      
      bool writeSetting(uint8_t settingId, uint8_t value);
      bool writeSetting(uint8_t settingId, const String &value);

      void displayFillRegion(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t character);
      void displayWriteChar(uint8_t x, uint8_t y, uint8_t character);
      bool displayWriteHorizontalString(uint8_t x, uint8_t y, const String &string);
      bool displayWriteVerticalString(uint8_t x, uint8_t y, const String &string);
      bool displayWriteString(uint8_t x, uint8_t y, uint length, const CharAtPos *charAtPos);
  };

}

#endif // __RUNCAMDRIVER_H__