#ifndef DYNAMIXEL_HAX_ADDRESSES_HPP_
#define DYNAMIXEL_HAX_ADDRESSES_HPP_

#include <cstdint>

// Added 2026-10-07: DYNAMIXEL PRO H-series Advanced firmware (R(A)).
// Source: https://emanual.robotis.com/docs/en/dxl/pro/h54-100-s500-ra/
// Shared table: H54-100-S500-R(A), H54-200-S500-R(A), H42-20-S300-R(A).
// HX (legacy R) has different addresses. No Protocol Type, Startup
// Configuration or Backup Ready register is documented for H(A).

namespace DYNAMIXEL {
namespace HAX {
constexpr std::uint16_t MODEL_NUMBER = 0;
constexpr std::uint16_t MODEL_INFORMATION = 2;
constexpr std::uint16_t FIRMWARE_VERSION = 6;
constexpr std::uint16_t ID = 7;
constexpr std::uint16_t BAUD_RATE = 8;
constexpr std::uint16_t RETURN_DELAY_TIME = 9;
constexpr std::uint16_t DRIVE_MODE = 10;
constexpr std::uint16_t OPERATING_MODE = 11;
constexpr std::uint16_t SECONDARY_ID = 12;
constexpr std::uint16_t HOMING_OFFSET = 20;
constexpr std::uint16_t MOVING_THRESHOLD = 24;
constexpr std::uint16_t TEMPERATURE_LIMIT = 31;
constexpr std::uint16_t MAX_VOLTAGE_LIMIT = 32;
constexpr std::uint16_t MIN_VOLTAGE_LIMIT = 34;
constexpr std::uint16_t PWM_LIMIT = 36;
constexpr std::uint16_t CURRENT_LIMIT = 38;
constexpr std::uint16_t ACCELERATION_LIMIT = 40;
constexpr std::uint16_t VELOCITY_LIMIT = 44;
constexpr std::uint16_t MAX_POSITION_LIMIT = 48;
constexpr std::uint16_t MIN_POSITION_LIMIT = 52;
constexpr std::uint16_t EXTERNAL_PORT_MODE_1 = 56;
constexpr std::uint16_t EXTERNAL_PORT_MODE_2 = 57;
constexpr std::uint16_t EXTERNAL_PORT_MODE_3 = 58;
constexpr std::uint16_t EXTERNAL_PORT_MODE_4 = 59;
constexpr std::uint16_t SHUTDOWN = 63;
constexpr std::uint16_t INDIRECT_ADDRESS_1 = 168;
constexpr std::uint16_t INDIRECT_ADDRESS_2 = 170;
constexpr std::uint16_t INDIRECT_ADDRESS_3 = 172;
constexpr std::uint16_t INDIRECT_ADDRESS_128 = 422;
constexpr std::uint16_t TORQUE_ENABLE = 512;
constexpr std::uint16_t LED_RED = 513;
constexpr std::uint16_t LED_GREEN = 514;
constexpr std::uint16_t LED_BLUE = 515;
constexpr std::uint16_t STATUS_RETURN_LEVEL = 516;
constexpr std::uint16_t REGISTERED_INSTRUCTION = 517;
constexpr std::uint16_t HARDWARE_ERROR_STATUS = 518;
constexpr std::uint16_t VELOCITY_I_GAIN = 524;
constexpr std::uint16_t VELOCITY_P_GAIN = 526;
constexpr std::uint16_t POSITION_D_GAIN = 528;
constexpr std::uint16_t POSITION_P_GAIN = 532;
constexpr std::uint16_t POSITION_I_GAIN = 530;
constexpr std::uint16_t FEEDFORWARD_2ND_GAIN = 536;
constexpr std::uint16_t FEEDFORWARD_1ST_GAIN = 538;
constexpr std::uint16_t BUS_WATCHDOG = 546;
constexpr std::uint16_t GOAL_PWM = 548;
constexpr std::uint16_t GOAL_CURRENT = 550;
constexpr std::uint16_t GOAL_VELOCITY = 552;
constexpr std::uint16_t PROFILE_ACCELERATION = 556;
constexpr std::uint16_t PROFILE_VELOCITY = 560;
constexpr std::uint16_t GOAL_POSITION = 564;
constexpr std::uint16_t REALTIME_TICK = 568;
constexpr std::uint16_t MOVING = 570;
constexpr std::uint16_t MOVING_STATUS = 571;
constexpr std::uint16_t PRESENT_PWM = 572;
constexpr std::uint16_t PRESENT_CURRENT = 574;
constexpr std::uint16_t PRESENT_VELOCITY = 576;
constexpr std::uint16_t PRESENT_POSITION = 580;
constexpr std::uint16_t VELOCITY_TRAJECTORY = 584;
constexpr std::uint16_t POSITION_TRAJECTORY = 588;
constexpr std::uint16_t PRESENT_INPUT_VOLTAGE = 592;
constexpr std::uint16_t PRESENT_TEMPERATURE = 594;
constexpr std::uint16_t EXTERNAL_PORT_DATA_1 = 600;
constexpr std::uint16_t EXTERNAL_PORT_DATA_2 = 602;
constexpr std::uint16_t EXTERNAL_PORT_DATA_3 = 604;
constexpr std::uint16_t EXTERNAL_PORT_DATA_4 = 606;
constexpr std::uint16_t INDIRECT_DATA_1 = 634;
constexpr std::uint16_t INDIRECT_DATA_2 = 635;
constexpr std::uint16_t INDIRECT_DATA_3 = 636;
constexpr std::uint16_t INDIRECT_DATA_128 = 761;

constexpr std::uint8_t SIZE_MODEL_NUMBER = 2;
constexpr std::uint8_t SIZE_MODEL_INFORMATION = 4;
constexpr std::uint8_t SIZE_FIRMWARE_VERSION = 1;
constexpr std::uint8_t SIZE_ID = 1;
constexpr std::uint8_t SIZE_BAUD_RATE = 1;
constexpr std::uint8_t SIZE_RETURN_DELAY_TIME = 1;
constexpr std::uint8_t SIZE_DRIVE_MODE = 1;
constexpr std::uint8_t SIZE_OPERATING_MODE = 1;
constexpr std::uint8_t SIZE_SECONDARY_ID = 1;
constexpr std::uint8_t SIZE_HOMING_OFFSET = 4;
constexpr std::uint8_t SIZE_MOVING_THRESHOLD = 4;
constexpr std::uint8_t SIZE_TEMPERATURE_LIMIT = 1;
constexpr std::uint8_t SIZE_MAX_VOLTAGE_LIMIT = 2;
constexpr std::uint8_t SIZE_MIN_VOLTAGE_LIMIT = 2;
constexpr std::uint8_t SIZE_PWM_LIMIT = 2;
constexpr std::uint8_t SIZE_CURRENT_LIMIT = 2;
constexpr std::uint8_t SIZE_ACCELERATION_LIMIT = 4;
constexpr std::uint8_t SIZE_VELOCITY_LIMIT = 4;
constexpr std::uint8_t SIZE_MAX_POSITION_LIMIT = 4;
constexpr std::uint8_t SIZE_MIN_POSITION_LIMIT = 4;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_MODE_1 = 1;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_MODE_2 = 1;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_MODE_3 = 1;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_MODE_4 = 1;
constexpr std::uint8_t SIZE_SHUTDOWN = 1;
constexpr std::uint8_t SIZE_INDIRECT_ADDRESS_1 = 2;
constexpr std::uint8_t SIZE_INDIRECT_ADDRESS_2 = 2;
constexpr std::uint8_t SIZE_INDIRECT_ADDRESS_3 = 2;
constexpr std::uint8_t SIZE_INDIRECT_ADDRESS_128 = 2;
constexpr std::uint8_t SIZE_TORQUE_ENABLE = 1;
constexpr std::uint8_t SIZE_LED_RED = 1;
constexpr std::uint8_t SIZE_LED_GREEN = 1;
constexpr std::uint8_t SIZE_LED_BLUE = 1;
constexpr std::uint8_t SIZE_STATUS_RETURN_LEVEL = 1;
constexpr std::uint8_t SIZE_REGISTERED_INSTRUCTION = 1;
constexpr std::uint8_t SIZE_HARDWARE_ERROR_STATUS = 1;
constexpr std::uint8_t SIZE_VELOCITY_I_GAIN = 2;
constexpr std::uint8_t SIZE_VELOCITY_P_GAIN = 2;
constexpr std::uint8_t SIZE_POSITION_D_GAIN = 2;
constexpr std::uint8_t SIZE_POSITION_P_GAIN = 2;
constexpr std::uint8_t SIZE_POSITION_I_GAIN = 2;
constexpr std::uint8_t SIZE_FEEDFORWARD_2ND_GAIN = 2;
constexpr std::uint8_t SIZE_FEEDFORWARD_1ST_GAIN = 2;
constexpr std::uint8_t SIZE_BUS_WATCHDOG = 1;
constexpr std::uint8_t SIZE_GOAL_PWM = 2;
constexpr std::uint8_t SIZE_GOAL_CURRENT = 2;
constexpr std::uint8_t SIZE_GOAL_VELOCITY = 4;
constexpr std::uint8_t SIZE_PROFILE_ACCELERATION = 4;
constexpr std::uint8_t SIZE_PROFILE_VELOCITY = 4;
constexpr std::uint8_t SIZE_GOAL_POSITION = 4;
constexpr std::uint8_t SIZE_REALTIME_TICK = 2;
constexpr std::uint8_t SIZE_MOVING = 1;
constexpr std::uint8_t SIZE_MOVING_STATUS = 1;
constexpr std::uint8_t SIZE_PRESENT_PWM = 2;
constexpr std::uint8_t SIZE_PRESENT_CURRENT = 2;
constexpr std::uint8_t SIZE_PRESENT_VELOCITY = 4;
constexpr std::uint8_t SIZE_PRESENT_POSITION = 4;
constexpr std::uint8_t SIZE_VELOCITY_TRAJECTORY = 4;
constexpr std::uint8_t SIZE_POSITION_TRAJECTORY = 4;
constexpr std::uint8_t SIZE_PRESENT_INPUT_VOLTAGE = 2;
constexpr std::uint8_t SIZE_PRESENT_TEMPERATURE = 1;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_1 = 2;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_2 = 2;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_3 = 2;
constexpr std::uint8_t SIZE_EXTERNAL_PORT_DATA_4 = 2;
constexpr std::uint8_t SIZE_INDIRECT_DATA_1 = 1;
constexpr std::uint8_t SIZE_INDIRECT_DATA_2 = 1;
constexpr std::uint8_t SIZE_INDIRECT_DATA_3 = 1;
constexpr std::uint8_t SIZE_INDIRECT_DATA_128 = 1;

constexpr std::uint16_t INDIRECT_ADDRESS_COUNT = 128;
constexpr std::uint16_t INDIRECT_DATA_COUNT = 128;
// Valid index: 1..128. Indirect Address(n) = 168 + 2*(n-1);
// Indirect Data(n) = 634 + (n-1). Addresses are EEPROM; data is RAM.
}  // namespace HAX
}  // namespace DYNAMIXEL

// Compatibility with the existing ADDR_*/SIZE_* style.
#define ADDR_HAX_MODEL_NUMBER 0
#define ADDR_HAX_MODEL_INFORMATION 2
#define ADDR_HAX_FIRMWARE_VERSION 6
#define ADDR_HAX_ID 7
#define ADDR_HAX_BAUD_RATE 8
#define ADDR_HAX_RETURN_DELAY_TIME 9
#define ADDR_HAX_DRIVE_MODE 10
#define ADDR_HAX_OPERATING_MODE 11
#define ADDR_HAX_SECONDARY_ID 12
#define ADDR_HAX_HOMING_OFFSET 20
#define ADDR_HAX_MOVING_THRESHOLD 24
#define ADDR_HAX_TEMPERATURE_LIMIT 31
#define ADDR_HAX_MAX_VOLTAGE_LIMIT 32
#define ADDR_HAX_MIN_VOLTAGE_LIMIT 34
#define ADDR_HAX_PWM_LIMIT 36
#define ADDR_HAX_CURRENT_LIMIT 38
#define ADDR_HAX_ACCELERATION_LIMIT 40
#define ADDR_HAX_VELOCITY_LIMIT 44
#define ADDR_HAX_MAX_POSITION_LIMIT 48
#define ADDR_HAX_MIN_POSITION_LIMIT 52
#define ADDR_HAX_EXTERNAL_PORT_MODE_1 56
#define ADDR_HAX_EXTERNAL_PORT_MODE_2 57
#define ADDR_HAX_EXTERNAL_PORT_MODE_3 58
#define ADDR_HAX_EXTERNAL_PORT_MODE_4 59
#define ADDR_HAX_SHUTDOWN 63
#define ADDR_HAX_INDIRECT_ADDRESS_1 168
#define ADDR_HAX_INDIRECT_ADDRESS_2 170
#define ADDR_HAX_INDIRECT_ADDRESS_3 172
#define ADDR_HAX_INDIRECT_ADDRESS_128 422
#define ADDR_HAX_TORQUE_ENABLE 512
#define ADDR_HAX_LED_RED 513
#define ADDR_HAX_LED_GREEN 514
#define ADDR_HAX_LED_BLUE 515
#define ADDR_HAX_STATUS_RETURN_LEVEL 516
#define ADDR_HAX_REGISTERED_INSTRUCTION 517
#define ADDR_HAX_HARDWARE_ERROR_STATUS 518
#define ADDR_HAX_VELOCITY_I_GAIN 524
#define ADDR_HAX_VELOCITY_P_GAIN 526
#define ADDR_HAX_POSITION_D_GAIN 528
#define ADDR_HAX_POSITION_P_GAIN 532
#define ADDR_HAX_POSITION_I_GAIN 530
#define ADDR_HAX_FEEDFORWARD_2ND_GAIN 536
#define ADDR_HAX_FEEDFORWARD_1ST_GAIN 538
#define ADDR_HAX_BUS_WATCHDOG 546
#define ADDR_HAX_GOAL_PWM 548
#define ADDR_HAX_GOAL_CURRENT 550
#define ADDR_HAX_GOAL_VELOCITY 552
#define ADDR_HAX_PROFILE_ACCELERATION 556
#define ADDR_HAX_PROFILE_VELOCITY 560
#define ADDR_HAX_GOAL_POSITION 564
#define ADDR_HAX_REALTIME_TICK 568
#define ADDR_HAX_MOVING 570
#define ADDR_HAX_MOVING_STATUS 571
#define ADDR_HAX_PRESENT_PWM 572
#define ADDR_HAX_PRESENT_CURRENT 574
#define ADDR_HAX_PRESENT_VELOCITY 576
#define ADDR_HAX_PRESENT_POSITION 580
#define ADDR_HAX_VELOCITY_TRAJECTORY 584
#define ADDR_HAX_POSITION_TRAJECTORY 588
#define ADDR_HAX_PRESENT_INPUT_VOLTAGE 592
#define ADDR_HAX_PRESENT_TEMPERATURE 594
#define ADDR_HAX_EXTERNAL_PORT_DATA_1 600
#define ADDR_HAX_EXTERNAL_PORT_DATA_2 602
#define ADDR_HAX_EXTERNAL_PORT_DATA_3 604
#define ADDR_HAX_EXTERNAL_PORT_DATA_4 606
#define ADDR_HAX_INDIRECT_DATA_1 634
#define ADDR_HAX_INDIRECT_DATA_2 635
#define ADDR_HAX_INDIRECT_DATA_3 636
#define ADDR_HAX_INDIRECT_DATA_128 761
#define SIZE_HAX_MODEL_NUMBER 2
#define SIZE_HAX_MODEL_INFORMATION 4
#define SIZE_HAX_FIRMWARE_VERSION 1
#define SIZE_HAX_ID 1
#define SIZE_HAX_BAUD_RATE 1
#define SIZE_HAX_RETURN_DELAY_TIME 1
#define SIZE_HAX_DRIVE_MODE 1
#define SIZE_HAX_OPERATING_MODE 1
#define SIZE_HAX_SECONDARY_ID 1
#define SIZE_HAX_HOMING_OFFSET 4
#define SIZE_HAX_MOVING_THRESHOLD 4
#define SIZE_HAX_TEMPERATURE_LIMIT 1
#define SIZE_HAX_MAX_VOLTAGE_LIMIT 2
#define SIZE_HAX_MIN_VOLTAGE_LIMIT 2
#define SIZE_HAX_PWM_LIMIT 2
#define SIZE_HAX_CURRENT_LIMIT 2
#define SIZE_HAX_ACCELERATION_LIMIT 4
#define SIZE_HAX_VELOCITY_LIMIT 4
#define SIZE_HAX_MAX_POSITION_LIMIT 4
#define SIZE_HAX_MIN_POSITION_LIMIT 4
#define SIZE_HAX_EXTERNAL_PORT_MODE_1 1
#define SIZE_HAX_EXTERNAL_PORT_MODE_2 1
#define SIZE_HAX_EXTERNAL_PORT_MODE_3 1
#define SIZE_HAX_EXTERNAL_PORT_MODE_4 1
#define SIZE_HAX_SHUTDOWN 1
#define SIZE_HAX_INDIRECT_ADDRESS_1 2
#define SIZE_HAX_INDIRECT_ADDRESS_2 2
#define SIZE_HAX_INDIRECT_ADDRESS_3 2
#define SIZE_HAX_INDIRECT_ADDRESS_128 2
#define SIZE_HAX_TORQUE_ENABLE 1
#define SIZE_HAX_LED_RED 1
#define SIZE_HAX_LED_GREEN 1
#define SIZE_HAX_LED_BLUE 1
#define SIZE_HAX_STATUS_RETURN_LEVEL 1
#define SIZE_HAX_REGISTERED_INSTRUCTION 1
#define SIZE_HAX_HARDWARE_ERROR_STATUS 1
#define SIZE_HAX_VELOCITY_I_GAIN 2
#define SIZE_HAX_VELOCITY_P_GAIN 2
#define SIZE_HAX_POSITION_D_GAIN 2
#define SIZE_HAX_POSITION_P_GAIN 2
#define SIZE_HAX_POSITION_I_GAIN 2
#define SIZE_HAX_FEEDFORWARD_2ND_GAIN 2
#define SIZE_HAX_FEEDFORWARD_1ST_GAIN 2
#define SIZE_HAX_BUS_WATCHDOG 1
#define SIZE_HAX_GOAL_PWM 2
#define SIZE_HAX_GOAL_CURRENT 2
#define SIZE_HAX_GOAL_VELOCITY 4
#define SIZE_HAX_PROFILE_ACCELERATION 4
#define SIZE_HAX_PROFILE_VELOCITY 4
#define SIZE_HAX_GOAL_POSITION 4
#define SIZE_HAX_REALTIME_TICK 2
#define SIZE_HAX_MOVING 1
#define SIZE_HAX_MOVING_STATUS 1
#define SIZE_HAX_PRESENT_PWM 2
#define SIZE_HAX_PRESENT_CURRENT 2
#define SIZE_HAX_PRESENT_VELOCITY 4
#define SIZE_HAX_PRESENT_POSITION 4
#define SIZE_HAX_VELOCITY_TRAJECTORY 4
#define SIZE_HAX_POSITION_TRAJECTORY 4
#define SIZE_HAX_PRESENT_INPUT_VOLTAGE 2
#define SIZE_HAX_PRESENT_TEMPERATURE 1
#define SIZE_HAX_EXTERNAL_PORT_DATA_1 2
#define SIZE_HAX_EXTERNAL_PORT_DATA_2 2
#define SIZE_HAX_EXTERNAL_PORT_DATA_3 2
#define SIZE_HAX_EXTERNAL_PORT_DATA_4 2
#define SIZE_HAX_INDIRECT_DATA_1 1
#define SIZE_HAX_INDIRECT_DATA_2 1
#define SIZE_HAX_INDIRECT_DATA_3 1
#define SIZE_HAX_INDIRECT_DATA_128 1

#endif
