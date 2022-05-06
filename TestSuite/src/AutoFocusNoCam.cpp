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
#include "ScoreFinder.hpp"

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

    int data = 0;
    int count = 0;
    int increment = 10;
    double score = 0;
    int max_data_save = 0;
    double max_score = 0;
    int max_data = 0;
    ScoreFinder scoreFinder;
    Mat image;

    string set;
    cout << "Type in the name of set to be used: ";
    cin >> set;

    namedWindow("focused", WINDOW_NORMAL);
    resizeWindow("focused", 1024, 768);
    
    data = 1;
    auto begin = std::chrono::high_resolution_clock::now();

    while(count < (MAX_DATA / increment) + 1) {
        
        if(count == 1)
            data -= 1;

        score = 0.0;
        
        image = (imread(generate_filename(set, data)));
        image = image(Range(340,740), Range(760,1160));
        cout << "Image acquired at focus value: " << data << endl;
        
        score = scoreFinder.calcSobelScoreOverX(image, 3, 4, 45);

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
            values_to_scan[i] = max_data + diff + 1;
            diff += 1;
        }
    }
    else if(max_data == 250) {
        for(int i = 0; i < 10; i++)
            values_to_scan[i] = i + 245;
    }
    else if(max_data == 1) {
        for(int i = 0; i < 10; i++)
            values_to_scan[i] = i + 1;
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

        score = scoreFinder.calcSobelScoreOverX(image, 3, 4, 45);
        
        score += mean;
        cout << "Score : " << score << "\n\n";

        if(score > max_score) {
            max_score = score;
            max_data = values_to_scan[count];
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
    
    return 0;
}
