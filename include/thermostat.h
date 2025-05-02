#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "device.h"
#include "sensor.h"
#include <memory>

enum class ThermostatMode {
    OFF,
    HEAT,
    COOL,
    AUTO,
    FAN_ONLY
};

class Thermostat : public Device {
private:
    double currentTemperature;
    double targetTemperature;
    double minTemperature;
    double maxTemperature;
    bool isRunning;
    ThermostatMode mode;
    std::shared_ptr<Sensor> temperatureSensor;
    
public:
    Thermostat(const std::string& id, const std::string& name, const std::string& location,
               double minTemp = 10.0, double maxTemp = 32.0);
    
    // Thermostat specific methods
    bool setTargetTemperature(double temperature);
    double getTargetTemperature() const;
    double getCurrentTemperature() const;
    
    bool setMode(ThermostatMode newMode);
    ThermostatMode getMode() const;
    std::string getModeAsString() const;
    
    void connectSensor(std::shared_ptr<Sensor> sensor);
    void handleSensorReading(double value);
    
    // Check if heating/cooling is active
    bool isHeating() const;
    bool isCooling() const;
    
    // Override base class methods
    bool turnOn() override;
    bool turnOff() override;
    void displayStatus() const override;
};

#endif // THERMOSTAT_H