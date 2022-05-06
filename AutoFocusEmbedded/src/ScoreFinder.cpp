/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusNonEmbedded                    */
/* Module name: ScoreFinder                         */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Module for sharpness score          */
/* assessment                                       */

#include "ScoreFinder.hpp"

ScoreFinder::ScoreFinder() {
    score = 0.0;
}

double ScoreFinder::calcCannyScore(cv::Mat image, double threshold1, double threshold2, unsigned int aperatureSize, bool L2gradient) {
    cv::Mat canny;
    double mean = 0.0;

    cv::Canny(image, canny, threshold1, threshold2, aperatureSize, L2gradient);
    cv::Scalar temp = cv::mean(canny);
    mean = temp.val[0];
    score = mean;
    return score;
}

double ScoreFinder::calcLaplaceScore(cv::Mat image, unsigned int ksize, unsigned int scale, unsigned int delta) {
    double sd = 0;
    cv::Mat laplace, abs_dst, tmp_m, tmp_sd;
    
    cv::Laplacian(image, laplace, CV_8UC1, 3);
    
    cv::meanStdDev(laplace, tmp_m, tmp_sd);
    sd = tmp_sd.at<double>(0,0);
    score = sd;
    return score;
}

double ScoreFinder::calcSobelScore(cv::Mat image, unsigned int ksize, unsigned int scale, unsigned int delta) {
     cv::Mat sobel;
     double mean = 0.0;

     cv::Sobel(image, sobel, CV_8UC1, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
     cv::Scalar temp = cv::mean(sobel);
     mean = temp.val[0];
     score = mean;

     cv::Sobel(image, sobel, CV_8UC1, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
     temp = cv::mean(sobel);
     mean = temp.val[0];
     score += mean;

     return score;
}

double ScoreFinder::calcSobelScoreOverX(cv::Mat image, unsigned int ksize, unsigned int scale, unsigned int delta) {
     cv::Mat sobel;
     double mean = 0.0;

     cv::Sobel(image, sobel, CV_8UC1, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
     cv::Scalar temp = cv::mean(sobel);
     mean = temp.val[0];
     score = mean;

     return score;
}

double ScoreFinder::calcSobelScoreOverY(cv::Mat image, unsigned int ksize, unsigned int scale, unsigned int delta) {
     cv::Mat sobel;
     double mean = 0.0;

     cv::Sobel(image, sobel, CV_8UC1, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
     cv::Scalar temp = cv::mean(sobel);
     mean = temp.val[0];
     score = mean;

     return score;
}

double ScoreFinder::calcFFTScore(cv::Mat grayImg, unsigned int size) {
    fftSize = size;
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

	cv::Mat magnitude;
	cv::log(cv::abs(recon), magnitude);
	magnitude *= 20.0;
	cv::Scalar mean = cv::mean(magnitude);
    score = mean[0];
	return score;
}

cv::Mat ScoreFinder::fftShift(cv::Mat img) {
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

cv::Mat ScoreFinder::zeroOutCenter(cv::Mat img) {
	int cX = img.cols/2;
	int cY = img.rows/2;
	int dims = img.dims;
	cv::Mat *planes = new cv::Mat[dims];
	cv::split(img, planes);
	cv::Mat zeros = cv::Mat::zeros(cv::Size(fftSize*2, fftSize*2), CV_64F);

	for (int i = 0; i < dims; ++i) {
		zeros(cv::Range(0, zeros.rows), cv::Range(0, zeros.cols)).copyTo(
			planes[i](cv::Range(cY-fftSize, cY+fftSize), cv::Range(cX-fftSize, cX+fftSize))
		);
	}
	
	cv::merge(planes, dims, img);
	delete [] planes;
	return img;
}
