/* Title: Real-time Liquid Lens Focusing            */
/* Program: Test suite for sharpness score          */
/* assesment methods                                */
/* Module name: Trigger                             */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control over  */
/* the GPIO pin connected to cameras trigger.       */

#include "Trigger.hpp"

#define GPIO_PIN 17
#define GPIO_CHIPNAME "gpiochip0"

Trigger::Trigger() {

    try {
        chip = gpiod_chip_open_by_name(GPIO_CHIPNAME);
        if(!chip)
            throw(UNIT_OPEN_ERR);
	
	line = gpiod_chip_get_line(chip, GPIO_PIN);
	if(!line)
	    throw(GET_LINE_ERR);

	ret = gpiod_line_request_output(line, CONSUMER, 0);
        if(ret < 0)
            throw(SET_OUTPUT_PIN_ERR);
    }
    catch(int err_no) {
        switch(err_no) {
            case(UNIT_OPEN_ERR):
		std::cout << "GPIO : Unable to open unit";
                break;
	    case(GET_LINE_ERR):
		std::cout << "GPIO : Failed to get line" << GPIO_PIN; 
		break;
            case(SET_OUTPUT_PIN_ERR):
		std::cout << "GPIO : Unable to set pin" << GPIO_PIN << "as output";
		break;
	}
	gpiod_line_release(line);
	gpiod_chip_close(chip);
	exit(-1);
    }
}

Trigger::~Trigger() {
    gpiod_line_release(line);
    gpiod_chip_close(chip);    
}

void Trigger::send_trigger(int delay_ms) {
    
    value = 1;
    try {
        ret = gpiod_line_set_value(line, value);
        if(ret < 0)
	        throw(SET_VALUE_ERR);
        
	std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	
	value = 0;
        ret = gpiod_line_set_value(line, value);
	if(ret < 0) 
	    throw(SET_VALUE_ERR);
    }
    catch(int err_no) {
        std::cout << "GPIO : Error setting a value on a pin" << GPIO_PIN;
	exit(-1);
    }	
} 
