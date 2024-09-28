#include "a7670e.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace a7670e {
static const char *const TAG = "a7670e";

void A7670EComponent::setup() { ESP_LOGCONFIG(TAG, "Setting up A7670E GSM Module..."); }

void A7670EComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "A7670E GSM module connected with:");
  LOG_UPDATE_INTERVAL(this);
}

void A7670EComponent::update() {
  switch (this->state_) {
    case STATE_INIT:
      ESP_LOGD(TAG, "Initializing GSM module");
      this->set_state(STATE_AWAIT_CMD_AT);
      this->run_command("AT", 1000);
      break;
    default:
      ESP_LOGE(TAG, "An unhandled state occurred in update: %i", this->state_);
      this->set_state(STATE_IDLE);
      break;
  }
}

void A7670EComponent::loop() {
  if (this->command_pending_) {
    bool completed = this->read_available_data();
    if (completed) {
      this->on_command_response();
    }
  }
}

bool A7670EComponent::read_available_data() {
  if (!this->command_pending_) {
    ESP_LOGD(TAG, "No data to read because no command is running");
    return false;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);

    if (byte == ASCII_CR) {
      continue;
    }

    if (byte >= 0x7F) {
      byte = '?';
    }

    if (byte == ASCII_LF) {
      byte = 0;
      this->command_response_data_.push_back(byte);
      return true;
    }

    this->command_response_data_.push_back(byte);
  }

  return false;
}

bool A7670EComponent::is_command_expired() {
  if (!this->command_pending_ || this->command_expiration_time_ == NO_EXPIRATION) {
    return false;
  }

  uint32_t current_time = millis();
  return current_time >= this->command_expiration_time_;
}

void A7670EComponent::finish_command() {
  this->command_pending_ = false;
  this->command_expiration_time_ = NO_EXPIRATION;
  this->command_response_data_.clear();
}

void A7670EComponent::set_state(State state) {
  ESP_LOGD(TAG, "Setting state to %d", state);
  this->state_ = state;
  if (this->state_sensor_ != nullptr) {
    this->state_sensor_->publish_state(this->get_state_name());
  }
}

std::string A7670EComponent::response_as_string() {
  if (this->command_response_data_.back() == '\0') {  // If response is null-terminated string we can just cast it
    return std::string(this->command_response_data_.begin(), this->command_response_data_.end());
  } else {  // Otherwise we add the terminator ourselves
    char *cstr = new char[this->command_response_data_.size() + 1];
    std::memcpy(cstr, this->command_response_data_.data(), this->command_response_data_.size());
    cstr[this->command_response_data_.size()] = '\0';
    return std::string(cstr);
  }
}

void A7670EComponent::on_command_response() {
  std::string response_string = this->response_as_string();
  switch (this->state_) {
    case STATE_AWAIT_CMD_AT:
      if (str_equals_case_insensitive(response_string, "OK")) {
        ESP_LOGI(TAG, "It's alive");
        this->set_state(STATE_IDLE);
      }
      break;
    default:
      ESP_LOGE(TAG, "An unhandled state occurred in on_command_response: %i", this->state_);
      this->set_state(STATE_IDLE);
      break;
  }

  this->finish_command();
}

void A7670EComponent::run_command(std::string cmd, uint32_t timeout) {
  ESP_LOGV(TAG, "S: %s", cmd.c_str());
  this->write_str(cmd.c_str());
  this->command_pending_ = true;
  this->command_expiration_time_ = millis() + timeout;
  this->write_byte(ASCII_CR);  // Commit the command
}

void A7670EComponent::send_sms(std::string message) {}

std::string A7670EComponent::get_state_name() {
  switch (this->state_) {
    case STATE_INIT:
      return "INIT";
    default:
      return "UNKNOWN#" + this->state_;
  }
}

}  // namespace a7670e
}  // namespace esphome
