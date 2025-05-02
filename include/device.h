#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <memory>
#include <iostream>

enum class DeviceState {
    OFF,
    ON,
    ERROR
};

// Base class for all devices in the smart home system
class Device {
protected:
    std::string id;
    std::string name;
    std::string location;
    DeviceState state;
    
public:
    Device(const std::string& id, const std::string& name, const std::string& location);
    virtual ~Device() = default;

    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getLocation() const;
    DeviceState getState() const;

    // Control methods
    virtual bool turnOn();
    virtual bool turnOff();
    virtual void reset();

    // Utility methods
    virtual void displayStatus() const;
    virtual std::string getStateAsString() const;
    
    // Factory method for creating devices
    template <typename T, typename... Args>
    static std::shared_ptr<T> createDevice(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

#endif // DEVICE_H