/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: ADM00931                            */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control of    */
/* ADM00931 liquid lens driver board via I2C        */

#pragma once

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fstream>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#define ADAPTER_OPEN_ERR -1
#define IOCTL_CALL_ERR -2
#define WRITE_ERR -3

class ADM00931 {
    private:
        int fileDescriptor;
        char filename[20];

    public:
        ADM00931();
        ~ADM00931();
        void setValue(int data);
};
