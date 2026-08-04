# dynamixel_sdk_interface

This ROS 1 package wraps DYNAMIXEL Protocol 2.0 communication behind a thread-safe, model-family-aware C++ API.

## Build

Place this package, `dynamixel_sdk`, and `lib_functions` in the same catkin workspace, then build and source it:

```bash
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

## Scan a DYNAMIXEL bus

`dynamixel_scan_node` is a read-only, one-shot diagnostic executable. It opens the selected serial device, sends one Protocol 2.0 Broadcast Ping, queries the model number of each responding ID, prints the result, and exits. It does not change torque, operating mode, ID, baud rate, or EEPROM values.

Stop every controller or tool already using the U2D2 port before scanning. Only one process can own the serial device at a time.

Prefer a persistent Linux device path when available:

```bash
ls -l /dev/serial/by-id/

roslaunch dynamixel_sdk_interface dynamixel_scan.launch \
  device_name:=/dev/serial/by-id/usb-FTDI_USB__-__Serial_Converter-if00-port0 \
  baudrate:=4000000 \
  min_id:=1 \
  max_id:=4
```

The conventional U2D2 path also works:

```bash
roslaunch dynamixel_sdk_interface dynamixel_scan.launch \
  device_name:=/dev/ttyUSB0 \
  baudrate:=4000000
```

Run the executable directly with ROS private parameters if a launch file is not needed:

```bash
rosrun dynamixel_sdk_interface dynamixel_scan_node \
  _device_name:=/dev/ttyUSB0 \
  _baudrate:=4000000 \
  _min_id:=0 \
  _max_id:=252
```

Launch arguments:

| Argument | Default | Meaning |
| --- | ---: | --- |
| `device_name` | `/dev/ttyUSB0` | U2D2 or other serial device path |
| `baudrate` | `4000000` | Bus baud rate in bit/s |
| `min_id` | `0` | Lowest ID included in the report |
| `max_id` | `252` | Highest ID included in the report |
| `lock_timeout_ms` | `20` | Maximum wait for the interface port mutex; not a packet timeout |
| `show_missing` | `false` | Print every ID that did not respond |

Example output:

```text
[ID 1] model=PH54-200-S500-R (2020), family=PX, position_resolution=1003846 pulse/rev
[ID 4] model=H54-200-S500-R (54024), family=HX, position_resolution=501923 pulse/rev
Scan complete: found=2, recognized=2, unexpected_errors=0.
```

The baud rate is not auto-detected. Run the scanner once per candidate baud rate if devices may use different settings. An unregistered model is still reported with its numeric model number; add its verified metadata to `makeDeviceInfoForModelNumber()` before using family-specific commands or SI-unit conversion.

## Basic C++ usage

The constructor opens the port and applies the requested baud rate. Check `isPortOpen()` before issuing commands:

```cpp
#include <dynamixel_sdk_interface/dynamixel_sdk_interface.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

DYNAMIXEL_SDK_INTERFACE dxl("/dev/ttyUSB0", 4000000);
if(!dxl.isPortOpen())
{
    const DXL_INTERFACE_RESULT result = dxl.getLastResult();
    // Report result.status and result.communication_result, then stop safely.
    return;
}

std::vector<uint8_t> detected_ids;
if(dxl.broadcastPing(detected_ids))
{
    for(std::size_t index = 0; index < detected_ids.size(); ++index)
    {
        const uint8_t id = detected_ids[index];
        uint16_t model_number = 0;
        DXL_DEVICE_INFO info;
        if(dxl.ping(id, &model_number)
            && DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(model_number, info))
        {
            dxl.setDeviceInfo(id, info);
        }
    }
}
```

For a known ID, `detectAndConfigureDevice()` combines the model query and built-in metadata registration:

```cpp
const uint8_t id = 1;
if(!dxl.detectAndConfigureDevice(id))
    return;

if(!dxl.disableTorqueGeneral(id))
    return;
if(!dxl.changeOperatingModeGeneral(id, DXL_OPERATING_MODE::POSITION_CONTROL_MODE))
    return;
if(!dxl.writeProfileVelocityRadiansPerSecond(id, 0.5))
    return;
if(!dxl.enableTorqueGeneral(id))
    return;
if(!dxl.writeGoalPositionRadians(id, 0.25))
    return;
```

Always validate robot-level joint limits, direction, zero offset, collision constraints, and motion feasibility before sending the final goal.

Once every ID has exact metadata, a mixed PX/HX group can use SI-unit helpers. The implementation resolves each model family's address and sends a Bulk Write:

```cpp
const std::vector<uint8_t> ids = {1, 2, 3, 4};
for(std::size_t index = 0; index < ids.size(); ++index)
{
    if(!dxl.detectAndConfigureDevice(ids[index]))
        return;
}

const std::vector<double> goal_radians = {0.0, 0.2, -0.3, 0.1};
if(!dxl.writeGoalPositionsRadians(ids, goal_radians))
    return;
```

Bulk Write success confirms packet transmission, not per-device acceptance. Use `writeControlItemsChecked()` during configuration when each device's returned error must be checked.

## Family and model metadata

`DXL_MODEL_FAMILY` selects the control-table layout:

- `PX`: DYNAMIXEL-P / PRO+ layout
- `HX`: legacy DYNAMIXEL PRO H layout
- `XX`: common X-series layout

The family alone is sufficient for raw register commands. Physical-unit conversion additionally requires exact model metadata because products sharing a control table can have different position resolution, velocity scale, and current scale.

Built-in model-number mappings:

| Model number | Model | Family | Position resolution (pulse/rev) |
| ---: | --- | --- | ---: |
| `2020` | PH54-200-S500-R | PX | 1,003,846 |
| `2010` | PH54-100-S500-R | PX | 1,003,846 |
| `2000` | PH42-020-S300-R | PX | 607,500 |
| `2120` | PM54-060-S250-R | PX | 502,834 |
| `2110` | PM54-040-S250-R | PX | 502,834 |
| `2100` | PM42-010-S260-R | PX | 526,374 |
| `54024` | H54-200-S500-R | HX | 501,923 |
| `53768` | H54-100-S500-R | HX | 501,923 |

For the current arm, manual configuration is equivalent to:

```cpp
dxl.setDeviceInfo(1, DYNAMIXEL_SDK_INTERFACE::makePh54DeviceInfo());
dxl.setDeviceInfo(2, DYNAMIXEL_SDK_INTERFACE::makePh54DeviceInfo());
dxl.setDeviceInfo(3, DYNAMIXEL_SDK_INTERFACE::makePh54DeviceInfo());
dxl.setDeviceInfo(4, DYNAMIXEL_SDK_INTERFACE::makeLegacyH54DeviceInfo());
```

Manual metadata is a fallback; automatic model-number detection is safer when exact models can vary. Joint direction, zero offset, and linkage calibration are robot-level properties and must not be embedded in actuator metadata.

`makeCommonXSeriesDeviceInfo()` sets the common 4,096 pulse/rev position resolution but intentionally leaves the current unit unset because it differs across X-series models. Current-to-ampere conversion fails until exact model metadata supplies `current_unit_ampere`.

## General control API

- `broadcastPing()` discovers responding Protocol 2.0 IDs without scanning every ID individually.
- `readControlItem()` and `writeControlItem()` resolve logical control items to family-specific addresses.
- `readControlItems()` and `writeControlItems()` use Bulk Read/Write, so mixed PX/HX groups are supported.
- `writeControlItemsChecked()` sends individual Tx/Rx writes when configuration code must inspect each device response.
- Register width, signedness, and read-only access are validated before transmission.
- Typed convenience functions cover torque, operating mode, watchdog, gains, PWM/current/velocity/position goals, status, and SI-unit conversion.
- `readRegister()`, `writeRegister()`, and `writeRegisterTxOnly()` expose validated raw-register access for items not yet present in the logical enum.
- Legacy address-based and Sync Read/Write functions remain available for compatibility.

## Threading and recovery

- Every operation touching the serial port uses one `std::timed_mutex`.
- Lock contention returns `false` with `DXL_INTERFACE_STATUS::LOCK_TIMEOUT`; it does not wait indefinitely.
- Metadata and last-result access use short-lived `std::mutex` sections because they do not perform I/O.
- `getLastResult()` distinguishes lock, port, communication, device, argument, and unsupported-operation failures.
- `reopenPort()` closes and recreates the SDK `PortHandler`, then reapplies the configured baud rate.
- Object destruction requires all caller threads to have stopped using the interface.

The interface exposes recovery primitives but deliberately does not choose the controller's retry policy. A hardware controller should count consecutive communication failures, stop issuing motion commands, apply bounded backoff, and call `reopenPort()` only after a configured failure count or outage duration. After reconnecting, rediscover or verify every expected ID, restore volatile configuration in a safe order, and only then re-enable torque.

```cpp
int32_t position = 0;
if(!dxl.readPresentPositionGeneral(id, position))
{
    const DXL_INTERFACE_RESULT result = dxl.getLastResult();
    if(result.status == DXL_INTERFACE_STATUS::COMMUNICATION_ERROR
        || result.status == DXL_INTERFACE_STATUS::PORT_NOT_OPEN)
    {
        // Increment the controller's failure counter and enter its safe state.
        // A separate recovery state may call dxl.reopenPort() after the threshold.
    }
}
```
