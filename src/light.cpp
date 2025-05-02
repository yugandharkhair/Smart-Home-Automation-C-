#include "../include/light.h"

Light::Light(const std::string& id, const std::string& name, const std::string& location,
             bool dimmable, bool colorChangeable)
    : Device(id, name, location),
      brightness(100),
      color(LightColor::WHITE),
      dimmable(dimmable),
      colorChangeable(colorChangeable),
      customRed(255),
      customGreen(255),
      customBlue(255) {
}

bool Light::setBrightness(int level) {
    if (!dimmable) {
        return false;
    }
    
    if (level < 0) {
        brightness = 0;
    } else if (level > 100) {
        brightness = 100;
    } else {
        brightness = level;
    }
    
    if (brightness == 0) {
        state = DeviceState::OFF;
    } else if (state == DeviceState::OFF) {
        state = DeviceState::ON;
    }
    
    return true;
}

int Light::getBrightness() const {
    return brightness;
}

bool Light::setColor(LightColor newColor) {
    if (!colorChangeable && newColor != LightColor::WHITE && newColor != LightColor::WARM_WHITE) {
        return false;
    }
    
    color = newColor;
    
    // Reset custom color values based on selected preset
    switch (color) {
        case LightColor::WHITE:
            customRed = 255;
            customGreen = 255;
            customBlue = 255;
            break;
        case LightColor::WARM_WHITE:
            customRed = 255;
            customGreen = 223;
            customBlue = 180;
            break;
        case LightColor::RED:
            customRed = 255;
            customGreen = 0;
            customBlue = 0;
            break;
        case LightColor::GREEN:
            customRed = 0;
            customGreen = 255;
            customBlue = 0;
            break;
        case LightColor::BLUE:
            customRed = 0;
            customGreen = 0;
            customBlue = 255;
            break;
        case LightColor::YELLOW:
            customRed = 255;
            customGreen = 255;
            customBlue = 0;
            break;
        case LightColor::PURPLE:
            customRed = 128;
            customGreen = 0;
            customBlue = 128;
            break;
        case LightColor::CUSTOM:
            // Keep existing custom values
            break;
    }
    
    return true;
}

LightColor Light::getColor() const {
    return color;
}

bool Light::setCustomColor(int red, int green, int blue) {
    if (!colorChangeable) {
        return false;
    }
    
    customRed = std::max(0, std::min(255, red));
    customGreen = std::max(0, std::min(255, green));
    customBlue = std::max(0, std::min(255, blue));
    
    color = LightColor::CUSTOM;
    
    return true;
}

void Light::getCustomColor(int& red, int& green, int& blue) const {
    red = customRed;
    green = customGreen;
    blue = customBlue;
}

bool Light::isDimmable() const {
    return dimmable;
}

bool Light::isColorChangeable() const {
    return colorChangeable;
}

bool Light::turnOn() {
    bool result = Device::turnOn();
    if (result && brightness == 0) {
        brightness = 100;
    }
    return result;
}

void Light::displayStatus() const {
    Device::displayStatus();
    std::cout << "Brightness: " << brightness << "%" << std::endl;
    std::cout << "Color: " << getColorAsString() << std::endl;
    
    if (color == LightColor::CUSTOM) {
        std::cout << "RGB: (" << customRed << ", " << customGreen << ", " << customBlue << ")" << std::endl;
    }
    
    std::cout << "Features: " 
              << (dimmable ? "Dimmable" : "Non-dimmable")
              << (colorChangeable ? ", Color-changeable" : "")
              << std::endl;
}

std::string Light::getColorAsString() const {
    switch (color) {
        case LightColor::WHITE:
            return "White";
        case LightColor::WARM_WHITE:
            return "Warm White";
        case LightColor::RED:
            return "Red";
        case LightColor::GREEN:
            return "Green";
        case LightColor::BLUE:
            return "Blue";
        case LightColor::YELLOW:
            return "Yellow";
        case LightColor::PURPLE:
            return "Purple";
        case LightColor::CUSTOM:
            return "Custom";
        default:
            return "Unknown";
    }
}