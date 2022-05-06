/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: SobelMethod                         */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class for Fast Fourier Transform    */
/* sharpness score assessment method                */

#pragma once

#include <opencv2/imgproc.hpp>

class SobelMethod {
public:
    SobelFocus();
    double calculateScore(cv::Mat grayImg);
}
