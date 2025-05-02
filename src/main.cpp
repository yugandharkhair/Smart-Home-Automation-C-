#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "../include/smart_home.h"
#include "../include/light.h"
#include "../include/thermostat.h"
#include "../include/sensor.h"

void displayMenu() {
    std::cout << "\n===== Smart Home Automation System =====\n";
    std::cout << "1. Add a new device\n";
    std::cout << "2. Control a device\n";
    std::cout << "3. Create automation schedule\n";
    std::cout << "4. View system status\n";
    std::cout << "5. View location status\n";
    std::cout << "6. Run demo\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

void runDemo(SmartHome& home) {
    std::cout << "\n==== Running Smart Home Demo ====\n";
    
    // Create devices in different locations
    std::cout << "Creating devices...\n";
    auto livingRoomLight = home.createLight("Living Room Main Light", "Living Room", true, true);
    auto kitchenLight = home.createLight("Kitchen Light", "Kitchen", true, false);
    auto bedroomLight = home.createLight("Bedroom Light", "Bedroom", true, true);
    
    auto livingRoomThermostat = home.createThermostat("Living Room Thermostat", "Living Room");
    
    auto tempSensor = home.createSensor("Temperature Sensor", "Living Room", 10.0, 30.0, "°C");
    
    // Connect sensor to thermostat
    std::cout << "Connecting temperature sensor to thermostat...\n";
    livingRoomThermostat->connectSensor(tempSensor);
    
    // Start sensor monitoring
    std::cout << "Starting temperature monitoring...\n";
    tempSensor->startMonitoring();
    
    // Display initial system status
    std::cout << "\nInitial system status:\n";
    home.displaySystemStatus();
    
    // Turn on living room devices
    std::cout << "\nTurning on living room devices...\n";
    home.turnOnLocation("Living Room");
    
    // Configure thermostat
    std::cout << "Setting thermostat to 23°C in heating mode...\n";
    livingRoomThermostat->setTargetTemperature(23.0);
    livingRoomThermostat->setMode(ThermostatMode::HEAT);
    
    // Configure living room light
    std::cout << "Setting living room light to warm white at 80% brightness...\n";
    livingRoomLight->setBrightness(80);
    livingRoomLight->setColor(LightColor::WARM_WHITE);
    
    // Short delay to allow the sensor to update
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Display living room status
    std::cout << "\nLiving room status after configuration:\n";
    home.displayLocationStatus("Living Room");
    
    // Create a scheduled task to turn off lights at night
    std::cout << "\nCreating a scheduled task to turn off lights at 11:00 PM daily...\n";
    
    // Get current time and set it to 11:00 PM for demo purposes
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&timeT);
    timeInfo->tm_hour = 23;
    timeInfo->tm_min = 0;
    timeInfo->tm_sec = 0;
    auto scheduledTime = std::chrono::system_clock::from_time_t(std::mktime(timeInfo));
    
    home.createScheduledTask("Turn Off All Lights", scheduledTime, ScheduleFrequency::DAILY, [&home]() {
        std::cout << "Scheduled task executed: Turning off all lights\n";
        
        for (auto light : home.getLights()) {
            light->turnOff();
        }
    });
    
    // Create a scene for "Movie Night"
    std::cout << "\nCreating a 'Movie Night' scene...\n";
    auto movieNightScene = [&livingRoomLight, &livingRoomThermostat]() {
        std::cout << "Activating 'Movie Night' scene\n";
        
        livingRoomLight->setBrightness(30);
        livingRoomLight->setColor(LightColor::WARM_WHITE);
        livingRoomThermostat->setTargetTemperature(22.0);
    };
    
    std::cout << "Activating 'Movie Night' scene...\n";
    movieNightScene();
    
    // Short delay to see the effect
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Display final system status
    std::cout << "\nFinal system status:\n";
    home.displaySystemStatus();
    
    std::cout << "\nDemo completed!\n";
}

int main() {
    std::cout << "Welcome to Smart Home Automation System\n";
    
    // Create a new smart home system
    SmartHome home("My Smart Home");
    
    bool running = true;
    while (running) {
        displayMenu();
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        switch (choice) {
            case 0:
                running = false;
                std::cout << "Exiting system. Goodbye!\n";
                break;
                
            case 1: {
                std::cout << "\n=== Add a New Device ===\n";
                std::cout << "Device type:\n";
                std::cout << "1. Light\n";
                std::cout << "2. Thermostat\n";
                std::cout << "3. Sensor\n";
                std::cout << "Choice: ";
                
                int deviceType;
                std::cin >> deviceType;
                std::cin.ignore();
                
                std::string name, location;
                
                std::cout << "Device name: ";
                std::getline(std::cin, name);
                
                std::cout << "Location: ";
                std::getline(std::cin, location);
                
                switch (deviceType) {
                    case 1: {
                        bool dimmable, colorChangeable;
                        
                        std::cout << "Dimmable (1=yes, 0=no): ";
                        std::cin >> dimmable;
                        
                        std::cout << "Color changeable (1=yes, 0=no): ";
                        std::cin >> colorChangeable;
                        
                        auto light = home.createLight(name, location, dimmable, colorChangeable);
                        std::cout << "Light created with ID: " << light->getId() << std::endl;
                        break;
                    }
                        
                    case 2: {
                        double minTemp, maxTemp;
                        
                        std::cout << "Minimum temperature: ";
                        std::cin >> minTemp;
                        
                        std::cout << "Maximum temperature: ";
                        std::cin >> maxTemp;
                        
                        auto thermostat = home.createThermostat(name, location, minTemp, maxTemp);
                        std::cout << "Thermostat created with ID: " << thermostat->getId() << std::endl;
                        break;
                    }
                        
                    case 3: {
                        double minValue, maxValue;
                        std::string unit;
                        
                        std::cout << "Minimum value: ";
                        std::cin >> minValue;
                        
                        std::cout << "Maximum value: ";
                        std::cin >> maxValue;
                        
                        std::cin.ignore();
                        std::cout << "Unit: ";
                        std::getline(std::cin, unit);
                        
                        auto sensor = home.createSensor(name, location, minValue, maxValue, unit);
                        std::cout << "Sensor created with ID: " << sensor->getId() << std::endl;
                        break;
                    }
                        
                    default:
                        std::cout << "Invalid device type.\n";
                        break;
                }
                
                break;
            }
                
            case 2: {
                std::cout << "\n=== Control a Device ===\n";
                std::string deviceId;
                
                std::cout << "Enter device ID: ";
                std::getline(std::cin, deviceId);
                
                auto device = home.getDevice(deviceId);
                if (!device) {
                    std::cout << "Device not found.\n";
                    break;
                }
                
                std::cout << "Current status:\n";
                device->displayStatus();
                
                std::cout << "\nAction:\n";
                std::cout << "1. Turn On\n";
                std::cout << "2. Turn Off\n";
                
                auto light = std::dynamic_pointer_cast<Light>(device);
                auto thermostat = std::dynamic_pointer_cast<Thermostat>(device);
                auto sensor = std::dynamic_pointer_cast<Sensor>(device);
                
                if (light) {
                    std::cout << "3. Set Brightness\n";
                    if (light->isColorChangeable()) {
                        std::cout << "4. Change Color\n";
                    }
                } else if (thermostat) {
                    std::cout << "3. Set Temperature\n";
                    std::cout << "4. Change Mode\n";
                } else if (sensor) {
                    std::cout << "3. Start Monitoring\n";
                    std::cout << "4. Stop Monitoring\n";
                }
                
                std::cout << "Choice: ";
                int action;
                std::cin >> action;
                
                switch (action) {
                    case 1:
                        if (device->turnOn()) {
                            std::cout << "Device turned on.\n";
                        } else {
                            std::cout << "Failed to turn on device.\n";
                        }
                        break;
                        
                    case 2:
                        if (device->turnOff()) {
                            std::cout << "Device turned off.\n";
                        } else {
                            std::cout << "Failed to turn off device.\n";
                        }
                        break;
                        
                    case 3:
                        if (light) {
                            int brightness;
                            std::cout << "Enter brightness (0-100): ";
                            std::cin >> brightness;
                            
                            if (light->setBrightness(brightness)) {
                                std::cout << "Brightness set to " << brightness << "%.\n";
                            } else {
                                std::cout << "Failed to set brightness.\n";
                            }
                        } else if (thermostat) {
                            double temperature;
                            std::cout << "Enter target temperature: ";
                            std::cin >> temperature;
                            
                            if (thermostat->setTargetTemperature(temperature)) {
                                std::cout << "Target temperature set to " << temperature << "°C.\n";
                            } else {
                                std::cout << "Failed to set temperature.\n";
                            }
                        } else if (sensor) {
                            sensor->startMonitoring();
                            std::cout << "Sensor monitoring started.\n";
                        }
                        break;
                        
                    case 4:
                        if (light && light->isColorChangeable()) {
                            std::cout << "Select color:\n";
                            std::cout << "1. White\n";
                            std::cout << "2. Warm White\n";
                            std::cout << "3. Red\n";
                            std::cout << "4. Green\n";
                            std::cout << "5. Blue\n";
                            std::cout << "6. Yellow\n";
                            std::cout << "7. Purple\n";
                            std::cout << "8. Custom\n";
                            
                            int colorChoice;
                            std::cout << "Choice: ";
                            std::cin >> colorChoice;
                            
                            if (colorChoice >= 1 && colorChoice <= 7) {
                                LightColor color = static_cast<LightColor>(colorChoice - 1);
                                if (light->setColor(color)) {
                                    std::cout << "Color changed.\n";
                                } else {
                                    std::cout << "Failed to change color.\n";
                                }
                            } else if (colorChoice == 8) {
                                int r, g, b;
                                std::cout << "Enter RGB values (0-255):\n";
                                std::cout << "Red: ";
                                std::cin >> r;
                                std::cout << "Green: ";
                                std::cin >> g;
                                std::cout << "Blue: ";
                                std::cin >> b;
                                
                                if (light->setCustomColor(r, g, b)) {
                                    std::cout << "Custom color set.\n";
                                } else {
                                    std::cout << "Failed to set custom color.\n";
                                }
                            } else {
                                std::cout << "Invalid color choice.\n";
                            }
                        } else if (thermostat) {
                            std::cout << "Select mode:\n";
                            std::cout << "1. Off\n";
                            std::cout << "2. Heat\n";
                            std::cout << "3. Cool\n";
                            std::cout << "4. Auto\n";
                            std::cout << "5. Fan Only\n";
                            
                            int modeChoice;
                            std::cout << "Choice: ";
                            std::cin >> modeChoice;
                            
                            if (modeChoice >= 1 && modeChoice <= 5) {
                                ThermostatMode mode = static_cast<ThermostatMode>(modeChoice - 1);
                                if (thermostat->setMode(mode)) {
                                    std::cout << "Mode changed.\n";
                                } else {
                                    std::cout << "Failed to change mode.\n";
                                }
                            } else {
                                std::cout << "Invalid mode choice.\n";
                            }
                        } else if (sensor) {
                            sensor->stopMonitoring();
                            std::cout << "Sensor monitoring stopped.\n";
                        }
                        break;
                        
                    default:
                        std::cout << "Invalid action.\n";
                        break;
                }
                
                break;
            }
                
            case 3: {
                std::cout << "\n=== Create Automation Schedule ===\n";
                std::string scheduleName;
                int frequency;
                
                std::cout << "Schedule name: ";
                std::getline(std::cin, scheduleName);
                
                std::cout << "Frequency:\n";
                std::cout << "1. Once\n";
                std::cout << "2. Daily\n";
                std::cout << "3. Weekly\n";
                std::cout << "4. Monthly\n";
                std::cout << "Choice: ";
                std::cin >> frequency;
                
                if (frequency < 1 || frequency > 4) {
                    std::cout << "Invalid frequency choice.\n";
                    break;
                }
                
                ScheduleFrequency scheduleFreq = static_cast<ScheduleFrequency>(frequency - 1);
                
                int hour, minute;
                std::cout << "Time (24-hour format):\n";
                std::cout << "Hour (0-23): ";
                std::cin >> hour;
                
                std::cout << "Minute (0-59): ";
                std::cin >> minute;
                
                if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
                    std::cout << "Invalid time.\n";
                    break;
                }
                
                // Get current time and set the hour/minute
                auto now = std::chrono::system_clock::now();
                auto timeT = std::chrono::system_clock::to_time_t(now);
                std::tm* timeInfo = std::localtime(&timeT);
                timeInfo->tm_hour = hour;
                timeInfo->tm_min = minute;
                timeInfo->tm_sec = 0;
                auto scheduledTime = std::chrono::system_clock::from_time_t(std::mktime(timeInfo));
                
                // If the scheduled time has already passed today, move it to tomorrow
                if (scheduledTime <= now) {
                    timeInfo->tm_mday += 1;
                    scheduledTime = std::chrono::system_clock::from_time_t(std::mktime(timeInfo));
                }
                
                std::cout << "\nAction:\n";
                std::cout << "1. Turn on all devices\n";
                std::cout << "2. Turn off all devices\n";
                std::cout << "3. Turn on devices in a location\n";
                std::cout << "4. Turn off devices in a location\n";
                
                int actionChoice;
                std::cout << "Choice: ";
                std::cin >> actionChoice;
                std::cin.ignore();
                
                std::function<void()> taskFunction;
                
                switch (actionChoice) {
                    case 1:
                        taskFunction = [&home]() {
                            home.turnOnAllDevices();
                        };
                        break;
                        
                    case 2:
                        taskFunction = [&home]() {
                            home.turnOffAllDevices();
                        };
                        break;
                        
                    case 3: {
                        std::string location;
                        std::cout << "Enter location: ";
                        std::getline(std::cin, location);
                        
                        taskFunction = [&home, location]() {
                            home.turnOnLocation(location);
                        };
                        break;
                    }
                        
                    case 4: {
                        std::string location;
                        std::cout << "Enter location: ";
                        std::getline(std::cin, location);
                        
                        taskFunction = [&home, location]() {
                            home.turnOffLocation(location);
                        };
                        break;
                    }
                        
                    default:
                        std::cout << "Invalid action choice.\n";
                        break;
                }
                
                if (taskFunction) {
                    if (home.createScheduledTask(scheduleName, scheduledTime, scheduleFreq, taskFunction)) {
                        std::cout << "Scheduled task created successfully.\n";
                    } else {
                        std::cout << "Failed to create scheduled task.\n";
                    }
                }
                
                break;
            }
                
            case 4:
                std::cout << "\n=== System Status ===\n";
                home.displaySystemStatus();
                break;
                
            case 5: {
                std::cout << "\n=== Location Status ===\n";
                std::string location;
                
                // Show available locations
                auto locations = home.getLocations();
                std::cout << "Available locations:\n";
                for (const auto& loc : locations) {
                    std::cout << "- " << loc << std::endl;
                }
                
                std::cout << "Enter location: ";
                std::getline(std::cin, location);
                
                home.displayLocationStatus(location);
                break;
            }
                
            case 6:
                runDemo(home);
                break;
                
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
    
    return 0;
}