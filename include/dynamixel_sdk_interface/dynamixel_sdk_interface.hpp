#ifndef __DYNAMIXEL_SDK_INTERFACE_HPP___
#define __DYNAMIXEL_SDK_INTERFACE_HPP___

#include <mutex>
#include <iostream>
#include <map>
#include <lib_functions/iostream_lib.h>

#include <dynamixel_sdk/dynamixel_sdk.h>

#include <dynamixel_sdk_interface/dynamixel_sdk_enums.hpp>
#include <dynamixel_sdk_interface/dynamixel_px_addresses.hpp>
#include <dynamixel_sdk_interface/dynamixel_xx_addresses.hpp>
#include <dynamixel_sdk_interface/dynamixel_hx_addresses.hpp>

enum class DXL_MODEL_FAMILY
{
    PX,
    XX,
    HX
};

class DYNAMIXEL_SDK_INTERFACE
{
private:
    dynamixel::PortHandler *portHandler;
    dynamixel::PacketHandler *packetHandler;
    std::mutex mtx;
    std::map<uint8_t, DXL_MODEL_FAMILY> model_families_;

    bool terminate_ = false;

protected:
    uint8_t dxl_error_ = 0;
    int dxl_comm_result_ = COMM_TX_FAIL;

public:
    DYNAMIXEL_SDK_INTERFACE(const char* device_name, unsigned int baudrate);
    ~DYNAMIXEL_SDK_INTERFACE();

public:
    bool write1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t DATA);
    bool write2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t DATA);
    bool write4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t DATA);
    bool read1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t* DATA);
    bool read2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t* DATA);
    bool read4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t* DATA);

    // bool writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE);
    // bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE);

public:
    bool reboot(uint8_t ID);
    bool enableTorque(uint8_t ID);
    bool disableTorque(uint8_t ID);

    bool turnOnLED(uint8_t ID);
    bool turnOffLED(uint8_t ID);

    // bool changeDriveMode(uint8_t ID, uint8_t mode);
    bool changeOperatingMode(uint8_t ID, DXL_OPERATING_MODE mode);

    bool writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int32_t> DATA);
    bool writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int16_t> DATA);
    bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<uint8_t>& DATA);
    bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int16_t>& DATA);
    bool readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int32_t>& DATA);

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

    // Model-independent wrapper API. Unregistered IDs use PX for backward
    // compatibility. These wrappers keep mixed address tables out of callers.
    void setModelFamily(uint8_t ID, DXL_MODEL_FAMILY family);
    DXL_MODEL_FAMILY getModelFamily(uint8_t ID) const;

    bool enableTorqueGeneral(uint8_t ID);
    bool disableTorqueGeneral(uint8_t ID);
    bool changeOperatingModeGeneral(uint8_t ID, DXL_OPERATING_MODE mode);
    bool writeVelocityIGainGeneral(uint8_t ID, uint16_t velocity_i_gain);
    bool writeProfileVelocityGeneral(uint8_t ID, int32_t profile_velocity);
    bool writeProfileAccelerationGeneral(uint8_t ID, int32_t profile_acceleration);
    bool readPresentPositionGeneral(const std::vector<uint8_t>& IDs, std::vector<int32_t>& DATA);
    bool readPresentVelocityGeneral(const std::vector<uint8_t>& IDs, std::vector<int32_t>& DATA);
    bool writeGoalPositionGeneral(const std::vector<uint8_t>& IDs, const std::vector<int32_t>& DATA);
};

#endif
