# Updated 2026-10-07: build the same library and scanner with ROS1 or ROS2.
cmake_minimum_required(VERSION 3.5)
project(dynamixel_sdk_interface VERSION 0.0.0 LANGUAGES CXX)

set(DYNAMIXEL_ROS_VERSION "$ENV{ROS_VERSION}" CACHE STRING "ROS major version: 1 or 2")
if(DYNAMIXEL_ROS_VERSION STREQUAL "1")
  find_package(catkin REQUIRED COMPONENTS lib_functions dynamixel_sdk roscpp)
  catkin_package(
    INCLUDE_DIRS include
    LIBRARIES ${PROJECT_NAME}
    CATKIN_DEPENDS lib_functions dynamixel_sdk)
elseif(DYNAMIXEL_ROS_VERSION STREQUAL "2")
  find_package(ament_cmake REQUIRED)
  find_package(lib_functions REQUIRED)
  find_package(dynamixel_sdk REQUIRED)
  find_package(rclcpp REQUIRED)
else()
  message(FATAL_ERROR "Source the ROS1 or ROS2 setup file before building (ROS_VERSION=1 or 2).")
endif()
find_package(Threads REQUIRED)

# The public interface and communication library do not depend on a ROS client API.
add_library(${PROJECT_NAME} SHARED src/dynamixel_sdk_interface.cpp)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

add_executable(dynamixel_scan_node src/dynamixel_scan_node.cpp)
target_compile_definitions(dynamixel_scan_node PRIVATE DYNAMIXEL_ROS_VERSION=${DYNAMIXEL_ROS_VERSION})
target_link_libraries(dynamixel_scan_node PUBLIC ${PROJECT_NAME})

# Hardware-free control-table/model/unit regression checks.
include(CTest)
if(BUILD_TESTING AND (DYNAMIXEL_ROS_VERSION STREQUAL "2" OR CATKIN_ENABLE_TESTING))
  add_executable(dynamixel_hax_control_table_test test/test_hax.cpp)
  target_link_libraries(dynamixel_hax_control_table_test PRIVATE ${PROJECT_NAME})
  add_test(NAME dynamixel_hax_control_table COMMAND dynamixel_hax_control_table_test)
endif()

if(DYNAMIXEL_ROS_VERSION STREQUAL "1")
  target_include_directories(${PROJECT_NAME} PUBLIC ${catkin_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${dynamixel_sdk_LIBRARIES})
  target_link_libraries(dynamixel_scan_node PUBLIC ${catkin_LIBRARIES})
  if(catkin_EXPORTED_TARGETS)
    add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
  endif()
  install(TARGETS ${PROJECT_NAME} dynamixel_scan_node
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
  install(DIRECTORY include/${PROJECT_NAME}/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
  install(FILES launch/dynamixel_scan.launch
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch)
  install(FILES README.md LICENSE DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
else()
  ament_target_dependencies(${PROJECT_NAME} PUBLIC lib_functions dynamixel_sdk)
  target_compile_features(dynamixel_scan_node PRIVATE cxx_std_17)
  ament_target_dependencies(dynamixel_scan_node PUBLIC rclcpp)
  install(TARGETS ${PROJECT_NAME}
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)
  install(TARGETS dynamixel_scan_node DESTINATION lib/${PROJECT_NAME})
  install(DIRECTORY include/ DESTINATION include)
  install(FILES launch/dynamixel_scan.launch.py DESTINATION share/${PROJECT_NAME}/launch)
  install(FILES README.md LICENSE DESTINATION share/${PROJECT_NAME})
  ament_export_include_directories(include)
  ament_export_libraries(${PROJECT_NAME})
  ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
  ament_export_dependencies(lib_functions dynamixel_sdk)
  ament_package(CONFIG_EXTRAS cmake/dynamixel_sdk_interface-extras.cmake)
endif()
