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
    
    i2c_fd = open(filename, O_RDWR);
    if(i2c_fd < 0)
        throw(std::runtime_error("I2C : Unable to open adapter"));
    if(ioctl(i2c_fd, I2C_SLAVE, I2C_ADDR) < 0)
        throw(std::runtime_error("I2C : Invalid ioctl call"));
    if(write(i2c_fd, "0", 1) != 1)
        throw(std::runtime_error("I2C : Failed to write initializing value to file"));
}

ADM00931::~ADM00931() {
    close(i2c_fd);
}

void ADM00931::send_focus_value(int data) {
    if(write(i2c_fd, &data, 1) != 1)
        throw(std::runtime_error("I2C : Unable to write to a value file"));
}
