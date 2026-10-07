#include <dynamixel_sdk_interface/dynamixel_sdk_interface.hpp>

// Updated 2026-10-07: retain one scan algorithm for both ROS client APIs.
#if DYNAMIXEL_ROS_VERSION == 2
#include <rclcpp/rclcpp.hpp>
#define DXL_INFO(OUT) RCLCPP_INFO_STREAM(private_node->get_logger(), OUT)
#define DXL_WARN(OUT) RCLCPP_WARN_STREAM(private_node->get_logger(), OUT)
#define DXL_ERROR(OUT) RCLCPP_ERROR_STREAM(private_node->get_logger(), OUT)
#elif DYNAMIXEL_ROS_VERSION == 1
#include <ros/ros.h>
#define DXL_INFO(OUT) ROS_INFO_STREAM(OUT)
#define DXL_WARN(OUT) ROS_WARN_STREAM(OUT)
#define DXL_ERROR(OUT) ROS_ERROR_STREAM(OUT)
#else
#error "DYNAMIXEL_ROS_VERSION must be 1 or 2 (set by CMake)."
#endif

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace
{
bool rosOk()
{
#if DYNAMIXEL_ROS_VERSION == 2
    return rclcpp::ok();
#else
    return ros::ok();
#endif
}

struct RosShutdown
{
    ~RosShutdown()
    {
#if DYNAMIXEL_ROS_VERSION == 2
        rclcpp::shutdown();
#else
        ros::shutdown();
#endif
    }
};

constexpr int MIN_DYNAMIXEL_ID = 0;
constexpr int MAX_DYNAMIXEL_ID = 252;

const char* familyName(DXL_MODEL_FAMILY family)
{
    switch(family)
    {
        case DXL_MODEL_FAMILY::PX:
            return "PX";
        case DXL_MODEL_FAMILY::XX:
            return "XX";
        case DXL_MODEL_FAMILY::HX:
            return "HX";
        case DXL_MODEL_FAMILY::HAX:
            return "HAX";
    }
    return "UNKNOWN";
}

const char* statusName(DXL_INTERFACE_STATUS status)
{
    switch(status)
    {
        case DXL_INTERFACE_STATUS::SUCCESS:
            return "SUCCESS";
        case DXL_INTERFACE_STATUS::LOCK_TIMEOUT:
            return "LOCK_TIMEOUT";
        case DXL_INTERFACE_STATUS::PORT_NOT_OPEN:
            return "PORT_NOT_OPEN";
        case DXL_INTERFACE_STATUS::PORT_OPEN_FAILED:
            return "PORT_OPEN_FAILED";
        case DXL_INTERFACE_STATUS::INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";
        case DXL_INTERFACE_STATUS::UNSUPPORTED:
            return "UNSUPPORTED";
        case DXL_INTERFACE_STATUS::COMMUNICATION_ERROR:
            return "COMMUNICATION_ERROR";
        case DXL_INTERFACE_STATUS::DEVICE_ERROR:
            return "DEVICE_ERROR";
    }
    return "UNKNOWN";
}

}

int main(int argc, char** argv)
{
#if DYNAMIXEL_ROS_VERSION == 2
    rclcpp::init(argc, argv);
    RosShutdown shutdown;
    auto private_node = std::make_shared<rclcpp::Node>("dynamixel_scan");
#else
    ros::init(argc, argv, "dynamixel_scan");
    RosShutdown shutdown;
    ros::NodeHandle private_node("~");
#endif

    std::string device_name;
    int baudrate = 0;
    int min_id = 0;
    int max_id = 0;
    int lock_timeout_ms = 0;
    bool show_missing = false;

#if DYNAMIXEL_ROS_VERSION == 2
    device_name = private_node->declare_parameter<std::string>("device_name", "/dev/ttyUSB0");
    baudrate = private_node->declare_parameter<int>("baudrate", 4000000);
    min_id = private_node->declare_parameter<int>("min_id", MIN_DYNAMIXEL_ID);
    max_id = private_node->declare_parameter<int>("max_id", MAX_DYNAMIXEL_ID);
    lock_timeout_ms = private_node->declare_parameter<int>("lock_timeout_ms", 20);
    show_missing = private_node->declare_parameter<bool>("show_missing", false);
#else
    private_node.param<std::string>("device_name", device_name, "/dev/ttyUSB0");
    private_node.param("baudrate", baudrate, 4000000);
    private_node.param("min_id", min_id, MIN_DYNAMIXEL_ID);
    private_node.param("max_id", max_id, MAX_DYNAMIXEL_ID);
    private_node.param("lock_timeout_ms", lock_timeout_ms, 20);
    private_node.param("show_missing", show_missing, false);
#endif

    if(device_name.empty())
    {
        DXL_ERROR("Parameter 'device_name' must not be empty.");
        return 1;
    }
    if(baudrate <= 0)
    {
        DXL_ERROR("Parameter 'baudrate' must be positive: " << baudrate);
        return 1;
    }
    if(min_id < MIN_DYNAMIXEL_ID || max_id > MAX_DYNAMIXEL_ID || min_id > max_id)
    {
        DXL_ERROR("Invalid ID range [" << min_id << ", " << max_id
            << "]. Valid DYNAMIXEL IDs are 0 through 252.");
        return 1;
    }
    if(lock_timeout_ms <= 0)
    {
        DXL_ERROR("Parameter 'lock_timeout_ms' must be positive: "
            << lock_timeout_ms);
        return 1;
    }

    DYNAMIXEL_SDK_INTERFACE interface(
        device_name.c_str(), static_cast<unsigned int>(baudrate));
    interface.setLockTimeout(std::chrono::milliseconds(lock_timeout_ms));

    if(!interface.isPortOpen())
    {
        const DXL_INTERFACE_RESULT result = interface.getLastResult();
        DXL_ERROR("Failed to open " << device_name << " at " << baudrate
            << " bps: status=" << statusName(result.status)
            << ", communication_result=" << result.communication_result);
        return 2;
    }

    DXL_INFO("Scanning DYNAMIXEL Protocol 2.0 IDs " << min_id << " through "
        << max_id << " on " << device_name << " at " << baudrate << " bps.");

    int found_count = 0;
    int recognized_count = 0;
    int unexpected_error_count = 0;
    std::vector<uint8_t> detected_ids;

    if(!interface.broadcastPing(detected_ids))
    {
        const DXL_INTERFACE_RESULT result = interface.getLastResult();
        if(result.status != DXL_INTERFACE_STATUS::COMMUNICATION_ERROR
            || result.communication_result != COMM_RX_TIMEOUT)
        {
            DXL_ERROR("Broadcast ping failed: status=" << statusName(result.status)
                << ", communication_result=" << result.communication_result
                << ", device_error=" << static_cast<int>(result.device_error));
            return 3;
        }
    }

    std::sort(detected_ids.begin(), detected_ids.end());
    detected_ids.erase(
        std::unique(detected_ids.begin(), detected_ids.end()), detected_ids.end());

    std::vector<bool> responded(MAX_DYNAMIXEL_ID + 1, false);
    for(std::size_t index = 0; index < detected_ids.size() && rosOk(); ++index)
    {
        const int id = detected_ids[index];
        if(id < min_id || id > max_id)
            continue;

        responded[id] = true;
        ++found_count;
        uint16_t model_number = 0;
        if(!interface.ping(static_cast<uint8_t>(id), &model_number))
        {
            const DXL_INTERFACE_RESULT result = interface.getLastResult();
            ++unexpected_error_count;
            DXL_WARN("[ID " << id << "] discovered, but model query failed: status="
                << statusName(result.status)
                << ", communication_result=" << result.communication_result
                << ", device_error=" << static_cast<int>(result.device_error));
            continue;
        }

        DXL_DEVICE_INFO info;
        if(DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(model_number, info))
        {
            interface.setDeviceInfo(static_cast<uint8_t>(id), info);
            ++recognized_count;
            DXL_INFO("[ID " << id << "] model=" << info.model_name
                << " (" << model_number << ")"
                << ", family=" << familyName(info.family)
                << ", position_resolution="
                << static_cast<long long>(info.position_resolution) << " pulse/rev");
        }
        else
        {
            DXL_WARN("[ID " << id << "] model_number=" << model_number
                << " is present but not registered in the built-in model table.");
        }
    }

    if(show_missing)
    {
        for(int id = min_id; id <= max_id; ++id)
        {
            if(!responded[id])
                DXL_INFO("[ID " << id << "] no response");
        }
    }

    if(!rosOk())
    {
        DXL_WARN("DYNAMIXEL scan interrupted.");
        return 130;
    }

    DXL_INFO("Scan complete: found=" << found_count
        << ", recognized=" << recognized_count
        << ", unexpected_errors=" << unexpected_error_count << '.');

    if(found_count == 0)
    {
        DXL_ERROR("No DYNAMIXEL responded. Check port exclusivity, baudrate, power, and wiring.");
        return 3;
    }

    return 0;
}
