import esphome.codegen as cg
from esphome.components import text_sensor, uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

a76xx_ns = cg.esphome_ns.namespace("a76xx")
A76XXComponent = a76xx_ns.class_(
    "A76XXComponent", cg.PollingComponent, uart.UARTComponent, text_sensor.TextSensor
)
A76XXComponentRef = A76XXComponent.operator("ref")

CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend(
    {cv.GenerateID(): cv.declare_id(A76XXComponent)}
).extend(cv.polling_component_schema("1s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await uart.register_uart_device(var, config)
