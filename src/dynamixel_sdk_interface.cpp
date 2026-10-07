#include <dynamixel_sdk_interface/dynamixel_sdk_interface.hpp>

#include <cmath>
#include <limits>

namespace
{
    constexpr double TWO_PI = 6.28318530717958647692;
    constexpr uint8_t NO_DEVICE_ID = 0xFF;
}

DYNAMIXEL_SDK_INTERFACE::DYNAMIXEL_SDK_INTERFACE(const char* device_name, unsigned int baudrate)
    : portHandler(0),
      packetHandler(dynamixel::PacketHandler::getPacketHandler()),
      device_name_(device_name == 0 ? "" : device_name),
      baudrate_(baudrate),
      lock_timeout_(std::chrono::milliseconds(20)),
      port_open_(false),
      dxl_error_(0),
      dxl_comm_result_(COMM_TX_FAIL)
{
    if(device_name_.empty())
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "constructor", NO_DEVICE_ID, COMM_TX_FAIL);
        return;
    }

    portHandler = dynamixel::PortHandler::getPortHandler(device_name_.c_str());
    openPort();
}

DYNAMIXEL_SDK_INTERFACE::~DYNAMIXEL_SDK_INTERFACE()
{
    std::lock_guard<std::timed_mutex> lock(port_mutex_);
    closePortUnlocked();
    delete portHandler;
    portHandler = 0;
}

bool DYNAMIXEL_SDK_INTERFACE::tryLockPort(
    std::unique_lock<std::timed_mutex>& lock,
    const char* operation,
    uint8_t ID,
    bool require_open)
{
    const std::chrono::milliseconds timeout = getLockTimeout();
    if(!lock.try_lock_for(timeout))
    {
        setResult(DXL_INTERFACE_STATUS::LOCK_TIMEOUT, operation, ID, COMM_PORT_BUSY);
        DEBUG_CERR("DYNAMIXEL port lock timed out while " << operation << "." << std::endl);
        return false;
    }

    if(require_open && !port_open_.load())
    {
        setResult(DXL_INTERFACE_STATUS::PORT_NOT_OPEN, operation, ID, COMM_TX_FAIL);
        DEBUG_CERR("DYNAMIXEL port is not open while " << operation << "." << std::endl);
        return false;
    }

    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::openPortUnlocked()
{
    if(portHandler == 0 || device_name_.empty())
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "openPort", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    closePortUnlocked();

    if(!portHandler->openPort())
    {
        setResult(DXL_INTERFACE_STATUS::PORT_OPEN_FAILED, "openPort", NO_DEVICE_ID, COMM_TX_FAIL);
        DEBUG_CERR("Failed to open DYNAMIXEL port " << device_name_ << "." << std::endl);
        return false;
    }

    if(!portHandler->setBaudRate(baudrate_))
    {
        portHandler->closePort();
        setResult(DXL_INTERFACE_STATUS::PORT_OPEN_FAILED, "setBaudRate", NO_DEVICE_ID, COMM_TX_FAIL);
        DEBUG_CERR("Failed to set DYNAMIXEL baudrate to " << baudrate_ << "." << std::endl);
        return false;
    }

    port_open_.store(true);
    setResult(DXL_INTERFACE_STATUS::SUCCESS, "openPort", NO_DEVICE_ID, COMM_SUCCESS);
    DEBUG_COUT("Opened DYNAMIXEL port " << device_name_ << " at " << baudrate_ << " bps." << std::endl);
    return true;
}

void DYNAMIXEL_SDK_INTERFACE::closePortUnlocked()
{
    if(portHandler != 0)
        portHandler->closePort();
    port_open_.store(false);
}

bool DYNAMIXEL_SDK_INTERFACE::openPort()
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "openPort", NO_DEVICE_ID, false))
        return false;
    return openPortUnlocked();
}

bool DYNAMIXEL_SDK_INTERFACE::closePort()
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "closePort", NO_DEVICE_ID, false))
        return false;

    closePortUnlocked();
    setResult(DXL_INTERFACE_STATUS::SUCCESS, "closePort", NO_DEVICE_ID, COMM_SUCCESS);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::reopenPort()
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "reopenPort", NO_DEVICE_ID, false))
        return false;

    closePortUnlocked();
    delete portHandler;
    portHandler = dynamixel::PortHandler::getPortHandler(device_name_.c_str());
    return openPortUnlocked();
}

bool DYNAMIXEL_SDK_INTERFACE::clearPort()
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "clearPort", NO_DEVICE_ID))
        return false;

    portHandler->clearPort();
    setResult(DXL_INTERFACE_STATUS::SUCCESS, "clearPort", NO_DEVICE_ID, COMM_SUCCESS);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::isPortOpen() const
{
    return port_open_.load();
}

std::string DYNAMIXEL_SDK_INTERFACE::getDeviceName() const
{
    return device_name_;
}

unsigned int DYNAMIXEL_SDK_INTERFACE::getBaudrate() const
{
    return baudrate_;
}

void DYNAMIXEL_SDK_INTERFACE::setLockTimeout(std::chrono::milliseconds timeout)
{
    if(timeout.count() < 0)
        return;

    std::lock_guard<std::mutex> lock(device_mutex_);
    lock_timeout_ = timeout;
}

std::chrono::milliseconds DYNAMIXEL_SDK_INTERFACE::getLockTimeout() const
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    return lock_timeout_;
}

DXL_INTERFACE_RESULT DYNAMIXEL_SDK_INTERFACE::getLastResult() const
{
    std::lock_guard<std::mutex> lock(result_mutex_);
    return last_result_;
}

void DYNAMIXEL_SDK_INTERFACE::setResult(
    DXL_INTERFACE_STATUS status,
    const char* operation,
    uint8_t ID,
    int communication_result,
    uint8_t device_error)
{
    std::lock_guard<std::mutex> lock(result_mutex_);
    last_result_.status = status;
    last_result_.communication_result = communication_result;
    last_result_.device_error = device_error;
    last_result_.id = ID;
    last_result_.operation = operation == 0 ? "" : operation;
}

bool DYNAMIXEL_SDK_INTERFACE::finishTransaction(
    const char* operation,
    uint8_t ID,
    int communication_result,
    uint8_t device_error)
{
    dxl_comm_result_ = communication_result;
    dxl_error_ = device_error;

    if(communication_result != COMM_SUCCESS)
    {
        setResult(DXL_INTERFACE_STATUS::COMMUNICATION_ERROR, operation, ID, communication_result, device_error);
        DEBUG_CERR("[DXL " << static_cast<int>(ID) << "] "
            << operation << " failed: "
            << packetHandler->getTxRxResult(communication_result) << std::endl);
        return false;
    }

    if(device_error != 0)
    {
        setResult(DXL_INTERFACE_STATUS::DEVICE_ERROR, operation, ID, communication_result, device_error);
        DEBUG_CERR("[DXL " << static_cast<int>(ID) << "] "
            << operation << " returned: "
            << packetHandler->getRxPacketError(device_error) << std::endl);
        return false;
    }

    setResult(DXL_INTERFACE_STATUS::SUCCESS, operation, ID, communication_result, 0);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::broadcastPing(std::vector<uint8_t>& IDs)
{
    IDs.clear();
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "broadcastPing", NO_DEVICE_ID))
        return false;

    const int communication_result = packetHandler->broadcastPing(portHandler, IDs);
    return finishTransaction("broadcastPing", NO_DEVICE_ID, communication_result);
}

bool DYNAMIXEL_SDK_INTERFACE::ping(uint8_t ID, uint16_t* model_number)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "ping", ID))
        return false;

    uint16_t detected_model = 0;
    uint8_t device_error = 0;
    const int communication_result = packetHandler->ping(
        portHandler, ID, &detected_model, &device_error);

    if(!finishTransaction("ping", ID, communication_result, device_error))
        return false;

    if(model_number != 0)
        *model_number = detected_model;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::reboot(uint8_t ID)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "reboot", ID))
        return false;

    uint8_t device_error = 0;
    const int communication_result = packetHandler->reboot(portHandler, ID, &device_error);
    return finishTransaction("reboot", ID, communication_result, device_error);
}

bool DYNAMIXEL_SDK_INTERFACE::writeRegister(
    uint8_t ID,
    uint16_t address,
    uint8_t size,
    uint32_t data)
{
    if(size == 1 && data <= std::numeric_limits<uint8_t>::max())
        return write1ByteTxRx(ID, address, static_cast<uint8_t>(data));
    if(size == 2 && data <= std::numeric_limits<uint16_t>::max())
        return write2ByteTxRx(ID, address, static_cast<uint16_t>(data));
    if(size == 4)
        return write4ByteTxRx(ID, address, data);

    setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeRegister", ID, COMM_TX_FAIL);
    return false;
}

bool DYNAMIXEL_SDK_INTERFACE::writeRegisterTxOnly(
    uint8_t ID,
    uint16_t address,
    uint8_t size,
    uint32_t data)
{
    if(size == 1 && data <= std::numeric_limits<uint8_t>::max())
        return write1ByteTxOnly(ID, address, static_cast<uint8_t>(data));
    if(size == 2 && data <= std::numeric_limits<uint16_t>::max())
        return write2ByteTxOnly(ID, address, static_cast<uint16_t>(data));
    if(size == 4)
        return write4ByteTxOnly(ID, address, data);

    setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeRegisterTxOnly", ID, COMM_TX_FAIL);
    return false;
}

bool DYNAMIXEL_SDK_INTERFACE::readRegister(
    uint8_t ID,
    uint16_t address,
    uint8_t size,
    uint32_t& data)
{
    uint32_t value = 0;
    if(size == 1)
    {
        uint8_t byte_value = 0;
        if(!read1ByteTxRx(ID, address, &byte_value))
            return false;
        value = byte_value;
    }
    else if(size == 2)
    {
        uint16_t word_value = 0;
        if(!read2ByteTxRx(ID, address, &word_value))
            return false;
        value = word_value;
    }
    else if(size == 4)
    {
        if(!read4ByteTxRx(ID, address, &value))
            return false;
    }
    else
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readRegister", ID, COMM_TX_FAIL);
        return false;
    }

    data = value;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::write1ByteTxOnly(uint8_t ID, uint16_t ADDR, uint8_t DATA)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "write1ByteTxOnly", ID))
        return false;

    const int communication_result = packetHandler->write1ByteTxOnly(portHandler, ID, ADDR, DATA);
    return finishTransaction("write1ByteTxOnly", ID, communication_result);
}

bool DYNAMIXEL_SDK_INTERFACE::write1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t DATA)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "write1ByteTxRx", ID))
        return false;

    uint8_t device_error = 0;
    const int communication_result = packetHandler->write1ByteTxRx(
        portHandler, ID, ADDR, DATA, &device_error);
    return finishTransaction("write1ByteTxRx", ID, communication_result, device_error);
}

bool DYNAMIXEL_SDK_INTERFACE::write2ByteTxOnly(uint8_t ID, uint16_t ADDR, uint16_t DATA)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "write2ByteTxOnly", ID))
        return false;

    const int communication_result = packetHandler->write2ByteTxOnly(portHandler, ID, ADDR, DATA);
    return finishTransaction("write2ByteTxOnly", ID, communication_result);
}

bool DYNAMIXEL_SDK_INTERFACE::write2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t DATA)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "write2ByteTxRx", ID))
        return false;

    uint8_t device_error = 0;
    const int communication_result = packetHandler->write2ByteTxRx(
        portHandler, ID, ADDR, DATA, &device_error);
    return finishTransaction("write2ByteTxRx", ID, communication_result, device_error);
}

bool DYNAMIXEL_SDK_INTERFACE::write4ByteTxOnly(uint8_t ID, uint16_t ADDR, uint32_t DATA)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "write4ByteTxOnly", ID))
        return false;

    const int communication_result = packetHandler->write4ByteTxOnly(portHandler, ID, ADDR, DATA);
    return finishTransaction("write4ByteTxOnly", ID, communication_result);
}

bool DYNAMIXEL_SDK_INTERFACE::write4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t DATA)
{
    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "write4ByteTxRx", ID))
        return false;

    uint8_t device_error = 0;
    const int communication_result = packetHandler->write4ByteTxRx(
        portHandler, ID, ADDR, DATA, &device_error);
    return finishTransaction("write4ByteTxRx", ID, communication_result, device_error);
}

bool DYNAMIXEL_SDK_INTERFACE::read1ByteTxRx(uint8_t ID, uint16_t ADDR, uint8_t* DATA)
{
    if(DATA == 0)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "read1ByteTxRx", ID, COMM_TX_FAIL);
        return false;
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "read1ByteTxRx", ID))
        return false;

    uint8_t value = 0;
    uint8_t device_error = 0;
    const int communication_result = packetHandler->read1ByteTxRx(
        portHandler, ID, ADDR, &value, &device_error);
    if(!finishTransaction("read1ByteTxRx", ID, communication_result, device_error))
        return false;

    *DATA = value;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::read2ByteTxRx(uint8_t ID, uint16_t ADDR, uint16_t* DATA)
{
    if(DATA == 0)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "read2ByteTxRx", ID, COMM_TX_FAIL);
        return false;
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "read2ByteTxRx", ID))
        return false;

    uint16_t value = 0;
    uint8_t device_error = 0;
    const int communication_result = packetHandler->read2ByteTxRx(
        portHandler, ID, ADDR, &value, &device_error);
    if(!finishTransaction("read2ByteTxRx", ID, communication_result, device_error))
        return false;

    *DATA = value;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::read4ByteTxRx(uint8_t ID, uint16_t ADDR, uint32_t* DATA)
{
    if(DATA == 0)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "read4ByteTxRx", ID, COMM_TX_FAIL);
        return false;
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "read4ByteTxRx", ID))
        return false;

    uint32_t value = 0;
    uint8_t device_error = 0;
    const int communication_result = packetHandler->read4ByteTxRx(
        portHandler, ID, ADDR, &value, &device_error);
    if(!finishTransaction("read4ByteTxRx", ID, communication_result, device_error))
        return false;

    *DATA = value;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSyncRaw(
    const std::vector<uint8_t>& IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    const std::vector<uint32_t>& DATA)
{
    if(IDs.size() != DATA.size() || (SIZE != 1 && SIZE != 2 && SIZE != 4))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeGroupSync", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    if(IDs.empty())
    {
        setResult(DXL_INTERFACE_STATUS::SUCCESS, "writeGroupSync", NO_DEVICE_ID, COMM_SUCCESS);
        return true;
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "writeGroupSync", NO_DEVICE_ID))
        return false;

    dynamixel::GroupSyncWrite group_sync_write(portHandler, packetHandler, ADDR, SIZE);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        uint8_t bytes[4] = {0, 0, 0, 0};
        for(uint8_t byte_idx = 0; byte_idx < SIZE; ++byte_idx)
            bytes[byte_idx] = static_cast<uint8_t>((DATA[idx] >> (8 * byte_idx)) & 0xFF);

        if(!group_sync_write.addParam(IDs[idx], bytes))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeGroupSync.addParam", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }

    const int communication_result = group_sync_write.txPacket();
    group_sync_write.clearParam();
    return finishTransaction("writeGroupSync", NO_DEVICE_ID, communication_result);
}

bool DYNAMIXEL_SDK_INTERFACE::readGroupSyncRaw(
    const std::vector<uint8_t>& IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<uint32_t>& DATA)
{
    if(SIZE != 1 && SIZE != 2 && SIZE != 4)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readGroupSync", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    if(IDs.empty())
    {
        DATA.clear();
        setResult(DXL_INTERFACE_STATUS::SUCCESS, "readGroupSync", NO_DEVICE_ID, COMM_SUCCESS);
        return true;
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "readGroupSync", NO_DEVICE_ID))
        return false;

    dynamixel::GroupSyncRead group_sync_read(portHandler, packetHandler, ADDR, SIZE);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!group_sync_read.addParam(IDs[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readGroupSync.addParam", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }

    const int communication_result = group_sync_read.txRxPacket();
    if(!finishTransaction("readGroupSync", NO_DEVICE_ID, communication_result))
        return false;

    std::vector<uint32_t> values(IDs.size(), 0);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        uint8_t device_error = 0;
        group_sync_read.getError(IDs[idx], &device_error);
        if(device_error != 0)
            return finishTransaction("readGroupSync", IDs[idx], COMM_SUCCESS, device_error);

        if(!group_sync_read.isAvailable(IDs[idx], ADDR, SIZE))
        {
            setResult(DXL_INTERFACE_STATUS::COMMUNICATION_ERROR, "readGroupSync.isAvailable", IDs[idx], COMM_RX_FAIL);
            return false;
        }
        values[idx] = group_sync_read.getData(IDs[idx], ADDR, SIZE);
    }

    group_sync_read.clearParam();
    DATA.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSync(
    std::vector<uint8_t> IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<int32_t> DATA)
{
    std::vector<uint32_t> values(DATA.size(), 0);
    for(size_t idx = 0; idx < DATA.size(); ++idx)
        values[idx] = static_cast<uint32_t>(DATA[idx]);
    return writeGroupSyncRaw(IDs, ADDR, SIZE, values);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSync(
    std::vector<uint8_t> IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<int16_t> DATA)
{
    std::vector<uint32_t> values(DATA.size(), 0);
    for(size_t idx = 0; idx < DATA.size(); ++idx)
        values[idx] = static_cast<uint16_t>(DATA[idx]);
    return writeGroupSyncRaw(IDs, ADDR, SIZE, values);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGroupSync(
    std::vector<uint8_t> IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<uint8_t> DATA)
{
    std::vector<uint32_t> values(DATA.begin(), DATA.end());
    return writeGroupSyncRaw(IDs, ADDR, SIZE, values);
}

bool DYNAMIXEL_SDK_INTERFACE::readGroupSync(
    std::vector<uint8_t> IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<uint8_t>& DATA)
{
    std::vector<uint32_t> raw_values;
    if(!readGroupSyncRaw(IDs, ADDR, SIZE, raw_values))
        return false;

    std::vector<uint8_t> values(raw_values.size(), 0);
    for(size_t idx = 0; idx < raw_values.size(); ++idx)
        values[idx] = static_cast<uint8_t>(raw_values[idx]);
    DATA.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readGroupSync(
    std::vector<uint8_t> IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<int16_t>& DATA)
{
    std::vector<uint32_t> raw_values;
    if(!readGroupSyncRaw(IDs, ADDR, SIZE, raw_values))
        return false;

    std::vector<int16_t> values(raw_values.size(), 0);
    for(size_t idx = 0; idx < raw_values.size(); ++idx)
        values[idx] = static_cast<int16_t>(raw_values[idx]);
    DATA.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readGroupSync(
    std::vector<uint8_t> IDs,
    uint16_t ADDR,
    uint8_t SIZE,
    std::vector<int32_t>& DATA)
{
    std::vector<uint32_t> raw_values;
    if(!readGroupSyncRaw(IDs, ADDR, SIZE, raw_values))
        return false;

    std::vector<int32_t> values(raw_values.size(), 0);
    for(size_t idx = 0; idx < raw_values.size(); ++idx)
        values[idx] = static_cast<int32_t>(raw_values[idx]);
    DATA.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::getRegisterInfo(
    DXL_MODEL_FAMILY family,
    DXL_CONTROL_ITEM item,
    DXL_REGISTER_INFO& info) const
{
    const auto set_info = [&info](uint16_t address, uint8_t size, bool is_signed) -> bool
    {
        info.address = address;
        info.size = size;
        info.is_signed = is_signed;
        info.writable = true;
        return true;
    };

    const auto set_read_only = [&info](uint16_t address, uint8_t size, bool is_signed) -> bool
    {
        info.address = address;
        info.size = size;
        info.is_signed = is_signed;
        info.writable = false;
        return true;
    };

    if(family == DXL_MODEL_FAMILY::PX)
    {
        switch(item)
        {
            case DXL_CONTROL_ITEM::MODEL_NUMBER: return set_read_only(ADDR_PX_MODEL_NUMBER, SIZE_PX_MODEL_NUMBER, false);
            case DXL_CONTROL_ITEM::MODEL_INFORMATION: return set_read_only(ADDR_PX_MODEL_INFORMATION, SIZE_PX_MODEL_INFORMATION, false);
            case DXL_CONTROL_ITEM::FIRMWARE_VERSION: return set_read_only(ADDR_PX_FIRMWARE_VERSION, SIZE_PX_FIRMWARE_VERSION, false);
            case DXL_CONTROL_ITEM::ID: return set_info(ADDR_PX_ID, SIZE_PX_ID, false);
            case DXL_CONTROL_ITEM::BAUD_RATE: return set_info(ADDR_PX_BAUD_RATE, SIZE_PX_BAUD_RATE, false);
            case DXL_CONTROL_ITEM::RETURN_DELAY_TIME: return set_info(ADDR_PX_RETURN_DELAY_TIME, SIZE_PX_RETURN_DELAY_TIME, false);
            case DXL_CONTROL_ITEM::DRIVE_MODE: return set_info(ADDR_PX_DRIVE_MODE, SIZE_PX_DRIVE_MODE, false);
            case DXL_CONTROL_ITEM::OPERATING_MODE: return set_info(ADDR_PX_OPERATING_MODE, SIZE_PX_OPERATING_MODE, false);
            case DXL_CONTROL_ITEM::SECONDARY_ID: return set_info(ADDR_PX_SECONDARY_ID, SIZE_PX_SECONDARY_ID, false);
            case DXL_CONTROL_ITEM::PROTOCOL_TYPE: return set_info(ADDR_PX_PROTOCOL_TYPE, SIZE_PX_PROTOCOL_TYPE, false);
            case DXL_CONTROL_ITEM::HOMING_OFFSET: return set_info(ADDR_PX_HOMING_OFFSET, SIZE_PX_HOMING_OFFSET, true);
            case DXL_CONTROL_ITEM::MOVING_THRESHOLD: return set_info(ADDR_PX_MOVING_THRESHOLD, SIZE_PX_MOVING_THRESHOLD, false);
            case DXL_CONTROL_ITEM::TEMPERATURE_LIMIT: return set_info(ADDR_PX_TEMPERATURE_LIMIT, SIZE_PX_TEMPERATURE_LIMIT, false);
            case DXL_CONTROL_ITEM::MAX_VOLTAGE_LIMIT: return set_info(ADDR_PX_MAX_VOLTAGE_LIMIT, SIZE_PX_MAX_VOLTAGE_LIMIT, false);
            case DXL_CONTROL_ITEM::MIN_VOLTAGE_LIMIT: return set_info(ADDR_PX_MIN_VOLTAGE_LIMIT, SIZE_PX_MIN_VOLTAGE_LIMIT, false);
            case DXL_CONTROL_ITEM::PWM_LIMIT: return set_info(ADDR_PX_PWM_LIMIT, SIZE_PX_PWM_LIMIT, false);
            case DXL_CONTROL_ITEM::CURRENT_LIMIT: return set_info(ADDR_PX_CURRENT_LIMIT, SIZE_PX_CURRENT_LIMIT, false);
            case DXL_CONTROL_ITEM::ACCELERATION_LIMIT: return set_info(ADDR_PX_ACCELERATION_LIMIT, SIZE_PX_ACCELERATION_LIMIT, false);
            case DXL_CONTROL_ITEM::VELOCITY_LIMIT: return set_info(ADDR_PX_VELOCITY_LIMIT, SIZE_PX_VELOCITY_LIMIT, false);
            case DXL_CONTROL_ITEM::MAX_POSITION_LIMIT: return set_info(ADDR_PX_MAX_POSITION_LIMIT, SIZE_PX_MAX_POSITION_LIMIT, true);
            case DXL_CONTROL_ITEM::MIN_POSITION_LIMIT: return set_info(ADDR_PX_MIN_POSITION_LIMIT, SIZE_PX_MIN_POSITION_LIMIT, true);
            case DXL_CONTROL_ITEM::STARTUP_CONFIGURATION: return set_info(ADDR_PX_STARTUP_CONFIGURATION, SIZE_PX_STARTUP_CONFIGURATION, false);
            case DXL_CONTROL_ITEM::SHUTDOWN: return set_info(ADDR_PX_SHUTDOWN, SIZE_PX_SHUTDOWN, false);
            case DXL_CONTROL_ITEM::TORQUE_ENABLE: return set_info(ADDR_PX_TORQUE_ENABLE, SIZE_PX_TORQUE_ENABLE, false);
            case DXL_CONTROL_ITEM::LED_RED: return set_info(ADDR_PX_LED_RED, SIZE_PX_LED_RED, false);
            case DXL_CONTROL_ITEM::LED_GREEN: return set_info(ADDR_PX_LED_GREEN, SIZE_PX_LED_GREEN, false);
            case DXL_CONTROL_ITEM::LED_BLUE: return set_info(ADDR_PX_LED_BLUE, SIZE_PX_LED_BLUE, false);
            case DXL_CONTROL_ITEM::STATUS_RETURN_LEVEL: return set_info(ADDR_PX_STATUS_RETURN_LEVEL, SIZE_PX_STATUS_RETURN_LEVEL, false);
            case DXL_CONTROL_ITEM::REGISTERED_INSTRUCTION: return set_read_only(ADDR_PX_REGISTERED_INSTRUCTION, SIZE_PX_REGISTERED_INSTRUCTION, false);
            case DXL_CONTROL_ITEM::HARDWARE_ERROR_STATUS: return set_read_only(ADDR_PX_HARDWARE_ERROR_STATUS, SIZE_PX_HARDWARE_ERROR_STATUS, false);
            case DXL_CONTROL_ITEM::VELOCITY_I_GAIN: return set_info(ADDR_PX_VELOCITY_I_GAIN, SIZE_PX_VELOCITY_I_GAIN, false);
            case DXL_CONTROL_ITEM::VELOCITY_P_GAIN: return set_info(ADDR_PX_VELOCITY_P_GAIN, SIZE_PX_VELOCITY_P_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_D_GAIN: return set_info(ADDR_PX_POSITION_D_GAIN, SIZE_PX_POSITION_D_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_I_GAIN: return set_info(ADDR_PX_POSITION_I_GAIN, SIZE_PX_POSITION_I_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_P_GAIN: return set_info(ADDR_PX_POSITION_P_GAIN, SIZE_PX_POSITION_P_GAIN, false);
            case DXL_CONTROL_ITEM::FEEDFORWARD_2ND_GAIN: return set_info(ADDR_PX_FEEDFORWARD_2ND_GAIN, SIZE_PX_FEEDFORWARD_2ND_GAIN, false);
            case DXL_CONTROL_ITEM::FEEDFORWARD_1ST_GAIN: return set_info(ADDR_PX_FEEDFORWARD_1ST_GAIN, SIZE_PX_FEEDFORWARD_1ST_GAIN, false);
            case DXL_CONTROL_ITEM::BUS_WATCHDOG: return set_info(ADDR_PX_BUS_WATCHDOG, SIZE_PX_BUS_WATCHDOG, true);
            case DXL_CONTROL_ITEM::GOAL_PWM: return set_info(ADDR_PX_GOAL_PWM, SIZE_PX_GOAL_PWM, true);
            case DXL_CONTROL_ITEM::GOAL_CURRENT: return set_info(ADDR_PX_GOAL_CURRENT, SIZE_PX_GOAL_CURRENT, true);
            case DXL_CONTROL_ITEM::GOAL_VELOCITY: return set_info(ADDR_PX_GOAL_VELOCITY, SIZE_PX_GOAL_VELOCITY, true);
            case DXL_CONTROL_ITEM::PROFILE_ACCELERATION: return set_info(ADDR_PX_PROFILE_ACCELERATION, SIZE_PX_PROFILE_ACCELERATION, false);
            case DXL_CONTROL_ITEM::PROFILE_VELOCITY: return set_info(ADDR_PX_PROFILE_VELOCITY, SIZE_PX_PROFILE_VELOCITY, false);
            case DXL_CONTROL_ITEM::GOAL_POSITION: return set_info(ADDR_PX_GOAL_POSITION, SIZE_PX_GOAL_POSITION, true);
            case DXL_CONTROL_ITEM::REALTIME_TICK: return set_read_only(ADDR_PX_REALTIME_TICK, SIZE_PX_REALTIME_TICK, false);
            case DXL_CONTROL_ITEM::MOVING: return set_read_only(ADDR_PX_MOVING, SIZE_PX_MOVING, false);
            case DXL_CONTROL_ITEM::MOVING_STATUS: return set_read_only(ADDR_PX_MOVING_STATUS, SIZE_PX_MOVING_STATUS, false);
            case DXL_CONTROL_ITEM::PRESENT_PWM: return set_read_only(ADDR_PX_PRESENT_PWM, SIZE_PX_PRESENT_PWM, true);
            case DXL_CONTROL_ITEM::PRESENT_CURRENT: return set_read_only(ADDR_PX_PRESENT_CURRENT, SIZE_PX_PRESENT_CURRENT, true);
            case DXL_CONTROL_ITEM::PRESENT_VELOCITY: return set_read_only(ADDR_PX_PRESENT_VELOCITY, SIZE_PX_PRESENT_VELOCITY, true);
            case DXL_CONTROL_ITEM::PRESENT_POSITION: return set_read_only(ADDR_PX_PRESENT_POSITION, SIZE_PX_PRESENT_POSITION, true);
            case DXL_CONTROL_ITEM::VELOCITY_TRAJECTORY: return set_read_only(ADDR_PX_VELOCITY_TRAJECTORY, SIZE_PX_VELOCITY_TRAJECTORY, true);
            case DXL_CONTROL_ITEM::POSITION_TRAJECTORY: return set_read_only(ADDR_PX_POSITION_TRAJECTORY, SIZE_PX_POSITION_TRAJECTORY, true);
            case DXL_CONTROL_ITEM::PRESENT_INPUT_VOLTAGE: return set_read_only(ADDR_PX_PRESENT_INPUT_VOLTAGE, SIZE_PX_PRESENT_INPUT_VOLTAGE, false);
            case DXL_CONTROL_ITEM::PRESENT_TEMPERATURE: return set_read_only(ADDR_PX_PRESENT_TEMPERATURE, SIZE_PX_PRESENT_TEMPERATURE, false);
            case DXL_CONTROL_ITEM::BACKUP_READY: return set_read_only(ADDR_PX_BACKUP_READY, SIZE_PX_BACKUP_READY, false);
            default: return false;
        }
    }

    if(family == DXL_MODEL_FAMILY::HAX)
    {
        switch(item)
        {
            case DXL_CONTROL_ITEM::MODEL_NUMBER: return set_read_only(ADDR_HAX_MODEL_NUMBER, SIZE_HAX_MODEL_NUMBER, false);
            case DXL_CONTROL_ITEM::MODEL_INFORMATION: return set_read_only(ADDR_HAX_MODEL_INFORMATION, SIZE_HAX_MODEL_INFORMATION, false);
            case DXL_CONTROL_ITEM::FIRMWARE_VERSION: return set_read_only(ADDR_HAX_FIRMWARE_VERSION, SIZE_HAX_FIRMWARE_VERSION, false);
            case DXL_CONTROL_ITEM::ID: return set_info(ADDR_HAX_ID, SIZE_HAX_ID, false);
            case DXL_CONTROL_ITEM::BAUD_RATE: return set_info(ADDR_HAX_BAUD_RATE, SIZE_HAX_BAUD_RATE, false);
            case DXL_CONTROL_ITEM::RETURN_DELAY_TIME: return set_info(ADDR_HAX_RETURN_DELAY_TIME, SIZE_HAX_RETURN_DELAY_TIME, false);
            case DXL_CONTROL_ITEM::DRIVE_MODE: return set_info(ADDR_HAX_DRIVE_MODE, SIZE_HAX_DRIVE_MODE, false);
            case DXL_CONTROL_ITEM::OPERATING_MODE: return set_info(ADDR_HAX_OPERATING_MODE, SIZE_HAX_OPERATING_MODE, false);
            case DXL_CONTROL_ITEM::SECONDARY_ID: return set_info(ADDR_HAX_SECONDARY_ID, SIZE_HAX_SECONDARY_ID, false);
            case DXL_CONTROL_ITEM::HOMING_OFFSET: return set_info(ADDR_HAX_HOMING_OFFSET, SIZE_HAX_HOMING_OFFSET, true);
            case DXL_CONTROL_ITEM::MOVING_THRESHOLD: return set_info(ADDR_HAX_MOVING_THRESHOLD, SIZE_HAX_MOVING_THRESHOLD, false);
            case DXL_CONTROL_ITEM::TEMPERATURE_LIMIT: return set_info(ADDR_HAX_TEMPERATURE_LIMIT, SIZE_HAX_TEMPERATURE_LIMIT, false);
            case DXL_CONTROL_ITEM::MAX_VOLTAGE_LIMIT: return set_info(ADDR_HAX_MAX_VOLTAGE_LIMIT, SIZE_HAX_MAX_VOLTAGE_LIMIT, false);
            case DXL_CONTROL_ITEM::MIN_VOLTAGE_LIMIT: return set_info(ADDR_HAX_MIN_VOLTAGE_LIMIT, SIZE_HAX_MIN_VOLTAGE_LIMIT, false);
            case DXL_CONTROL_ITEM::PWM_LIMIT: return set_info(ADDR_HAX_PWM_LIMIT, SIZE_HAX_PWM_LIMIT, false);
            case DXL_CONTROL_ITEM::CURRENT_LIMIT: return set_info(ADDR_HAX_CURRENT_LIMIT, SIZE_HAX_CURRENT_LIMIT, false);
            case DXL_CONTROL_ITEM::ACCELERATION_LIMIT: return set_info(ADDR_HAX_ACCELERATION_LIMIT, SIZE_HAX_ACCELERATION_LIMIT, false);
            case DXL_CONTROL_ITEM::VELOCITY_LIMIT: return set_info(ADDR_HAX_VELOCITY_LIMIT, SIZE_HAX_VELOCITY_LIMIT, false);
            case DXL_CONTROL_ITEM::MAX_POSITION_LIMIT: return set_info(ADDR_HAX_MAX_POSITION_LIMIT, SIZE_HAX_MAX_POSITION_LIMIT, true);
            case DXL_CONTROL_ITEM::MIN_POSITION_LIMIT: return set_info(ADDR_HAX_MIN_POSITION_LIMIT, SIZE_HAX_MIN_POSITION_LIMIT, true);
            case DXL_CONTROL_ITEM::SHUTDOWN: return set_info(ADDR_HAX_SHUTDOWN, SIZE_HAX_SHUTDOWN, false);
            case DXL_CONTROL_ITEM::TORQUE_ENABLE: return set_info(ADDR_HAX_TORQUE_ENABLE, SIZE_HAX_TORQUE_ENABLE, false);
            case DXL_CONTROL_ITEM::LED_RED: return set_info(ADDR_HAX_LED_RED, SIZE_HAX_LED_RED, false);
            case DXL_CONTROL_ITEM::LED_GREEN: return set_info(ADDR_HAX_LED_GREEN, SIZE_HAX_LED_GREEN, false);
            case DXL_CONTROL_ITEM::LED_BLUE: return set_info(ADDR_HAX_LED_BLUE, SIZE_HAX_LED_BLUE, false);
            case DXL_CONTROL_ITEM::STATUS_RETURN_LEVEL: return set_info(ADDR_HAX_STATUS_RETURN_LEVEL, SIZE_HAX_STATUS_RETURN_LEVEL, false);
            case DXL_CONTROL_ITEM::REGISTERED_INSTRUCTION: return set_read_only(ADDR_HAX_REGISTERED_INSTRUCTION, SIZE_HAX_REGISTERED_INSTRUCTION, false);
            case DXL_CONTROL_ITEM::HARDWARE_ERROR_STATUS: return set_read_only(ADDR_HAX_HARDWARE_ERROR_STATUS, SIZE_HAX_HARDWARE_ERROR_STATUS, false);
            case DXL_CONTROL_ITEM::VELOCITY_I_GAIN: return set_info(ADDR_HAX_VELOCITY_I_GAIN, SIZE_HAX_VELOCITY_I_GAIN, false);
            case DXL_CONTROL_ITEM::VELOCITY_P_GAIN: return set_info(ADDR_HAX_VELOCITY_P_GAIN, SIZE_HAX_VELOCITY_P_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_D_GAIN: return set_info(ADDR_HAX_POSITION_D_GAIN, SIZE_HAX_POSITION_D_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_I_GAIN: return set_info(ADDR_HAX_POSITION_I_GAIN, SIZE_HAX_POSITION_I_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_P_GAIN: return set_info(ADDR_HAX_POSITION_P_GAIN, SIZE_HAX_POSITION_P_GAIN, false);
            case DXL_CONTROL_ITEM::FEEDFORWARD_2ND_GAIN: return set_info(ADDR_HAX_FEEDFORWARD_2ND_GAIN, SIZE_HAX_FEEDFORWARD_2ND_GAIN, false);
            case DXL_CONTROL_ITEM::FEEDFORWARD_1ST_GAIN: return set_info(ADDR_HAX_FEEDFORWARD_1ST_GAIN, SIZE_HAX_FEEDFORWARD_1ST_GAIN, false);
            case DXL_CONTROL_ITEM::BUS_WATCHDOG: return set_info(ADDR_HAX_BUS_WATCHDOG, SIZE_HAX_BUS_WATCHDOG, true);
            case DXL_CONTROL_ITEM::GOAL_PWM: return set_info(ADDR_HAX_GOAL_PWM, SIZE_HAX_GOAL_PWM, true);
            case DXL_CONTROL_ITEM::GOAL_CURRENT: return set_info(ADDR_HAX_GOAL_CURRENT, SIZE_HAX_GOAL_CURRENT, true);
            case DXL_CONTROL_ITEM::GOAL_VELOCITY: return set_info(ADDR_HAX_GOAL_VELOCITY, SIZE_HAX_GOAL_VELOCITY, true);
            case DXL_CONTROL_ITEM::PROFILE_ACCELERATION: return set_info(ADDR_HAX_PROFILE_ACCELERATION, SIZE_HAX_PROFILE_ACCELERATION, false);
            case DXL_CONTROL_ITEM::PROFILE_VELOCITY: return set_info(ADDR_HAX_PROFILE_VELOCITY, SIZE_HAX_PROFILE_VELOCITY, false);
            case DXL_CONTROL_ITEM::GOAL_POSITION: return set_info(ADDR_HAX_GOAL_POSITION, SIZE_HAX_GOAL_POSITION, true);
            case DXL_CONTROL_ITEM::REALTIME_TICK: return set_read_only(ADDR_HAX_REALTIME_TICK, SIZE_HAX_REALTIME_TICK, false);
            case DXL_CONTROL_ITEM::MOVING: return set_read_only(ADDR_HAX_MOVING, SIZE_HAX_MOVING, false);
            case DXL_CONTROL_ITEM::MOVING_STATUS: return set_read_only(ADDR_HAX_MOVING_STATUS, SIZE_HAX_MOVING_STATUS, false);
            case DXL_CONTROL_ITEM::PRESENT_PWM: return set_read_only(ADDR_HAX_PRESENT_PWM, SIZE_HAX_PRESENT_PWM, true);
            case DXL_CONTROL_ITEM::PRESENT_CURRENT: return set_read_only(ADDR_HAX_PRESENT_CURRENT, SIZE_HAX_PRESENT_CURRENT, true);
            case DXL_CONTROL_ITEM::PRESENT_VELOCITY: return set_read_only(ADDR_HAX_PRESENT_VELOCITY, SIZE_HAX_PRESENT_VELOCITY, true);
            case DXL_CONTROL_ITEM::PRESENT_POSITION: return set_read_only(ADDR_HAX_PRESENT_POSITION, SIZE_HAX_PRESENT_POSITION, true);
            case DXL_CONTROL_ITEM::VELOCITY_TRAJECTORY: return set_read_only(ADDR_HAX_VELOCITY_TRAJECTORY, SIZE_HAX_VELOCITY_TRAJECTORY, true);
            case DXL_CONTROL_ITEM::POSITION_TRAJECTORY: return set_read_only(ADDR_HAX_POSITION_TRAJECTORY, SIZE_HAX_POSITION_TRAJECTORY, true);
            case DXL_CONTROL_ITEM::PRESENT_INPUT_VOLTAGE: return set_read_only(ADDR_HAX_PRESENT_INPUT_VOLTAGE, SIZE_HAX_PRESENT_INPUT_VOLTAGE, false);
            case DXL_CONTROL_ITEM::PRESENT_TEMPERATURE: return set_read_only(ADDR_HAX_PRESENT_TEMPERATURE, SIZE_HAX_PRESENT_TEMPERATURE, false);
            default: return false;
        }
    }

    if(family == DXL_MODEL_FAMILY::XX)
    {
        switch(item)
        {
            case DXL_CONTROL_ITEM::MODEL_NUMBER: return set_read_only(ADDR_XX_MODEL_NUMBER, SIZE_XX_MODEL_NUMBER, false);
            case DXL_CONTROL_ITEM::MODEL_INFORMATION: return set_read_only(ADDR_XX_MODEL_INFORMATION, SIZE_XX_MODEL_INFORMATION, false);
            case DXL_CONTROL_ITEM::FIRMWARE_VERSION: return set_read_only(ADDR_XX_FIRMWARE_VERSION, SIZE_XX_FIRMWARE_VERSION, false);
            case DXL_CONTROL_ITEM::ID: return set_info(ADDR_XX_ID, SIZE_XX_ID, false);
            case DXL_CONTROL_ITEM::BAUD_RATE: return set_info(ADDR_XX_BAUD_RATE, SIZE_XX_BAUD_RATE, false);
            case DXL_CONTROL_ITEM::RETURN_DELAY_TIME: return set_info(ADDR_XX_RETURN_DELAY_TIME, SIZE_XX_RETURN_DELAY_TIME, false);
            case DXL_CONTROL_ITEM::DRIVE_MODE: return set_info(ADDR_XX_DRIVE_MODE, SIZE_XX_DRIVE_MODE, false);
            case DXL_CONTROL_ITEM::OPERATING_MODE: return set_info(ADDR_XX_OPERATING_MODE, SIZE_XX_OPERATING_MODE, false);
            case DXL_CONTROL_ITEM::SECONDARY_ID: return set_info(ADDR_XX_SECONDARY_ID, SIZE_XX_SECONDARY_ID, false);
            case DXL_CONTROL_ITEM::PROTOCOL_TYPE: return set_info(ADDR_XX_PROTOCOL_TYPE, SIZE_XX_PROTOCOL_TYPE, false);
            case DXL_CONTROL_ITEM::HOMING_OFFSET: return set_info(ADDR_XX_HOMING_OFFSET, SIZE_XX_HOMING_OFFSET, true);
            case DXL_CONTROL_ITEM::MOVING_THRESHOLD: return set_info(ADDR_XX_MOVING_THRESHOLD, SIZE_XX_MOVING_THRESHOLD, false);
            case DXL_CONTROL_ITEM::TEMPERATURE_LIMIT: return set_info(ADDR_XX_TEMPERATURE_LIMIT, SIZE_XX_TEMPERATURE_LIMIT, false);
            case DXL_CONTROL_ITEM::MAX_VOLTAGE_LIMIT: return set_info(ADDR_XX_MAX_VOLTAGE_LIMIT, SIZE_XX_MAX_VOLTAGE_LIMIT, false);
            case DXL_CONTROL_ITEM::MIN_VOLTAGE_LIMIT: return set_info(ADDR_XX_MIN_VOLTAGE_LIMIT, SIZE_XX_MIN_VOLTAGE_LIMIT, false);
            case DXL_CONTROL_ITEM::PWM_LIMIT: return set_info(ADDR_XX_PWM_LIMIT, SIZE_XX_PWM_LIMIT, false);
            case DXL_CONTROL_ITEM::CURRENT_LIMIT: return set_info(ADDR_XX_CURRENT_LIMIT, SIZE_XX_CURRENT_LIMIT, false);
            case DXL_CONTROL_ITEM::ACCELERATION_LIMIT: return set_info(ADDR_XX_ACCELERATION_LIMIT, SIZE_XX_ACCELERATION_LIMIT, false);
            case DXL_CONTROL_ITEM::VELOCITY_LIMIT: return set_info(ADDR_XX_VELOCITY_LIMIT, SIZE_XX_VELOCITY_LIMIT, false);
            case DXL_CONTROL_ITEM::MAX_POSITION_LIMIT: return set_info(ADDR_XX_MAX_POSITION_LIMIT, SIZE_XX_MAX_POSITION_LIMIT, true);
            case DXL_CONTROL_ITEM::MIN_POSITION_LIMIT: return set_info(ADDR_XX_MIN_POSITION_LIMIT, SIZE_XX_MIN_POSITION_LIMIT, true);
            case DXL_CONTROL_ITEM::STARTUP_CONFIGURATION: return set_info(ADDR_XX_STARTUP_CONFIGURATION, SIZE_XX_STARTUP_CONFIGURATION, false);
            case DXL_CONTROL_ITEM::SHUTDOWN: return set_info(ADDR_XX_SHUTDOWN, SIZE_XX_SHUTDOWN, false);
            case DXL_CONTROL_ITEM::TORQUE_ENABLE: return set_info(ADDR_XX_TORQUE_ENABLE, SIZE_XX_TORQUE_ENABLE, false);
            case DXL_CONTROL_ITEM::LED: return set_info(ADDR_XX_LED, SIZE_XX_LED, false);
            case DXL_CONTROL_ITEM::STATUS_RETURN_LEVEL: return set_info(ADDR_XX_STATUS_RETURN_LEVEL, SIZE_XX_STATUS_RETURN_LEVEL, false);
            case DXL_CONTROL_ITEM::REGISTERED_INSTRUCTION: return set_read_only(ADDR_XX_REGISTERED_INSTRUCTION, SIZE_XX_REGISTERED_INSTRUCTION, false);
            case DXL_CONTROL_ITEM::HARDWARE_ERROR_STATUS: return set_read_only(ADDR_XX_HARDWARE_ERROR_STATUS, SIZE_XX_HARDWARE_ERROR_STATUS, false);
            case DXL_CONTROL_ITEM::VELOCITY_I_GAIN: return set_info(ADDR_XX_VELOCITY_I_GAIN, SIZE_XX_VELOCITY_I_GAIN, false);
            case DXL_CONTROL_ITEM::VELOCITY_P_GAIN: return set_info(ADDR_XX_VELOCITY_P_GAIN, SIZE_XX_VELOCITY_P_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_D_GAIN: return set_info(ADDR_XX_POSITION_D_GAIN, SIZE_XX_POSITION_D_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_I_GAIN: return set_info(ADDR_XX_POSITION_I_GAIN, SIZE_XX_POSITION_I_GAIN, false);
            case DXL_CONTROL_ITEM::POSITION_P_GAIN: return set_info(ADDR_XX_POSITION_P_GAIN, SIZE_XX_POSITION_P_GAIN, false);
            case DXL_CONTROL_ITEM::FEEDFORWARD_2ND_GAIN: return set_info(ADDR_XX_FEEDFORWARD_2ND_GAIN, SIZE_XX_FEEDFORWARD_2ND_GAIN, false);
            case DXL_CONTROL_ITEM::FEEDFORWARD_1ST_GAIN: return set_info(ADDR_XX_FEEDFORWARD_1ST_GAIN, SIZE_XX_FEEDFORWARD_1ST_GAIN, false);
            case DXL_CONTROL_ITEM::BUS_WATCHDOG: return set_info(ADDR_XX_BUS_WATCHDOG, SIZE_XX_BUS_WATCHDOG, true);
            case DXL_CONTROL_ITEM::GOAL_PWM: return set_info(ADDR_XX_GOAL_PWM, SIZE_XX_GOAL_PWM, true);
            case DXL_CONTROL_ITEM::GOAL_CURRENT: return set_info(ADDR_XX_GOAL_CURRENT, SIZE_XX_GOAL_CURRENT, true);
            case DXL_CONTROL_ITEM::GOAL_VELOCITY: return set_info(ADDR_XX_GOAL_VELOCITY, SIZE_XX_GOAL_VELOCITY, true);
            case DXL_CONTROL_ITEM::PROFILE_ACCELERATION: return set_info(ADDR_XX_PROFILE_ACCELERATION, SIZE_XX_PROFILE_ACCELERATION, false);
            case DXL_CONTROL_ITEM::PROFILE_VELOCITY: return set_info(ADDR_XX_PROFILE_VELOCITY, SIZE_XX_PROFILE_VELOCITY, false);
            case DXL_CONTROL_ITEM::GOAL_POSITION: return set_info(ADDR_XX_GOAL_POSITION, SIZE_XX_GOAL_POSITION, true);
            case DXL_CONTROL_ITEM::REALTIME_TICK: return set_read_only(ADDR_XX_REALTIME_TICK, SIZE_XX_REALTIME_TICK, false);
            case DXL_CONTROL_ITEM::MOVING: return set_read_only(ADDR_XX_MOVING, SIZE_XX_MOVING, false);
            case DXL_CONTROL_ITEM::MOVING_STATUS: return set_read_only(ADDR_XX_MOVING_STATUS, SIZE_XX_MOVING_STATUS, false);
            case DXL_CONTROL_ITEM::PRESENT_PWM: return set_read_only(ADDR_XX_PRESENT_PWM, SIZE_XX_PRESENT_PWM, true);
            case DXL_CONTROL_ITEM::PRESENT_CURRENT: return set_read_only(ADDR_XX_PRESENT_CURRENT, SIZE_XX_PRESENT_CURRENT, true);
            case DXL_CONTROL_ITEM::PRESENT_VELOCITY: return set_read_only(ADDR_XX_PRESENT_VELOCITY, SIZE_XX_PRESENT_VELOCITY, true);
            case DXL_CONTROL_ITEM::PRESENT_POSITION: return set_read_only(ADDR_XX_PRESENT_POSITION, SIZE_XX_PRESENT_POSITION, true);
            case DXL_CONTROL_ITEM::VELOCITY_TRAJECTORY: return set_read_only(ADDR_XX_VELOCITY_TRAJECTORY, SIZE_XX_VELOCITY_TRAJECTORY, true);
            case DXL_CONTROL_ITEM::POSITION_TRAJECTORY: return set_read_only(ADDR_XX_POSITION_TRAJECTORY, SIZE_XX_POSITION_TRAJECTORY, true);
            case DXL_CONTROL_ITEM::PRESENT_INPUT_VOLTAGE: return set_read_only(ADDR_XX_PRESENT_INPUT_VOLTAGE, SIZE_XX_PRESENT_INPUT_VOLTAGE, false);
            case DXL_CONTROL_ITEM::PRESENT_TEMPERATURE: return set_read_only(ADDR_XX_PRESENT_TEMPERATURE, SIZE_XX_PRESENT_TEMPERATURE, false);
            case DXL_CONTROL_ITEM::BACKUP_READY: return set_read_only(ADDR_XX_BACKUP_READY, SIZE_XX_BACKUP_READY, false);
            default: return false;
        }
    }

    switch(item)
    {
        case DXL_CONTROL_ITEM::MODEL_NUMBER: return set_read_only(ADDR_HX_MODEL_NUMBER, SIZE_HX_MODEL_NUMBER, false);
        case DXL_CONTROL_ITEM::MODEL_INFORMATION: return set_read_only(ADDR_HX_MODEL_INFORMATION, SIZE_HX_MODEL_INFORMATION, false);
        case DXL_CONTROL_ITEM::FIRMWARE_VERSION: return set_read_only(ADDR_HX_FIRMWARE_VERSION, SIZE_HX_FIRMWARE_VERSION, false);
        case DXL_CONTROL_ITEM::ID: return set_info(ADDR_HX_ID, SIZE_HX_ID, false);
        case DXL_CONTROL_ITEM::BAUD_RATE: return set_info(ADDR_HX_BAUD_RATE, SIZE_HX_BAUD_RATE, false);
        case DXL_CONTROL_ITEM::RETURN_DELAY_TIME: return set_info(ADDR_HX_RETURN_DELAY_TIME, SIZE_HX_RETURN_DELAY_TIME, false);
        case DXL_CONTROL_ITEM::OPERATING_MODE: return set_info(ADDR_HX_OPERATING_MODE, SIZE_HX_OPERATING_MODE, false);
        case DXL_CONTROL_ITEM::HOMING_OFFSET: return set_info(ADDR_HX_HOMING_OFFSET, SIZE_HX_HOMING_OFFSET, true);
        case DXL_CONTROL_ITEM::MOVING_THRESHOLD: return set_info(ADDR_HX_MOVING_THRESHOLD, SIZE_HX_MOVING_THRESHOLD, false);
        case DXL_CONTROL_ITEM::TEMPERATURE_LIMIT: return set_info(ADDR_HX_TEMPERATURE_LIMIT, SIZE_HX_TEMPERATURE_LIMIT, false);
        case DXL_CONTROL_ITEM::MAX_VOLTAGE_LIMIT: return set_info(ADDR_HX_MAX_VOLTAGE_LIMIT, SIZE_HX_MAX_VOLTAGE_LIMIT, false);
        case DXL_CONTROL_ITEM::MIN_VOLTAGE_LIMIT: return set_info(ADDR_HX_MIN_VOLTAGE_LIMIT, SIZE_HX_MIN_VOLTAGE_LIMIT, false);
        case DXL_CONTROL_ITEM::CURRENT_LIMIT: return set_info(ADDR_HX_TORQUE_LIMIT, SIZE_HX_TORQUE_LIMIT, false);
        case DXL_CONTROL_ITEM::TORQUE_LIMIT: return set_info(ADDR_HX_TORQUE_LIMIT, SIZE_HX_TORQUE_LIMIT, false);
        case DXL_CONTROL_ITEM::ACCELERATION_LIMIT: return set_info(ADDR_HX_ACCELERATION_LIMIT, SIZE_HX_ACCELERATION_LIMIT, false);
        case DXL_CONTROL_ITEM::VELOCITY_LIMIT: return set_info(ADDR_HX_VELOCITY_LIMIT, SIZE_HX_VELOCITY_LIMIT, false);
        case DXL_CONTROL_ITEM::MAX_POSITION_LIMIT: return set_info(ADDR_HX_MAX_POSITION_LIMIT, SIZE_HX_MAX_POSITION_LIMIT, true);
        case DXL_CONTROL_ITEM::MIN_POSITION_LIMIT: return set_info(ADDR_HX_MIN_POSITION_LIMIT, SIZE_HX_MIN_POSITION_LIMIT, true);
        case DXL_CONTROL_ITEM::SHUTDOWN: return set_info(ADDR_HX_SHUTDOWN, SIZE_HX_SHUTDOWN, false);
        case DXL_CONTROL_ITEM::TORQUE_ENABLE: return set_info(ADDR_HX_TORQUE_ENABLE, SIZE_HX_TORQUE_ENABLE, false);
        case DXL_CONTROL_ITEM::LED_RED: return set_info(ADDR_HX_LED_RED, SIZE_HX_LED_RED, false);
        case DXL_CONTROL_ITEM::LED_GREEN: return set_info(ADDR_HX_LED_GREEN, SIZE_HX_LED_GREEN, false);
        case DXL_CONTROL_ITEM::LED_BLUE: return set_info(ADDR_HX_LED_BLUE, SIZE_HX_LED_BLUE, false);
        case DXL_CONTROL_ITEM::STATUS_RETURN_LEVEL: return set_info(ADDR_HX_STATUS_RETURN_LEVEL, SIZE_HX_STATUS_RETURN_LEVEL, false);
        case DXL_CONTROL_ITEM::REGISTERED_INSTRUCTION: return set_read_only(ADDR_HX_REGISTERED_INSTRUCTION, SIZE_HX_REGISTERED_INSTRUCTION, false);
        case DXL_CONTROL_ITEM::HARDWARE_ERROR_STATUS: return set_read_only(ADDR_HX_HARDWARE_ERROR_STATUS, SIZE_HX_HARDWARE_ERROR_STATUS, false);
        case DXL_CONTROL_ITEM::VELOCITY_I_GAIN: return set_info(ADDR_HX_VELOCITY_I_GAIN, SIZE_HX_VELOCITY_I_GAIN, false);
        case DXL_CONTROL_ITEM::VELOCITY_P_GAIN: return set_info(ADDR_HX_VELOCITY_P_GAIN, SIZE_HX_VELOCITY_P_GAIN, false);
        case DXL_CONTROL_ITEM::POSITION_P_GAIN: return set_info(ADDR_HX_POSITION_P_GAIN, SIZE_HX_POSITION_P_GAIN, false);
        case DXL_CONTROL_ITEM::GOAL_CURRENT: return set_info(ADDR_HX_GOAL_TORQUE, SIZE_HX_GOAL_TORQUE, true);
        case DXL_CONTROL_ITEM::GOAL_TORQUE: return set_info(ADDR_HX_GOAL_TORQUE, SIZE_HX_GOAL_TORQUE, true);
        case DXL_CONTROL_ITEM::GOAL_VELOCITY: return set_info(ADDR_HX_GOAL_VELOCITY, SIZE_HX_GOAL_VELOCITY, true);
        case DXL_CONTROL_ITEM::GOAL_ACCELERATION: return set_info(ADDR_HX_GOAL_ACCELERATION, SIZE_HX_GOAL_ACCELERATION, false);
        case DXL_CONTROL_ITEM::PROFILE_ACCELERATION: return set_info(ADDR_HX_GOAL_ACCELERATION, SIZE_HX_GOAL_ACCELERATION, false);
        case DXL_CONTROL_ITEM::PROFILE_VELOCITY: return set_info(ADDR_HX_GOAL_VELOCITY, SIZE_HX_GOAL_VELOCITY, false);
        case DXL_CONTROL_ITEM::GOAL_POSITION: return set_info(ADDR_HX_GOAL_POSITION, SIZE_HX_GOAL_POSITION, true);
        case DXL_CONTROL_ITEM::MOVING: return set_read_only(ADDR_HX_MOVING, SIZE_HX_MOVING, false);
        case DXL_CONTROL_ITEM::PRESENT_CURRENT: return set_read_only(ADDR_HX_PRESENT_CURRENT, SIZE_HX_PRESENT_CURRENT, true);
        case DXL_CONTROL_ITEM::PRESENT_VELOCITY: return set_read_only(ADDR_HX_PRESENT_VELOCITY, SIZE_HX_PRESENT_VELOCITY, true);
        case DXL_CONTROL_ITEM::PRESENT_POSITION: return set_read_only(ADDR_HX_PRESENT_POSITION, SIZE_HX_PRESENT_POSITION, true);
        case DXL_CONTROL_ITEM::PRESENT_INPUT_VOLTAGE: return set_read_only(ADDR_HX_PRESENT_INPUT_VOLTAGE, SIZE_HX_PRESENT_INPUT_VOLTAGE, false);
        case DXL_CONTROL_ITEM::PRESENT_TEMPERATURE: return set_read_only(ADDR_HX_PRESENT_TEMPERATURE, SIZE_HX_PRESENT_TEMPERATURE, false);
        default: return false;
    }
}

bool DYNAMIXEL_SDK_INTERFACE::supportsControlItem(uint8_t ID, DXL_CONTROL_ITEM item) const
{
    DXL_REGISTER_INFO info;
    return getRegisterInfo(getModelFamily(ID), item, info);
}

bool DYNAMIXEL_SDK_INTERFACE::isControlItemWritable(uint8_t ID, DXL_CONTROL_ITEM item) const
{
    DXL_REGISTER_INFO info;
    return getRegisterInfo(getModelFamily(ID), item, info) && info.writable;
}

bool DYNAMIXEL_SDK_INTERFACE::supportsOperatingMode(uint8_t ID, DXL_OPERATING_MODE mode) const
{
    const DXL_MODEL_FAMILY family = getModelFamily(ID);
    if(family == DXL_MODEL_FAMILY::HX)
    {
        return mode == DXL_OPERATING_MODE::CURRENT_CONTROL_MODE
            || mode == DXL_OPERATING_MODE::VELOCITY_CONTROL_MODE
            || mode == DXL_OPERATING_MODE::POSITION_CONTROL_MODE
            || mode == DXL_OPERATING_MODE::EXTENDED_POSITION_MODE;
    }

    if(family == DXL_MODEL_FAMILY::PX || family == DXL_MODEL_FAMILY::HAX)
    {
        return mode == DXL_OPERATING_MODE::CURRENT_CONTROL_MODE
            || mode == DXL_OPERATING_MODE::VELOCITY_CONTROL_MODE
            || mode == DXL_OPERATING_MODE::POSITION_CONTROL_MODE
            || mode == DXL_OPERATING_MODE::EXTENDED_POSITION_MODE
            || mode == DXL_OPERATING_MODE::PWM_CONTROL_MODE;
    }

    return true;
}

uint32_t DYNAMIXEL_SDK_INTERFACE::encodeRegisterValue(
    int32_t value,
    const DXL_REGISTER_INFO& info)
{
    if(info.size == 1)
        return static_cast<uint8_t>(value);
    if(info.size == 2)
        return static_cast<uint16_t>(value);
    return static_cast<uint32_t>(value);
}

int32_t DYNAMIXEL_SDK_INTERFACE::decodeRegisterValue(
    uint32_t value,
    const DXL_REGISTER_INFO& info)
{
    if(info.size == 1)
        return info.is_signed ? static_cast<int8_t>(value) : static_cast<uint8_t>(value);
    if(info.size == 2)
        return info.is_signed ? static_cast<int16_t>(value) : static_cast<uint16_t>(value);
    return static_cast<int32_t>(value);
}

bool DYNAMIXEL_SDK_INTERFACE::isRegisterValueValid(
    int32_t value,
    const DXL_REGISTER_INFO& info)
{
    if(info.size == 1)
    {
        if(info.is_signed)
            return value >= std::numeric_limits<int8_t>::min()
                && value <= std::numeric_limits<int8_t>::max();
        return value >= 0 && value <= std::numeric_limits<uint8_t>::max();
    }

    if(info.size == 2)
    {
        if(info.is_signed)
            return value >= std::numeric_limits<int16_t>::min()
                && value <= std::numeric_limits<int16_t>::max();
        return value >= 0 && value <= std::numeric_limits<uint16_t>::max();
    }

    if(info.size == 4)
        return info.is_signed || value >= 0;

    return false;
}

bool DYNAMIXEL_SDK_INTERFACE::writeControlItem(
    uint8_t ID,
    DXL_CONTROL_ITEM item,
    int32_t DATA)
{
    DXL_REGISTER_INFO info;
    if(!getRegisterInfo(getModelFamily(ID), item, info))
    {
        setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "writeControlItem", ID, COMM_NOT_AVAILABLE);
        return false;
    }

    if(!info.writable)
    {
        setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "writeControlItem.readOnly", ID, COMM_NOT_AVAILABLE);
        return false;
    }

    if(!isRegisterValueValid(DATA, info))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItem.range", ID, COMM_TX_FAIL);
        return false;
    }

    const uint32_t encoded = encodeRegisterValue(DATA, info);
    if(info.size == 1)
        return write1ByteTxRx(ID, info.address, static_cast<uint8_t>(encoded));
    if(info.size == 2)
        return write2ByteTxRx(ID, info.address, static_cast<uint16_t>(encoded));
    if(info.size == 4)
        return write4ByteTxRx(ID, info.address, encoded);

    setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItem", ID, COMM_TX_FAIL);
    return false;
}

bool DYNAMIXEL_SDK_INTERFACE::readControlItem(
    uint8_t ID,
    DXL_CONTROL_ITEM item,
    int32_t& DATA)
{
    DXL_REGISTER_INFO info;
    if(!getRegisterInfo(getModelFamily(ID), item, info))
    {
        setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "readControlItem", ID, COMM_NOT_AVAILABLE);
        return false;
    }

    uint32_t raw_value = 0;
    if(info.size == 1)
    {
        uint8_t value = 0;
        if(!read1ByteTxRx(ID, info.address, &value))
            return false;
        raw_value = value;
    }
    else if(info.size == 2)
    {
        uint16_t value = 0;
        if(!read2ByteTxRx(ID, info.address, &value))
            return false;
        raw_value = value;
    }
    else if(info.size == 4)
    {
        if(!read4ByteTxRx(ID, info.address, &raw_value))
            return false;
    }
    else
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readControlItem", ID, COMM_TX_FAIL);
        return false;
    }

    DATA = decodeRegisterValue(raw_value, info);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeControlItemGroup(
    const std::vector<uint8_t>& IDs,
    DXL_CONTROL_ITEM item,
    const std::vector<int32_t>& DATA)
{
    if(IDs.size() != DATA.size())
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItems", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    if(IDs.empty())
    {
        setResult(DXL_INTERFACE_STATUS::SUCCESS, "writeControlItems", NO_DEVICE_ID, COMM_SUCCESS);
        return true;
    }

    std::vector<DXL_REGISTER_INFO> registers(IDs.size());
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!getRegisterInfo(getModelFamily(IDs[idx]), item, registers[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "writeControlItems", IDs[idx], COMM_NOT_AVAILABLE);
            return false;
        }

        if(!registers[idx].writable)
        {
            setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "writeControlItems.readOnly", IDs[idx], COMM_NOT_AVAILABLE);
            return false;
        }
        if(!isRegisterValueValid(DATA[idx], registers[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItems.range", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "writeControlItems", NO_DEVICE_ID))
        return false;

    dynamixel::GroupBulkWrite group_bulk_write(portHandler, packetHandler);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        const uint32_t encoded = encodeRegisterValue(DATA[idx], registers[idx]);
        uint8_t bytes[4] = {0, 0, 0, 0};
        for(uint8_t byte_idx = 0; byte_idx < registers[idx].size; ++byte_idx)
            bytes[byte_idx] = static_cast<uint8_t>((encoded >> (8 * byte_idx)) & 0xFF);

        if(!group_bulk_write.addParam(
            IDs[idx], registers[idx].address, registers[idx].size, bytes))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItems.addParam", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }

    const int communication_result = group_bulk_write.txPacket();
    group_bulk_write.clearParam();
    return finishTransaction("writeControlItems", NO_DEVICE_ID, communication_result);
}

bool DYNAMIXEL_SDK_INTERFACE::readControlItemGroup(
    const std::vector<uint8_t>& IDs,
    DXL_CONTROL_ITEM item,
    std::vector<int32_t>& DATA)
{
    if(IDs.empty())
    {
        DATA.clear();
        setResult(DXL_INTERFACE_STATUS::SUCCESS, "readControlItems", NO_DEVICE_ID, COMM_SUCCESS);
        return true;
    }

    std::vector<DXL_REGISTER_INFO> registers(IDs.size());
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!getRegisterInfo(getModelFamily(IDs[idx]), item, registers[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "readControlItems", IDs[idx], COMM_NOT_AVAILABLE);
            return false;
        }
    }

    std::unique_lock<std::timed_mutex> lock(port_mutex_, std::defer_lock);
    if(!tryLockPort(lock, "readControlItems", NO_DEVICE_ID))
        return false;

    dynamixel::GroupBulkRead group_bulk_read(portHandler, packetHandler);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!group_bulk_read.addParam(
            IDs[idx], registers[idx].address, registers[idx].size))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readControlItems.addParam", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }

    const int communication_result = group_bulk_read.txRxPacket();
    if(!finishTransaction("readControlItems", NO_DEVICE_ID, communication_result))
        return false;

    std::vector<int32_t> values(IDs.size(), 0);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        uint8_t device_error = 0;
        group_bulk_read.getError(IDs[idx], &device_error);
        if(device_error != 0)
            return finishTransaction("readControlItems", IDs[idx], COMM_SUCCESS, device_error);

        if(!group_bulk_read.isAvailable(
            IDs[idx], registers[idx].address, registers[idx].size))
        {
            setResult(DXL_INTERFACE_STATUS::COMMUNICATION_ERROR, "readControlItems.isAvailable", IDs[idx], COMM_RX_FAIL);
            return false;
        }

        const uint32_t raw_value = group_bulk_read.getData(
            IDs[idx], registers[idx].address, registers[idx].size);
        values[idx] = decodeRegisterValue(raw_value, registers[idx]);
    }

    group_bulk_read.clearParam();
    DATA.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeControlItems(
    const std::vector<uint8_t>& IDs,
    DXL_CONTROL_ITEM item,
    const std::vector<int32_t>& DATA)
{
    return writeControlItemGroup(IDs, item, DATA);
}

bool DYNAMIXEL_SDK_INTERFACE::writeControlItemsChecked(
    const std::vector<uint8_t>& IDs,
    DXL_CONTROL_ITEM item,
    const std::vector<int32_t>& DATA)
{
    if(IDs.size() != DATA.size())
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItemsChecked", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        DXL_REGISTER_INFO info;
        if(!getRegisterInfo(getModelFamily(IDs[idx]), item, info) || !info.writable)
        {
            setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "writeControlItemsChecked", IDs[idx], COMM_NOT_AVAILABLE);
            return false;
        }
        if(!isRegisterValueValid(DATA[idx], info))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeControlItemsChecked.range", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }

    if(IDs.empty())
    {
        setResult(DXL_INTERFACE_STATUS::SUCCESS, "writeControlItemsChecked", NO_DEVICE_ID, COMM_SUCCESS);
        return true;
    }

    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!writeControlItem(IDs[idx], item, DATA[idx]))
            return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readControlItems(
    const std::vector<uint8_t>& IDs,
    DXL_CONTROL_ITEM item,
    std::vector<int32_t>& DATA)
{
    return readControlItemGroup(IDs, item, DATA);
}

void DYNAMIXEL_SDK_INTERFACE::setModelFamily(uint8_t ID, DXL_MODEL_FAMILY family)
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    DXL_DEVICE_INFO& info = devices_[ID];
    if(info.family_configured && info.family != family)
        info = DXL_DEVICE_INFO();
    info.family = family;
    info.family_configured = true;
}

void DYNAMIXEL_SDK_INTERFACE::setModelFamilies(
    const std::vector<uint8_t>& IDs,
    DXL_MODEL_FAMILY family)
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        DXL_DEVICE_INFO& info = devices_[IDs[idx]];
        if(info.family_configured && info.family != family)
            info = DXL_DEVICE_INFO();
        info.family = family;
        info.family_configured = true;
    }
}

DXL_MODEL_FAMILY DYNAMIXEL_SDK_INTERFACE::getModelFamily(uint8_t ID) const
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    const std::map<uint8_t, DXL_DEVICE_INFO>::const_iterator found = devices_.find(ID);
    return found == devices_.end() ? DXL_MODEL_FAMILY::PX : found->second.family;
}

bool DYNAMIXEL_SDK_INTERFACE::isModelFamilyConfigured(uint8_t ID) const
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    const std::map<uint8_t, DXL_DEVICE_INFO>::const_iterator found = devices_.find(ID);
    return found != devices_.end() && found->second.family_configured;
}

void DYNAMIXEL_SDK_INTERFACE::setDeviceInfo(uint8_t ID, const DXL_DEVICE_INFO& info)
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    devices_[ID] = info;
    devices_[ID].family_configured = true;
}

bool DYNAMIXEL_SDK_INTERFACE::getDeviceInfo(uint8_t ID, DXL_DEVICE_INFO& info) const
{
    std::lock_guard<std::mutex> lock(device_mutex_);
    const std::map<uint8_t, DXL_DEVICE_INFO>::const_iterator found = devices_.find(ID);
    if(found == devices_.end())
        return false;

    info = found->second;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::setPositionResolution(
    uint8_t ID,
    double pulses_per_revolution)
{
    if(!std::isfinite(pulses_per_revolution) || pulses_per_revolution <= 0.0)
        return false;

    std::lock_guard<std::mutex> lock(device_mutex_);
    devices_[ID].position_resolution = pulses_per_revolution;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::detectAndConfigureDevice(uint8_t ID)
{
    uint16_t model_number = 0;
    if(!ping(ID, &model_number))
        return false;

    DXL_DEVICE_INFO info;
    if(!makeDeviceInfoForModelNumber(model_number, info))
    {
        setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "detectAndConfigureDevice", ID, COMM_NOT_AVAILABLE);
        return false;
    }

    setDeviceInfo(ID, info);
    setResult(DXL_INTERFACE_STATUS::SUCCESS, "detectAndConfigureDevice", ID, COMM_SUCCESS);
    return true;
}

DXL_DEVICE_INFO DYNAMIXEL_SDK_INTERFACE::makePh54DeviceInfo()
{
    DXL_DEVICE_INFO info;
    info.model_name = "PH54-200/100-S500-R";
    info.family = DXL_MODEL_FAMILY::PX;
    info.family_configured = true;
    info.position_resolution = DXL_MODEL_CONSTANTS::PH54_POSITION_RESOLUTION;
    info.velocity_unit_rpm = DXL_MODEL_CONSTANTS::PH54_VELOCITY_UNIT_RPM;
    info.profile_velocity_unit_rpm = DXL_MODEL_CONSTANTS::PH54_VELOCITY_UNIT_RPM;
    info.current_unit_ampere = DXL_MODEL_CONSTANTS::PH54_CURRENT_UNIT_AMPERE;
    return info;
}

DXL_DEVICE_INFO DYNAMIXEL_SDK_INTERFACE::makeLegacyH54DeviceInfo()
{
    DXL_DEVICE_INFO info;
    info.model_name = "H54-200/100-S500-R";
    info.family = DXL_MODEL_FAMILY::HX;
    info.family_configured = true;
    info.position_resolution = DXL_MODEL_CONSTANTS::LEGACY_H54_POSITION_RESOLUTION;
    info.velocity_unit_rpm = DXL_MODEL_CONSTANTS::LEGACY_H54_VELOCITY_UNIT_RPM;
    info.profile_velocity_unit_rpm = DXL_MODEL_CONSTANTS::LEGACY_H54_VELOCITY_UNIT_RPM;
    info.current_unit_ampere = DXL_MODEL_CONSTANTS::LEGACY_H54_CURRENT_UNIT_AMPERE;
    return info;
}

DXL_DEVICE_INFO DYNAMIXEL_SDK_INTERFACE::makeHax54DeviceInfo()
{
    DXL_DEVICE_INFO info;
    info.model_name = "H54-200/100-S500-R(A)";
    info.family = DXL_MODEL_FAMILY::HAX;
    info.family_configured = true;
    info.position_resolution = DXL_MODEL_CONSTANTS::HAX_H54_POSITION_RESOLUTION;
    info.velocity_unit_rpm = DXL_MODEL_CONSTANTS::HAX_VELOCITY_UNIT_RPM;
    info.profile_velocity_unit_rpm = DXL_MODEL_CONSTANTS::HAX_VELOCITY_UNIT_RPM;
    info.current_unit_ampere = DXL_MODEL_CONSTANTS::HAX_CURRENT_UNIT_AMPERE;
    return info;
}

DXL_DEVICE_INFO DYNAMIXEL_SDK_INTERFACE::makeHax42DeviceInfo()
{
    DXL_DEVICE_INFO info = makeHax54DeviceInfo();
    info.model_name = "H42-20-S300-R(A)";
    info.position_resolution = DXL_MODEL_CONSTANTS::HAX_H42_POSITION_RESOLUTION;
    return info;
}

DXL_DEVICE_INFO DYNAMIXEL_SDK_INTERFACE::makeCommonXSeriesDeviceInfo()
{
    DXL_DEVICE_INFO info;
    info.model_name = "X-Series 12-bit position";
    info.family = DXL_MODEL_FAMILY::XX;
    info.family_configured = true;
    info.position_resolution = DXL_MODEL_CONSTANTS::X_SERIES_12_BIT_POSITION_RESOLUTION;
    info.velocity_unit_rpm = DXL_MODEL_CONSTANTS::COMMON_X_SERIES_VELOCITY_UNIT_RPM;
    info.profile_velocity_unit_rpm = DXL_MODEL_CONSTANTS::COMMON_X_SERIES_VELOCITY_UNIT_RPM;
    return info;
}

bool DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(
    uint16_t model_number,
    DXL_DEVICE_INFO& info)
{
    DXL_DEVICE_INFO detected;
    switch(model_number)
    {
        case DXL_MODEL_CONSTANTS::PH54_200_MODEL_NUMBER:
            detected = makePh54DeviceInfo();
            detected.model_name = "PH54-200-S500-R";
            break;
        case DXL_MODEL_CONSTANTS::PH54_100_MODEL_NUMBER:
            detected = makePh54DeviceInfo();
            detected.model_name = "PH54-100-S500-R";
            break;
        case DXL_MODEL_CONSTANTS::PH42_020_MODEL_NUMBER:
            detected = makePh54DeviceInfo();
            detected.model_name = "PH42-020-S300-R";
            detected.position_resolution = DXL_MODEL_CONSTANTS::PH42_POSITION_RESOLUTION;
            break;
        case DXL_MODEL_CONSTANTS::PM54_060_MODEL_NUMBER:
            detected = makePh54DeviceInfo();
            detected.model_name = "PM54-060-S250-R";
            detected.position_resolution = DXL_MODEL_CONSTANTS::PM54_POSITION_RESOLUTION;
            break;
        case DXL_MODEL_CONSTANTS::PM54_040_MODEL_NUMBER:
            detected = makePh54DeviceInfo();
            detected.model_name = "PM54-040-S250-R";
            detected.position_resolution = DXL_MODEL_CONSTANTS::PM54_POSITION_RESOLUTION;
            break;
        case DXL_MODEL_CONSTANTS::PM42_010_MODEL_NUMBER:
            detected = makePh54DeviceInfo();
            detected.model_name = "PM42-010-S260-R";
            detected.position_resolution = DXL_MODEL_CONSTANTS::PM42_POSITION_RESOLUTION;
            break;
        case DXL_MODEL_CONSTANTS::LEGACY_H54_200_MODEL_NUMBER:
            detected = makeLegacyH54DeviceInfo();
            detected.model_name = "H54-200-S500-R";
            break;
        case DXL_MODEL_CONSTANTS::LEGACY_H54_100_MODEL_NUMBER:
            detected = makeLegacyH54DeviceInfo();
            detected.model_name = "H54-100-S500-R";
            break;
        case DXL_MODEL_CONSTANTS::HAX_H54_200_MODEL_NUMBER:
            detected = makeHax54DeviceInfo();
            detected.model_name = "H54-200-S500-R(A)";
            break;
        case DXL_MODEL_CONSTANTS::HAX_H54_100_MODEL_NUMBER:
            detected = makeHax54DeviceInfo();
            detected.model_name = "H54-100-S500-R(A)";
            break;
        case DXL_MODEL_CONSTANTS::HAX_H42_020_MODEL_NUMBER:
            detected = makeHax42DeviceInfo();
            break;
        default:
            return false;
    }

    detected.model_number = model_number;
    info = detected;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::radiansToRawPosition(
    uint8_t ID,
    double radians,
    int32_t& raw_position) const
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info)
        || !std::isfinite(radians)
        || info.position_resolution <= 0.0)
        return false;

    const double raw_value = radians * info.position_resolution / TWO_PI;
    if(raw_value < static_cast<double>(std::numeric_limits<int32_t>::min())
        || raw_value > static_cast<double>(std::numeric_limits<int32_t>::max()))
        return false;

    raw_position = static_cast<int32_t>(std::llround(raw_value));
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::rawPositionToRadians(
    uint8_t ID,
    int32_t raw_position,
    double& radians) const
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info) || info.position_resolution <= 0.0)
        return false;

    radians = static_cast<double>(raw_position) * TWO_PI / info.position_resolution;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::radiansPerSecondToRawVelocity(
    uint8_t ID,
    double radians_per_second,
    int32_t& raw_velocity) const
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info)
        || !std::isfinite(radians_per_second)
        || info.velocity_unit_rpm <= 0.0)
        return false;

    const double rpm = radians_per_second * 60.0 / TWO_PI;
    const double raw_value = rpm / info.velocity_unit_rpm;
    if(raw_value < static_cast<double>(std::numeric_limits<int32_t>::min())
        || raw_value > static_cast<double>(std::numeric_limits<int32_t>::max()))
        return false;

    raw_velocity = static_cast<int32_t>(std::llround(raw_value));
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::rawVelocityToRadiansPerSecond(
    uint8_t ID,
    int32_t raw_velocity,
    double& radians_per_second) const
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info) || info.velocity_unit_rpm <= 0.0)
        return false;

    radians_per_second = static_cast<double>(raw_velocity)
        * info.velocity_unit_rpm * TWO_PI / 60.0;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::ampereToRawCurrent(
    uint8_t ID,
    double ampere,
    int16_t& raw_current) const
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info)
        || !std::isfinite(ampere)
        || info.current_unit_ampere <= 0.0)
        return false;

    const double raw_value = ampere / info.current_unit_ampere;
    if(raw_value < static_cast<double>(std::numeric_limits<int16_t>::min())
        || raw_value > static_cast<double>(std::numeric_limits<int16_t>::max()))
        return false;

    raw_current = static_cast<int16_t>(std::llround(raw_value));
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::rawCurrentToAmpere(
    uint8_t ID,
    int16_t raw_current,
    double& ampere) const
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info) || info.current_unit_ampere <= 0.0)
        return false;

    ampere = static_cast<double>(raw_current) * info.current_unit_ampere;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::enableTorque(uint8_t ID, uint16_t ADDR)
{
    return write1ByteTxRx(ID, ADDR, static_cast<uint8_t>(DXL_TORQUE::TORQUE_ON));
}

bool DYNAMIXEL_SDK_INTERFACE::disableTorque(uint8_t ID, uint16_t ADDR)
{
    return write1ByteTxRx(ID, ADDR, static_cast<uint8_t>(DXL_TORQUE::TORQUE_OFF));
}

bool DYNAMIXEL_SDK_INTERFACE::changeOperatingMode(
    uint8_t ID,
    uint16_t ADDR,
    DXL_OPERATING_MODE mode)
{
    return write1ByteTxRx(ID, ADDR, static_cast<uint8_t>(mode));
}

bool DYNAMIXEL_SDK_INTERFACE::set_watchdog(uint8_t ID, int TIMER)
{
    if(TIMER < 0 || TIMER > 127)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "set_watchdog", ID, COMM_TX_FAIL);
        return false;
    }
    return writeBusWatchdogGeneral(ID, static_cast<uint8_t>(TIMER));
}

bool DYNAMIXEL_SDK_INTERFACE::set_watchdogs(std::vector<uint8_t> IDs, int TIMER)
{
    if(TIMER < 0 || TIMER > 127)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "set_watchdogs", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    const std::vector<int32_t> values(IDs.size(), TIMER);
    return writeControlItems(IDs, DXL_CONTROL_ITEM::BUS_WATCHDOG, values);
}

bool DYNAMIXEL_SDK_INTERFACE::toggleAllTorque(
    std::vector<uint8_t> IDs,
    bool toggle)
{
    const std::vector<int32_t> values(IDs.size(), toggle ? 1 : 0);
    return writeControlItems(IDs, DXL_CONTROL_ITEM::TORQUE_ENABLE, values);
}

bool DYNAMIXEL_SDK_INTERFACE::turnOffXxLed(uint8_t ID)
{
    return write1ByteTxRx(ID, ADDR_XX_LED, static_cast<uint8_t>(DXL_LED::LED_OFF));
}

bool DYNAMIXEL_SDK_INTERFACE::turnOnXxLed(uint8_t ID)
{
    return write1ByteTxRx(ID, ADDR_XX_LED, static_cast<uint8_t>(DXL_LED::LED_ON));
}

bool DYNAMIXEL_SDK_INTERFACE::turnPxLed(
    uint8_t ID,
    uint8_t red,
    uint8_t green,
    uint8_t blue)
{
    bool success = write1ByteTxRx(ID, ADDR_PX_LED_RED, red);
    success = write1ByteTxRx(ID, ADDR_PX_LED_GREEN, green) && success;
    success = write1ByteTxRx(ID, ADDR_PX_LED_BLUE, blue) && success;
    return success;
}

bool DYNAMIXEL_SDK_INTERFACE::setLedGeneral(uint8_t ID, bool enabled)
{
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX)
        return writeControlItem(ID, DXL_CONTROL_ITEM::LED, enabled ? 1 : 0);

    const uint8_t value = enabled ? 255 : 0;
    return setRgbLedGeneral(ID, value, value, value);
}

bool DYNAMIXEL_SDK_INTERFACE::setRgbLedGeneral(
    uint8_t ID,
    uint8_t red,
    uint8_t green,
    uint8_t blue)
{
    if(getModelFamily(ID) == DXL_MODEL_FAMILY::XX)
    {
        setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "setRgbLedGeneral", ID, COMM_NOT_AVAILABLE);
        return false;
    }

    bool success = writeControlItem(ID, DXL_CONTROL_ITEM::LED_RED, red);
    success = writeControlItem(ID, DXL_CONTROL_ITEM::LED_GREEN, green) && success;
    success = writeControlItem(ID, DXL_CONTROL_ITEM::LED_BLUE, blue) && success;
    return success;
}

bool DYNAMIXEL_SDK_INTERFACE::enableTorqueGeneral(uint8_t ID)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::TORQUE_ENABLE, 1);
}

bool DYNAMIXEL_SDK_INTERFACE::disableTorqueGeneral(uint8_t ID)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::TORQUE_ENABLE, 0);
}

bool DYNAMIXEL_SDK_INTERFACE::changeOperatingModeGeneral(
    uint8_t ID,
    DXL_OPERATING_MODE mode)
{
    if(!supportsOperatingMode(ID, mode))
    {
        setResult(DXL_INTERFACE_STATUS::UNSUPPORTED, "changeOperatingModeGeneral", ID, COMM_NOT_AVAILABLE);
        return false;
    }
    return writeControlItem(ID, DXL_CONTROL_ITEM::OPERATING_MODE, static_cast<int32_t>(mode));
}

bool DYNAMIXEL_SDK_INTERFACE::readBusWatchdogGeneral(uint8_t ID, int8_t& timer)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::BUS_WATCHDOG, value))
        return false;
    timer = static_cast<int8_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeBusWatchdogGeneral(uint8_t ID, uint8_t timer)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::BUS_WATCHDOG, timer);
}

bool DYNAMIXEL_SDK_INTERFACE::clearBusWatchdogGeneral(uint8_t ID)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::BUS_WATCHDOG, 0);
}

bool DYNAMIXEL_SDK_INTERFACE::writeCurrentLimitGeneral(uint8_t ID, int16_t current_limit)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::CURRENT_LIMIT, current_limit);
}

bool DYNAMIXEL_SDK_INTERFACE::writeTorqueLimitGeneral(uint8_t ID, uint16_t torque_limit)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::TORQUE_LIMIT, torque_limit);
}

bool DYNAMIXEL_SDK_INTERFACE::writeVelocityIGainGeneral(uint8_t ID, uint16_t velocity_i_gain)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::VELOCITY_I_GAIN, velocity_i_gain);
}

bool DYNAMIXEL_SDK_INTERFACE::writeVelocityPGainGeneral(uint8_t ID, uint16_t velocity_p_gain)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::VELOCITY_P_GAIN, velocity_p_gain);
}

bool DYNAMIXEL_SDK_INTERFACE::writePositionDGainGeneral(uint8_t ID, uint16_t position_d_gain)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::POSITION_D_GAIN, position_d_gain);
}

bool DYNAMIXEL_SDK_INTERFACE::writePositionIGainGeneral(uint8_t ID, uint16_t position_i_gain)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::POSITION_I_GAIN, position_i_gain);
}

bool DYNAMIXEL_SDK_INTERFACE::writePositionPGainGeneral(uint8_t ID, uint16_t position_p_gain)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::POSITION_P_GAIN, position_p_gain);
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileVelocityGeneral(uint8_t ID, int32_t profile_velocity)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::PROFILE_VELOCITY, profile_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileAccelerationGeneral(uint8_t ID, int32_t profile_acceleration)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::PROFILE_ACCELERATION, profile_acceleration);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalPWMGeneral(uint8_t ID, int16_t& goal_pwm)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::GOAL_PWM, value))
        return false;
    goal_pwm = static_cast<int16_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPWMGeneral(uint8_t ID, int16_t goal_pwm)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::GOAL_PWM, goal_pwm);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalCurrentGeneral(uint8_t ID, int16_t& goal_current)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::GOAL_CURRENT, value))
        return false;
    goal_current = static_cast<int16_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalCurrentGeneral(uint8_t ID, int16_t goal_current)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::GOAL_CURRENT, goal_current);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalTorqueGeneral(uint8_t ID, int16_t& goal_torque)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::GOAL_TORQUE, value))
        return false;
    goal_torque = static_cast<int16_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalTorqueGeneral(uint8_t ID, int16_t goal_torque)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::GOAL_TORQUE, goal_torque);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalVelocityGeneral(uint8_t ID, int32_t& goal_velocity)
{
    return readControlItem(ID, DXL_CONTROL_ITEM::GOAL_VELOCITY, goal_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalVelocityGeneral(uint8_t ID, int32_t goal_velocity)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::GOAL_VELOCITY, goal_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalPositionGeneral(uint8_t ID, int32_t& goal_position)
{
    return readControlItem(ID, DXL_CONTROL_ITEM::GOAL_POSITION, goal_position);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPositionGeneral(uint8_t ID, int32_t goal_position)
{
    return writeControlItem(ID, DXL_CONTROL_ITEM::GOAL_POSITION, goal_position);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPWMGeneral(uint8_t ID, int16_t& present_pwm)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::PRESENT_PWM, value))
        return false;
    present_pwm = static_cast<int16_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentCurrentGeneral(uint8_t ID, int16_t& present_current)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::PRESENT_CURRENT, value))
        return false;
    present_current = static_cast<int16_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocityGeneral(uint8_t ID, int32_t& present_velocity)
{
    return readControlItem(ID, DXL_CONTROL_ITEM::PRESENT_VELOCITY, present_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPositionGeneral(uint8_t ID, int32_t& present_position)
{
    return readControlItem(ID, DXL_CONTROL_ITEM::PRESENT_POSITION, present_position);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentInputVoltageGeneral(
    uint8_t ID,
    uint16_t& present_voltage)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::PRESENT_INPUT_VOLTAGE, value))
        return false;
    present_voltage = static_cast<uint16_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentTemperatureGeneral(
    uint8_t ID,
    uint8_t& present_temperature)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::PRESENT_TEMPERATURE, value))
        return false;
    present_temperature = static_cast<uint8_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readHardwareErrorStatusGeneral(
    uint8_t ID,
    uint8_t& hardware_error_status)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::HARDWARE_ERROR_STATUS, value))
        return false;
    hardware_error_status = static_cast<uint8_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readTorqueEnabledGeneral(uint8_t ID, bool& enabled)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::TORQUE_ENABLE, value))
        return false;
    enabled = value != 0;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readMovingGeneral(uint8_t ID, bool& moving)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::MOVING, value))
        return false;
    moving = value != 0;
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readMovingStatusGeneral(uint8_t ID, uint8_t& moving_status)
{
    int32_t value = 0;
    if(!readControlItem(ID, DXL_CONTROL_ITEM::MOVING_STATUS, value))
        return false;
    moving_status = static_cast<uint8_t>(value);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPositionGeneral(
    const std::vector<uint8_t>& IDs,
    std::vector<int32_t>& DATA)
{
    return readControlItems(IDs, DXL_CONTROL_ITEM::PRESENT_POSITION, DATA);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocityGeneral(
    const std::vector<uint8_t>& IDs,
    std::vector<int32_t>& DATA)
{
    return readControlItems(IDs, DXL_CONTROL_ITEM::PRESENT_VELOCITY, DATA);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentCurrentGeneral(
    const std::vector<uint8_t>& IDs,
    std::vector<int16_t>& DATA)
{
    std::vector<int32_t> raw_values;
    if(!readControlItems(IDs, DXL_CONTROL_ITEM::PRESENT_CURRENT, raw_values))
        return false;

    std::vector<int16_t> values(raw_values.size(), 0);
    for(size_t idx = 0; idx < raw_values.size(); ++idx)
        values[idx] = static_cast<int16_t>(raw_values[idx]);
    DATA.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPositionGeneral(
    const std::vector<uint8_t>& IDs,
    const std::vector<int32_t>& DATA)
{
    return writeControlItems(IDs, DXL_CONTROL_ITEM::GOAL_POSITION, DATA);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPositionRadians(uint8_t ID, double radians)
{
    int32_t raw_position = 0;
    if(!radiansToRawPosition(ID, radians, raw_position))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeGoalPositionRadians", ID, COMM_TX_FAIL);
        return false;
    }
    return writeGoalPositionGeneral(ID, raw_position);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPositionsRadians(
    const std::vector<uint8_t>& IDs,
    const std::vector<double>& radians)
{
    if(IDs.size() != radians.size())
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeGoalPositionsRadians", NO_DEVICE_ID, COMM_TX_FAIL);
        return false;
    }

    std::vector<int32_t> raw_positions(IDs.size(), 0);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!radiansToRawPosition(IDs[idx], radians[idx], raw_positions[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeGoalPositionsRadians", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }
    return writeGoalPositionGeneral(IDs, raw_positions);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPositionRadians(uint8_t ID, double& radians)
{
    int32_t raw_position = 0;
    if(!readPresentPositionGeneral(ID, raw_position))
        return false;
    if(!rawPositionToRadians(ID, raw_position, radians))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readPresentPositionRadians", ID, COMM_TX_FAIL);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPositionsRadians(
    const std::vector<uint8_t>& IDs,
    std::vector<double>& radians)
{
    std::vector<int32_t> raw_positions;
    if(!readPresentPositionGeneral(IDs, raw_positions))
        return false;

    std::vector<double> values(IDs.size(), 0.0);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!rawPositionToRadians(IDs[idx], raw_positions[idx], values[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readPresentPositionsRadians", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }
    radians.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocityRadiansPerSecond(
    uint8_t ID,
    double& radians_per_second)
{
    int32_t raw_velocity = 0;
    if(!readPresentVelocityGeneral(ID, raw_velocity))
        return false;
    if(!rawVelocityToRadiansPerSecond(ID, raw_velocity, radians_per_second))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readPresentVelocityRadiansPerSecond", ID, COMM_TX_FAIL);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocitiesRadiansPerSecond(
    const std::vector<uint8_t>& IDs,
    std::vector<double>& radians_per_second)
{
    std::vector<int32_t> raw_velocities;
    if(!readPresentVelocityGeneral(IDs, raw_velocities))
        return false;

    std::vector<double> values(IDs.size(), 0.0);
    for(size_t idx = 0; idx < IDs.size(); ++idx)
    {
        if(!rawVelocityToRadiansPerSecond(IDs[idx], raw_velocities[idx], values[idx]))
        {
            setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readPresentVelocitiesRadiansPerSecond", IDs[idx], COMM_TX_FAIL);
            return false;
        }
    }
    radians_per_second.swap(values);
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentCurrentAmpere(uint8_t ID, double& ampere)
{
    int16_t raw_current = 0;
    if(!readPresentCurrentGeneral(ID, raw_current))
        return false;
    if(!rawCurrentToAmpere(ID, raw_current, ampere))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "readPresentCurrentAmpere", ID, COMM_TX_FAIL);
        return false;
    }
    return true;
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalCurrentAmpere(uint8_t ID, double ampere)
{
    int16_t raw_current = 0;
    if(!ampereToRawCurrent(ID, ampere, raw_current))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeGoalCurrentAmpere", ID, COMM_TX_FAIL);
        return false;
    }
    return writeGoalCurrentGeneral(ID, raw_current);
}

bool DYNAMIXEL_SDK_INTERFACE::writeCurrentLimitAmpere(uint8_t ID, double ampere)
{
    int16_t raw_current = 0;
    if(ampere < 0.0 || !ampereToRawCurrent(ID, ampere, raw_current))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeCurrentLimitAmpere", ID, COMM_TX_FAIL);
        return false;
    }
    return writeCurrentLimitGeneral(ID, raw_current);
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileVelocityRadiansPerSecond(
    uint8_t ID,
    double radians_per_second)
{
    DXL_DEVICE_INFO info;
    if(!getDeviceInfo(ID, info)
        || !std::isfinite(radians_per_second)
        || radians_per_second < 0.0
        || info.profile_velocity_unit_rpm <= 0.0)
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeProfileVelocityRadiansPerSecond", ID, COMM_TX_FAIL);
        return false;
    }

    const double rpm = radians_per_second * 60.0 / TWO_PI;
    const double raw_value = rpm / info.profile_velocity_unit_rpm;
    if(raw_value > static_cast<double>(std::numeric_limits<int32_t>::max()))
    {
        setResult(DXL_INTERFACE_STATUS::INVALID_ARGUMENT, "writeProfileVelocityRadiansPerSecond", ID, COMM_TX_FAIL);
        return false;
    }

    return writeProfileVelocityGeneral(ID, static_cast<int32_t>(std::llround(raw_value)));
}

bool DYNAMIXEL_SDK_INTERFACE::writeCurrentLimit(uint8_t ID, int16_t current_limit)
{
    return writeCurrentLimitGeneral(ID, current_limit);
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileVelocity(uint8_t ID, int32_t profile_velocity)
{
    return writeProfileVelocityGeneral(ID, profile_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::writeProfileAcceleration(uint8_t ID, int32_t profile_acceleration)
{
    return writeProfileAccelerationGeneral(ID, profile_acceleration);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalCurrent(uint8_t ID, int16_t& goal_current)
{
    return readGoalCurrentGeneral(ID, goal_current);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalCurrent(uint8_t ID, int16_t goal_current)
{
    return writeGoalCurrentGeneral(ID, goal_current);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalVelocity(uint8_t ID, int32_t& goal_velocity)
{
    return readGoalVelocityGeneral(ID, goal_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalVelocity(uint8_t ID, int32_t goal_velocity)
{
    return writeGoalVelocityGeneral(ID, goal_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::readGoalPosition(uint8_t ID, int32_t& goal_position)
{
    return readGoalPositionGeneral(ID, goal_position);
}

bool DYNAMIXEL_SDK_INTERFACE::writeGoalPosition(uint8_t ID, int32_t goal_position)
{
    return writeGoalPositionGeneral(ID, goal_position);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPWM(uint8_t ID, int16_t& present_pwm)
{
    return readPresentPWMGeneral(ID, present_pwm);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentCurrent(uint8_t ID, int16_t& present_current)
{
    return readPresentCurrentGeneral(ID, present_current);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentVelocity(uint8_t ID, int32_t& present_velocity)
{
    return readPresentVelocityGeneral(ID, present_velocity);
}

bool DYNAMIXEL_SDK_INTERFACE::readPresentPosition(uint8_t ID, int32_t& present_position)
{
    return readPresentPositionGeneral(ID, present_position);
}
