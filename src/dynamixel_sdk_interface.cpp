#include <dynamixel_sdk_interface/dynamixel_sdk_interface.hpp>

DYNAMIXEL_SDK_INTERFACE::DYNAMIXEL_SDK_INTERFACE(const char* device_name, unsigned int baudrate)
{
    portHandler = dynamixel::PortHandler::getPortHandler(device_name);

    packetHandler = dynamixel::PacketHandler::getPacketHandler();

    // Open port
    if(portHandler->openPort())
    {
        DEBUG_COUT("Succeed to open the port..!" << std::endl);
    }
    else
    {
        DEBUG_COUT("Failed to open the port!!!" << std::endl);
        terminate_ = true;
    }

    // Set baudrate
    if(portHandler->setBaudRate(baudrate))
    {
        DEBUG_COUT("Succeed to set baudrate to " << baudrate << "." << std::endl);
    }
    else
    {
        DEBUG_COUT("Failed to set the baudrate!!!" << std::endl);
        terminate_ = true;
    }
}

DYNAMIXEL_SDK_INTERFACE::~DYNAMIXEL_SDK_INTERFACE()
{
    portHandler->closePort();
}

bool DYNAMIXEL_SDK_INTERFACE::reboot(uint8_t ID)
{
    dxl_comm_result_ =  packetHandler->reboot(portHandler, ID, &dxl_error_);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    else if(dxl_error_ != 0)
    {
        DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
        return false;
    }
    DEBUG_COUT("[DXL " << (int)ID <<  "]: Rebooted!" << std::endl);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::enableTorque(uint8_t ID, uint16_t ADDR)
{
    if(write1ByteTxRx(ID, ADDR, static_cast<uint8_t>(DXL_TORQUE::TORQUE_ON)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Torque enabled!" << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to torque enable." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::disableTorque(uint8_t ID, uint16_t ADDR)
{
    if(write1ByteTxRx(ID, ADDR, static_cast<uint8_t>(DXL_TORQUE::TORQUE_OFF)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Torque disabled!" << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to torque disable." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::set_watchdog(uint8_t ID, int TIMER)
{
    if(write1ByteTxRx(ID, ADDR_XX_BUS_WATCHDOG, static_cast<uint8_t>(TIMER)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Watchdog timer set!" << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set Watchdog timer." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::set_watchdogs(std::vector<uint8_t> IDs, int TIMER)
{
    std::vector<uint8_t> timers(IDs.size(), static_cast<int8_t>(TIMER));
    if(writeGroupSync(IDs, ADDR_XX_BUS_WATCHDOG, SIZE_XX_BUS_WATCHDOG, timers))
    {
        if(TIMER)
            DEBUG_COUT("All DXL Watchdog timer set!" << std::endl);

        return true;
    }
    else
    {
        std::cerr << "Failed to toggle torques." << std::endl << std::endl;
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::toggleAllTorque(std::vector<uint8_t> IDs, bool toggle)
{
    std::vector<uint8_t> toggles(IDs.size(), toggle);
    if(writeGroupSync(IDs, ADDR_XX_TORQUE_ENABLE, SIZE_XX_TORQUE_ENABLE, toggles))
    {
        if(toggle)
            DEBUG_COUT("All DXL torque enabled!" << std::endl);
        else
            DEBUG_COUT("All DXL torque disabled!" << std::endl);

        return true;
    }
    else
    {
        std::cerr << "Failed to toggle torques." << std::endl << std::endl;
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::turnOffXxLed(uint8_t ID)
{
    if(write1ByteTxRx(ID, ADDR_XX_LED, static_cast<uint8_t>(DXL_LED::LED_OFF)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: LED turned off!" << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to LED turn off." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::turnOnXxLed(uint8_t ID)
{
    if(write1ByteTxRx(ID, ADDR_XX_LED, static_cast<uint8_t>(DXL_LED::LED_ON)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: LED turned on!" << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to LED turn on." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::turnPxLed(uint8_t ID, uint8_t red, uint8_t green, uint8_t blue)
{
    if(write1ByteTxOnly(ID, ADDR_PX_LED_RED, red))
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Red LED: " << (int)red << std::endl);
    if(write1ByteTxOnly(ID, ADDR_PX_LED_GREEN, green))
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Green LED: " <<(int)green << std::endl);
    if(write1ByteTxOnly(ID, ADDR_PX_LED_BLUE, blue))
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Blue LED: " << (int)blue << std::endl);

    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::changeOperatingMode(uint8_t ID, uint16_t ADDR, DXL_OPERATING_MODE mode)
{
    if(write1ByteTxRx(ID, ADDR, static_cast<uint8_t>(mode)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Change operating mode to " << static_cast<uint16_t>(mode) << "!" << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to change operating mode." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int32_t> DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR, SIZE);

    if (IDs.size() != DATA.size())
    {
        std::cerr << "IDs and DATA size mismatch." << std::endl;
        return false;
    }
    if (SIZE != 4)
    {
        std::cerr << "Size of address and data size mismatch." << std::endl;
        return false;
    }

    for (size_t idx = 0; idx < IDs.size(); idx++) {
        uint8_t bytes[4];
        bytes[0] = DXL_LOBYTE(DXL_LOWORD(DATA[idx]));
        bytes[1] = DXL_HIBYTE(DXL_LOWORD(DATA[idx]));
        bytes[2] = DXL_LOBYTE(DXL_HIWORD(DATA[idx]));
        bytes[3] = DXL_HIBYTE(DXL_HIWORD(DATA[idx]));

        if (!groupSyncWrite.addParam(IDs[idx], bytes))
        {
            DEBUG_CERR("[DXL " << (int)IDs[idx] <<  "]: Failed to add param with GroupSyncWrite." << std::endl);
            return false;
        }
    }

    int dxl_comm_result = groupSyncWrite.txPacket();
    if (dxl_comm_result != COMM_SUCCESS)
    {
        DEBUG_CERR("GroupSyncWrite failed: " << packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }

    groupSyncWrite.clearParam();
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int16_t> DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR, SIZE);

    if (IDs.size() != DATA.size())
    {
        std::cerr << "IDs and DATA size mismatch." << std::endl;
        return false;
    }
    if (SIZE != 2)
    {
        std::cerr << "Size of address and data size mismatch." << std::endl;
        return false;
    }

    for (size_t idx = 0; idx < IDs.size(); idx++) {
        uint8_t bytes[2];
        bytes[0] = DXL_LOBYTE(DXL_LOWORD(DATA[idx]));
        bytes[1] = DXL_HIBYTE(DXL_LOWORD(DATA[idx]));

        if (!groupSyncWrite.addParam(IDs[idx], bytes))
        {
            DEBUG_CERR("[DXL " << (int)IDs[idx] <<  "]: Failed to add param with GroupSyncWrite." << std::endl);
            return false;
        }
    }

    int dxl_comm_result = groupSyncWrite.txPacket();
    if (dxl_comm_result != COMM_SUCCESS)
    {
        DEBUG_CERR("GroupSyncWrite failed: " << packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }

    groupSyncWrite.clearParam();
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<uint8_t> DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR, SIZE);

    if (IDs.size() != DATA.size())
    {
        std::cerr << "IDs and DATA size mismatch." << std::endl;
        return false;
    }
    if (SIZE != 1)
    {
        std::cerr << "Size of address and data size mismatch." << std::endl;
        return false;
    }

    for (size_t idx = 0; idx < IDs.size(); idx++) {
        uint8_t bytes[1];
        bytes[0] = DXL_LOBYTE(DXL_LOWORD(DATA[idx]));

        if (!groupSyncWrite.addParam(IDs[idx], bytes))
        {
            DEBUG_CERR("[DXL " << (int)IDs[idx] <<  "]: Failed to add param with GroupSyncWrite." << std::endl);
            return false;
        }
    }

    int dxl_comm_result = groupSyncWrite.txPacket();
    if (dxl_comm_result != COMM_SUCCESS)
    {
        DEBUG_CERR("GroupSyncWrite failed: " << packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }

    groupSyncWrite.clearParam();
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<uint8_t>& DATA)
{
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(5)))
    {
        dynamixel::GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR, SIZE);

        if (IDs.size() != DATA.size())
        {
            DATA.resize(IDs.size());
        }

        for (uint8_t ID : IDs) {
            if (!groupSyncRead.addParam(ID)) {
                DEBUG_CERR("[DXL " << static_cast<int>(ID) << "]: Failed to add param with GroupSyncRead." << std::endl);
                return false;
            }
        }

        dxl_comm_result_ = groupSyncRead.txRxPacket();
        if (dxl_comm_result_ != COMM_SUCCESS) {
            DEBUG_CERR("GroupSyncRead failed: " << packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
            return false;
        }

        for (size_t idx = 0; idx < IDs.size(); idx++) {
            uint8_t ID = IDs[idx];
            if (!groupSyncRead.isAvailable(ID, ADDR, SIZE)) {
                DEBUG_CERR("[DXL " << static_cast<int>(ID) << "]: GroupSyncRead data not available." << std::endl);
                return false;
            }

            DATA[idx] = groupSyncRead.getData(ID, ADDR, SIZE);
        }
        groupSyncRead.clearParam();
        return true;
    }

    else
    {
        DEBUG_CERR("Read locked while writing commands...");
        return false;        
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int16_t>& DATA)
{
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(5)))
    {
        dynamixel::GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR, SIZE);

        if (IDs.size() != DATA.size())
        {
            DATA.resize(IDs.size());
        }

        for (uint8_t ID : IDs) {
            if (!groupSyncRead.addParam(ID)) {
                DEBUG_CERR("[DXL " << static_cast<int>(ID) << "]: Failed to add param with GroupSyncRead." << std::endl);
                return false;
            }
        }

        dxl_comm_result_ = groupSyncRead.txRxPacket();
        if (dxl_comm_result_ != COMM_SUCCESS) {
            DEBUG_CERR("GroupSyncRead failed: " << packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
            return false;
        }

        for (size_t idx = 0; idx < IDs.size(); idx++) {
            uint8_t ID = IDs[idx];
            if (!groupSyncRead.isAvailable(ID, ADDR, SIZE)) {
                DEBUG_CERR("[DXL " << static_cast<int>(ID) << "]: GroupSyncRead data not available." << std::endl);
                return false;
            }

            DATA[idx] = groupSyncRead.getData(ID, ADDR, SIZE);
        }

        groupSyncRead.clearParam();
        return true;
    }
    else
    {
        DEBUG_CERR("Read locked while writing commands...");
        return false;        
    }
}


bool DYNAMIXEL_SDK_INTERFACE::readGroupSync(std::vector<uint8_t> IDs, uint16_t ADDR, uint8_t SIZE, std::vector<int32_t>& DATA)
{
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(5)))
    {
        dynamixel::GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR, SIZE);

        if (IDs.size() != DATA.size())
        {
            DATA.resize(IDs.size());
        }

        for (uint8_t ID : IDs) {
            if (!groupSyncRead.addParam(ID)) {
                DEBUG_CERR("[DXL " << static_cast<int>(ID) << "]: Failed to add param with GroupSyncRead." << std::endl);
                return false;
            }
        }

        dxl_comm_result_ = groupSyncRead.txRxPacket();
        if (dxl_comm_result_ != COMM_SUCCESS) {
            DEBUG_CERR("GroupSyncRead failed: " << packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
            return false;
        }

        for (size_t idx = 0; idx < IDs.size(); idx++) {
            uint8_t ID = IDs[idx];
            if (!groupSyncRead.isAvailable(ID, ADDR, SIZE)) {
                DEBUG_CERR("[DXL " << static_cast<int>(ID) << "]: GroupSyncRead data not available." << std::endl);
                return false;
            }

            DATA[idx] = groupSyncRead.getData(ID, ADDR, SIZE);
        }

        groupSyncRead.clearParam();
        return true;
    }
    else
    {
        DEBUG_CERR("Read locked while writing commands...");
        return false;        
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeCurrentLimit(uint8_t ID, uint16_t ADDR, int16_t current_limit)
{
    if(write2ByteTxRx(ID, ADDR, static_cast<uint16_t>(current_limit)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set current limit to " << current_limit << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set current limit." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileVelocity(uint8_t ID, uint16_t ADDR, int32_t profile_velocity)
{
    if(write4ByteTxRx(ID, ADDR, static_cast<uint16_t>(profile_velocity)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set profile velocity to " << profile_velocity << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set profile velocity." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileAcceleration(uint8_t ID, uint16_t ADDR, int32_t profile_acceleration)
{
    if(write4ByteTxRx(ID, ADDR, static_cast<uint16_t>(profile_acceleration)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set profile acceleration to " << profile_acceleration << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set profile acceleration." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalCurrent(uint8_t ID, uint16_t ADDR, int16_t& goal_current)
{
    if(read2ByteTxRx(ID, ADDR, reinterpret_cast<uint16_t*>(&goal_current)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set goal current to " << goal_current << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set goal current." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalCurrent(uint8_t ID, uint16_t ADDR, int16_t goal_current)
{
    if(write2ByteTxRx(ID, ADDR, static_cast<uint16_t>(goal_current)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set goal current to " << goal_current << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set goal current." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalVelocity(uint8_t ID, uint16_t ADDR, int32_t& goal_velocity)
{
    if(read4ByteTxRx(ID, ADDR, reinterpret_cast<uint32_t*>(&goal_velocity)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set goal velocity to " << goal_velocity << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set goal velocity." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalVelocity(uint8_t ID, uint16_t ADDR, int32_t goal_velocity)
{
    if(write4ByteTxRx(ID, ADDR, static_cast<uint32_t>(goal_velocity)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set goal velocity to " << goal_velocity << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set goal velocity." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalPosition(uint8_t ID, uint16_t ADDR, int32_t& goal_position)
{
    if(read4ByteTxRx(ID, ADDR, reinterpret_cast<uint32_t*>(&goal_position)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set goal position to " << goal_position << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set goal position." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPosition(uint8_t ID, uint16_t ADDR, int32_t goal_position)
{
    if(write4ByteTxRx(ID, ADDR, static_cast<uint32_t>(goal_position)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Set goal position to " << goal_position << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to set goal position." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPWM(uint8_t ID, uint16_t ADDR, int16_t& present_pwm)
{
    if(read2ByteTxRx(ID, ADDR, reinterpret_cast<uint16_t*>(&present_pwm)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Present pwm: " << present_pwm << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to read present pwm." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentCurrent(uint8_t ID, uint16_t ADDR, int16_t& present_current)
{
    if(read2ByteTxRx(ID, ADDR, reinterpret_cast<uint16_t*>(&present_current)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Present current: " << present_current << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to read present current." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocity(uint8_t ID, uint16_t ADDR, int32_t& present_velocity)
{
    if(read4ByteTxRx(ID, ADDR, reinterpret_cast<uint32_t*>(&present_velocity)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Present velocity: " << present_velocity << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to read present velocity." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPosition(uint8_t ID, uint16_t ADDR, int32_t& present_position)
{
    if(read4ByteTxRx(ID, ADDR, reinterpret_cast<uint32_t*>(&present_position)))
    {
        DEBUG_COUT("[DXL " << (int)ID <<  "]: Present position: " << present_position << std::endl);
        return true;
    }
    else
    {
        DEBUG_CERR("[DXL " << (int)ID <<  "]: Failed to read present position." << std::endl);
        return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::write1ByteTxOnly(uint8_t ID, uint16_t ADDR, uint8_t DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);

    dxl_comm_result_ = packetHandler->write1ByteTxOnly(portHandler, ID, ADDR, DATA);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::write1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);

    dxl_comm_result_ = packetHandler->write1ByteTxRx(portHandler, ID, ADDR, DATA, &dxl_error_);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    else if(dxl_error_ != 0)
    {
        DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::write2ByteTxOnly(uint8_t ID, uint16_t ADDR, uint16_t DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);

    dxl_comm_result_ = packetHandler->write2ByteTxOnly(portHandler, ID, ADDR, DATA);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::write2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);

    dxl_comm_result_ = packetHandler->write2ByteTxRx(portHandler, ID, ADDR, DATA, &dxl_error_);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    else if(dxl_error_ != 0)
    {
        DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::write4ByteTxOnly(uint8_t ID, uint16_t ADDR, uint32_t DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);

    dxl_comm_result_ = packetHandler->write4ByteTxOnly(portHandler, ID, ADDR, DATA);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    return true;
}


bool DYNAMIXEL_SDK_INTERFACE::write4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t DATA)
{
    std::lock_guard<std::timed_mutex> lock(mtx);

    dxl_comm_result_ = packetHandler->write4ByteTxRx(portHandler, ID, ADDR, DATA, &dxl_error_);
    if(dxl_comm_result_ != COMM_SUCCESS)
    {
        DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
        return false;
    }
    else if(dxl_error_ != 0)
    {
        DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::read1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t* DATA)
{
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(5)))
    {
        dxl_comm_result_ = packetHandler->read1ByteTxRx(portHandler, ID, ADDR, static_cast<uint8_t*>(DATA), &dxl_error_);
        if(dxl_comm_result_ != COMM_SUCCESS)
        {
            DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
            return false;
        }
        else if(dxl_error_ != 0)
        {
            DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
            return false;
        }
        return true;
    }
    else
    {
        DEBUG_CERR("Read locked while writing commands...");
        return false;        
    }
}

bool DYNAMIXEL_SDK_INTERFACE::read2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t* DATA)
{
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(5)))
    {
        dxl_comm_result_ = packetHandler->read2ByteTxRx(portHandler, ID, ADDR, static_cast<uint16_t*>(DATA), &dxl_error_);
        if(dxl_comm_result_ != COMM_SUCCESS)
        {
            DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
            return false;
        }
        else if(dxl_error_ != 0)
        {
            DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
            return false;
        }
        return true;
    }
    else
    {
        DEBUG_CERR("Read locked while writing commands...");
        return false;        
    }
}

bool DYNAMIXEL_SDK_INTERFACE::read4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t* DATA)
{
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(5)))
    {
        dxl_comm_result_ = packetHandler->read4ByteTxRx(portHandler, ID, ADDR, static_cast<uint32_t*>(DATA), &dxl_error_);
        if(dxl_comm_result_ != COMM_SUCCESS)
        {
            DEBUG_COUT(packetHandler->getTxRxResult(dxl_comm_result_) << std::endl);
            return false;
        }
        else if(dxl_error_ != 0)
        {
            DEBUG_CERR(packetHandler->getRxPacketError(dxl_comm_result_) << std::endl);
            return false;
        }
        return true;
    }
    else
    {
        DEBUG_CERR("Read locked while writing commands...");
        return false;        
    }
}

void DYNAMIXEL_SDK_INTERFACE::setModelFamily(uint8_t ID, DXL_MODEL_FAMILY family)
{
    model_families_[ID] = family;
}

DXL_MODEL_FAMILY DYNAMIXEL_SDK_INTERFACE::getModelFamily(uint8_t ID) const
{
    const auto family = model_families_.find(ID);
    return family == model_families_.end() ? DXL_MODEL_FAMILY::PX : family->second;
}

bool DYNAMIXEL_SDK_INTERFACE::enableTorqueGeneral(uint8_t ID)
{
    uint16_t address = ADDR_PX_TORQUE_ENABLE;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_TORQUE_ENABLE;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_TORQUE_ENABLE;
    return write1ByteTxRx(ID, address, static_cast<uint8_t>(DXL_TORQUE::TORQUE_ON));
}

bool DYNAMIXEL_SDK_INTERFACE::disableTorqueGeneral(uint8_t ID)
{
    uint16_t address = ADDR_PX_TORQUE_ENABLE;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_TORQUE_ENABLE;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_TORQUE_ENABLE;
    return write1ByteTxRx(ID, address, static_cast<uint8_t>(DXL_TORQUE::TORQUE_OFF));
}

bool DYNAMIXEL_SDK_INTERFACE::changeOperatingModeGeneral(uint8_t ID, DXL_OPERATING_MODE mode)
{
    uint16_t address = ADDR_PX_OPERATING_MODE;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_OPERATING_MODE;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_OPERATING_MODE;
    return write1ByteTxRx(ID, address, static_cast<uint8_t>(mode));
}

bool DYNAMIXEL_SDK_INTERFACE::writeVelocityIGainGeneral(uint8_t ID, uint16_t velocity_i_gain)
{
    uint16_t address = ADDR_PX_VELOCITY_I_GAIN;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_VELOCITY_I_GAIN;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_VELOCITY_I_GAIN;
    return write2ByteTxRx(ID, address, velocity_i_gain);
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileVelocityGeneral(uint8_t ID, int32_t profile_velocity)
{
    uint16_t address = ADDR_PX_PROFILE_VELOCITY;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_PROFILE_VELOCITY;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_GOAL_VELOCITY;
    return write4ByteTxRx(ID, address, static_cast<uint32_t>(profile_velocity));
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileAccelerationGeneral(uint8_t ID, int32_t profile_acceleration)
{
    uint16_t address = ADDR_PX_PROFILE_ACCELERATION;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_PROFILE_ACCELERATION;
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_GOAL_ACCELERATION;
    return write4ByteTxRx(ID, address, static_cast<uint32_t>(profile_acceleration));
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPositionGeneral(
    const std::vector<uint8_t>& IDs, std::vector<int32_t>& DATA)
{
    DATA.resize(IDs.size());
    bool success = true;
    std::map<uint16_t, std::vector<size_t>> groups;
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        uint16_t address = ADDR_PX_PRESENT_POSITION;
        if(getModelFamily(IDs[idx]) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_PRESENT_POSITION;
        if(getModelFamily(IDs[idx]) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_PRESENT_POSITION;
        groups[address].push_back(idx);
    }
    for(const auto& group : groups)
    {
        std::vector<uint8_t> ids;
        std::vector<int32_t> values;
        for(size_t idx : group.second) ids.push_back(IDs[idx]);
        success &= readGroupSync(ids, group.first, 4, values);
        for(size_t idx = 0; idx < group.second.size(); ++idx)
            DATA[group.second[idx]] = values[idx];
    }
    return success;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocityGeneral(
    const std::vector<uint8_t>& IDs, std::vector<int32_t>& DATA)
{
    DATA.resize(IDs.size());
    bool success = true;
    std::map<uint16_t, std::vector<size_t>> groups;
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        uint16_t address = ADDR_PX_PRESENT_VELOCITY;
        if(getModelFamily(IDs[idx]) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_PRESENT_VELOCITY;
        if(getModelFamily(IDs[idx]) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_PRESENT_VELOCITY;
        groups[address].push_back(idx);
    }
    for(const auto& group : groups)
    {
        std::vector<uint8_t> ids;
        std::vector<int32_t> values;
        for(size_t idx : group.second) ids.push_back(IDs[idx]);
        success &= readGroupSync(ids, group.first, 4, values);
        for(size_t idx = 0; idx < group.second.size(); ++idx)
            DATA[group.second[idx]] = values[idx];
    }
    return success;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPositionGeneral(
    const std::vector<uint8_t>& IDs, const std::vector<int32_t>& DATA)
{
    if(IDs.size() != DATA.size()) return false;

    bool success = true;
    std::map<uint16_t, std::vector<size_t>> groups;
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        uint16_t address = ADDR_PX_GOAL_POSITION;
        if(getModelFamily(IDs[idx]) == DXL_MODEL_FAMILY::XX) address = ADDR_XX_GOAL_POSITION;
        if(getModelFamily(IDs[idx]) == DXL_MODEL_FAMILY::HX) address = ADDR_HX_GOAL_POSITION;
        groups[address].push_back(idx);
    }
    for(const auto& group : groups)
    {
        std::vector<uint8_t> ids;
        std::vector<int32_t> values;
        for(size_t idx : group.second)
        {
            ids.push_back(IDs[idx]);
            values.push_back(DATA[idx]);
        }
        success &= writeGroupSync(ids, group.first, 4, values);
    }
    return success;
}
