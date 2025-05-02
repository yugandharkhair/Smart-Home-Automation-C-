# Smart Home Automation System

A C++ object-oriented project demonstrating an intermediate-level implementation of a smart home control system.

## Overview

This project implements a flexible and extensible smart home automation system using modern C++ concepts. It models various smart devices (lights, thermostats, sensors) and allows controlling them through a central management system with scheduling capabilities.

## Features

- **Object-Oriented Design**: Inheritance hierarchy with a base `Device` class and specialized device implementations
- **Modern C++ Features**: Smart pointers, templates, lambda functions, and multi-threading
- **Device Control**: Turn devices on/off, adjust settings (brightness, temperature, etc.)
- **Location Management**: Group devices by location (rooms)
- **Scheduling System**: Create timed tasks with various frequency options
- **Sensor Simulation**: Simulated sensor readings with callbacks
- **Interactive UI**: Console-based interface for system control

## Project Structure

```
SmartHomeAutomation/
├── include/               # Header files
│   ├── device.h           # Base device class
│   ├── sensor.h           # Sensor device class
│   ├── light.h            # Light device class
│   ├── thermostat.h       # Thermostat device class
│   ├── scheduler.h        # Task scheduler
│   └── smart_home.h       # Main system controller
├── src/                   # Implementation files
│   ├── device.cpp
│   ├── sensor.cpp
│   ├── light.cpp
│   ├── thermostat.cpp
│   ├── scheduler.cpp
│   ├── smart_home.cpp
│   └── main.cpp           # Main application entry
└── CMakeLists.txt         # CMake build configuration
```

## Requirements

- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake (version 3.10 or higher)
- pthread support

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/SmartHomeAutomation.git
cd SmartHomeAutomation
```

2. Create a build directory and run CMake:
```bash
mkdir build
cd build
cmake ..
```

3. Build the project:
```bash
make
```

4. Run the application:
```bash
./SmartHome
```

## Usage

The application provides a text-based menu with the following options:
1. Add a new device (Light, Thermostat, Sensor)
2. Control a device (turn on/off, adjust settings)
3. Create automation schedules
4. View system status
5. View location status
6. Run demo (demonstrates various system features)

### Example: Adding a Light Device

1. Select option 1 (Add a new device)
2. Choose device type 1 (Light)
3. Enter device name and location
4. Specify if the light is dimmable and color-changeable

### Example: Creating a Schedule

1. Select option 3 (Create automation schedule)
2. Enter schedule name and frequency
3. Set time for the schedule
4. Choose action to perform (turn on/off devices)

## Design Patterns

- **Factory Method Pattern**: Used for device creation
- **Observer Pattern**: Used for sensor notifications
- **Singleton-like Pattern**: Central SmartHome controller

## Technologies Used

- C++17
- Smart pointers for memory management
- Multi-threading for concurrent operations
- CMake build system

## Future Improvements

- Persistent storage for system configuration
- Network connectivity for remote control
- GUI interface
- Support for additional device types
- Integration with actual IoT protocols

## License

This project is licensed under the MIT License - see the LICENSE file for details.
