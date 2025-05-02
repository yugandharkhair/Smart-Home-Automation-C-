#ifndef SENSOR_H
#define SENSOR_H

#include "device.h"
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <random>

// Base class for all sensors in the smart home system
class Sensor : public Device {
protected:
    double currentValue;
    double minValue;
    double maxValue;
    std::string unit;
    std::function<void(double)> valueChangeCallback;
    std::atomic<bool> isMonitoring;
    std::thread monitoringThread;
    
    // Random number generator for simulating sensor readings
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

public:
    Sensor(const std::string& id, const std::string& name, const std::string& location,
           double minValue, double maxValue, const std::string& unit);
    virtual ~Sensor();

    // Prevent copying and moving
    Sensor(const Sensor&) = delete;
    Sensor& operator=(const Sensor&) = delete;
    Sensor(Sensor&&) = delete;
    Sensor& operator=(Sensor&&) = delete;

    // Sensor specific methods
    double getValue() const;
    double getMinValue() const;
    double getMaxValue() const;
    std::string getUnit() const;
    
    // Set callback for value changes
    void setValueChangeCallback(const std::function<void(double)>& callback);
    
    // Start/stop monitoring (simulating sensor readings)
    virtual void startMonitoring(std::chrono::milliseconds interval = std::chrono::seconds(1));
    virtual void stopMonitoring();
    
    // Generate a simulated sensor reading
    virtual double generateReading();
    
    // Override base class methods
    void displayStatus() const override;
};

#endif // SENSOR_H