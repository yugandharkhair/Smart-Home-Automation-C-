#ifndef SMART_HOME_H
#define SMART_HOME_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "device.h"
#include "sensor.h"
#include "scheduler.h"

// Forward declarations to avoid circular dependencies
class Light;
class Thermostat;

// Main controller for the smart home system
class SmartHome {
private:
    std::string name;
    std::map<std::string, std::shared_ptr<Device>> devices;
    std::unique_ptr<Scheduler> scheduler;
    
    // Helper function to generate unique device IDs
    std::string generateDeviceId(const std::string& prefix) const;
    
public:
    SmartHome(const std::string& name);
    ~SmartHome();
    
    // Getters
    std::string getName() const;
    
    // Device management
    template <typename T, typename... Args>
    std::shared_ptr<T> addDevice(Args&&... args) {
        auto device = Device::createDevice<T>(std::forward<Args>(args)...);
        devices[device->getId()] = device;
        return std::static_pointer_cast<T>(device);
    }
    
    bool removeDevice(const std::string& deviceId);
    std::shared_ptr<Device> getDevice(const std::string& deviceId) const;
    
    // Type-specific getters
    std::vector<std::shared_ptr<Light>> getLights() const;
    std::vector<std::shared_ptr<Thermostat>> getThermostats() const;
    std::vector<std::shared_ptr<Sensor>> getSensors() const;
    
    // Convenience methods
    std::shared_ptr<Light> createLight(const std::string& name, const std::string& location,
                                     bool dimmable = true, bool colorChangeable = false);
                                     
    std::shared_ptr<Thermostat> createThermostat(const std::string& name, const std::string& location,
                                               double minTemp = 10.0, double maxTemp = 32.0);
                                               
    std::shared_ptr<Sensor> createSensor(const std::string& name, const std::string& location,
                                       double minValue, double maxValue, const std::string& unit);
    
    // Room and location management
    std::vector<std::string> getLocations() const;
    std::vector<std::shared_ptr<Device>> getDevicesByLocation(const std::string& location) const;
    
    // Scheduler access
    Scheduler& getScheduler();
    
    // Control methods
    bool turnOnAllDevices();
    bool turnOffAllDevices();
    bool turnOnLocation(const std::string& location);
    bool turnOffLocation(const std::string& location);
    
    // Automation
    bool createScheduledTask(const std::string& name, 
                           std::chrono::system_clock::time_point scheduledTime,
                           ScheduleFrequency frequency,
                           std::function<void()> taskFunction);
    
    // Status reporting
    void displaySystemStatus() const;
    void displayDeviceStatus(const std::string& deviceId) const;
    void displayLocationStatus(const std::string& location) const;
};

#endif // SMART_HOME_H