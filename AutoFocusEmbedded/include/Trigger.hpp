/* Title: Real-time Liquid Lens Focusing            */
/* Program: Test suite for sharpness score          */
/* assesment methods                                */
/* Module name: Trigger                             */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control over  */
/* the GPIO pin connected to cameras trigger.       */

#pragma once

#include <iostream>
#include <thread>
#include <gpiod.h>

#define UNIT_OPEN_ERR -1
#define GET_LINE_ERR -2
#define SET_VALUE_ERR -3
#define SET_OUTPUT_PIN_ERR -4 

#define CONSUMER "Consumer"

class Trigger {
    private: 
        int ret, value;
	struct gpiod_chip* chip;
	struct gpiod_line* line;
    
    public:
        Trigger();
        ~Trigger();
       
        void destroy_trigger();
        void setup_trigger();
        void send_trigger(int ms);
};
