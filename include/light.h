#ifndef LIGHT_H
#define LIGHT_H

#include "device.h"

enum class LightColor {
    WHITE,
    WARM_WHITE,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    CUSTOM
};

class Light : public Device {
private:
    int brightness;  // 0-100%
    LightColor color;
    bool dimmable;
    bool colorChangeable;
    int customRed;   // 0-255
    int customGreen; // 0-255
    int customBlue;  // 0-255

public:
    Light(const std::string& id, const std::string& name, const std::string& location,
          bool dimmable = true, bool colorChangeable = false);
    
    // Light specific controls
    bool setBrightness(int level);
    int getBrightness() const;
    
    bool setColor(LightColor newColor);
    LightColor getColor() const;
    
    bool setCustomColor(int red, int green, int blue);
    void getCustomColor(int& red, int& green, int& blue) const;
    
    bool isDimmable() const;
    bool isColorChangeable() const;
    
    // Override base class methods
    bool turnOn() override;
    void displayStatus() const override;
    std::string getColorAsString() const;
};

#endif // LIGHT_H