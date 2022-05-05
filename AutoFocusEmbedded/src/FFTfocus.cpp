/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: FFTSharpnessScore                   */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Module for Fast Fourier Transform   */
/* sharpness score assessment method                */

#include "FFTfocus.hpp"

FFTfocus::FFTfocus(int threshold, int size, bool measureSectors) : focusThreshold(threshold), size(size) {
	result.score = 0;
	result.lowerHalfScore = 0;
	result.upperHalfScore = 0;
	result.isSharp = false;
	this->measureSectors = measureSectors;
	this->scorePrecision = 2;
	result.mask = cv::Mat();
}

IFocusAlgorithm::FocusResult FFTfocus::calculate(cv::Mat grayImg) {

	if (measureSectors) {
		// calculate only on upper sector
		cv::Mat upper = getUpperRect(grayImg);
		result.upperHalfScore = calcFFTScore(upper);
		result.isUpperSharp = (result.upperHalfScore < focusThreshold) ? false : true;
		// calculate only on lower sector
		cv::Mat lower = getLowerRect(grayImg);
		result.lowerHalfScore = calcFFTScore(lower);
		result.isLowerSharp = (result.lowerHalfScore < focusThreshold) ? false : true;
		result.isSharp = result.isLowerSharp && result.isUpperSharp;
	} else {
		// full image
		result.score = calcFFTScore(grayImg);
		result.isSharp = (result.score < focusThreshold) ? false : true;
	}

	return result;
}

double FFTfocus::calcFFTScore(cv::Mat grayImg) {
	// expand input image to optimal size, on the border add zeros
	int x = cv::getOptimalDFTSize(grayImg.cols);
	int y = cv::getOptimalDFTSize(grayImg.rows);
	cv::Mat padded;
	cv::copyMakeBorder(grayImg, padded, 0, y - grayImg.rows, 0, x - grayImg.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	// add another plane to the image, so the complex result from dft can fit
	cv::Mat planes[] = {cv::Mat_<double>(padded), cv::Mat::zeros(padded.size(), CV_64F)};
	cv::Mat fft;
	cv::merge(planes, 2, fft);
	cv::dft(fft, fft);

	// crop the fft, if it has an odd number of rows or columns
	fft = fft(cv::Rect(0, 0, fft.cols & -2, fft.rows & -2));
	cv::Mat shifted(fft.size(), fft.type());
	shifted = fftShift(fft);

	// zero out the center of the FFT shift (remove low frequencies)
	shifted = zeroOutCenter(shifted);
	// inverse shift + idft to reconstruct the image
	shifted = fftShift(shifted);
	cv::Mat recon;
	cv::idft(shifted, recon, cv::DFT_SCALE);
	// crop the padded border
	recon = recon(cv::Rect(0, 0, grayImg.cols, grayImg.rows));
	result.mask = recon;

	cv::Mat magnitude;
	cv::log(cv::abs(recon), magnitude);
	magnitude *= 20.0;
	cv::Scalar mean = cv::mean(magnitude);
	return mean[0];
}

cv::Mat FFTfocus::fftShift(cv::Mat img) {
	cv::Mat shifted(img.size(), img.type());
	// get the center of the image
	int cX = img.cols/2;
	int cY = img.rows/2;
	int adjustX = img.cols % 2;
	int adjustY = img.rows % 2;

	// q0 -> q3 
	img(cv::Range(0, cY+adjustY), cv::Range(0, cX+adjustX)).copyTo(
		shifted(cv::Range(cY, img.rows), cv::Range(cX, img.cols))
	);
	// q3 -> q0
	img(cv::Range(cY, img.rows), cv::Range(cX, img.cols)).copyTo(
		shifted(cv::Range(0, cY+adjustY), cv::Range(0, cX+adjustX))
	);
	// q1 -> q2
	img(cv::Range(0, cY), cv::Range(cX+adjustX, img.cols)).copyTo(
		shifted(cv::Range(cY+adjustY, img.rows), cv::Range(0, cX))
	);
	// q2 -> q1
	img(cv::Range(cY+adjustY, img.rows), cv::Range(0, cX)).copyTo(
		shifted(cv::Range(0, cY), cv::Range(cX+adjustX, img.cols))
	);

	return shifted;
}

cv::Mat FFTfocus::zeroOutCenter(cv::Mat img) {
	int cX = img.cols/2;
	int cY = img.rows/2;
	int dims = img.dims;
	cv::Mat *planes = new cv::Mat[dims];
	cv::split(img, planes);
	cv::Mat zeros = cv::Mat::zeros(cv::Size(size*2, size*2), CV_64F);

	for (int i = 0; i < dims; ++i) {
		zeros(cv::Range(0, zeros.rows), cv::Range(0, zeros.cols)).copyTo(
			planes[i](cv::Range(cY-size, cY+size), cv::Range(cX-size, cX+size))
		);
	}
	
	cv::merge(planes, dims, img);
	delete [] planes;
	return img;
}
