#include <iostream>
#include <sstream>
#include <fstream>
#include <stdbool.h>
#include <thread>

#include "VideoCapture.hpp"
#include "ADM00931.hpp"
#include "ScoreFinder.hpp"

const unsigned int maxWrittableData = 255;

using namespace std;

int main() {
    
    int data = 0, count = 0, increment = 10,  maxData = 0;
    double score = 0.0, maxScore = 0.0;

    VideoCapture video;
    ADM00931 focus;
    ScoreFinder scoreFinder;

    cv::Mat image;
    cv::namedWindow("focused", cv::WINDOW_NORMAL);
    cv::resizeWindow("focused", 1920, 1080);

    focus.setValue(0);
    data = increment;
    auto begin = std::chrono::high_resolution_clock::now();
 
    while(count < maxWrittableData / increment + 1) {
        
        score = 0;

        image = video.getImage();
        image = image(cv::Range(340,740), cv::Range(760,1160));
        cout << "Image acquired at focus value: " << data - increment << endl;
        
        // last iteration, calc score for 250 focus value, dont send 260
        if(data != 260)
            focus.setValue(data);
        
        score = scoreFinder.calcSobelScore(image);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        
        //score = scoreFinder.calcLaplaceScore(image);
        //std::this_thread::sleep_for(std::chrono::milliseconds(80));
        
        //score = scoreFinder.calcCannyScore(image);
        //std::this_thread::sleep_for(std::chrono::milliseconds(70));
        
        //score = scoreFinder.calcFFTScore(image);
        //std::this_thread::sleep_for(std::chrono::milliseconds(80));
        
        cout << "Score : " << score << "\n\n";
        if(score > maxScore) {
            maxScore = score;
            maxData = data - increment;
        }
        
        data += increment;
        count++;
    }

    int values_to_scan[10] = {0};
    int diff = -6;

    if(maxData >= 10) {
        for(int i = -5; i <= 5; i++)
            values_to_scan[i + 5] = maxData + i;
    }
    else {
        for(int i = 1; i <= 10; i++)
            values_to_scan[i] = i;
    }
        
    count = 0;
    focus.setValue(values_to_scan[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    while(count < 10) {
        
        score = 0;
        data = values_to_scan[count];
              
    	image = video.getImage();
        image = image(cv::Range(340,740), cv::Range(760,1160));
        cout << "Image acquired at focus value" << values_to_scan[count] << endl;

        focus.setValue(data);
        
        score = scoreFinder.calcSobelScore(image);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        
        //score = scoreFinder.calcLaplaceScore(image);
        //std::this_thread::sleep_for(std::chrono::milliseconds(80));
        
        //score = scoreFinder.calcCannyScore(image);
        //std::this_thread::sleep_for(std::chrono::milliseconds(70));
        
        //score = scoreFinder.calcFFTScore(image);
        //std::this_thread::sleep_for(std::chrono::milliseconds(80));

        cout << "Score : " << score << "\n\n";
        if(score > maxScore) {
            maxScore = score;
            maxData = values_to_scan[count - 1];
        }
        
        count++;
    }

    focus.setValue(maxData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    image = video.getImage();
    cout << "Max focus value: " << maxData << endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()) / 1000000 << "ms" << std::endl;
    
    cv::Rect rect(660, 240, 600, 600);
    cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 5);
    imshow("focused", image);
    cv::waitKey(0);
    
    return 0;
}
