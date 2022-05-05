/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: Trigger                             */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control over  */
/* the GPIO pin connected to cameras trigger.       */


#include "Trigger.hpp"

#define GPIO_PIN 17
#define GPIO_CHIPNAME "gpiochip0"

Trigger::Trigger() {

    chip = gpiod_chip_open_by_name(GPIO_CHIPNAME);
    if(!chip)
        throw std::runtime_error("GPIO: Unable to open unit.");

	line = gpiod_chip_get_line(chip, GPIO_PIN);
	if(!line)
	    throw std::runtime_error("Failed to get line.");

	ret = gpiod_line_request_output(line, CONSUMER, 0);
    if(ret < 0)
        throw std::runtime_error("Unable to set output pin.");
}

Trigger::~Trigger() {
    gpiod_line_release(line);
    gpiod_chip_close(chip);    
}

void Trigger::send_trigger(int delay_ms) {
    
    value = 1;
    ret = gpiod_line_set_value(line, value);
    if(ret < 0)
	    throw std::runtime_error("Failed to set value.");
        
	std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	
	value = 0;

    ret = gpiod_line_set_value(line, value);
	if(ret < 0) 
	    throw std::runtime_error("Failed to set value.");
} 
