/* Title: Real-time Liquid Lens Focusing            */
/* Program: Test suite for sharpness score          */
/* assesment methods                                */
/* Module name: TestSetsGenerator                   */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This program captures a test set of */
/* images at every possible focus value.            */


#include <iostream>
#include <sstream>
#include <fstream>
#include <stdbool.h>
 
#include "VideoCapture.hpp"
#include "ADM00931.hpp"

#define MAX_DATA 255
#define ZERO_ASCII 48

using namespace std;

// returns a formatted filename for image or max_data_file
string generate_filename(int data, char set[32]) {
    
    ostringstream formatted_no;
    formatted_no.width(3);
    formatted_no.fill('0');
    formatted_no << data;
    
    ostringstream formatted_path;
    formatted_path << "../images/" << set;
    formatted_path << "/file" << formatted_no.str() << ".pgm";

    return formatted_path.str();
}

int main() {
    
    int data = 0;
    string filename;
    
    // name of the set
    char set[32] = "SET_25cm";

    VideoCapture video;
    FocusControl focus;

    while(data < MAX_DATA) {
	
    	printf("Capturing image at focus point:  %i\n", data);
        
        focus.send_focus_value(data);
    	cv::Mat image = video.getImage();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // save image
        filename = generate_filename(data, set);
        imwrite(filename, image);

        data++;
    }

    return 0;
}
