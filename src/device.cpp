#include "../include/device.h"

Device::Device(const std::string& id, const std::string& name, const std::string& location)
    : id(id), name(name), location(location), state(DeviceState::OFF) {
}

std::string Device::getId() const {
    return id;
}

std::string Device::getName() const {
    return name;
}

std::string Device::getLocation() const {
    return location;
}

DeviceState Device::getState() const {
    return state;
}

bool Device::turnOn() {
    if (state != DeviceState::ERROR) {
        state = DeviceState::ON;
        return true;
    }
    return false;
}

bool Device::turnOff() {
    if (state != DeviceState::ERROR) {
        state = DeviceState::OFF;
        return true;
    }
    return false;
}

void Device::reset() {
    state = DeviceState::OFF;
}

void Device::displayStatus() const {
    std::cout << "Device: " << name << " (" << id << ")" << std::endl;
    std::cout << "Location: " << location << std::endl;
    std::cout << "Status: " << getStateAsString() << std::endl;
}

std::string Device::getStateAsString() const {
    switch (state) {
        case DeviceState::ON:
            return "ON";
        case DeviceState::OFF:
            return "OFF";
        case DeviceState::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}