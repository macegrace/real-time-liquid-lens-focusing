/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusNonEmbedded                    */
/* Module name: ScoreFinder                         */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class for Fast Fourier Transform    */
/* sharpness score assessment method                */

#pragma once

#include <opencv2/imgproc.hpp>

class ScoreFinder {
public:
	ScoreFinder();
	double calcFFTScore(cv::Mat grayImg, unsigned int size = 60);
    double calcSobelScore(cv::Mat image, unsigned int ksize = 3, unsigned int scale = 1, unsigned int  delta = 0);    
    double calcSobelScoreOverX(cv::Mat image, unsigned int ksize = 3, unsigned int scale = 1, unsigned int  delta = 0);
    double calcSobelScoreOverY(cv::Mat image, unsigned int ksize = 3, unsigned int scale = 1, unsigned int  delta = 0);
    double calcLaplaceScore(cv::Mat image, unsigned int ksize = 3, unsigned int scale = 1, unsigned int delta = 0);
    double calcCannyScore(cv::Mat image, double threshold1 = 60, double threshold2 = 60, unsigned int aperatureSize = 3, bool L2gradient = false);

private:
    double score;
    unsigned int fftSize;
	cv::Mat fftShift(cv::Mat input);
	cv::Mat zeroOutCenter(cv::Mat input);
};
