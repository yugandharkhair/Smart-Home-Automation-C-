#include "../include/smart_home.h"
#include "../include/light.h"
#include "../include/thermostat.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

SmartHome::SmartHome(const std::string& name) : name(name) {
    scheduler = std::make_unique<Scheduler>();
    scheduler->start();
}

SmartHome::~SmartHome() {
    scheduler->stop();
    devices.clear();
}

std::string SmartHome::getName() const {
    return name;
}

std::string SmartHome::generateDeviceId(const std::string& prefix) const {
    int count = 0;
    std::string deviceId;
    
    // Find largest existing ID with this prefix
    for (const auto& pair : devices) {
        const std::string& id = pair.first;
        if (id.find(prefix) == 0) {
            try {
                int num = std::stoi(id.substr(prefix.length()));
                count = std::max(count, num + 1);
            } catch (const std::exception&) {
                // Ignore parsing errors
            }
        }
    }
    
    return prefix + std::to_string(count);
}

bool SmartHome::removeDevice(const std::string& deviceId) {
    auto it = devices.find(deviceId);
    if (it == devices.end()) {
        return false;
    }
    
    devices.erase(it);
    return true;
}

std::shared_ptr<Device> SmartHome::getDevice(const std::string& deviceId) const {
    auto it = devices.find(deviceId);
    if (it == devices.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::shared_ptr<Light>> SmartHome::getLights() const {
    std::vector<std::shared_ptr<Light>> lights;
    
    for (const auto& pair : devices) {
        auto light = std::dynamic_pointer_cast<Light>(pair.second);
        if (light) {
            lights.push_back(light);
        }
    }
    
    return lights;
}

std::vector<std::shared_ptr<Thermostat>> SmartHome::getThermostats() const {
    std::vector<std::shared_ptr<Thermostat>> thermostats;
    
    for (const auto& pair : devices) {
        auto thermostat = std::dynamic_pointer_cast<Thermostat>(pair.second);
        if (thermostat) {
            thermostats.push_back(thermostat);
        }
    }
    
    return thermostats;
}

std::vector<std::shared_ptr<Sensor>> SmartHome::getSensors() const {
    std::vector<std::shared_ptr<Sensor>> sensors;
    
    for (const auto& pair : devices) {
        auto sensor = std::dynamic_pointer_cast<Sensor>(pair.second);
        if (sensor) {
            sensors.push_back(sensor);
        }
    }
    
    return sensors;
}

std::shared_ptr<Light> SmartHome::createLight(const std::string& name, const std::string& location,
                                            bool dimmable, bool colorChangeable) {
    std::string id = generateDeviceId("LIGHT_");
    return addDevice<Light>(id, name, location, dimmable, colorChangeable);
}

std::shared_ptr<Thermostat> SmartHome::createThermostat(const std::string& name, const std::string& location,
                                                      double minTemp, double maxTemp) {
    std::string id = generateDeviceId("THERM_");
    return addDevice<Thermostat>(id, name, location, minTemp, maxTemp);
}

std::shared_ptr<Sensor> SmartHome::createSensor(const std::string& name, const std::string& location,
                                              double minValue, double maxValue, const std::string& unit) {
    std::string id = generateDeviceId("SENSOR_");
    return addDevice<Sensor>(id, name, location, minValue, maxValue, unit);
}

std::vector<std::string> SmartHome::getLocations() const {
    std::vector<std::string> locations;
    
    for (const auto& pair : devices) {
        const std::string& location = pair.second->getLocation();
        
        // Add location if not already in the list
        if (std::find(locations.begin(), locations.end(), location) == locations.end()) {
            locations.push_back(location);
        }
    }
    
    return locations;
}

std::vector<std::shared_ptr<Device>> SmartHome::getDevicesByLocation(const std::string& location) const {
    std::vector<std::shared_ptr<Device>> result;
    
    for (const auto& pair : devices) {
        if (pair.second->getLocation() == location) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

Scheduler& SmartHome::getScheduler() {
    return *scheduler;
}

bool SmartHome::turnOnAllDevices() {
    bool success = true;
    
    for (const auto& pair : devices) {
        if (!pair.second->turnOn()) {
            success = false;
        }
    }
    
    return success;
}

bool SmartHome::turnOffAllDevices() {
    bool success = true;
    
    for (const auto& pair : devices) {
        if (!pair.second->turnOff()) {
            success = false;
        }
    }
    
    return success;
}

bool SmartHome::turnOnLocation(const std::string& location) {
    bool success = true;
    
    for (const auto& pair : devices) {
        if (pair.second->getLocation() == location) {
            if (!pair.second->turnOn()) {
                success = false;
            }
        }
    }
    
    return success;
}

bool SmartHome::turnOffLocation(const std::string& location) {
    bool success = true;
    
    for (const auto& pair : devices) {
        if (pair.second->getLocation() == location) {
            if (!pair.second->turnOff()) {
                success = false;
            }
        }
    }
    
    return success;
}

bool SmartHome::createScheduledTask(const std::string& name, 
                                  std::chrono::system_clock::time_point scheduledTime,
                                  ScheduleFrequency frequency,
                                  std::function<void()> taskFunction) {
    std::string taskId = "TASK_" + std::to_string(scheduler->getAllTasks().size());
    return scheduler->addTask(taskId, name, scheduledTime, frequency, taskFunction);
}

void SmartHome::displaySystemStatus() const {
    std::cout << "=== " << name << " System Status ===" << std::endl;
    std::cout << "Total Devices: " << devices.size() << std::endl;
    
    // Count devices by type
    int lightCount = 0;
    int thermostatCount = 0;
    int sensorCount = 0;
    int otherCount = 0;
    
    for (const auto& pair : devices) {
        if (std::dynamic_pointer_cast<Light>(pair.second)) {
            lightCount++;
        } else if (std::dynamic_pointer_cast<Thermostat>(pair.second)) {
            thermostatCount++;
        } else if (std::dynamic_pointer_cast<Sensor>(pair.second)) {
            sensorCount++;
        } else {
            otherCount++;
        }
    }
    
    std::cout << "  - Lights: " << lightCount << std::endl;
    std::cout << "  - Thermostats: " << thermostatCount << std::endl;
    std::cout << "  - Sensors: " << sensorCount << std::endl;
    if (otherCount > 0) {
        std::cout << "  - Other Devices: " << otherCount << std::endl;
    }
    
    // List all locations
    auto locations = getLocations();
    std::cout << "Locations: " << locations.size() << std::endl;
    for (const auto& location : locations) {
        auto locationDevices = getDevicesByLocation(location);
        std::cout << "  - " << location << ": " << locationDevices.size() << " devices" << std::endl;
    }
    
    // Show scheduled tasks
    auto tasks = scheduler->getAllTasks();
    std::cout << "Scheduled Tasks: " << tasks.size() << std::endl;
    for (const auto& task : tasks) {
        std::cout << "  - " << task->name << " (" << Scheduler::frequencyToString(task->frequency)
                  << "): Next execution at " << Scheduler::timePointToString(task->scheduledTime) << std::endl;
    }
    
    std::cout << "===========================" << std::endl;
}

void SmartHome::displayDeviceStatus(const std::string& deviceId) const {
    auto device = getDevice(deviceId);
    if (!device) {
        std::cout << "Device with ID " << deviceId << " not found." << std::endl;
        return;
    }
    
    device->displayStatus();
}

void SmartHome::displayLocationStatus(const std::string& location) const {
    auto devices = getDevicesByLocation(location);
    if (devices.empty()) {
        std::cout << "No devices found in location: " << location << std::endl;
        return;
    }
    
    std::cout << "=== " << location << " Status ===" << std::endl;
    std::cout << "Total Devices: " << devices.size() << std::endl;
    
    for (const auto& device : devices) {
        std::cout << "\n--- " << device->getName() << " (" << device->getId() << ") ---" << std::endl;
        device->displayStatus();
    }
    
    std::cout << "=======================" << std::endl;
}