#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace a7670e {

enum State {
  STATE_INIT = 0,
  STATE_DEVICE_PWR_ON,
  STATE_DEVICE_WAIT_READY,
  STATE_IDLE,
  STATE_AWAIT_CMD_AT,
  STATE_ERROR_CMD_AT,
  STATE_AWAIT_CMD_COPS_CURRENT,
  STATE_ERROR_CMD_COPS_CURRENT,
};

static const uint32_t NO_EXPIRATION = 0;
static const char ASCII_CR = 0x0D;
static const char ASCII_LF = 0x0A;

class A7670EComponent : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  void loop() override;

 protected:
  bool command_pending_{false};
  uint32_t command_expiration_time_{0};
  std::vector<uint8_t> command_response_data_;
  State state_{STATE_INIT};

  /**
   * Reads any available UART data into the command response data buffer.
   *
   * If the read byte is a line feed the data is considered complete.
   * Any non-printable characters are replaced with '?' and carriage returns are removed.
   *
   * @returns True if the data is complete, false if not
   */
  bool read_available_data();

  /**
   * Checks whether the currently pending command (if any) has expired or not.
   * @returns True if the currently pending command has taken longer than its configured expiration time. False if not
   * expired or if no task is running.
   */
  bool is_command_expired();

  /**
   * Clears any command related data from the state.
   */
  void finish_command();

  /**
   * Invoked when the UART device has finished responding to the pending command.
   * Make sure to also call finish_command();
   */
  void on_command_response();

  /**
   * A utility method to set the current state of the state machine.
   * Has the added benefit of traceability.
   */
  void set_state(State state);

  /**
   * Converts the data that is in the response buffer to a std::string.
   */
  std::string response_as_string();

  /**
   * Sends a command to the UART device.
   */
  void run_command(std::string cmd, uint32_t timeout);
};
}  // namespace a7670e
}  // namespace esphome
