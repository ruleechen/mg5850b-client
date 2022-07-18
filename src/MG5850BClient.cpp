#include "MG5850BClient.h"

namespace Victor::Components {

  #define MG5850B_COMMAND_BYTES                 7
  #define MG5850B_COMMAND_HEAD1                 0x55
  #define MG5850B_COMMAND_HEAD2                 0x5a
  #define MG5850B_COMMAND_TAIL                  0xfe
  #define MG5850B_COMMAND_ARGUMENT_EMPTY        0x00
  #define MG5850B_COMMAND_ARGUMENT_TRUE         0x01
  #define MG5850B_COMMAND_ARGUMENT_FALSE        0x00
  #define MG5850B_COMMAND_RADAR_ENABLE_READ     0x89
  #define MG5850B_COMMAND_RADAR_ENABLE_WRITE    0x09
  #define MG5850B_COMMAND_RADAR_DISTANCE_READ   0x81
  #define MG5850B_COMMAND_RADAR_DISTANCE_WRITE  0x01
  #define MG5850B_COMMAND_DELAY_TIME_READ       0x82
  #define MG5850B_COMMAND_DELAY_TIME_WRITE      0x02
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

  void MG5850BClient::receive(const uint8_t ch) {
    _receiveBuffer.push_back(ch);
    if (ch == MG5850B_COMMAND_TAIL) {
      if (_receiveBuffer.size() == MG5850B_COMMAND_BYTES) { // validate length
        const auto command = _receiveBuffer[2];
        const auto argHigh = _receiveBuffer[3];
        const auto argLow  = _receiveBuffer[4];
        const auto checkCode = _receiveBuffer[5];
        const auto calculated = _calculateCheckCode(command, argHigh, argLow);
        if (checkCode == calculated) { // validate `check code`
          switch (command) {
            case MG5850B_COMMAND_RADAR_ENABLE_READ:
            case MG5850B_COMMAND_RADAR_ENABLE_WRITE:
            case MG5850B_COMMAND_LIGHT_ENABLE_READ:
            case MG5850B_COMMAND_LIGHT_ENABLE_WRITE: {
              if (_enabledCallback != nullptr) {
                const bool enabled = argLow == MG5850B_COMMAND_ARGUMENT_TRUE;
                _enabledCallback(enabled);
              }
              break;
            }
            case MG5850B_COMMAND_RADAR_DISTANCE_READ:
            case MG5850B_COMMAND_RADAR_DISTANCE_WRITE:
            case MG5850B_COMMAND_DELAY_TIME_READ:
            case MG5850B_COMMAND_DELAY_TIME_WRITE:
            case MG5850B_COMMAND_LIGHT_HIGH_READ:
            case MG5850B_COMMAND_LIGHT_HIGH_WRITE:
            case MG5850B_COMMAND_LIGHT_LOW_READ:
            case MG5850B_COMMAND_LIGHT_LOW_WRITE: {
              if (_valueCallback != nullptr) {
                const uint16_t value = ((uint16_t)argHigh << 8) | argLow;
                _valueCallback(value);
              }
              break;
            }
            default: {
              break;
            }
          }
        }
      }
      // reset buffer
      _receiveBuffer.clear();
    }
  }

  void MG5850BClient::getRadarEnable(const TEnabledCallback cb) {
    _getEnable(MG5850B_COMMAND_RADAR_ENABLE_READ, cb);
  }

  void MG5850BClient::setRadarEnable(const bool enable, const TEnabledCallback cb) {
    _setEnable(MG5850B_COMMAND_RADAR_ENABLE_WRITE, enable, cb);
  }

  void MG5850BClient::getRadarDistance(const TValueCallback cb) {
    _getValue(MG5850B_COMMAND_RADAR_DISTANCE_READ, cb);
  }

  void MG5850BClient::setRadarDistance(const uint16_t value, const TValueCallback cb) {
    const auto valueFix = std::max<uint16_t>(0, std::min<uint16_t>(15, value)); // 0~15
    _setValue(MG5850B_COMMAND_RADAR_DISTANCE_WRITE, valueFix, cb);
  }

  void MG5850BClient::getDelayTime(const TValueCallback cb) {
    _getValue(MG5850B_COMMAND_DELAY_TIME_READ, cb);
  }

  void MG5850BClient::setDelayTime(const uint16_t value, const TValueCallback cb) {
    _setValue(MG5850B_COMMAND_DELAY_TIME_WRITE, value, cb);
  }

  void MG5850BClient::getLightEnable(const TEnabledCallback cb) {
    _getEnable(MG5850B_COMMAND_LIGHT_ENABLE_READ, cb);
  }

  void MG5850BClient::setLightEnable(const bool enable, const TEnabledCallback cb) {
    _setEnable(MG5850B_COMMAND_LIGHT_ENABLE_WRITE, enable, cb);
  }

  void MG5850BClient::getLightHigh(const TValueCallback cb) {
    _getValue(MG5850B_COMMAND_LIGHT_HIGH_READ, cb);
  }

  void MG5850BClient::setLightHigh(const uint16_t value, const TValueCallback cb) {
    const auto valueFix = std::max<uint16_t>(0, std::min<uint16_t>(1023, value)); // 0~1023
    _setValue(MG5850B_COMMAND_LIGHT_HIGH_WRITE, valueFix, cb);
  }

  void MG5850BClient::getLightLow(const TValueCallback cb) {
    _getValue(MG5850B_COMMAND_LIGHT_LOW_READ, cb);
  }

  void MG5850BClient::setLightLow(const uint16_t value, const TValueCallback cb) {
    const auto valueFix = std::max<uint16_t>(0, std::min<uint16_t>(1023, value)); // 0~1023
    _setValue(MG5850B_COMMAND_LIGHT_LOW_WRITE, valueFix, cb);
  }

  void MG5850BClient::_getEnable(const uint8_t command, const TEnabledCallback cb) {
    _clearCallbacks();
    _enabledCallback = cb;
    _emitCommand(
      command,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  void MG5850BClient::_setEnable(const uint8_t command, const bool enable, const TEnabledCallback cb) {
    _clearCallbacks();
    _enabledCallback = cb;
    _emitCommand(
      command,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      enable ? MG5850B_COMMAND_ARGUMENT_TRUE : MG5850B_COMMAND_ARGUMENT_FALSE
    );
  }

  void MG5850BClient::_getValue(const uint8_t command, const TValueCallback cb) {
    _clearCallbacks();
    _valueCallback = cb;
    _emitCommand(
      command,
      MG5850B_COMMAND_ARGUMENT_EMPTY,
      MG5850B_COMMAND_ARGUMENT_EMPTY
    );
  }

  void MG5850BClient::_setValue(const uint8_t command, const uint16_t value, const TValueCallback cb) {
    _clearCallbacks();
    _valueCallback = cb;
    const uint8_t argHigh = value >> 8;
    const uint8_t argLow  = value;
    _emitCommand(
      command,
      argHigh,
      argLow
    );
  }

  uint8_t MG5850BClient::_calculateCheckCode(const uint8_t command, const uint8_t argHigh, const uint8_t argLow) {
    const auto checkCode = command ^ argHigh ^ argLow;
    return checkCode;
  }

  void MG5850BClient::_emitCommand(const uint8_t command, const uint8_t argHigh, const uint8_t argLow) {
    if (onCommand == nullptr) {
      return;
    }
    const auto checkCode = _calculateCheckCode(command, argHigh, argLow);
    const uint8_t payload[] = {
      MG5850B_COMMAND_HEAD1,
      MG5850B_COMMAND_HEAD2,
      command,
      argHigh,
      argLow,
      checkCode,
      MG5850B_COMMAND_TAIL,
    };
    onCommand(payload, MG5850B_COMMAND_BYTES);
  }

  void MG5850BClient::_clearCallbacks() {
    _enabledCallback = nullptr;
    _valueCallback = nullptr;
  }

} // namespace Victor::Components
