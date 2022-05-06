/* Title: Real-time Liquid Lens Focusing            */
/* Program: Embedded camera system auto-focus       */
/* Module name: AutoFocusEmbedded                   */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Main function of the auto-focus     */
/* program includes the capture and prrocess loop   */
/* that evaluates image sharpness scores.           */

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdbool.h>
#include <thread>

#include "VideoCapture.hpp"
#include "ADM00931.hpp"
#include "ScoreFinder.hpp"

const unsigned int minData = 0;
const unsigned int maxData = 255;

using namespace std;

int main() {
    
    int increment = 10;
    int data, count, max_data, max_data_save = (0, 0, 0, 0);
    double max_score, score = (0.0, 0.0);

    VideoCapture video;
    ADM00931 focus;
    ScoreFinder scoreFinder;
    cv::Mat image, image_orig, max_image, rect1, rect2, rect3, rect4;
    
    cv::namedWindow("focused", cv::WINDOW_NORMAL);
    cv::resizeWindow("focused", 1024, 768);

    char filename[64] = "../focused_images/image1.pgm";

    focus.send_focus_value(0);
    data = increment;

    auto begin = std::chrono::high_resolution_clock::now();
 
    while(count < maxData / increment + 1) {
        
        score = 0;

        //if(count == 0)
        //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    	
        image_orig = video.get_image();
        
        /*imshow("focused", image);
        if(count == 0)
            cv::waitKey(5);
        else
            cv::waitKey(1);
        */

        //image = image(cv::Range(340,740), cv::Range(760,1160));
        image = image_orig(cv::Range(240,840), cv::Range(660,1260));

        cout << "Image acquired at focus value: " << data - increment << endl;
        
        if(data != 260) {
            focus.send_focus_value(data);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        
        //algo.calculate(image);
        //score = algo.result.score;
        
        score = scoreFinder.calcSobelScore(image, 3, 4, 45);

        /*cv::Mat sobel;
        cv::Sobel(image, sobel, CV_8UC1, 1, 0, 3, 4, 45);
        cv::Scalar temp = cv::mean(sobel);
        float mean = temp.val[0];

        score += mean;

        cv::Sobel(image, sobel, CV_8UC1, 0, 1, 3, 4, 45);
        temp = cv::mean(sobel);
        mean = temp.val[0];

        score += mean;*/
        
        cout << "Score : " << score << "\n\n";
        
        if(score > max_score) {
            max_score = score;
            max_data = data - increment;
            cout << "Saving max_image" << endl;
            max_image = image_orig;
        }
        
        //ostringstream imname;
        //imname << "../images/SET1/image" << data - increment << ".pgm";
        //cv::imwrite(imname.str(), image);

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
        for(int i = 0; i < 10; i++)
            values_to_scan[i] = 240 + i;
    }
    else if(max_data <= 8) {
        for(int i = 0; i < 10; i++)
            values_to_scan[i] = i + 1;
    }

        
    count = 0;
    focus.send_focus_value(values_to_scan[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    while(count < 10) {
        
        score = 0;
        data = values_to_scan[count];
              
    	image = video.get_image();

        //imshow("focused", image);
        //cv::waitKey(1);
        
        //image = image(cv::Range(340,740), cv::Range(760,1160));
        image = image(cv::Range(240,840), cv::Range(660,1260));
        cout << "Image acquired at focus value" << values_to_scan[count] << endl;

        focus.send_focus_value(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        
        //algo.calculate(image);
        //score = algo.result.score;

        cv::Mat sobel;
        cv::Sobel(image, sobel, CV_8UC1, 1, 0, 3, 4, 45);
        cv::Scalar temp = cv::mean(sobel);
        float mean = temp.val[0];

        score += mean;

        cv::Sobel(image, sobel, CV_8UC1, 0, 1, 3, 4, 45);
        temp = cv::mean(sobel);
        mean = temp.val[0];

        score += mean;
        cout << "Score : " << score << "\n\n";

        if(score > max_score) {
            max_score = score;
            max_data = values_to_scan[count - 1];
        }
        
        //ostringstream imname;
        //imname << "../images/SET1/image" << data << ".pgm";
        //cv::imwrite(imname.str(), image);

        count++;
    }

    focus.send_focus_value(max_data);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    image = video.get_image();
    cout << "Max focus value: " << max_data << endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()) / 1000000 << "ms" << std::endl;
    
    if(max_data == max_data_save) {
        cv::Rect rect(660, 240, 600, 600);
        cv::rectangle(max_image, rect, cv::Scalar(0, 255, 0), 5);
        imshow("focused", max_image);
        cv::waitKey(0);
    }
    else {
        cv::Rect rect(660, 240, 600, 600);
        cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 5);
        imshow("focused", image);
        cv::waitKey(0);
    }
    
    
    return 0;
}
