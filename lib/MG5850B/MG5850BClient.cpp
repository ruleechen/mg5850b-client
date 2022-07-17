#include "MG5850BClient.h"

namespace Victor::Components {

  #define MG5850B_COMMAND_HEAD_BYTE1            0x55
  #define MG5850B_COMMAND_HEAD_BYTE2            0x5a
  #define MG5850B_COMMAND_TAIL                  0xfe
  #define MG5850B_COMMAND_ARGUMENT_EMPTY        0x00
  #define MG5850B_COMMAND_ARGUMENT_TRUE         0x01
  #define MG5850B_COMMAND_ARGUMENT_FALSE        0x00
  #define MG5850B_COMMAND_RADAR_ENABLE_READ     0x89
  #define MG5850B_COMMAND_RADAR_ENABLE_WRITE    0x09
  #define MG5850B_COMMAND_RADAR_DISTANCE_READ   0x81
  #define MG5850B_COMMAND_RADAR_DISTANCE_WRITE  0x01
  #define MG5850B_COMMAND_DELAY_LEVEL_READ      0x82
  #define MG5850B_COMMAND_DELAY_LEVEL_WRITE     0x02
  #define MG5850B_COMMAND_LIGHT_ENABLE_READ     0x83
  #define MG5850B_COMMAND_LIGHT_ENABLE_WRITE    0x03
  #define MG5850B_COMMAND_LIGHT_HIGH_READ       0x84
  #define MG5850B_COMMAND_LIGHT_HIGH_WRITE      0x04
  #define MG5850B_COMMAND_LIGHT_LOW_READ        0x85
  #define MG5850B_COMMAND_LIGHT_LOW_WRITE       0x05

  MG5850BClient::MG5850BClient() {}

  MG5850BClient::~MG5850BClient() {
    onCommand = nullptr;
    _clearCallbacks();
  }

  void MG5850BClient::receive(uint8_t ch) {
    _receiveBuffer.push_back(ch);
    if (ch == MG5850B_COMMAND_TAIL && _receiveBuffer.size() == 7) {
      const auto checkCode = _receiveBuffer[5];
      const auto calculated = _calculateCheckCode(_receiveBuffer[2], _receiveBuffer[3], _receiveBuffer[4]);
      if (checkCode == calculated) {
        const auto command = _receiveBuffer[2];
        // const auto argumentHigh = _receiveBuffer[3];
        const auto argumentLow  = _receiveBuffer[4];
        // switch
        if (
          command == MG5850B_COMMAND_RADAR_ENABLE_READ ||
          command == MG5850B_COMMAND_RADAR_ENABLE_WRITE
        ) {
          if (_enabledCallback != nullptr) {
            const auto enabled = argumentLow == MG5850B_COMMAND_ARGUMENT_TRUE;
            _enabledCallback(enabled);
          }
        } else if (
          command == MG5850B_COMMAND_RADAR_DISTANCE_READ ||
          command == MG5850B_COMMAND_RADAR_DISTANCE_WRITE
        ) {
          if (_levelCallback != nullptr) {
            const auto level = argumentLow;
            _levelCallback(level);
          }
        } else if (command == MG5850B_COMMAND_DELAY_LEVEL_READ) {
        } else if (command == MG5850B_COMMAND_DELAY_LEVEL_WRITE) {
        } else if (
          command == MG5850B_COMMAND_LIGHT_ENABLE_READ ||
          command == MG5850B_COMMAND_LIGHT_ENABLE_WRITE
        ) {
          if (_enabledCallback != nullptr) {
            const auto enabled = argumentLow == MG5850B_COMMAND_ARGUMENT_TRUE;
            _enabledCallback(enabled);
          }
        } else if (command == MG5850B_COMMAND_LIGHT_HIGH_READ) {
        } else if (command == MG5850B_COMMAND_LIGHT_HIGH_WRITE) {
        } else if (command == MG5850B_COMMAND_LIGHT_LOW_READ) {
        } else if (command == MG5850B_COMMAND_LIGHT_LOW_WRITE) {
        }
      }
      // reset buffer
      _receiveBuffer.clear();
    }
  }

  void MG5850BClient::getRadarEnable(const TEnabledCallback cb) {
    _clearCallbacks();
    _enabledCallback = cb;
    _sendCommand(
      MG5850B_COMMAND_RADAR_ENABLE_READ,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  void MG5850BClient::setRadarEnable(const bool enable, const TEnabledCallback cb) {
    _clearCallbacks();
    _enabledCallback = cb;
    _sendCommand(
      MG5850B_COMMAND_RADAR_ENABLE_WRITE,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      enable ? MG5850B_COMMAND_ARGUMENT_TRUE : MG5850B_COMMAND_ARGUMENT_FALSE
    );
  }

  void MG5850BClient::getRadarDistance(const TLevelCallback cb) {
    _clearCallbacks();
    _levelCallback = cb;
    _sendCommand(
      MG5850B_COMMAND_RADAR_DISTANCE_READ,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  bool MG5850BClient::setRadarDistance(const uint8_t level, const TLevelCallback cb) {
    if (level < 0 || level > 15) {
      return false;
    }
    _clearCallbacks();
    _levelCallback = cb;
    _sendCommand(
      MG5850B_COMMAND_RADAR_DISTANCE_WRITE,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      level
    );
    return true;
  }

  void MG5850BClient::getLightEnable(const TEnabledCallback cb) {
    _clearCallbacks();
    _enabledCallback = cb;
    _sendCommand(
      MG5850B_COMMAND_LIGHT_ENABLE_READ,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  void MG5850BClient::setLightEnable(const bool enable, const TEnabledCallback cb) {
    _clearCallbacks();
    _enabledCallback = cb;
    _sendCommand(
      MG5850B_COMMAND_LIGHT_ENABLE_WRITE,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      enable ? MG5850B_COMMAND_ARGUMENT_TRUE : MG5850B_COMMAND_ARGUMENT_FALSE
    );
  }

  uint8_t MG5850BClient::_calculateCheckCode(const uint8_t command, const uint8_t argumentHigh, const uint8_t argumentLow) {
    const auto checkCode = command ^ argumentHigh ^ argumentLow;
    return checkCode;
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
      _calculateCheckCode(command, argumentHigh, argumentLow),
      MG5850B_COMMAND_TAIL,
    };
    onCommand(payload, 7);
  }

  void MG5850BClient::_clearCallbacks() {
    _enabledCallback = nullptr;
    _levelCallback = nullptr;
  }

} // namespace Victor::Components
