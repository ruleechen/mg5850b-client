#ifndef MG5850BClient_h
#define MG5850BClient_h

#include <functional>
#include <Arduino.h>

namespace Victor::Components {
  class MG5850BClient {
   public:
    MG5850BClient();
    ~MG5850BClient();
    void receive(uint8_t ch);
    typedef std::function<void(const uint8_t* payload, const size_t size)> TCommandSend;
    TCommandSend onCommand = nullptr;
    // APIs
    typedef std::function<void(const bool enabled)> TEnabledCallback;
    void getRadarEnable(const TEnabledCallback cb);
    void setRadarEnable(const bool enable, const TEnabledCallback cb = nullptr);
    void getLightEnable(const TEnabledCallback cb);
    void setLightEnable(const bool enable, const TEnabledCallback cb = nullptr);

   private:
    std::vector<uint8_t> _receiveBuffer = {};
    void _sendCommand(const uint8_t command, const uint8_t argumentHigh, const uint8_t argumentLow);
    void _clearCallbacks();
    TEnabledCallback _getRadarEnableCb = nullptr;
    TEnabledCallback _setRadarEnableCb = nullptr;
    TEnabledCallback _getLightEnableCb = nullptr;
    TEnabledCallback _setLightEnableCb = nullptr;
  };
} // namespace Victor::Components

#endif // MG5850BClient_h
