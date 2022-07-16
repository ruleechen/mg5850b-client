#include "MG5850BClient.h"

namespace Victor::Components {

  #define MG5850B_COMMAND_HEAD_BYTE1         0x55
  #define MG5850B_COMMAND_HEAD_BYTE2         0x5a
  #define MG5850B_COMMAND_TAIL               0xfe
  #define MG5850B_COMMAND_ARGUMENT_EMPTY     0x00
  #define MG5850B_COMMAND_ARGUMENT_TRUE      0x01
  #define MG5850B_COMMAND_ARGUMENT_FALSE     0x00
  #define MG5850B_COMMAND_RADAR_ENABLE_READ  0x89
  #define MG5850B_COMMAND_RADAR_ENABLE_WRITE 0x09
  #define MG5850B_COMMAND_RADAR_LEVEL_READ   0x81
  #define MG5850B_COMMAND_RADAR_LEVEL_WRITE  0x01
  #define MG5850B_COMMAND_DELAY_LEVEL_READ   0x82
  #define MG5850B_COMMAND_DELAY_LEVEL_WRITE  0x02
  #define MG5850B_COMMAND_LIGHT_ENABLE_READ  0x83
  #define MG5850B_COMMAND_LIGHT_ENABLE_WRITE 0x03
  #define MG5850B_COMMAND_LIGHT_HIGH_READ    0x84
  #define MG5850B_COMMAND_LIGHT_HIGH_WRITE   0x04
  #define MG5850B_COMMAND_LIGHT_LOW_READ     0x85
  #define MG5850B_COMMAND_LIGHT_LOW_WRITE    0x05

  MG5850BClient::MG5850BClient() {}

  MG5850BClient::~MG5850BClient() {
    onCommand = nullptr;
    _clearCallbacks();
  }

  void MG5850BClient::receive(uint8_t ch) {
    _receiveBuffer.push_back(ch);
    if (ch == MG5850B_COMMAND_TAIL) {
      const auto command = _receiveBuffer[2];
      // const auto argumentHigh = _receiveBuffer[3];
      const auto argumentLow  = _receiveBuffer[4];
      _receiveBuffer.clear();
      // switch
      if (command == MG5850B_COMMAND_RADAR_ENABLE_READ) {
        if (_getRadarEnableCb != nullptr) {
          const auto enabled = argumentLow == MG5850B_COMMAND_ARGUMENT_TRUE;
          _getRadarEnableCb(enabled);
          _getRadarEnableCb = nullptr;
        }
      } else if (command == MG5850B_COMMAND_RADAR_ENABLE_WRITE) {
        if (_setRadarEnableCb != nullptr) {
          const auto enabled = argumentLow == MG5850B_COMMAND_ARGUMENT_TRUE;
          _setRadarEnableCb(enabled);
          _setRadarEnableCb = nullptr;
        }
      } else if (command == MG5850B_COMMAND_RADAR_LEVEL_READ) {
      } else if (command == MG5850B_COMMAND_RADAR_LEVEL_WRITE) {
      } else if (command == MG5850B_COMMAND_DELAY_LEVEL_READ) {
      } else if (command == MG5850B_COMMAND_DELAY_LEVEL_WRITE) {
      } else if (command == MG5850B_COMMAND_LIGHT_ENABLE_READ) {
        if (_setLightEnableCb != nullptr) {
          const auto enabled = argumentLow == MG5850B_COMMAND_ARGUMENT_TRUE;
          _setLightEnableCb(enabled);
          _setLightEnableCb = nullptr;
        }
      } else if (command == MG5850B_COMMAND_LIGHT_ENABLE_WRITE) {
        if (_getLightEnableCb != nullptr) {
          const auto enabled = argumentLow == MG5850B_COMMAND_ARGUMENT_TRUE;
          _getLightEnableCb(enabled);
          _getLightEnableCb = nullptr;
        }
      } else if (command == MG5850B_COMMAND_LIGHT_HIGH_READ) {
      } else if (command == MG5850B_COMMAND_LIGHT_HIGH_WRITE) {
      } else if (command == MG5850B_COMMAND_LIGHT_LOW_READ) {
      } else if (command == MG5850B_COMMAND_LIGHT_LOW_WRITE) {
      }
    }
  }

  void MG5850BClient::getRadarEnable(const TEnabledCallback cb) {
    _clearCallbacks();
    _getRadarEnableCb = cb;
    _sendCommand(
      MG5850B_COMMAND_RADAR_ENABLE_READ,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  void MG5850BClient::setRadarEnable(const bool enable, const TEnabledCallback cb) {
    _clearCallbacks();
    _setRadarEnableCb = cb;
    _sendCommand(
      MG5850B_COMMAND_RADAR_ENABLE_WRITE,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      enable ? MG5850B_COMMAND_ARGUMENT_TRUE : MG5850B_COMMAND_ARGUMENT_FALSE
    );
  }

  void MG5850BClient::getLightEnable(const TEnabledCallback cb) {
    _clearCallbacks();
    _getRadarEnableCb = cb;
    _sendCommand(
      MG5850B_COMMAND_LIGHT_ENABLE_READ,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  void MG5850BClient::setLightEnable(const bool enable, const TEnabledCallback cb) {
    _clearCallbacks();
    _setRadarEnableCb = cb;
    _sendCommand(
      MG5850B_COMMAND_LIGHT_ENABLE_WRITE,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      enable ? MG5850B_COMMAND_ARGUMENT_TRUE : MG5850B_COMMAND_ARGUMENT_FALSE
    );
  }

  void MG5850BClient::_sendCommand(const uint8_t command, const uint8_t argumentHigh, const uint8_t argumentLow) {
    if (onCommand == nullptr) {
      return;
    }
    const uint8_t payload[] = {
      MG5850B_COMMAND_HEAD_BYTE1,
      MG5850B_COMMAND_HEAD_BYTE2,
      command,
      argumentHigh,
      argumentLow,
      command ^ argumentHigh ^ argumentLow,
      MG5850B_COMMAND_TAIL,
    };
    onCommand(payload, 7);
  }

  void MG5850BClient::_clearCallbacks() {
    _getRadarEnableCb = nullptr;
    _setRadarEnableCb = nullptr;
    _getLightEnableCb = nullptr;
    _setLightEnableCb = nullptr;
  }

} // namespace Victor::Components
