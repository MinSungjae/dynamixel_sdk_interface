# dynamixel_sdk_interface

This ROS1/ROS2 package wraps DYNAMIXEL Protocol 2.0 communication behind a thread-safe, model-family-aware C++ API. The communication library and public headers do not use a ROS client API. The scanner selects roscpp or rclcpp at build time.

## Build

This package requires [`lib_functions`](https://github.com/MinSungjae/lib_functions),
which supplies the stream utilities included by the public interface header.
Place it in the same workspace's `src` directory before building. For a new
workspace where the package is not already present:

```bash
cd ~/pibot_ws/src  # Use your own workspace path for ROS1.
git clone https://github.com/MinSungjae/lib_functions.git
```

Use a `lib_functions` revision that supports your selected ROS build system
(catkin for ROS1, ament_cmake for ROS2). The current server uses the ROS1/ROS2
compatible version prepared alongside this interface. It is a header-only
package: declaring the CMake/package dependency supplies the include paths;
no separate `lib_functions` binary library needs to be linked.

Source exactly one ROS environment in a fresh terminal. `ROS_VERSION` selects catkin or ament_cmake in CMake and the format-3 package manifest. Use separate build/install directories for different ROS distributions. `dynamixel_sdk` must be the appropriate ROS1 or ROS2 SDK package; `lib_functions` must also support the selected build system.

ROS1 (Melodic example): place this package, the ROS1 `dynamixel_sdk`, and `lib_functions` in the same catkin workspace, then:

```bash
source /opt/ros/melodic/setup.bash
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

ROS2 (Humble, on the current server):

```bash
source /opt/ros/humble/setup.bash
cd ~/pibot_ws
colcon build --packages-up-to dynamixel_sdk_interface
source install/setup.bash
```

## Use the library in another package

Validation on 2026-10-07: Ubuntu 22.04 / ROS2 Humble package build and install,
external consumers through both ament dependencies and the imported CMake target,
model lookup/error-state smoke tests, and scanner/launch parameter validation
passed. Tests used an empty or nonexistent device path and did not communicate
with motors. The ROS1 branch preserves the Melodic API and build layout, but was
not rebuilt in this session.

Keep the existing C++ API and include path:

```cpp
#include <dynamixel_sdk_interface/dynamixel_sdk_interface.hpp>
```

Add `<depend>dynamixel_sdk_interface</depend>` to the consumer's `package.xml`.
For ROS1:

```cmake
find_package(catkin REQUIRED COMPONENTS roscpp dynamixel_sdk_interface)
catkin_package(CATKIN_DEPENDS dynamixel_sdk_interface)
add_executable(my_node src/my_node.cpp)
target_include_directories(my_node PRIVATE ${catkin_INCLUDE_DIRS})
target_link_libraries(my_node ${catkin_LIBRARIES})
```

For ROS2:

```cmake
find_package(ament_cmake REQUIRED)
find_package(dynamixel_sdk_interface REQUIRED)
add_executable(my_node src/my_node.cpp)
ament_target_dependencies(my_node dynamixel_sdk_interface)
```

Alternatively, ROS2 consumers can link the imported target:

```cmake
target_link_libraries(my_node PRIVATE
  dynamixel_sdk_interface::dynamixel_sdk_interface)
```

The package exports its public headers, shared library, `lib_functions`,
`dynamixel_sdk`, and thread dependency. If the consumer exports public headers
that include this interface, also use `ament_export_dependencies(dynamixel_sdk_interface)`
(ROS2) or `catkin_package(CATKIN_DEPENDS dynamixel_sdk_interface)` (ROS1).

## Scan a DYNAMIXEL bus

ROS2 uses the same node name, parameter names, defaults, and exit statuses.
Use the ROS2 launch file:

```bash
ros2 launch dynamixel_sdk_interface dynamixel_scan.launch.py \
  device_name:=/dev/ttyUSB0 baudrate:=4000000 min_id:=1 max_id:=4
```

Or run directly:

```bash
ros2 run dynamixel_sdk_interface dynamixel_scan_node --ros-args \
  -p device_name:=/dev/ttyUSB0 -p baudrate:=4000000 -p min_id:=1 -p max_id:=4
```

The following `roslaunch` / `rosrun` examples are for ROS1. ROS1 keeps the
original `dynamixel_scan.launch`; ROS2 installs `dynamixel_scan.launch.py`.

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

All four address headers provide typed C++11 constants under `DYNAMIXEL::PX`,
`DYNAMIXEL::HX`, `DYNAMIXEL::XX`, or `DYNAMIXEL::HAX` (for example,
`DYNAMIXEL::HX::GOAL_POSITION` and `DYNAMIXEL::XX::SIZE_GOAL_POSITION`). Each header
can be included on its own. Existing numeric `ADDR_*` / `SIZE_*` macros remain
available, including use in preprocessor conditions.

- `PX`: DYNAMIXEL-P / PRO+ layout
- `HX`: legacy DYNAMIXEL PRO H layout
- `HAX`: DYNAMIXEL PRO H with Advanced firmware, model suffix `R(A)`
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
| `54025` | H54-200-S500-R(A) | HAX | 1,003,846 |
| `53769` | H54-100-S500-R(A) | HAX | 1,003,846 |
| `51201` | H42-20-S300-R(A) | HAX | 607,500 |

### PRO H-series Advanced firmware (R(A))

The new public header is `dynamixel_hax_addresses.hpp`. It is self-contained and
provides both `DYNAMIXEL::HAX::*` constants and `ADDR_HAX_*` / `SIZE_HAX_*` macros,
including external-port and indirect-address/data entries. Common logical control
items resolve through `DXL_MODEL_FAMILY::HAX`; external ports and indirect entries
can use the existing raw-register API.

The official H(A) table differs from legacy `HX`: Torque Enable is 512, Goal
Position is 564, and Present Position is 580. H(A) uses 0.01 rpm per velocity
unit and 1 mA per current unit. H54 R(A) has 1,003,846 pulse/rev; H42 R(A) has
607,500 pulse/rev. H(A) has distinct Current Limit, Goal Current, and Profile
Acceleration/Velocity registers. Registers absent from the H(A) table, including
Protocol Type, Startup Configuration and Backup Ready, are reported unsupported.
The legacy `HX` table and its unit conversions are unchanged.

Select the table from the model number reported by Ping:

```cpp
if (!dxl.detectAndConfigureDevice(id)) {
    // Inspect dxl.getLastResult() before issuing commands.
}
```

For manual H54-100-S500-R(A) configuration:

```cpp
DXL_DEVICE_INFO info;
if (DYNAMIXEL_SDK_INTERFACE::makeDeviceInfoForModelNumber(53769, info)) {
    dxl.setDeviceInfo(id, info);
}
```

`makeHax54DeviceInfo()` and `makeHax42DeviceInfo()` are also available. For raw
register operations only, `dxl.setModelFamily(id, DXL_MODEL_FAMILY::HAX)` selects
the table but does not supply physical-unit conversion metadata. Existing callers
that explicitly select `HX` or use `ADDR_HX_*` must select HAX after a firmware
upgrade; low-level address-based calls are not automatically remapped. This code
supports the upgraded device; it does not install firmware on the actuator.

References: ROBOTIS [H54-100 R(A)](https://emanual.robotis.com/docs/en/dxl/pro/h54-100-s500-ra/),
[H54-200 R(A)](https://emanual.robotis.com/docs/en/dxl/pro/h54-200-s500-ra/),
[H42-20 R(A)](https://emanual.robotis.com/docs/en/dxl/pro/h42-20-s300-ra/).

HAX validation (2026-10-07, Ubuntu 22.04 / ROS2 Humble): package build, CTest,
the standalone C++11 address header, and installed consumers using both ament
dependencies and the imported target passed. Tests check 53 common register
entries against official table values, all three R(A) model mappings, signed
position/current and velocity conversions, unsupported registers/modes, and
legacy HX/PX/XX regressions. No motor communication or firmware flashing was
performed. Run `colcon test --packages-select dynamixel_sdk_interface` to repeat
the hardware-free control-table test.

For the original arm configuration, manual configuration is equivalent to:

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
- `readControlItems()` and `writeControlItems()` use Bulk Read/Write, so mixed PX/HX/HAX groups are supported.
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
