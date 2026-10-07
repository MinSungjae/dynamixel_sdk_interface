#ifndef __DYNAMIXEL_HX_ADDRESSES_HPP__
#define __DYNAMIXEL_HX_ADDRESSES_HPP__

// ROM Area
#define ADDR_HX_MODEL_NUMBER                0
#define ADDR_HX_MODEL_INFORMATION           2
#define ADDR_HX_FIRMWARE_VERSION            6
#define ADDR_HX_ID                          7
#define ADDR_HX_BAUD_RATE                   8
#define ADDR_HX_RETURN_DELAY_TIME           9
#define ADDR_HX_OPERATING_MODE              11
#define ADDR_HX_HOMING_OFFSET               13
#define ADDR_HX_MOVING_THRESHOLD            17
#define ADDR_HX_TEMPERATURE_LIMIT           21
#define ADDR_HX_MAX_VOLTAGE_LIMIT           22
#define ADDR_HX_MIN_VOLTAGE_LIMIT           24
#define ADDR_HX_ACCELERATION_LIMIT          26
#define ADDR_HX_TORQUE_LIMIT                30
#define ADDR_HX_VELOCITY_LIMIT              32
#define ADDR_HX_MAX_POSITION_LIMIT          36
#define ADDR_HX_MIN_POSITION_LIMIT          40
#define ADDR_HX_SHUTDOWN                    48

#define SIZE_HX_MODEL_NUMBER                2
#define SIZE_HX_MODEL_INFORMATION           4
#define SIZE_HX_FIRMWARE_VERSION            1
#define SIZE_HX_ID                          1
#define SIZE_HX_BAUD_RATE                   1
#define SIZE_HX_RETURN_DELAY_TIME           1
#define SIZE_HX_OPERATING_MODE              1
#define SIZE_HX_HOMING_OFFSET               4
#define SIZE_HX_MOVING_THRESHOLD            4
#define SIZE_HX_TEMPERATURE_LIMIT           1
#define SIZE_HX_MAX_VOLTAGE_LIMIT           2
#define SIZE_HX_MIN_VOLTAGE_LIMIT           2
#define SIZE_HX_ACCELERATION_LIMIT          4
#define SIZE_HX_TORQUE_LIMIT                2
#define SIZE_HX_VELOCITY_LIMIT              4
#define SIZE_HX_MAX_POSITION_LIMIT          4
#define SIZE_HX_MIN_POSITION_LIMIT          4
#define SIZE_HX_SHUTDOWN                    1

// RAM Area
#define ADDR_HX_TORQUE_ENABLE               562
#define ADDR_HX_LED_RED                     563
#define ADDR_HX_LED_GREEN                   564
#define ADDR_HX_LED_BLUE                    565
#define ADDR_HX_VELOCITY_I_GAIN             586
#define ADDR_HX_VELOCITY_P_GAIN             588
#define ADDR_HX_POSITION_P_GAIN             594
#define ADDR_HX_GOAL_POSITION               596
#define ADDR_HX_GOAL_VELOCITY               600
#define ADDR_HX_GOAL_TORQUE                 604
#define ADDR_HX_GOAL_ACCELERATION           606
#define ADDR_HX_MOVING                      610
#define ADDR_HX_PRESENT_POSITION            611
#define ADDR_HX_PRESENT_VELOCITY            615
#define ADDR_HX_PRESENT_CURRENT             621
#define ADDR_HX_PRESENT_INPUT_VOLTAGE       623
#define ADDR_HX_PRESENT_TEMPERATURE         625
#define ADDR_HX_EXTERNAL_PORT_DATA_1        626
#define ADDR_HX_EXTERNAL_PORT_DATA_2        628
#define ADDR_HX_EXTERNAL_PORT_DATA_3        630
#define ADDR_HX_EXTERNAL_PORT_DATA_4        632
#define ADDR_HX_REGISTERED_INSTRUCTION      890
#define ADDR_HX_STATUS_RETURN_LEVEL         891
#define ADDR_HX_HARDWARE_ERROR_STATUS       892

#define SIZE_HX_TORQUE_ENABLE               1
#define SIZE_HX_LED_RED                     1
#define SIZE_HX_LED_GREEN                   1
#define SIZE_HX_LED_BLUE                    1
#define SIZE_HX_VELOCITY_I_GAIN             2
#define SIZE_HX_VELOCITY_P_GAIN             2
#define SIZE_HX_POSITION_P_GAIN             2
#define SIZE_HX_GOAL_POSITION               4
#define SIZE_HX_GOAL_VELOCITY               4
#define SIZE_HX_GOAL_TORQUE                 2
#define SIZE_HX_GOAL_ACCELERATION           4
#define SIZE_HX_MOVING                      1
#define SIZE_HX_PRESENT_POSITION            4
#define SIZE_HX_PRESENT_VELOCITY            4
#define SIZE_HX_PRESENT_CURRENT             2
#define SIZE_HX_PRESENT_INPUT_VOLTAGE       2
#define SIZE_HX_PRESENT_TEMPERATURE         1
#define SIZE_HX_EXTERNAL_PORT_DATA_1        2
#define SIZE_HX_EXTERNAL_PORT_DATA_2        2
#define SIZE_HX_EXTERNAL_PORT_DATA_3        2
#define SIZE_HX_EXTERNAL_PORT_DATA_4        2
#define SIZE_HX_REGISTERED_INSTRUCTION      1
#define SIZE_HX_STATUS_RETURN_LEVEL         1
#define SIZE_HX_HARDWARE_ERROR_STATUS       1

#include <cstdint>

// Added 2026-10-07: typed constants alongside the existing macro API.
namespace DYNAMIXEL {
namespace HX {
constexpr std::uint16_t MODEL_NUMBER = ADDR_HX_MODEL_NUMBER;
constexpr std::uint16_t MODEL_INFORMATION = ADDR_HX_MODEL_INFORMATION;
constexpr std::uint16_t FIRMWARE_VERSION = ADDR_HX_FIRMWARE_VERSION;
constexpr std::uint16_t ID = ADDR_HX_ID;
constexpr std::uint16_t BAUD_RATE = ADDR_HX_BAUD_RATE;
constexpr std::uint16_t RETURN_DELAY_TIME = ADDR_HX_RETURN_DELAY_TIME;
constexpr std::uint16_t OPERATING_MODE = ADDR_HX_OPERATING_MODE;
constexpr std::uint16_t HOMING_OFFSET = ADDR_HX_HOMING_OFFSET;
constexpr std::uint16_t MOVING_THRESHOLD = ADDR_HX_MOVING_THRESHOLD;
constexpr std::uint16_t TEMPERATURE_LIMIT = ADDR_HX_TEMPERATURE_LIMIT;
constexpr std::uint16_t MAX_VOLTAGE_LIMIT = ADDR_HX_MAX_VOLTAGE_LIMIT;
constexpr std::uint16_t MIN_VOLTAGE_LIMIT = ADDR_HX_MIN_VOLTAGE_LIMIT;
constexpr std::uint16_t ACCELERATION_LIMIT = ADDR_HX_ACCELERATION_LIMIT;
constexpr std::uint16_t TORQUE_LIMIT = ADDR_HX_TORQUE_LIMIT;
constexpr std::uint16_t VELOCITY_LIMIT = ADDR_HX_VELOCITY_LIMIT;
constexpr std::uint16_t MAX_POSITION_LIMIT = ADDR_HX_MAX_POSITION_LIMIT;
constexpr std::uint16_t MIN_POSITION_LIMIT = ADDR_HX_MIN_POSITION_LIMIT;
constexpr std::uint16_t SHUTDOWN = ADDR_HX_SHUTDOWN;
constexpr std::uint8_t SIZE_MODEL_NUMBER = SIZE_HX_MODEL_NUMBER;
constexpr std::uint8_t SIZE_MODEL_INFORMATION = SIZE_HX_MODEL_INFORMATION;
constexpr std::uint8_t SIZE_FIRMWARE_VERSION = SIZE_HX_FIRMWARE_VERSION;
constexpr std::uint8_t SIZE_ID = SIZE_HX_ID;
constexpr std::uint8_t SIZE_BAUD_RATE = SIZE_HX_BAUD_RATE;
constexpr std::uint8_t SIZE_RETURN_DELAY_TIME = SIZE_HX_RETURN_DELAY_TIME;
constexpr std::uint8_t SIZE_OPERATING_MODE = SIZE_HX_OPERATING_MODE;
constexpr std::uint8_t SIZE_HOMING_OFFSET = SIZE_HX_HOMING_OFFSET;
constexpr std::uint8_t SIZE_MOVING_THRESHOLD = SIZE_HX_MOVING_THRESHOLD;
constexpr std::uint8_t SIZE_TEMPERATURE_LIMIT = SIZE_HX_TEMPERATURE_LIMIT;
constexpr std::uint8_t SIZE_MAX_VOLTAGE_LIMIT = SIZE_HX_MAX_VOLTAGE_LIMIT;
constexpr std::uint8_t SIZE_MIN_VOLTAGE_LIMIT = SIZE_HX_MIN_VOLTAGE_LIMIT;
constexpr std::uint8_t SIZE_ACCELERATION_LIMIT = SIZE_HX_ACCELERATION_LIMIT;
constexpr std::uint8_t SIZE_TORQUE_LIMIT = SIZE_HX_TORQUE_LIMIT;
constexpr std::uint8_t SIZE_VELOCITY_LIMIT = SIZE_HX_VELOCITY_LIMIT;
constexpr std::uint8_t SIZE_MAX_POSITION_LIMIT = SIZE_HX_MAX_POSITION_LIMIT;
constexpr std::uint8_t SIZE_MIN_POSITION_LIMIT = SIZE_HX_MIN_POSITION_LIMIT;
constexpr std::uint8_t SIZE_SHUTDOWN = SIZE_HX_SHUTDOWN;
constexpr std::uint16_t TORQUE_ENABLE = ADDR_HX_TORQUE_ENABLE;
constexpr std::uint16_t LED_RED = ADDR_HX_LED_RED;
constexpr std::uint16_t LED_GREEN = ADDR_HX_LED_GREEN;
constexpr std::uint16_t LED_BLUE = ADDR_HX_LED_BLUE;
constexpr std::uint16_t VELOCITY_I_GAIN = ADDR_HX_VELOCITY_I_GAIN;
constexpr std::uint16_t VELOCITY_P_GAIN = ADDR_HX_VELOCITY_P_GAIN;
constexpr std::uint16_t POSITION_P_GAIN = ADDR_HX_POSITION_P_GAIN;
constexpr std::uint16_t GOAL_POSITION = ADDR_HX_GOAL_POSITION;
constexpr std::uint16_t GOAL_VELOCITY = ADDR_HX_GOAL_VELOCITY;
constexpr std::uint16_t GOAL_TORQUE = ADDR_HX_GOAL_TORQUE;
constexpr std::uint16_t GOAL_ACCELERATION = ADDR_HX_GOAL_ACCELERATION;
constexpr std::uint16_t MOVING = ADDR_HX_MOVING;
constexpr std::uint16_t PRESENT_POSITION = ADDR_HX_PRESENT_POSITION;
constexpr std::uint16_t PRESENT_VELOCITY = ADDR_HX_PRESENT_VELOCITY;
constexpr std::uint16_t PRESENT_CURRENT = ADDR_HX_PRESENT_CURRENT;
constexpr std::uint16_t PRESENT_INPUT_VOLTAGE = ADDR_HX_PRESENT_INPUT_VOLTAGE;
constexpr std::uint16_t PRESENT_TEMPERATURE = ADDR_HX_PRESENT_TEMPERATURE;
constexpr std::uint16_t EXTERNAL_PORT_DATA_1 = ADDR_HX_EXTERNAL_PORT_DATA_1;
constexpr std::uint16_t EXTERNAL_PORT_DATA_2 = ADDR_HX_EXTERNAL_PORT_DATA_2;
constexpr std::uint16_t EXTERNAL_PORT_DATA_3 = ADDR_HX_EXTERNAL_PORT_DATA_3;
constexpr std::uint16_t EXTERNAL_PORT_DATA_4 = ADDR_HX_EXTERNAL_PORT_DATA_4;
constexpr std::uint16_t REGISTERED_INSTRUCTION = ADDR_HX_REGISTERED_INSTRUCTION;
constexpr std::uint16_t STATUS_RETURN_LEVEL = ADDR_HX_STATUS_RETURN_LEVEL;
constexpr std::uint16_t HARDWARE_ERROR_STATUS = ADDR_HX_HARDWARE_ERROR_STATUS;
constexpr std::uint8_t SIZE_TORQUE_ENABLE = SIZE_HX_TORQUE_ENABLE;
constexpr std::uint8_t SIZE_LED_RED = SIZE_HX_LED_RED;
constexpr std::uint8_t SIZE_LED_GREEN = SIZE_HX_LED_GREEN;
constexpr std::uint8_t SIZE_LED_BLUE = SIZE_HX_LED_BLUE;
constexpr std::uint8_t SIZE_VELOCITY_I_GAIN = SIZE_HX_VELOCITY_I_GAIN;
constexpr std::uint8_t SIZE_VELOCITY_P_GAIN = SIZE_HX_VELOCITY_P_GAIN;
constexpr std::uint8_t SIZE_POSITION_P_GAIN = SIZE_HX_POSITION_P_GAIN;
constexpr std::uint8_t SIZE_GOAL_POSITION = SIZE_HX_GOAL_POSITION;
constexpr std::uint8_t SIZE_GOAL_VELOCITY = SIZE_HX_GOAL_VELOCITY;
constexpr std::uint8_t SIZE_GOAL_TORQUE = SIZE_HX_GOAL_TORQUE;
constexpr std::uint8_t SIZE_GOAL_ACCELERATION = SIZE_HX_GOAL_ACCELERATION;
constexpr std::uint8_t SIZE_MOVING = SIZE_HX_MOVING;
constexpr std::uint8_t SIZE_PRESENT_POSITION = SIZE_HX_PRESENT_POSITION;
constexpr std::uint8_t SIZE_PRESENT_VELOCITY = SIZE_HX_PRESENT_VELOCITY;
constexpr std::uint8_t SIZE_PRESENT_CURRENT = SIZE_HX_PRESENT_CURRENT;
constexpr std::uint8_t SIZE_PRESENT_INPUT_VOLTAGE = SIZE_HX_PRESENT_INPUT_VOLTAGE;
constexpr std::uint8_t SIZE_PRESENT_TEMPERATURE = SIZE_HX_PRESENT_TEMPERATURE;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_1 = SIZE_HX_EXTERNAL_PORT_DATA_1;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_2 = SIZE_HX_EXTERNAL_PORT_DATA_2;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_3 = SIZE_HX_EXTERNAL_PORT_DATA_3;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_4 = SIZE_HX_EXTERNAL_PORT_DATA_4;
constexpr std::uint8_t SIZE_REGISTERED_INSTRUCTION = SIZE_HX_REGISTERED_INSTRUCTION;
constexpr std::uint8_t SIZE_STATUS_RETURN_LEVEL = SIZE_HX_STATUS_RETURN_LEVEL;
constexpr std::uint8_t SIZE_HARDWARE_ERROR_STATUS = SIZE_HX_HARDWARE_ERROR_STATUS;
}  // namespace HX
}  // namespace DYNAMIXEL

#endif
