#pragma once
#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace a76xx {
class A76XXComponent : public PollingComponent, public uart::UARTComponent, public text_sensor::TextSensor {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;

 protected:
  int command_timeout;

  const char *read_string_rn() { return this->read_string_delimited("\r\n"); }

  const char *read_string_r() { return this->read_string_delimited("\r"); }

  const char *read_string_delimited(const char *delimiter) {
    std::vector<uint8_t> byteArray;

    uint8_t read;

    int delimiterObservedCount = 0;

    while (this->read_byte(&read) && delimiterObservedCount < 2) {
      byteArray.push_back(read);

      if (compareLastBytes(byteArray, delimiter)) {
        delimiterObservedCount++;
      }
    }

    if (byteArray.back() == '\0') {  // If response is null-terminated string we can just cast it
      return reinterpret_cast<const char *>(byteArray.data());
    } else {  // Otherwise we add the terminator ourselves
      char *cstr = new char[byteArray.size() + 1];
      std::memcpy(cstr, byteArray.data(), byteArray.size());
      cstr[byteArray.size()] = '\0';
      return cstr;
    }
  }

  bool compareLastBytes(const std::vector<uint8_t> &byteArray, const char *str) {
    size_t strLength = std::strlen(str);

    if (byteArray.size() < strLength) {
      return false;
    }

    const uint8_t *vectorEnd = &byteArray[byteArray.size() - strLength];

    return std::memcmp(vectorEnd, str, strLength) == 0;
  }
};
}  // namespace a76xx
}  // namespace esphome
