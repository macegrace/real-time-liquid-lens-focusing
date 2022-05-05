/* Title: Real-time Liquid Lens Focusing            */
/* Program: Test suite for sharpness score          */
/* assesment methods                                */
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

class FocusControl {
    private:
        int i2c_fd;
        char filename[20];

    public:
        FocusControl();
        ~FocusControl();
        void send_focus_value(int data);
};
