#ifndef __DYNAMIXEL_PX_ADDRESSES_HPP__
#define __DYNAMIXEL_PX_ADDRESSES_HPP__

namespace DYNAMIXEL{
    namespace PX {
        constexpr uint16_t MODEL_NUMBER           = 0;
        constexpr uint16_t MODEL_INFORMATION      = 2;
        constexpr uint16_t FIRMWARE_VERSION       = 6;
        constexpr uint16_t ID                     = 7;
        constexpr uint16_t BAUD_RATE              = 8;
        constexpr uint16_t RETURN_DELAY_TIME      = 9;
        constexpr uint16_t DRIVE_MODE             = 10;
        constexpr uint16_t OPERATING_MODE         = 11;
        constexpr uint16_t SECONDARY_ID           = 12;
        constexpr uint16_t PROTOCOL_TYPE          = 13;
        constexpr uint16_t HOMING_OFFSET          = 20;
        constexpr uint16_t MOVING_THRESHOLD       = 24;
        constexpr uint16_t TEMPERATURE_LIMIT      = 31;
        constexpr uint16_t MAX_VOLTAGE_LIMIT      = 32;
        constexpr uint16_t MIN_VOLTAGE_LIMIT      = 34;
        constexpr uint16_t PWM_LIMIT              = 36;
        constexpr uint16_t CURRENT_LIMIT          = 38;
        constexpr uint16_t ACCELERATION_LIMIT     = 40;
        constexpr uint16_t VELOCITY_LIMIT         = 44;
        constexpr uint16_t MAX_POSITION_LIMIT     = 48;
        constexpr uint16_t MIN_POSITION_LIMIT     = 52;
        constexpr uint16_t EXTERNAL_PORT_MODE_1   = 56;
        constexpr uint16_t EXTERNAL_PORT_MODE_2   = 57;
        constexpr uint16_t EXTERNAL_PORT_MODE_3   = 58;
        constexpr uint16_t EXTERNAL_PORT_MODE_4   = 59;
        constexpr uint16_t STARTUP_CONFIGURATION  = 60;
        constexpr uint16_t SHUTDOWN               = 63;
        constexpr uint16_t TORQUE_ENABLE          = 512;
        constexpr uint16_t LED_RED                = 513;
        constexpr uint16_t LED_GREEN              = 514;
        constexpr uint16_t LED_BLUE               = 515;
        constexpr uint16_t STATUS_RETURN_LEVEL    = 516;
        constexpr uint16_t REGISTERED_INSTRUCTION = 517;
        constexpr uint16_t HARDWARE_ERROR_STATUS  = 518;
        constexpr uint16_t VELOCITY_I_GAIN        = 524;
        constexpr uint16_t VELOCITY_P_GAIN        = 526;
        constexpr uint16_t POSITION_D_GAIN        = 528;
        constexpr uint16_t POSITION_I_GAIN        = 530;
        constexpr uint16_t POSITION_P_GAIN        = 532;
        constexpr uint16_t FEEDFORWARD_2ND_GAIN   = 536;
        constexpr uint16_t FEEDFORWARD_1ST_GAIN   = 538;
        constexpr uint16_t BUS_WATCHDOG           = 546;
        constexpr uint16_t GOAL_PWM               = 548;
        constexpr uint16_t GOAL_CURRENT           = 550;
        constexpr uint16_t GOAL_VELOCITY          = 552;
        constexpr uint16_t PROFILE_ACCELERATION   = 556;
        constexpr uint16_t PROFILE_VELOCITY       = 560;
        constexpr uint16_t GOAL_POSITION          = 564;
        constexpr uint16_t REALTIME_TICK          = 568;
        constexpr uint16_t MOVING                 = 570;
        constexpr uint16_t MOVING_STATUS          = 571;
        constexpr uint16_t PRESENT_PWM            = 572;
        constexpr uint16_t PRESENT_CURRENT        = 574;
        constexpr uint16_t PRESENT_VELOCITY       = 576;
        constexpr uint16_t PRESENT_POSITION       = 580;
        constexpr uint16_t VELOCITY_TRAJECTORY    = 584;
        constexpr uint16_t POSITION_TRAJECTORY    = 588;
        constexpr uint16_t PRESENT_INPUT_VOLTAGE  = 592;
        constexpr uint16_t PRESENT_TEMPERATURE    = 594;

        constexpr uint8_t SIZE_MODEL_NUMBER            = 2;
        constexpr uint8_t SIZE_MODEL_INFORMATION       = 4;
        constexpr uint8_t SIZE_FIRMWARE_VERSION        = 1;
        constexpr uint8_t SIZE_ID                      = 1;
        constexpr uint8_t SIZE_BAUD_RATE               = 1;
        constexpr uint8_t SIZE_RETURN_DELAY_TIME       = 1;
        constexpr uint8_t SIZE_DRIVE_MODE              = 1;
        constexpr uint8_t SIZE_OPERATING_MODE          = 1;
        constexpr uint8_t SIZE_SECONDARY_ID            = 1;
        constexpr uint8_t SIZE_PROTOCOL_TYPE           = 1;
        constexpr uint8_t SIZE_HOMING_OFFSET           = 4;
        constexpr uint8_t SIZE_MOVING_THRESHOLD        = 4;
        constexpr uint8_t SIZE_TEMPERATURE_LIMIT       = 1;
        constexpr uint8_t SIZE_MAX_VOLTAGE_LIMIT       = 2;
        constexpr uint8_t SIZE_MIN_VOLTAGE_LIMIT       = 2;
        constexpr uint8_t SIZE_PWM_LIMIT               = 2;
        constexpr uint8_t SIZE_CURRENT_LIMIT           = 2;
        constexpr uint8_t SIZE_ACCELERATION_LIMIT      = 4;
        constexpr uint8_t SIZE_VELOCITY_LIMIT          = 4;
        constexpr uint8_t SIZE_MAX_POSITION_LIMIT      = 4;
        constexpr uint8_t SIZE_MIN_POSITION_LIMIT      = 4;
        constexpr uint8_t SIZE_EXTERNAL_PORT_MODE_1    = 1;
        constexpr uint8_t SIZE_EXTERNAL_PORT_MODE_2    = 1;
        constexpr uint8_t SIZE_EXTERNAL_PORT_MODE_3    = 1;
        constexpr uint8_t SIZE_EXTERNAL_PORT_MODE_4    = 1;
        constexpr uint8_t SIZE_STARTUP_CONFIGURATION   = 1;
        constexpr uint8_t SIZE_SHUTDOWN                = 1;
        constexpr uint8_t SIZE_TORQUE_ENABLE           = 1;
        constexpr uint8_t SIZE_LED_RED                 = 1;
        constexpr uint8_t SIZE_LED_GREEN               = 1;
        constexpr uint8_t SIZE_LED_BLUE                = 1;
        constexpr uint8_t SIZE_STATUS_RETURN_LEVEL     = 1;
        constexpr uint8_t SIZE_REGISTERED_INSTRUCTION  = 1;
        constexpr uint8_t SIZE_HARDWARE_ERROR_STATUS   = 1;
        constexpr uint8_t SIZE_VELOCITY_I_GAIN         = 2;
        constexpr uint8_t SIZE_VELOCITY_P_GAIN         = 2;
        constexpr uint8_t SIZE_POSITION_D_GAIN         = 2;
        constexpr uint8_t SIZE_POSITION_I_GAIN         = 2;
        constexpr uint8_t SIZE_POSITION_P_GAIN         = 2;
        constexpr uint8_t SIZE_FEEDFORWARD_2ND_GAIN    = 2;
        constexpr uint8_t SIZE_FEEDFORWARD_1ST_GAIN    = 2;
        constexpr uint8_t SIZE_BUS_WATCHDOG            = 1;
        constexpr uint8_t SIZE_GOAL_PWM                = 2;
        constexpr uint8_t SIZE_GOAL_CURRENT            = 2;
        constexpr uint8_t SIZE_GOAL_VELOCITY           = 4;
        constexpr uint8_t SIZE_PROFILE_ACCELERATION    = 4;
        constexpr uint8_t SIZE_PROFILE_VELOCITY        = 4;
        constexpr uint8_t SIZE_GOAL_POSITION           = 4;
        constexpr uint8_t SIZE_REALTIME_TICK           = 2;
        constexpr uint8_t SIZE_MOVING                  = 1;
        constexpr uint8_t SIZE_MOVING_STATUS           = 1;
        constexpr uint8_t SIZE_PRESENT_PWM             = 2;
        constexpr uint8_t SIZE_PRESENT_CURRENT         = 2;
        constexpr uint8_t SIZE_PRESENT_VELOCITY        = 4;
        constexpr uint8_t SIZE_PRESENT_POSITION        = 4;
        constexpr uint8_t SIZE_VELOCITY_TRAJECTORY     = 4;
        constexpr uint8_t SIZE_POSITION_TRAJECTORY     = 4;
        constexpr uint8_t SIZE_PRESENT_INPUT_VOLTAGE   = 2;
        constexpr uint8_t SIZE_PRESENT_TEMPERATURE     = 1;
    }
}

// ROM Area
#define ADDR_PX_MODEL_NUMBER                0    
#define ADDR_PX_MODEL_INFORMATION           2        
#define ADDR_PX_FIRMWARE_VERSION            6        
#define ADDR_PX_ID                          7
#define ADDR_PX_BAUD_RATE                   8
#define ADDR_PX_RETURN_DELAY_TIME           9        
#define ADDR_PX_DRIVE_MODE                  10
#define ADDR_PX_OPERATING_MODE              11    
#define ADDR_PX_SECONDARY_ID                12            
#define ADDR_PX_PROTOCOL_TYPE               13    
#define ADDR_PX_HOMING_OFFSET               20    
#define ADDR_PX_MOVING_THRESHOLD            24        
#define ADDR_PX_TEMPERATURE_LIMIT           31        
#define ADDR_PX_MAX_VOLTAGE_LIMIT           32        
#define ADDR_PX_MIN_VOLTAGE_LIMIT           34        
#define ADDR_PX_PWM_LIMIT                   36
#define ADDR_PX_CURRENT_LIMIT               38    
#define ADDR_PX_ACCELERATION_LIMIT          40        
#define ADDR_PX_VELOCITY_LIMIT              44    
#define ADDR_PX_MAX_POSITION_LIMIT          48        
#define ADDR_PX_MIN_POSITION_LIMIT          52        
#define ADDR_PX_EXTERNAL_PORT_MODE_1        56            
#define ADDR_PX_EXTERNAL_PORT_MODE_2        57            
#define ADDR_PX_EXTERNAL_PORT_MODE_3        58            
#define ADDR_PX_EXTERNAL_PORT_MODE_4        59            
#define ADDR_PX_STARTUP_CONFIGURATION       60            
#define ADDR_PX_SHUTDOWN                    63

#define SIZE_PX_MODEL_NUMBER                2    
#define SIZE_PX_MODEL_INFORMATION           4        
#define SIZE_PX_FIRMWARE_VERSION            1        
#define SIZE_PX_ID                          1
#define SIZE_PX_BAUD_RATE                   1
#define SIZE_PX_RETURN_DELAY_TIME           1        
#define SIZE_PX_DRIVE_MODE                  1
#define SIZE_PX_OPERATING_MODE              1    
#define SIZE_PX_SECONDARY_ID                1            
#define SIZE_PX_PROTOCOL_TYPE               1    
#define SIZE_PX_HOMING_OFFSET               4    
#define SIZE_PX_MOVING_THRESHOLD            4        
#define SIZE_PX_TEMPERATURE_LIMIT           1        
#define SIZE_PX_MAX_VOLTAGE_LIMIT           2        
#define SIZE_PX_MIN_VOLTAGE_LIMIT           2        
#define SIZE_PX_PWM_LIMIT                   2
#define SIZE_PX_CURRENT_LIMIT               2    
#define SIZE_PX_ACCELERATION_LIMIT          4        
#define SIZE_PX_VELOCITY_LIMIT              4    
#define SIZE_PX_MAX_POSITION_LIMIT          4        
#define SIZE_PX_MIN_POSITION_LIMIT          4        
#define SIZE_PX_EXTERNAL_PORT_MODE_1        1            
#define SIZE_PX_EXTERNAL_PORT_MODE_2        1            
#define SIZE_PX_EXTERNAL_PORT_MODE_3        1            
#define SIZE_PX_EXTERNAL_PORT_MODE_4        1            
#define SIZE_PX_STARTUP_CONFIGURATION       1            
#define SIZE_PX_SHUTDOWN                    1

// RAM Area
#define ADDR_PX_TORQUE_ENABLE               512
#define ADDR_PX_LED_RED                     513
#define ADDR_PX_LED_GREEN                   514
#define ADDR_PX_LED_BLUE                    515
#define ADDR_PX_STATUS_RETURN_LEVEL         516
#define ADDR_PX_REGISTERED_INSTRUCTION      517
#define ADDR_PX_HARDWARE_ERROR_STATUS       518
#define ADDR_PX_VELOCITY_I_GAIN             524
#define ADDR_PX_VELOCITY_P_GAIN             526
#define ADDR_PX_POSITION_D_GAIN             528
#define ADDR_PX_POSITION_I_GAIN             530
#define ADDR_PX_POSITION_P_GAIN             532
#define ADDR_PX_FEEDFORWARD_2ND_GAIN        536
#define ADDR_PX_FEEDFORWARD_1ST_GAIN        538
#define ADDR_PX_BUS_WATCHDOG                546
#define ADDR_PX_GOAL_PWM                    548
#define ADDR_PX_GOAL_CURRENT                550
#define ADDR_PX_GOAL_VELOCITY               552
#define ADDR_PX_PROFILE_ACCELERATION        556
#define ADDR_PX_PROFILE_VELOCITY            560
#define ADDR_PX_GOAL_POSITION               564
#define ADDR_PX_REALTIME_TICK               568
#define ADDR_PX_MOVING                      570
#define ADDR_PX_MOVING_STATUS               571
#define ADDR_PX_PRESENT_PWM                 572
#define ADDR_PX_PRESENT_CURRENT             574
#define ADDR_PX_PRESENT_VELOCITY            576
#define ADDR_PX_PRESENT_POSITION            580
#define ADDR_PX_VELOCITY_TRAJECTORY         584
#define ADDR_PX_POSITION_TRAJECTORY         588
#define ADDR_PX_PRESENT_INPUT_VOLTAGE       592
#define ADDR_PX_PRESENT_TEMPERATURE         594

#define SIZE_PX_TORQUE_ENABLE               1
#define SIZE_PX_LED_RED                     1
#define SIZE_PX_LED_GREEN                   1
#define SIZE_PX_LED_BLUE                    1
#define SIZE_PX_STATUS_RETURN_LEVEL         1
#define SIZE_PX_REGISTERED_INSTRUCTION      1
#define SIZE_PX_HARDWARE_ERROR_STATUS       1
#define SIZE_PX_VELOCITY_I_GAIN             2
#define SIZE_PX_VELOCITY_P_GAIN             2
#define SIZE_PX_POSITION_D_GAIN             2
#define SIZE_PX_POSITION_I_GAIN             2
#define SIZE_PX_POSITION_P_GAIN             2
#define SIZE_PX_FEEDFORWARD_2ND_GAIN        2
#define SIZE_PX_FEEDFORWARD_1ST_GAIN        2
#define SIZE_PX_BUS_WATCHDOG                1
#define SIZE_PX_GOAL_PWM                    2
#define SIZE_PX_GOAL_CURRENT                2
#define SIZE_PX_GOAL_VELOCITY               4
#define SIZE_PX_PROFILE_ACCELERATION        4
#define SIZE_PX_PROFILE_VELOCITY            4
#define SIZE_PX_GOAL_POSITION               4
#define SIZE_PX_REALTIME_TICK               2
#define SIZE_PX_MOVING                      1
#define SIZE_PX_MOVING_STATUS               1
#define SIZE_PX_PRESENT_PWM                 2
#define SIZE_PX_PRESENT_CURRENT             2
#define SIZE_PX_PRESENT_VELOCITY            4
#define SIZE_PX_PRESENT_POSITION            4
#define SIZE_PX_VELOCITY_TRAJECTORY         4
#define SIZE_PX_POSITION_TRAJECTORY         4
#define SIZE_PX_PRESENT_INPUT_VOLTAGE       2
#define SIZE_PX_PRESENT_TEMPERATURE         1

#endif