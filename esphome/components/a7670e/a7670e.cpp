#include "a7670e.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace a76xx {
static const char *const TAG = "a7670e";

void A76XXComponent::setup() { ESP_LOGCONFIG(TAG, "Setting up A7670E GSM Module..."); }

void A76XXComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "A7670E GSM module connected with:");
  LOG_PIN("  RX pin: ", this->rx_pin_);
  LOG_PIN("  TX pin: ", this->tx_pin_);
  ESP_LOGCONFIG(TAG, "  Baud rate: %u", this->get_baud_rate());
  LOG_UPDATE_INTERVAL(this);
}

void A76XXComponent::update() {}
}  // namespace a76xx
}  // namespace esphome
