#ifndef DYNAMIXEL_SDK_INTERFACE_HPP_
#define DYNAMIXEL_SDK_INTERFACE_HPP_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <lib_functions/iostream_lib.h>

#include <dynamixel_sdk/dynamixel_sdk.h>

#include <dynamixel_sdk_interface/dynamixel_hx_addresses.hpp>
#include <dynamixel_sdk_interface/dynamixel_hax_addresses.hpp>
#include <dynamixel_sdk_interface/dynamixel_px_addresses.hpp>
#include <dynamixel_sdk_interface/dynamixel_sdk_enums.hpp>
#include <dynamixel_sdk_interface/dynamixel_xx_addresses.hpp>

enum class DXL_MODEL_FAMILY
{
    PX,
    XX,
    HX,
    HAX  // PRO H-series with Advanced firmware (R(A)); legacy HX is unchanged.
};

enum class DXL_CONTROL_ITEM
{
    MODEL_NUMBER,
    MODEL_INFORMATION,
    FIRMWARE_VERSION,
    ID,
    BAUD_RATE,
    RETURN_DELAY_TIME,
    DRIVE_MODE,
    OPERATING_MODE,
    SECONDARY_ID,
    PROTOCOL_TYPE,
    HOMING_OFFSET,
    MOVING_THRESHOLD,
    TEMPERATURE_LIMIT,
    MAX_VOLTAGE_LIMIT,
    MIN_VOLTAGE_LIMIT,
    PWM_LIMIT,
    CURRENT_LIMIT,
    TORQUE_LIMIT,
    ACCELERATION_LIMIT,
    VELOCITY_LIMIT,
    MAX_POSITION_LIMIT,
    MIN_POSITION_LIMIT,
    STARTUP_CONFIGURATION,
    SHUTDOWN,
    TORQUE_ENABLE,
    LED,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    STATUS_RETURN_LEVEL,
    REGISTERED_INSTRUCTION,
    HARDWARE_ERROR_STATUS,
    VELOCITY_I_GAIN,
    VELOCITY_P_GAIN,
    POSITION_D_GAIN,
    POSITION_I_GAIN,
    POSITION_P_GAIN,
    FEEDFORWARD_2ND_GAIN,
    FEEDFORWARD_1ST_GAIN,
    BUS_WATCHDOG,
    GOAL_PWM,
    GOAL_CURRENT,
    GOAL_TORQUE,
    GOAL_VELOCITY,
    GOAL_ACCELERATION,
    PROFILE_ACCELERATION,
    PROFILE_VELOCITY,
    GOAL_POSITION,
    REALTIME_TICK,
    MOVING,
    MOVING_STATUS,
    PRESENT_PWM,
    PRESENT_CURRENT,
    PRESENT_VELOCITY,
    PRESENT_POSITION,
    VELOCITY_TRAJECTORY,
    POSITION_TRAJECTORY,
    PRESENT_INPUT_VOLTAGE,
    PRESENT_TEMPERATURE,
    BACKUP_READY
};

struct DXL_REGISTER_INFO
{
    uint16_t address;
    uint8_t size;
    bool is_signed;
    bool writable;

    DXL_REGISTER_INFO()
        : address(0), size(0), is_signed(false), writable(false)
    {
    }
};

enum class DXL_INTERFACE_STATUS
{
    SUCCESS,
    LOCK_TIMEOUT,
    PORT_NOT_OPEN,
    PORT_OPEN_FAILED,
    INVALID_ARGUMENT,
    UNSUPPORTED,
    COMMUNICATION_ERROR,
    DEVICE_ERROR
};

struct DXL_INTERFACE_RESULT
{
    DXL_INTERFACE_STATUS status;
    int communication_result;
    uint8_t device_error;
    uint8_t id;
    std::string operation;

    DXL_INTERFACE_RESULT()
        : status(DXL_INTERFACE_STATUS::SUCCESS),
          communication_result(COMM_SUCCESS),
          device_error(0),
          id(0xFF)
    {
    }

    bool ok() const
    {
        return status == DXL_INTERFACE_STATUS::SUCCESS;
    }
};

struct DXL_DEVICE_INFO
{
    uint16_t model_number;
    std::string model_name;
    DXL_MODEL_FAMILY family;
    bool family_configured;
    double position_resolution;
    double velocity_unit_rpm;
    double profile_velocity_unit_rpm;
    double current_unit_ampere;
    double voltage_unit_volt;
    double temperature_unit_celsius;

    DXL_DEVICE_INFO()
        : model_number(0),
          family(DXL_MODEL_FAMILY::PX),
          family_configured(false),
          position_resolution(0.0),
          velocity_unit_rpm(0.0),
          profile_velocity_unit_rpm(0.0),
          current_unit_ampere(0.0),
          voltage_unit_volt(0.1),
          temperature_unit_celsius(1.0)
    {
    }
};

namespace DXL_MODEL_CONSTANTS
{
    constexpr uint16_t PH54_200_MODEL_NUMBER = 2020;
    constexpr uint16_t PH54_100_MODEL_NUMBER = 2010;
    constexpr uint16_t PH42_020_MODEL_NUMBER = 2000;
    constexpr uint16_t PM54_060_MODEL_NUMBER = 2120;
    constexpr uint16_t PM54_040_MODEL_NUMBER = 2110;
    constexpr uint16_t PM42_010_MODEL_NUMBER = 2100;
    constexpr uint16_t LEGACY_H54_200_MODEL_NUMBER = 54024;
    constexpr uint16_t LEGACY_H54_100_MODEL_NUMBER = 53768;
    constexpr uint16_t HAX_H54_200_MODEL_NUMBER = 54025;
    constexpr uint16_t HAX_H54_100_MODEL_NUMBER = 53769;
    constexpr uint16_t HAX_H42_020_MODEL_NUMBER = 51201;

    constexpr double PH54_POSITION_RESOLUTION = 1003846.0;
    constexpr double PH42_POSITION_RESOLUTION = 607500.0;
    constexpr double PM54_POSITION_RESOLUTION = 502834.0;
    constexpr double PM42_POSITION_RESOLUTION = 526374.0;
    constexpr double LEGACY_H54_POSITION_RESOLUTION = 501923.0;
    constexpr double HAX_H54_POSITION_RESOLUTION = 1003846.0;
    constexpr double HAX_H42_POSITION_RESOLUTION = 607500.0;
    constexpr double HAX_VELOCITY_UNIT_RPM = 0.01;
    constexpr double HAX_CURRENT_UNIT_AMPERE = 0.001;
    constexpr double X_SERIES_12_BIT_POSITION_RESOLUTION = 4096.0;
    constexpr double PH54_VELOCITY_UNIT_RPM = 0.01;
    constexpr double LEGACY_H54_VELOCITY_UNIT_RPM = 0.00199234;
    constexpr double COMMON_X_SERIES_VELOCITY_UNIT_RPM = 0.229;
    constexpr double PH54_CURRENT_UNIT_AMPERE = 0.001;
    constexpr double LEGACY_H54_CURRENT_UNIT_AMPERE = 0.01611328;
}

class DYNAMIXEL_SDK_INTERFACE
{
private:
    dynamixel::PortHandler *portHandler;
    dynamixel::PacketHandler *packetHandler;
    mutable std::timed_mutex port_mutex_;
    mutable std::mutex device_mutex_;
    mutable std::mutex result_mutex_;
    std::map<uint8_t, DXL_DEVICE_INFO> devices_;
    std::string device_name_;
    unsigned int baudrate_;
    std::chrono::milliseconds lock_timeout_;
    std::atomic<bool> port_open_;
    DXL_INTERFACE_RESULT last_result_;

protected:
    uint8_t dxl_error_;
    int dxl_comm_result_;

public:
    DYNAMIXEL_SDK_INTERFACE(const char* device_name, unsigned int baudrate);
    ~DYNAMIXEL_SDK_INTERFACE();

    DYNAMIXEL_SDK_INTERFACE(const DYNAMIXEL_SDK_INTERFACE&) = delete;
    DYNAMIXEL_SDK_INTERFACE& operator=(const DYNAMIXEL_SDK_INTERFACE&) = delete;

protected:
    bool write1ByteTxOnly(uint8_t ID, uint16_t ADDR, uint8_t DATA);
    bool write1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t DATA);
    bool write2ByteTxOnly(uint8_t ID, uint16_t ADDR, uint16_t DATA);
    bool write2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t DATA);
    bool write4ByteTxOnly(uint8_t ID, uint16_t ADDR, uint32_t DATA);
    bool write4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t DATA);
    bool read1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t* DATA);
    bool read2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t* DATA);
    bool read4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t* DATA);

private:
    bool tryLockPort(std::unique_lock<std::timed_mutex>& lock, const char* operation, uint8_t ID, bool require_open = true);
    bool openPortUnlocked();
    void closePortUnlocked();
    bool finishTransaction(const char* operation, uint8_t ID, int communication_result, uint8_t device_error = 0);
    void setResult(DXL_INTERFACE_STATUS status, const char* operation, uint8_t ID, int communication_result, uint8_t device_error = 0);
    bool writeGroupSyncRaw(const std::vector<uint8_t>& IDs, uint16_t ADDR, uint8_t SIZE, const std::vector<uint32_t>& DATA);
    bool readGroupSyncRaw(const std::vector<uint8_t>& IDs, uint16_t ADDR, uint8_t SIZE, std::vector<uint32_t>& DATA);
    bool writeControlItemGroup(const std::vector<uint8_t>& IDs, DXL_CONTROL_ITEM item, const std::vector<int32_t>& DATA);
    bool readControlItemGroup(const std::vector<uint8_t>& IDs, DXL_CONTROL_ITEM item, std::vector<int32_t>& DATA);
    static int32_t decodeRegisterValue(uint32_t value, const DXL_REGISTER_INFO& info);
    static uint32_t encodeRegisterValue(int32_t value, const DXL_REGISTER_INFO& info);
    static bool isRegisterValueValid(int32_t value, const DXL_REGISTER_INFO& info);

public:
    bool openPort();
    bool closePort();
    bool reopenPort();
    bool clearPort();
    bool isPortOpen() const;
    std::string getDeviceName() const;
    unsigned int getBaudrate() const;
    void setLockTimeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds getLockTimeout() const;
    DXL_INTERFACE_RESULT getLastResult() const;

    bool broadcastPing(std::vector<uint8_t>& IDs);
    bool ping(uint8_t ID, uint16_t* model_number = 0);
    bool reboot(uint8_t ID);

    bool writeRegister(uint8_t ID, uint16_t address, uint8_t size, uint32_t data);
    bool writeRegisterTxOnly(uint8_t ID, uint16_t address, uint8_t size, uint32_t data);
    bool readRegister(uint8_t ID, uint16_t address, uint8_t size, uint32_t& data);

    bool getRegisterInfo(DXL_MODEL_FAMILY family, DXL_CONTROL_ITEM item, DXL_REGISTER_INFO& info) const;
    bool supportsControlItem(uint8_t ID, DXL_CONTROL_ITEM item) const;
    bool isControlItemWritable(uint8_t ID, DXL_CONTROL_ITEM item) const;
    bool supportsOperatingMode(uint8_t ID, DXL_OPERATING_MODE mode) const;
    bool writeControlItem(uint8_t ID, DXL_CONTROL_ITEM item, int32_t DATA);
    bool readControlItem(uint8_t ID, DXL_CONTROL_ITEM item, int32_t& DATA);
    bool writeControlItems(const std::vector<uint8_t>& IDs, DXL_CONTROL_ITEM item, const std::vector<int32_t>& DATA);
    bool writeControlItemsChecked(const std::vector<uint8_t>& IDs, DXL_CONTROL_ITEM item, const std::vector<int32_t>& DATA);
    bool readControlItems(const std::vector<uint8_t>& IDs, DXL_CONTROL_ITEM item, std::vector<int32_t>& DATA);

    void setModelFamily(uint8_t ID, DXL_MODEL_FAMILY family);
    void setModelFamilies(const std::vector<uint8_t>& IDs, DXL_MODEL_FAMILY family);
    DXL_MODEL_FAMILY getModelFamily(uint8_t ID) const;
    bool isModelFamilyConfigured(uint8_t ID) const;
    void setDeviceInfo(uint8_t ID, const DXL_DEVICE_INFO& info);
    bool getDeviceInfo(uint8_t ID, DXL_DEVICE_INFO& info) const;
    bool setPositionResolution(uint8_t ID, double pulses_per_revolution);
    bool detectAndConfigureDevice(uint8_t ID);

    static DXL_DEVICE_INFO makePh54DeviceInfo();
    static DXL_DEVICE_INFO makeLegacyH54DeviceInfo();
    static DXL_DEVICE_INFO makeHax54DeviceInfo();
    static DXL_DEVICE_INFO makeHax42DeviceInfo();
    static DXL_DEVICE_INFO makeCommonXSeriesDeviceInfo();
    static bool makeDeviceInfoForModelNumber(uint16_t model_number, DXL_DEVICE_INFO& info);

    bool radiansToRawPosition(uint8_t ID, double radians, int32_t& raw_position) const;
    bool rawPositionToRadians(uint8_t ID, int32_t raw_position, double& radians) const;
    bool radiansPerSecondToRawVelocity(uint8_t ID, double radians_per_second, int32_t& raw_velocity) const;
    bool rawVelocityToRadiansPerSecond(uint8_t ID, int32_t raw_velocity, double& radians_per_second) const;
    bool ampereToRawCurrent(uint8_t ID, double ampere, int16_t& raw_current) const;
    bool rawCurrentToAmpere(uint8_t ID, int16_t raw_current, double& ampere) const;

    bool enableTorque(uint8_t ID, uint16_t ADDR);
    bool disableTorque(uint8_t ID, uint16_t ADDR);
    bool changeOperatingMode(uint8_t ID, uint16_t ADDR, DXL_OPERATING_MODE mode);

    bool set_watchdog(uint8_t ID, int TIMER);
    bool set_watchdogs(std::vector<uint8_t> IDs, int TIMER);
    bool toggleAllTorque(std::vector<uint8_t> IDs, bool toggle);

    bool turnOnXxLed(uint8_t ID);
    bool turnOffXxLed(uint8_t ID);
    bool turnPxLed(uint8_t ID, uint8_t red, uint8_t green, uint8_t blue);
    bool setLedGeneral(uint8_t ID, bool enabled);
    bool setRgbLedGeneral(uint8_t ID, uint8_t red, uint8_t green, uint8_t blue);

    bool writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int32_t> DATA);
    bool writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int16_t> DATA);
    bool writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<uint8_t> DATA);
    bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<uint8_t>& DATA);
    bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int16_t>& DATA);
    bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int32_t>& DATA);

    bool enableTorqueGeneral(uint8_t ID);
    bool disableTorqueGeneral(uint8_t ID);
    bool changeOperatingModeGeneral(uint8_t ID, DXL_OPERATING_MODE mode);
    bool readBusWatchdogGeneral(uint8_t ID, int8_t& timer);
    bool writeBusWatchdogGeneral(uint8_t ID, uint8_t timer);
    bool clearBusWatchdogGeneral(uint8_t ID);
    bool writeCurrentLimitGeneral(uint8_t ID, int16_t current_limit);
    bool writeTorqueLimitGeneral(uint8_t ID, uint16_t torque_limit);
    bool writeVelocityIGainGeneral(uint8_t ID, uint16_t velocity_i_gain);
    bool writeVelocityPGainGeneral(uint8_t ID, uint16_t velocity_p_gain);
    bool writePositionDGainGeneral(uint8_t ID, uint16_t position_d_gain);
    bool writePositionIGainGeneral(uint8_t ID, uint16_t position_i_gain);
    bool writePositionPGainGeneral(uint8_t ID, uint16_t position_p_gain);
    bool writeProfileVelocityGeneral(uint8_t ID, int32_t profile_velocity);
    bool writeProfileAccelerationGeneral(uint8_t ID, int32_t profile_acceleration);
    bool readGoalPWMGeneral(uint8_t ID, int16_t& goal_pwm);
    bool writeGoalPWMGeneral(uint8_t ID, int16_t goal_pwm);
    bool readGoalCurrentGeneral(uint8_t ID, int16_t& goal_current);
    bool writeGoalCurrentGeneral(uint8_t ID, int16_t goal_current);
    bool readGoalTorqueGeneral(uint8_t ID, int16_t& goal_torque);
    bool writeGoalTorqueGeneral(uint8_t ID, int16_t goal_torque);
    bool readGoalVelocityGeneral(uint8_t ID, int32_t& goal_velocity);
    bool writeGoalVelocityGeneral(uint8_t ID, int32_t goal_velocity);
    bool readGoalPositionGeneral(uint8_t ID, int32_t& goal_position);
    bool writeGoalPositionGeneral(uint8_t ID, int32_t goal_position);
    bool readPresentPWMGeneral(uint8_t ID, int16_t& present_pwm);
    bool readPresentCurrentGeneral(uint8_t ID, int16_t& present_current);
    bool readPresentVelocityGeneral(uint8_t ID, int32_t& present_velocity);
    bool readPresentPositionGeneral(uint8_t ID, int32_t& present_position);
    bool readPresentInputVoltageGeneral(uint8_t ID, uint16_t& present_voltage);
    bool readPresentTemperatureGeneral(uint8_t ID, uint8_t& present_temperature);
    bool readHardwareErrorStatusGeneral(uint8_t ID, uint8_t& hardware_error_status);
    bool readTorqueEnabledGeneral(uint8_t ID, bool& enabled);
    bool readMovingGeneral(uint8_t ID, bool& moving);
    bool readMovingStatusGeneral(uint8_t ID, uint8_t& moving_status);
    bool readPresentPositionGeneral(const std::vector<uint8_t>& IDs, std::vector<int32_t>& DATA);
    bool readPresentVelocityGeneral(const std::vector<uint8_t>& IDs, std::vector<int32_t>& DATA);
    bool readPresentCurrentGeneral(const std::vector<uint8_t>& IDs, std::vector<int16_t>& DATA);
    bool writeGoalPositionGeneral(const std::vector<uint8_t>& IDs, const std::vector<int32_t>& DATA);

    bool writeGoalPositionRadians(uint8_t ID, double radians);
    bool writeGoalPositionsRadians(const std::vector<uint8_t>& IDs, const std::vector<double>& radians);
    bool readPresentPositionRadians(uint8_t ID, double& radians);
    bool readPresentPositionsRadians(const std::vector<uint8_t>& IDs, std::vector<double>& radians);
    bool readPresentVelocityRadiansPerSecond(uint8_t ID, double& radians_per_second);
    bool readPresentVelocitiesRadiansPerSecond(const std::vector<uint8_t>& IDs, std::vector<double>& radians_per_second);
    bool readPresentCurrentAmpere(uint8_t ID, double& ampere);
    bool writeGoalCurrentAmpere(uint8_t ID, double ampere);
    bool writeCurrentLimitAmpere(uint8_t ID, double ampere);
    bool writeProfileVelocityRadiansPerSecond(uint8_t ID, double radians_per_second);

    bool writeCurrentLimit(uint8_t ID, int16_t current_limit);
    bool writeProfileVelocity(uint8_t ID, int32_t profile_velocity);
    bool writeProfileAcceleration(uint8_t ID, int32_t profile_acceleration);
    bool readGoalCurrent(uint8_t ID, int16_t& goal_current);
    bool writeGoalCurrent(uint8_t ID, int16_t goal_current);
    bool readGoalVelocity(uint8_t ID, int32_t& goal_velocity);
    bool writeGoalVelocity(uint8_t ID, int32_t goal_velocity);
    bool readGoalPosition(uint8_t ID, int32_t& goal_position);
    bool writeGoalPosition(uint8_t ID, int32_t goal_position);
    bool readPresentPWM(uint8_t ID, int16_t& present_pwm);
    bool readPresentCurrent(uint8_t ID, int16_t& present_current);
    bool readPresentVelocity(uint8_t ID, int32_t& present_velocity);
    bool readPresentPosition(uint8_t ID, int32_t& present_position);
};

#endif
