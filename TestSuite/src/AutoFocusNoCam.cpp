/* Title: Real-time Liquid Lens Focusing            */
/* Program: Test suite for sharpness score          */
/* assesment methods                                */
/* Module name: AutoFocusNoCamera                   */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: The program performs a simulated    */
/* auto-focus on the data acquired by               */
/* TestSetsGenerator                                */

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdbool.h>
#include <thread>
#include <opencv2/opencv.hpp>

#define MAX_DATA 255

using namespace std;
using namespace cv;

string generate_filename(string set, int data) {
    ostringstream filename_oss;
    string filename;
    ostringstream data_formatted;
    data_formatted << setw(3) << setfill('0') << data;
    
    filename_oss << "../images/" << set << "/file" << data_formatted.str() << ".pgm";
    filename = filename_oss.str();
    cout << filename << endl;
    return filename;
}

int main() {

    string set = "SET_15cm";
    //cout << "Type in the name of set to be used: ";
    //cin >> set;

    int data = 0;
    int count = 0;
    int increment = 10;
    double score = 0;
    int max_data_save = 0;
    double max_score = 0;
    int max_data = 0;

    Mat image;

    namedWindow("focused", WINDOW_NORMAL);
    resizeWindow("focused", 1024, 768);
    
    data = increment;
    auto begin = std::chrono::high_resolution_clock::now();

    cout << useOptimized() << endl;

    while(count < MAX_DATA / increment) {
        
        score = 0;
        
        image = (imread(generate_filename(set, data)));
        image = image(Range(340,740), Range(760,1160));
        cout << "Image acquired at focus value: " << data << endl;
        
        //Mat sobel;
        Sobel(image, image, CV_8UC1, 1, 0, 3, 4, 45);
        Scalar temp = cv::mean(image);
        float mean = temp.val[0];
        score += mean;
        
        /*
        Sobel(image, sobel, CV_8UC1, 0, 1, 3, 4, 45);
        temp = cv::mean(sobel);
        mean = temp.val[0];
        score += mean;
        */

        if(score > max_score) {
            max_score = score;
            max_data = data;
        }
        
        cout << "Score : " << score << "\n\n";
        
        data += increment;
        count++;
    }

    cout << "\n--==Max data = " << max_data << "==--\n";
    max_data_save = max_data;

    int values_to_scan[10] = {0};
    int diff = -6;

    if(max_data > 8 && max_data < 247) {
        for(int i = 0; i < 10; i++) {
            if(diff == 0) {
                i--;
                diff += 1;
                continue;
            }
            values_to_scan[i] = max_data + diff;
            diff += 1;
        }
    }
    else if(max_data >= 247) {
        values_to_scan[0] = 240;
        values_to_scan[1] = 241;
        values_to_scan[2] = 242;
        values_to_scan[3] = 243;
        values_to_scan[4] = 244;
        values_to_scan[5] = 245;
        values_to_scan[6] = 246;
        values_to_scan[7] = 247;
        values_to_scan[8] = 248;
        values_to_scan[9] = 249;
    }
    else if(max_data <= 8) {
        values_to_scan[0] = 1;
        values_to_scan[1] = 2;
        values_to_scan[2] = 3;
        values_to_scan[3] = 4;
        values_to_scan[4] = 5;
        values_to_scan[5] = 6;
        values_to_scan[6] = 7;
        values_to_scan[7] = 8;
        values_to_scan[8] = 9;
        values_to_scan[9] = 10;
    }

        
    count = 0;

    while(count < 10) {
        
        score = 0;
        data = values_to_scan[count];
        
        image = (imread(generate_filename(set, data)));
        image = image(Range(340,740), Range(760,1160));
        cout << "Image acquired at focus value" << values_to_scan[count] << endl;

        Sobel(image, image, CV_8UC1, 1, 0, 3, 4, 45);
        Scalar temp = cv::mean(image);
        float mean = temp.val[0];

        score += mean;
        
        /*
        Sobel(image, sobel, CV_8UC1, 0, 1, 3, 4, 45);
        temp = cv::mean(sobel);
        mean = temp.val[0];
        */

        score += mean;
        cout << "Score : " << score << "\n\n";

        if(score > max_score) {
            max_score = score;
            max_data = values_to_scan[count - 1];
        }
        
        count++;
    }
    string max_filename = generate_filename(set, max_data);
    image = imread(max_filename);
    cout << "Max focus value: " << max_data << "\n";

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()) / 1000000 << "ms" << std::endl;
    
    //imshow("focused", image);
    //waitKey(0);
    
    cout << getBuildInformation() << endl;

    return 0;
}
