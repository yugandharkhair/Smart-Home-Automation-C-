#include "../include/thermostat.h"
#include <iomanip>

Thermostat::Thermostat(const std::string& id, const std::string& name, const std::string& location,
                       double minTemp, double maxTemp)
    : Device(id, name, location),
      currentTemperature(22.0),
      targetTemperature(22.0),
      minTemperature(minTemp),
      maxTemperature(maxTemp),
      isRunning(false),
      mode(ThermostatMode::OFF) {
}

bool Thermostat::setTargetTemperature(double temperature) {
    if (temperature < minTemperature || temperature > maxTemperature) {
        return false;
    }
    
    targetTemperature = temperature;
    return true;
}

double Thermostat::getTargetTemperature() const {
    return targetTemperature;
}

double Thermostat::getCurrentTemperature() const {
    return currentTemperature;
}

bool Thermostat::setMode(ThermostatMode newMode) {
    if (state == DeviceState::ERROR) {
        return false;
    }
    
    mode = newMode;
    
    if (mode == ThermostatMode::OFF) {
        isRunning = false;
        state = DeviceState::OFF;
    } else {
        state = DeviceState::ON;
    }
    
    return true;
}

ThermostatMode Thermostat::getMode() const {
    return mode;
}

std::string Thermostat::getModeAsString() const {
    switch (mode) {
        case ThermostatMode::OFF:
            return "OFF";
        case ThermostatMode::HEAT:
            return "HEAT";
        case ThermostatMode::COOL:
            return "COOL";
        case ThermostatMode::AUTO:
            return "AUTO";
        case ThermostatMode::FAN_ONLY:
            return "FAN ONLY";
        default:
            return "UNKNOWN";
    }
}

void Thermostat::connectSensor(std::shared_ptr<Sensor> sensor) {
    if (sensor) {
        temperatureSensor = sensor;
        temperatureSensor->setValueChangeCallback([this](double value) {
            handleSensorReading(value);
        });
    }
}

void Thermostat::handleSensorReading(double value) {
    currentTemperature = value;
    
    // Simple thermostat logic
    if (mode == ThermostatMode::HEAT) {
        isRunning = currentTemperature < targetTemperature;
    } else if (mode == ThermostatMode::COOL) {
        isRunning = currentTemperature > targetTemperature;
    } else if (mode == ThermostatMode::AUTO) {
        double tolerance = 0.5; // Half a degree tolerance
        if (currentTemperature < targetTemperature - tolerance) {
            isRunning = true; // Heating
        } else if (currentTemperature > targetTemperature + tolerance) {
            isRunning = true; // Cooling
        } else {
            isRunning = false; // Within acceptable range
        }
    } else {
        isRunning = false;
    }
}

bool Thermostat::isHeating() const {
    if (!isRunning) {
        return false;
    }
    
    return mode == ThermostatMode::HEAT || 
           (mode == ThermostatMode::AUTO && currentTemperature < targetTemperature);
}

bool Thermostat::isCooling() const {
    if (!isRunning) {
        return false;
    }
    
    return mode == ThermostatMode::COOL || 
           (mode == ThermostatMode::AUTO && currentTemperature > targetTemperature);
}

bool Thermostat::turnOn() {
    bool result = Device::turnOn();
    if (result && mode == ThermostatMode::OFF) {
        mode = ThermostatMode::AUTO;
    }
    return result;
}

bool Thermostat::turnOff() {
    bool result = Device::turnOff();
    if (result) {
        mode = ThermostatMode::OFF;
        isRunning = false;
    }
    return result;
}

void Thermostat::displayStatus() const {
    Device::displayStatus();
    std::cout << "Current Temperature: " << std::fixed << std::setprecision(1) << currentTemperature << " °C" << std::endl;
    std::cout << "Target Temperature: " << std::fixed << std::setprecision(1) << targetTemperature << " °C" << std::endl;
    std::cout << "Mode: " << getModeAsString() << std::endl;
    std::cout << "System Status: " << (isRunning ? "Running" : "Idle") << std::endl;
    
    if (isRunning) {
        std::cout << "Action: " << (isHeating() ? "Heating" : (isCooling() ? "Cooling" : "Fan Only")) << std::endl;
    }
}