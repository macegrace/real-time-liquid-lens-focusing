/* Title: Real-time Liquid Lens Focusing            */
/* Program: Non-embedded camera system auto-focus   */
/* Module name: AutoFocusNonEmbedded                */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Main function of the auto-focus     */
/* program includes the capture and prrocess loop   */
/* that evaluates image sharpness scores.           */

#include <stdio.h>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "BackgroundProcessing.hpp"
#include "CamControl.hpp"
#include "Flexiboard.hpp"
#include "ScoreFinder.hpp"

int maxData = 0, height = 0, width = 0, softCount = 0, data = 0;
int secondIterationData[15];

double maxScore = 0.0, score = 0.0;

bool frameReady = true;
bool firstIterationDone = false, exitProg = false, secondIteration = false;

const int maxDataValue = 65535;
const int increment1 = maxDataValue / 25;
const int yMin = 373;
const int yMax = 723;
const int xMin = 793;
const int xMax = 1143;
const int lowDataThreshold = 2570;
const int highDataThreshold = 62966;
const int increment2 = 256;

Flexiboard driver("/dev/ttyUSB0");
idsCamera cam;
ScoreFinder scoreFinder;

void imageProcessing(cv::Mat frame){

    cv::Mat sobel;
    cv::Scalar meanTemp;
    float mean;
    
    score = 0;

    if(!firstIterationDone)
        driver.setValue(data + increment1); 
        // write next value, before image processing, so the lens has enough time to re-focus
    
    
    frame = frame(cv::Range(yMin, yMax), cv::Range(xMin, xMax)); // crop image for faster processing (square in the middle of the frame)
    
    score = scoreFinder.calcSobelScore(frame);

    //score = scoreFinder.calcLaplaceScore(frame);
    //std::this_thread::sleep_for(std::chrono::milliseconds(80)); 

    //score = scoreFinder.calcCannyScore(frame);
    //std::this_thread::sleep_for(std::chrono::milliseconds(70));
    
    //score = scoreFinder.calcFFTScore(frame, 60); 
    //std::this_thread::sleep_for(std::chrono::milliseconds(80));   
    
    std::cout << "Image captured at focus data: " << data << "processed. Score: " << score << std::endl << std::endl;

    if(score > maxScore) {
        maxScore = score;
        maxData = data;

        // std::cout << "Saving new max focus data: " << maxData << std::endl;
    }
    
    if(!firstIterationDone)
        data += increment1;

    if(data > maxDataValue - increment1)
        firstIterationDone = true;

    if(firstIterationDone) {    
        if(!secondIteration) {  // start second iteration
            secondIteration = true;
            
            // calculate data for next steps
            if(maxData > lowDataThreshold && maxData < highDataThreshold) {
                for(int i = -5; i <= 5; i++) {
                    secondIterationData[i + 5] = maxData + (i * increment2);
                }
            }
            else if(maxData >= highDataThreshold) {
                for(int i = 0; i <= 10; i++) {
                    secondIterationData[i] = highDataThreshold + (i * increment2);
                }
            }
            else if(maxData <= lowDataThreshold) {
                for(int i = 0; i <= 10; i++) {
                    secondIterationData[i] = i * increment2;
                }
            }
        }
        
        if(softCount < 10) { // scan 10 frames, using smaller increment1
            // std::cout << "soft focus -- writing " << secondIterationData[softCount] << " to the driverboard" << std::endl;
            data = secondIterationData[softCount];
            driver.setValue(data);
            softCount++;
        }
        else {  // focusing finished
            exitProg = true;  // exit after finishing
            data = maxData;
            driver.setValue(maxData);
        }
    }
}

int main(int , char **) {
    
	BackgroundProcessing camProcessor([](cv::Mat s){
		imageProcessing(s);
	});

    int cnt = 0;
    bool autoGain = true;

    driver.setValue(0);     // reset lens focus

    cam.setDefaultCam();
    cam.setAutoGain(&autoGain);
    cam.getResolution(&width, &height);

	cv::Mat frame(height, width, CV_8UC1);
    auto begin = std::chrono::high_resolution_clock::now();
    
    for(;;) {
        cam.getFrame(&frame);

        if(cnt >= 3)    // skip first 3 frames due to camera initialization
            camProcessor.frameReady(frame);

        cv::imshow("Live", frame);
        cv::waitKey(1);
        
        if(exitProg) {
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()) / 1000000000.0 << "s" << std::endl;
            return 0;
        }
        cnt++;
    }

    return 0;
}
