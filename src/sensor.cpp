#include "../include/sensor.h"
#include <iomanip>

Sensor::Sensor(const std::string& id, const std::string& name, const std::string& location,
               double minValue, double maxValue, const std::string& unit)
    : Device(id, name, location),
      currentValue(0.0),
      minValue(minValue),
      maxValue(maxValue),
      unit(unit),
      isMonitoring(false),
      gen(rd()),
      dis(minValue, maxValue) {
}

Sensor::~Sensor() {
    stopMonitoring();
}

double Sensor::getValue() const {
    return currentValue;
}

double Sensor::getMinValue() const {
    return minValue;
}

double Sensor::getMaxValue() const {
    return maxValue;
}

std::string Sensor::getUnit() const {
    return unit;
}

void Sensor::setValueChangeCallback(const std::function<void(double)>& callback) {
    valueChangeCallback = callback;
}

void Sensor::startMonitoring(std::chrono::milliseconds interval) {
    if (isMonitoring) {
        return;
    }
    
    isMonitoring = true;
    monitoringThread = std::thread([this, interval]() {
        while (isMonitoring) {
            currentValue = generateReading();
            
            if (valueChangeCallback) {
                valueChangeCallback(currentValue);
            }
            
            std::this_thread::sleep_for(interval);
        }
    });
}

void Sensor::stopMonitoring() {
    if (isMonitoring) {
        isMonitoring = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
}

double Sensor::generateReading() {
    return dis(gen);
}

void Sensor::displayStatus() const {
    Device::displayStatus();
    std::cout << "Current Value: " << std::fixed << std::setprecision(2) << currentValue << " " << unit << std::endl;
    std::cout << "Range: " << minValue << " - " << maxValue << " " << unit << std::endl;
    std::cout << "Monitoring: " << (isMonitoring ? "Active" : "Inactive") << std::endl;
}