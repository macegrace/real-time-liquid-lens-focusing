/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: ADM00931                            */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control of    */
/* ADM00931 liquid lens driver board via I2C        */

#include "ADM00931.hpp"

#define ADAPTER_ADDR 0x01
#define I2C_ADDR 0x23
#define MAX_FILENAME_LENGTH 16

ADM00931::ADM00931() {
    
    snprintf(filename, MAX_FILENAME_LENGTH, "/dev/i2c-%d", ADAPTER_ADDR);
    
    fileDescriptor = open(filename, O_RDWR);
    if(fileDescriptor < 0)
        throw(std::runtime_error("I2C : Unable to open adapter"));
    if(ioctl(fileDescriptor, I2C_SLAVE, I2C_ADDR) < 0)
        throw(std::runtime_error("I2C : Invalid ioctl call"));
    if(write(fileDescriptor, "0", 1) != 1)
        throw(std::runtime_error("I2C : Failed to write initializing value to file"));
}

ADM00931::~ADM00931() {
    close(fileDescriptor);
}

void ADM00931::setValue(int data) {
    if(write(fileDescriptor, &data, 1) != 1)
        throw(std::runtime_error("I2C : Unable to write to a value file"));
}
