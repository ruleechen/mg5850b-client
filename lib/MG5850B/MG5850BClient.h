#ifndef MG5850BClient_h
#define MG5850BClient_h

#include <functional>
#include <Arduino.h>

namespace Victor::Components {
  class MG5850BClient {
   public:
    MG5850BClient();
    ~MG5850BClient();
    void receive(const uint8_t ch);
    typedef std::function<void(const uint8_t* payload, const size_t size)> TCommandSend;
    TCommandSend onCommand = nullptr;
    // APIs
    typedef std::function<void(const bool enabled)> TEnabledCallback;
    typedef std::function<void(const uint16_t value)> TValueCallback;
    void getRadarEnable(const TEnabledCallback cb);
    void setRadarEnable(const bool enable, const TEnabledCallback cb = nullptr);
    void getRadarDistance(const TValueCallback cb);
    void setRadarDistance(const uint16_t value, const TValueCallback cb = nullptr);
    void getDelayTime(const TValueCallback cb);
    void setDelayTime(const uint16_t value, const TValueCallback cb = nullptr);
    void getLightEnable(const TEnabledCallback cb);
    void setLightEnable(const bool enable, const TEnabledCallback cb = nullptr);
    void getLightHigh(const TValueCallback cb);
    void setLightHigh(const uint16_t value, const TValueCallback cb = nullptr);
    void getLightLow(const TValueCallback cb);
    void setLightLow(const uint16_t value, const TValueCallback cb = nullptr);

   private:
    std::vector<uint8_t> _receiveBuffer = {};
    void _getEnable(const uint8_t command, const TEnabledCallback cb);
    void _setEnable(const uint8_t command, const bool enable, const TEnabledCallback cb);
    void _getValue(const uint8_t command, const TValueCallback cb);
    void _setValue(const uint8_t command, const uint16_t value, const TValueCallback cb);
    static uint8_t _calculateCheckCode(const uint8_t command, const uint8_t argHigh, const uint8_t argLow);
    void _emitCommand(const uint8_t command, const uint8_t argHigh, const uint8_t argLow);
    void _clearCallbacks();
    TEnabledCallback _enabledCallback = nullptr;
    TValueCallback _valueCallback = nullptr;
  };
} // namespace Victor::Components

#endif // MG5850BClient_h
