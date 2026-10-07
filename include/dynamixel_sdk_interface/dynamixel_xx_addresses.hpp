#ifndef __DYNAMIXEL_XX_ADDRESSES_HPP__
#define __DYNAMIXEL_XX_ADDRESSES_HPP__

// ROM Area
#define ADDR_XX_MODEL_NUMBER                0
#define ADDR_XX_MODEL_INFORMATION           2
#define ADDR_XX_FIRMWARE_VERSION            6
#define ADDR_XX_ID                          7
#define ADDR_XX_BAUD_RATE                   8
#define ADDR_XX_RETURN_DELAY_TIME           9
#define ADDR_XX_DRIVE_MODE                  10
#define ADDR_XX_OPERATING_MODE              11
#define ADDR_XX_SECONDARY_ID                12
#define ADDR_XX_PROTOCOL_TYPE               13
#define ADDR_XX_HOMING_OFFSET               20
#define ADDR_XX_MOVING_THRESHOLD            24
#define ADDR_XX_TEMPERATURE_LIMIT           31
#define ADDR_XX_MAX_VOLTAGE_LIMIT           32
#define ADDR_XX_MIN_VOLTAGE_LIMIT           34
#define ADDR_XX_PWM_LIMIT                   36
#define ADDR_XX_CURRENT_LIMIT               38
#define ADDR_XX_ACCELERATION_LIMIT          40
#define ADDR_XX_VELOCITY_LIMIT              44
#define ADDR_XX_MAX_POSITION_LIMIT          48
#define ADDR_XX_MIN_POSITION_LIMIT          52
#define ADDR_XX_STARTUP_CONFIGURATION       60
#define ADDR_XX_PWM_SLOPE                   62
#define ADDR_XX_SHUTDOWN                    63

#define SIZE_XX_MODEL_NUMBER                2
#define SIZE_XX_MODEL_INFORMATION           4
#define SIZE_XX_FIRMWARE_VERSION            1
#define SIZE_XX_ID                          1
#define SIZE_XX_BAUD_RATE                   1
#define SIZE_XX_RETURN_DELAY_TIME           1
#define SIZE_XX_DRIVE_MODE                  1
#define SIZE_XX_OPERATING_MODE              1
#define SIZE_XX_SECONDARY_ID                1
#define SIZE_XX_PROTOCOL_TYPE               1
#define SIZE_XX_HOMING_OFFSET               4
#define SIZE_XX_MOVING_THRESHOLD            4
#define SIZE_XX_TEMPERATURE_LIMIT           1
#define SIZE_XX_MAX_VOLTAGE_LIMIT           2
#define SIZE_XX_MIN_VOLTAGE_LIMIT           2
#define SIZE_XX_PWM_LIMIT                   2
#define SIZE_XX_CURRENT_LIMIT               2
#define SIZE_XX_ACCELERATION_LIMIT          4
#define SIZE_XX_VELOCITY_LIMIT              4
#define SIZE_XX_MAX_POSITION_LIMIT          4
#define SIZE_XX_MIN_POSITION_LIMIT          4
#define SIZE_XX_STARTUP_CONFIGURATION       1
#define SIZE_XX_PWM_SLOPE                   1
#define SIZE_XX_SHUTDOWN                    1

// RAM Area
#define ADDR_XX_TORQUE_ENABLE               64
#define ADDR_XX_LED                         65
#define ADDR_XX_STATUS_RETURN_LEVEL         68
#define ADDR_XX_REGISTERED_INSTRUCTION      69
#define ADDR_XX_HARDWARE_ERROR_STATUS       70
#define ADDR_XX_VELOCITY_I_GAIN             76
#define ADDR_XX_VELOCITY_P_GAIN             78
#define ADDR_XX_POSITION_D_GAIN             80
#define ADDR_XX_POSITION_I_GAIN             82
#define ADDR_XX_POSITION_P_GAIN             84
#define ADDR_XX_FEEDFORWARD_2ND_GAIN        88
#define ADDR_XX_FEEDFORWARD_1ST_GAIN        90
#define ADDR_XX_BUS_WATCHDOG                98
#define ADDR_XX_GOAL_PWM                    100
#define ADDR_XX_GOAL_CURRENT                102
#define ADDR_XX_GOAL_VELOCITY               104
#define ADDR_XX_PROFILE_ACCELERATION        108
#define ADDR_XX_PROFILE_VELOCITY            112
#define ADDR_XX_GOAL_POSITION               116
#define ADDR_XX_REALTIME_TICK               120
#define ADDR_XX_MOVING                      122
#define ADDR_XX_MOVING_STATUS               123
#define ADDR_XX_PRESENT_PWM                 124
#define ADDR_XX_PRESENT_CURRENT             126
#define ADDR_XX_PRESENT_VELOCITY            128
#define ADDR_XX_PRESENT_POSITION            132
#define ADDR_XX_VELOCITY_TRAJECTORY         136
#define ADDR_XX_POSITION_TRAJECTORY         140
#define ADDR_XX_PRESENT_INPUT_VOLTAGE       144
#define ADDR_XX_PRESENT_TEMPERATURE         146
#define ADDR_XX_BACKUP_READY                147

#define SIZE_XX_TORQUE_ENABLE               1
#define SIZE_XX_LED                         1
#define SIZE_XX_STATUS_RETURN_LEVEL         1
#define SIZE_XX_REGISTERED_INSTRUCTION      1
#define SIZE_XX_HARDWARE_ERROR_STATUS       1
#define SIZE_XX_VELOCITY_I_GAIN             2
#define SIZE_XX_VELOCITY_P_GAIN             2
#define SIZE_XX_POSITION_D_GAIN             2
#define SIZE_XX_POSITION_I_GAIN             2
#define SIZE_XX_POSITION_P_GAIN             2
#define SIZE_XX_FEEDFORWARD_2ND_GAIN        2
#define SIZE_XX_FEEDFORWARD_1ST_GAIN        2
#define SIZE_XX_BUS_WATCHDOG                1
#define SIZE_XX_GOAL_PWM                    2
#define SIZE_XX_GOAL_CURRENT                2
#define SIZE_XX_GOAL_VELOCITY               4
#define SIZE_XX_PROFILE_ACCELERATION        4
#define SIZE_XX_PROFILE_VELOCITY            4
#define SIZE_XX_GOAL_POSITION               4
#define SIZE_XX_REALTIME_TICK               2
#define SIZE_XX_MOVING                      1
#define SIZE_XX_MOVING_STATUS               1
#define SIZE_XX_PRESENT_PWM                 2
#define SIZE_XX_PRESENT_CURRENT             2
#define SIZE_XX_PRESENT_VELOCITY            4
#define SIZE_XX_PRESENT_POSITION            4
#define SIZE_XX_VELOCITY_TRAJECTORY         4
#define SIZE_XX_POSITION_TRAJECTORY         4
#define SIZE_XX_PRESENT_INPUT_VOLTAGE       2
#define SIZE_XX_PRESENT_TEMPERATURE         1
#define SIZE_XX_BACKUP_READY                1

#include <cstdint>

// Added 2026-10-07: typed constants alongside the existing macro API.
namespace DYNAMIXEL {
namespace XX {
constexpr std::uint16_t MODEL_NUMBER = ADDR_XX_MODEL_NUMBER;
constexpr std::uint16_t MODEL_INFORMATION = ADDR_XX_MODEL_INFORMATION;
constexpr std::uint16_t FIRMWARE_VERSION = ADDR_XX_FIRMWARE_VERSION;
constexpr std::uint16_t ID = ADDR_XX_ID;
constexpr std::uint16_t BAUD_RATE = ADDR_XX_BAUD_RATE;
constexpr std::uint16_t RETURN_DELAY_TIME = ADDR_XX_RETURN_DELAY_TIME;
constexpr std::uint16_t DRIVE_MODE = ADDR_XX_DRIVE_MODE;
constexpr std::uint16_t OPERATING_MODE = ADDR_XX_OPERATING_MODE;
constexpr std::uint16_t SECONDARY_ID = ADDR_XX_SECONDARY_ID;
constexpr std::uint16_t PROTOCOL_TYPE = ADDR_XX_PROTOCOL_TYPE;
constexpr std::uint16_t HOMING_OFFSET = ADDR_XX_HOMING_OFFSET;
constexpr std::uint16_t MOVING_THRESHOLD = ADDR_XX_MOVING_THRESHOLD;
constexpr std::uint16_t TEMPERATURE_LIMIT = ADDR_XX_TEMPERATURE_LIMIT;
constexpr std::uint16_t MAX_VOLTAGE_LIMIT = ADDR_XX_MAX_VOLTAGE_LIMIT;
constexpr std::uint16_t MIN_VOLTAGE_LIMIT = ADDR_XX_MIN_VOLTAGE_LIMIT;
constexpr std::uint16_t PWM_LIMIT = ADDR_XX_PWM_LIMIT;
constexpr std::uint16_t CURRENT_LIMIT = ADDR_XX_CURRENT_LIMIT;
constexpr std::uint16_t ACCELERATION_LIMIT = ADDR_XX_ACCELERATION_LIMIT;
constexpr std::uint16_t VELOCITY_LIMIT = ADDR_XX_VELOCITY_LIMIT;
constexpr std::uint16_t MAX_POSITION_LIMIT = ADDR_XX_MAX_POSITION_LIMIT;
constexpr std::uint16_t MIN_POSITION_LIMIT = ADDR_XX_MIN_POSITION_LIMIT;
constexpr std::uint16_t STARTUP_CONFIGURATION = ADDR_XX_STARTUP_CONFIGURATION;
constexpr std::uint16_t PWM_SLOPE = ADDR_XX_PWM_SLOPE;
constexpr std::uint16_t SHUTDOWN = ADDR_XX_SHUTDOWN;
constexpr std::uint8_t SIZE_MODEL_NUMBER = SIZE_XX_MODEL_NUMBER;
constexpr std::uint8_t SIZE_MODEL_INFORMATION = SIZE_XX_MODEL_INFORMATION;
constexpr std::uint8_t SIZE_FIRMWARE_VERSION = SIZE_XX_FIRMWARE_VERSION;
constexpr std::uint8_t SIZE_ID = SIZE_XX_ID;
constexpr std::uint8_t SIZE_BAUD_RATE = SIZE_XX_BAUD_RATE;
constexpr std::uint8_t SIZE_RETURN_DELAY_TIME = SIZE_XX_RETURN_DELAY_TIME;
constexpr std::uint8_t SIZE_DRIVE_MODE = SIZE_XX_DRIVE_MODE;
constexpr std::uint8_t SIZE_OPERATING_MODE = SIZE_XX_OPERATING_MODE;
constexpr std::uint8_t SIZE_SECONDARY_ID = SIZE_XX_SECONDARY_ID;
constexpr std::uint8_t SIZE_PROTOCOL_TYPE = SIZE_XX_PROTOCOL_TYPE;
constexpr std::uint8_t SIZE_HOMING_OFFSET = SIZE_XX_HOMING_OFFSET;
constexpr std::uint8_t SIZE_MOVING_THRESHOLD = SIZE_XX_MOVING_THRESHOLD;
constexpr std::uint8_t SIZE_TEMPERATURE_LIMIT = SIZE_XX_TEMPERATURE_LIMIT;
constexpr std::uint8_t SIZE_MAX_VOLTAGE_LIMIT = SIZE_XX_MAX_VOLTAGE_LIMIT;
constexpr std::uint8_t SIZE_MIN_VOLTAGE_LIMIT = SIZE_XX_MIN_VOLTAGE_LIMIT;
constexpr std::uint8_t SIZE_PWM_LIMIT = SIZE_XX_PWM_LIMIT;
constexpr std::uint8_t SIZE_CURRENT_LIMIT = SIZE_XX_CURRENT_LIMIT;
constexpr std::uint8_t SIZE_ACCELERATION_LIMIT = SIZE_XX_ACCELERATION_LIMIT;
constexpr std::uint8_t SIZE_VELOCITY_LIMIT = SIZE_XX_VELOCITY_LIMIT;
constexpr std::uint8_t SIZE_MAX_POSITION_LIMIT = SIZE_XX_MAX_POSITION_LIMIT;
constexpr std::uint8_t SIZE_MIN_POSITION_LIMIT = SIZE_XX_MIN_POSITION_LIMIT;
constexpr std::uint8_t SIZE_STARTUP_CONFIGURATION = SIZE_XX_STARTUP_CONFIGURATION;
constexpr std::uint8_t SIZE_PWM_SLOPE = SIZE_XX_PWM_SLOPE;
constexpr std::uint8_t SIZE_SHUTDOWN = SIZE_XX_SHUTDOWN;
constexpr std::uint16_t TORQUE_ENABLE = ADDR_XX_TORQUE_ENABLE;
constexpr std::uint16_t LED = ADDR_XX_LED;
constexpr std::uint16_t STATUS_RETURN_LEVEL = ADDR_XX_STATUS_RETURN_LEVEL;
constexpr std::uint16_t REGISTERED_INSTRUCTION = ADDR_XX_REGISTERED_INSTRUCTION;
constexpr std::uint16_t HARDWARE_ERROR_STATUS = ADDR_XX_HARDWARE_ERROR_STATUS;
constexpr std::uint16_t VELOCITY_I_GAIN = ADDR_XX_VELOCITY_I_GAIN;
constexpr std::uint16_t VELOCITY_P_GAIN = ADDR_XX_VELOCITY_P_GAIN;
constexpr std::uint16_t POSITION_D_GAIN = ADDR_XX_POSITION_D_GAIN;
constexpr std::uint16_t POSITION_I_GAIN = ADDR_XX_POSITION_I_GAIN;
constexpr std::uint16_t POSITION_P_GAIN = ADDR_XX_POSITION_P_GAIN;
constexpr std::uint16_t FEEDFORWARD_2ND_GAIN = ADDR_XX_FEEDFORWARD_2ND_GAIN;
constexpr std::uint16_t FEEDFORWARD_1ST_GAIN = ADDR_XX_FEEDFORWARD_1ST_GAIN;
constexpr std::uint16_t BUS_WATCHDOG = ADDR_XX_BUS_WATCHDOG;
constexpr std::uint16_t GOAL_PWM = ADDR_XX_GOAL_PWM;
constexpr std::uint16_t GOAL_CURRENT = ADDR_XX_GOAL_CURRENT;
constexpr std::uint16_t GOAL_VELOCITY = ADDR_XX_GOAL_VELOCITY;
constexpr std::uint16_t PROFILE_ACCELERATION = ADDR_XX_PROFILE_ACCELERATION;
constexpr std::uint16_t PROFILE_VELOCITY = ADDR_XX_PROFILE_VELOCITY;
constexpr std::uint16_t GOAL_POSITION = ADDR_XX_GOAL_POSITION;
constexpr std::uint16_t REALTIME_TICK = ADDR_XX_REALTIME_TICK;
constexpr std::uint16_t MOVING = ADDR_XX_MOVING;
constexpr std::uint16_t MOVING_STATUS = ADDR_XX_MOVING_STATUS;
constexpr std::uint16_t PRESENT_PWM = ADDR_XX_PRESENT_PWM;
constexpr std::uint16_t PRESENT_CURRENT = ADDR_XX_PRESENT_CURRENT;
constexpr std::uint16_t PRESENT_VELOCITY = ADDR_XX_PRESENT_VELOCITY;
constexpr std::uint16_t PRESENT_POSITION = ADDR_XX_PRESENT_POSITION;
constexpr std::uint16_t VELOCITY_TRAJECTORY = ADDR_XX_VELOCITY_TRAJECTORY;
constexpr std::uint16_t POSITION_TRAJECTORY = ADDR_XX_POSITION_TRAJECTORY;
constexpr std::uint16_t PRESENT_INPUT_VOLTAGE = ADDR_XX_PRESENT_INPUT_VOLTAGE;
constexpr std::uint16_t PRESENT_TEMPERATURE = ADDR_XX_PRESENT_TEMPERATURE;
constexpr std::uint16_t BACKUP_READY = ADDR_XX_BACKUP_READY;
constexpr std::uint8_t SIZE_TORQUE_ENABLE = SIZE_XX_TORQUE_ENABLE;
constexpr std::uint8_t SIZE_LED = SIZE_XX_LED;
constexpr std::uint8_t SIZE_STATUS_RETURN_LEVEL = SIZE_XX_STATUS_RETURN_LEVEL;
constexpr std::uint8_t SIZE_REGISTERED_INSTRUCTION = SIZE_XX_REGISTERED_INSTRUCTION;
constexpr std::uint8_t SIZE_HARDWARE_ERROR_STATUS = SIZE_XX_HARDWARE_ERROR_STATUS;
constexpr std::uint8_t SIZE_VELOCITY_I_GAIN = SIZE_XX_VELOCITY_I_GAIN;
constexpr std::uint8_t SIZE_VELOCITY_P_GAIN = SIZE_XX_VELOCITY_P_GAIN;
constexpr std::uint8_t SIZE_POSITION_D_GAIN = SIZE_XX_POSITION_D_GAIN;
constexpr std::uint8_t SIZE_POSITION_I_GAIN = SIZE_XX_POSITION_I_GAIN;
constexpr std::uint8_t SIZE_POSITION_P_GAIN = SIZE_XX_POSITION_P_GAIN;
constexpr std::uint8_t SIZE_FEEDFORWARD_2ND_GAIN = SIZE_XX_FEEDFORWARD_2ND_GAIN;
constexpr std::uint8_t SIZE_FEEDFORWARD_1ST_GAIN = SIZE_XX_FEEDFORWARD_1ST_GAIN;
constexpr std::uint8_t SIZE_BUS_WATCHDOG = SIZE_XX_BUS_WATCHDOG;
constexpr std::uint8_t SIZE_GOAL_PWM = SIZE_XX_GOAL_PWM;
constexpr std::uint8_t SIZE_GOAL_CURRENT = SIZE_XX_GOAL_CURRENT;
constexpr std::uint8_t SIZE_GOAL_VELOCITY = SIZE_XX_GOAL_VELOCITY;
constexpr std::uint8_t SIZE_PROFILE_ACCELERATION = SIZE_XX_PROFILE_ACCELERATION;
constexpr std::uint8_t SIZE_PROFILE_VELOCITY = SIZE_XX_PROFILE_VELOCITY;
constexpr std::uint8_t SIZE_GOAL_POSITION = SIZE_XX_GOAL_POSITION;
constexpr std::uint8_t SIZE_REALTIME_TICK = SIZE_XX_REALTIME_TICK;
constexpr std::uint8_t SIZE_MOVING = SIZE_XX_MOVING;
constexpr std::uint8_t SIZE_MOVING_STATUS = SIZE_XX_MOVING_STATUS;
constexpr std::uint8_t SIZE_PRESENT_PWM = SIZE_XX_PRESENT_PWM;
constexpr std::uint8_t SIZE_PRESENT_CURRENT = SIZE_XX_PRESENT_CURRENT;
constexpr std::uint8_t SIZE_PRESENT_VELOCITY = SIZE_XX_PRESENT_VELOCITY;
constexpr std::uint8_t SIZE_PRESENT_POSITION = SIZE_XX_PRESENT_POSITION;
constexpr std::uint8_t SIZE_VELOCITY_TRAJECTORY = SIZE_XX_VELOCITY_TRAJECTORY;
constexpr std::uint8_t SIZE_POSITION_TRAJECTORY = SIZE_XX_POSITION_TRAJECTORY;
constexpr std::uint8_t SIZE_PRESENT_INPUT_VOLTAGE = SIZE_XX_PRESENT_INPUT_VOLTAGE;
constexpr std::uint8_t SIZE_PRESENT_TEMPERATURE = SIZE_XX_PRESENT_TEMPERATURE;
constexpr std::uint8_t SIZE_BACKUP_READY = SIZE_XX_BACKUP_READY;
}  // namespace XX
}  // namespace DYNAMIXEL

#endif
