from esphome import automation
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MESSAGE, CONF_TRIGGER_ID

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@brentco"]
MULTI_CONF = True

# Configs
CONF_ON_NETWORK_CHANGED = "on_network_changed"
CONF_ON_SMS_SEND_COMPLETED = "on_sms_send_completed"


a7670e_ns = cg.esphome_ns.namespace("a7670e")
A7670EComponent = a7670e_ns.class_("A7670EComponent", cg.Component)

# When the network is changed, this trigger should be invoked
A7670ENetworkChangedTrigger = a7670e_ns.class_(
    "A7670ENetworkChangedTrigger",
    automation.Trigger.template(cg.bool_, cg.std_string),  # connected, network name
)

A7670ESmsSendCompletedTrigger = a7670e_ns.class_(
    "A7670ESmsSendCompletedTrigger",
    automation.Trigger.template(cg.bool_),  # success
)

A7670ESendSmsAction = a7670e_ns.class_("A7670ESendSmsAction", automation.Action)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(A7670EComponent),
            cv.Optional(CONF_ON_NETWORK_CHANGED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        A7670ENetworkChangedTrigger
                    )
                }
            ),
            cv.Optional(CONF_ON_SMS_SEND_COMPLETED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        A7670ESmsSendCompletedTrigger
                    )
                }
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "a7670e", require_tx=True, require_rx=True, baud_rate=9600
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for conf in config.get(CONF_ON_NETWORK_CHANGED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger, [(cg.bool_, "connected"), (cg.std_string, "network_name")], conf
        )

    for conf in config.get(CONF_ON_SMS_SEND_COMPLETED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.bool_, "success")], conf)


A7670E_SEND_SMS_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(A7670EComponent),
        cv.Required(CONF_MESSAGE): cv.templatable(cv.string),
    }
)


@automation.register_action(
    "a7670e.send_sms", A7670ESendSmsAction, A7670E_SEND_SMS_SCHEMA
)
async def a7670e_send_sms_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_MESSAGE], args, cg.std_string)
    cg.add(var.set_message(template_))
    return var
