/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: FFTSharpnessScore                   */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class for Fast Fourier Transform    */
/* sharpness score assessment method                */

#pragma once

#include <opencv2/imgproc.hpp>
#include "IFocusAlgorithm.hpp"

class FFTfocus : public IFocusAlgorithm {
public:
	FFTfocus(int threshold = 10, int size = 60, bool measureSectors = false);
	IFocusAlgorithm::FocusResult calculate(cv::Mat image) override;

private:
	int focusThreshold;
	int size;
	double calcFFTScore(cv::Mat grayImg);
	cv::Mat fftShift(cv::Mat input);
	cv::Mat zeroOutCenter(cv::Mat input);
};
