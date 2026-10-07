// Reference values checked against ROBOTIS PRO H(A) control tables, 2026-10-07.
#include <dynamixel_sdk_interface/dynamixel_sdk_interface.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>
#define CHECK(condition) do { if(!(condition)) throw std::runtime_error(#condition); } while(false)
struct ExpectedRegister {
    DXL_CONTROL_ITEM item;
    uint16_t address;
    uint8_t size;
    bool is_signed;
    bool writable;
};
const ExpectedRegister expected[] = {
    {DXL_CONTROL_ITEM::MODEL_NUMBER, 0, 2, false, false},
    {DXL_CONTROL_ITEM::MODEL_INFORMATION, 2, 4, false, false},
    {DXL_CONTROL_ITEM::FIRMWARE_VERSION, 6, 1, false, false},
    {DXL_CONTROL_ITEM::ID, 7, 1, false, true},
    {DXL_CONTROL_ITEM::BAUD_RATE, 8, 1, false, true},
    {DXL_CONTROL_ITEM::RETURN_DELAY_TIME, 9, 1, false, true},
    {DXL_CONTROL_ITEM::DRIVE_MODE, 10, 1, false, true},
    {DXL_CONTROL_ITEM::OPERATING_MODE, 11, 1, false, true},
    {DXL_CONTROL_ITEM::SECONDARY_ID, 12, 1, false, true},
    {DXL_CONTROL_ITEM::HOMING_OFFSET, 20, 4, true, true},
    {DXL_CONTROL_ITEM::MOVING_THRESHOLD, 24, 4, false, true},
    {DXL_CONTROL_ITEM::TEMPERATURE_LIMIT, 31, 1, false, true},
    {DXL_CONTROL_ITEM::MAX_VOLTAGE_LIMIT, 32, 2, false, true},
    {DXL_CONTROL_ITEM::MIN_VOLTAGE_LIMIT, 34, 2, false, true},
    {DXL_CONTROL_ITEM::PWM_LIMIT, 36, 2, false, true},
    {DXL_CONTROL_ITEM::CURRENT_LIMIT, 38, 2, false, true},
    {DXL_CONTROL_ITEM::ACCELERATION_LIMIT, 40, 4, false, true},
    {DXL_CONTROL_ITEM::VELOCITY_LIMIT, 44, 4, false, true},
    {DXL_CONTROL_ITEM::MAX_POSITION_LIMIT, 48, 4, true, true},
    {DXL_CONTROL_ITEM::MIN_POSITION_LIMIT, 52, 4, true, true},
    {DXL_CONTROL_ITEM::SHUTDOWN, 63, 1, false, true},
    {DXL_CONTROL_ITEM::TORQUE_ENABLE, 512, 1, false, true},
    {DXL_CONTROL_ITEM::LED_RED, 513, 1, false, true},
    {DXL_CONTROL_ITEM::LED_GREEN, 514, 1, false, true},
    {DXL_CONTROL_ITEM::LED_BLUE, 515, 1, false, true},
    {DXL_CONTROL_ITEM::STATUS_RETURN_LEVEL, 516, 1, false, true},
    {DXL_CONTROL_ITEM::REGISTERED_INSTRUCTION, 517, 1, false, false},
    {DXL_CONTROL_ITEM::HARDWARE_ERROR_STATUS, 518, 1, false, false},
    {DXL_CONTROL_ITEM::VELOCITY_I_GAIN, 524, 2, false, true},
    {DXL_CONTROL_ITEM::VELOCITY_P_GAIN, 526, 2, false, true},
    {DXL_CONTROL_ITEM::POSITION_D_GAIN, 528, 2, false, true},
    {DXL_CONTROL_ITEM::POSITION_P_GAIN, 532, 2, false, true},
    {DXL_CONTROL_ITEM::POSITION_I_GAIN, 530, 2, false, true},
    {DXL_CONTROL_ITEM::FEEDFORWARD_2ND_GAIN, 536, 2, false, true},
    {DXL_CONTROL_ITEM::FEEDFORWARD_1ST_GAIN, 538, 2, false, true},
    {DXL_CONTROL_ITEM::BUS_WATCHDOG, 546, 1, true, true},
    {DXL_CONTROL_ITEM::GOAL_PWM, 548, 2, true, true},
    {DXL_CONTROL_ITEM::GOAL_CURRENT, 550, 2, true, true},
    {DXL_CONTROL_ITEM::GOAL_VELOCITY, 552, 4, true, true},
    {DXL_CONTROL_ITEM::PROFILE_ACCELERATION, 556, 4, false, true},
    {DXL_CONTROL_ITEM::PROFILE_VELOCITY, 560, 4, false, true},
    {DXL_CONTROL_ITEM::GOAL_POSITION, 564, 4, true, true},
    {DXL_CONTROL_ITEM::REALTIME_TICK, 568, 2, false, false},
    {DXL_CONTROL_ITEM::MOVING, 570, 1, false, false},
    {DXL_CONTROL_ITEM::MOVING_STATUS, 571, 1, false, false},
    {DXL_CONTROL_ITEM::PRESENT_PWM, 572, 2, true, false},
    {DXL_CONTROL_ITEM::PRESENT_CURRENT, 574, 2, true, false},
    {DXL_CONTROL_ITEM::PRESENT_VELOCITY, 576, 4, true, false},
    {DXL_CONTROL_ITEM::PRESENT_POSITION, 580, 4, true, false},
    {DXL_CONTROL_ITEM::VELOCITY_TRAJECTORY, 584, 4, true, false},
    {DXL_CONTROL_ITEM::POSITION_TRAJECTORY, 588, 4, true, false},
    {DXL_CONTROL_ITEM::PRESENT_INPUT_VOLTAGE, 592, 2, false, false},
    {DXL_CONTROL_ITEM::PRESENT_TEMPERATURE, 594, 1, false, false},
};
int main()
{
    DYNAMIXEL_SDK_INTERFACE interface("", 4000000); // No serial port is opened.
    DXL_REGISTER_INFO reg;
    for(const auto& entry : expected)
    {
        CHECK(interface.getRegisterInfo(DXL_MODEL_FAMILY::HAX, entry.item, reg));
        CHECK(reg.address == entry.address);
        CHECK(reg.size == entry.size);
        CHECK(reg.is_signed == entry.is_signed);
        CHECK(reg.writable == entry.writable);
    }
    for(auto item : {DXL_CONTROL_ITEM::PROTOCOL_TYPE,
                     DXL_CONTROL_ITEM::STARTUP_CONFIGURATION,
                     DXL_CONTROL_ITEM::BACKUP_READY,
                     DXL_CONTROL_ITEM::GOAL_ACCELERATION,
                     DXL_CONTROL_ITEM::TORQUE_LIMIT})
        CHECK(!interface.getRegisterInfo(DXL_MODEL_FAMILY::HAX, item, reg));

    struct Model { uint16_t number; const char* name; double resolution; };
    const Model models[] = {
        {53769, "H54-100-S500-R(A)", 1003846.0},
        {54025, "H54-200-S500-R(A)", 1003846.0},
        {51201, "H42-20-S300-R(A)", 607500.0},
    };
    for(const auto& model : models)
    {
        DXL_DEVICE_INFO info;
        CHECK(DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(model.number, info));
        CHECK(info.model_number == model.number);
        CHECK(info.model_name == model.name);
        CHECK(info.family == DXL_MODEL_FAMILY::HAX && info.family_configured);
        CHECK(info.position_resolution == model.resolution);
        CHECK(info.velocity_unit_rpm == 0.01);
        CHECK(info.profile_velocity_unit_rpm == 0.01);
        CHECK(info.current_unit_ampere == 0.001);
        interface.setDeviceInfo(1, info);
        CHECK(interface.getModelFamily(1) == DXL_MODEL_FAMILY::HAX);
        CHECK(interface.supportsOperatingMode(1, DXL_OPERATING_MODE::PWM_CONTROL_MODE));
        CHECK(!interface.supportsOperatingMode(1, DXL_OPERATING_MODE::CURRENT_POSITION_MODE));
        CHECK(!interface.isControlItemWritable(1, DXL_CONTROL_ITEM::PRESENT_POSITION));
        CHECK(interface.isControlItemWritable(1, DXL_CONTROL_ITEM::PROFILE_VELOCITY));
        int32_t raw_position = 0;
        CHECK(interface.radiansToRawPosition(1, 2.0 * std::acos(-1.0), raw_position));
        CHECK(raw_position == static_cast<int32_t>(model.resolution));
        double radians = 0;
        CHECK(interface.rawPositionToRadians(1, -raw_position, radians));
        CHECK(std::abs(radians + 2.0 * std::acos(-1.0)) < 1e-9);
        int16_t raw_current = 0;
        CHECK(interface.ampereToRawCurrent(1, -1.0, raw_current));
        CHECK(raw_current == -1000);
        double current = 0;
        CHECK(interface.rawCurrentToAmpere(1, raw_current, current));
        CHECK(std::abs(current + 1.0) < 1e-9);
        double velocity = 0;
        CHECK(interface.rawVelocityToRadiansPerSecond(1, 6000, velocity));
        CHECK(std::abs(velocity - 2.0 * std::acos(-1.0)) < 1e-9);
    }
    // Regression: legacy HX and existing PX/XX keep their original tables.
    CHECK(interface.getRegisterInfo(DXL_MODEL_FAMILY::HX, DXL_CONTROL_ITEM::TORQUE_ENABLE, reg));
    CHECK(reg.address == 562);
    CHECK(interface.getRegisterInfo(DXL_MODEL_FAMILY::HX, DXL_CONTROL_ITEM::GOAL_POSITION, reg));
    CHECK(reg.address == 596);
    CHECK(interface.getRegisterInfo(DXL_MODEL_FAMILY::PX, DXL_CONTROL_ITEM::TORQUE_ENABLE, reg));
    CHECK(reg.address == 512);
    CHECK(interface.getRegisterInfo(DXL_MODEL_FAMILY::XX, DXL_CONTROL_ITEM::TORQUE_ENABLE, reg));
    CHECK(reg.address == 64);
    DXL_DEVICE_INFO legacy;
    CHECK(DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(53768, legacy));
    CHECK(legacy.family == DXL_MODEL_FAMILY::HX && legacy.position_resolution == 501923.0);
    CHECK(DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(54024, legacy));
    CHECK(legacy.family == DXL_MODEL_FAMILY::HX);
    // Header namespace and macro interfaces agree; indirect range endpoints match.
    CHECK(DYNAMIXEL::HAX::GOAL_POSITION == ADDR_HAX_GOAL_POSITION);
    CHECK(ADDR_HAX_INDIRECT_ADDRESS_128 == ADDR_HAX_INDIRECT_ADDRESS_1 + 2 * 127);
    CHECK(ADDR_HAX_INDIRECT_DATA_128 == ADDR_HAX_INDIRECT_DATA_1 + 127);
    std::cout << "HAX registers, models, units and legacy regressions passed\n";
}
