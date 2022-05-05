/* Title: Real-time Liquid Lens Focusing            */
/* Program: Non-embedded camera system auto-focus   */
/* Module name: Flexiboard                          */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class for communication with        */ 
/* Flexiboard driver. Sends/receives messages using */
/* SerialPort class.                                */

#pragma once

#include <vector>
#include <memory>
#include "SerialPort.hpp"

class Flexiboard {
    private:
        int readLen;
        std::unique_ptr<SerialPort>port;

        void writeRegister8(unsigned char addr, unsigned char data);
        void writeRegister16(unsigned char addr, unsigned short data);
        void writeFrame(unsigned char addr, unsigned short data, unsigned int dataLength);

        unsigned char readRegister(unsigned char addr);
        std::vector <unsigned char> readFrame();
        // returns vector of data read
        
        void checkChecksum(std::vector<unsigned char> buffer);
        
    public:
        Flexiboard(char* path);
        
        bool isAnalogMode();
        bool isStandbyMode();
        void setAnalogMode();
        void setStandbyMode();
        void clearModes();
        // exits standby and/or analog modes 
        
        void saveState();
        // saves the current FOCUS value and MODE to EEPROM to be loaded after reset
        
        int softwareVersion();
        
        bool isNotResponding();
        bool isThermalShutdown();
        bool isOverloaded();
        // returns true if the driver is overloaded (Vh could not reach 70V)
        
        void setValue(unsigned short value);
};

static void printMessage(std::vector <unsigned char> message);  // debug
