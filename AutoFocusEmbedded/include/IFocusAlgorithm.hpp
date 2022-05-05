/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: IFocusAlgortihm                     */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class for Focus algorithm           */


#pragma once

#include <opencv2/core.hpp>
#include <Poco/SharedPtr.h>

class IFocusAlgorithm {
public:
	typedef Poco::SharedPtr<IFocusAlgorithm> Ptr;

	struct FocusResult {
		double score;
		double upperHalfScore;
		double lowerHalfScore;
		bool isSharp;
		bool isUpperSharp;
		bool isLowerSharp;
		cv::Mat mask;
	};

	virtual ~IFocusAlgorithm();
	virtual FocusResult calculate(cv::Mat image) = 0;

	inline cv::Mat getUpperRect(const cv::Mat &image) { // (6/8 of width and 6/8 of upper half height)
		return image(cv::Rect(image.cols * 0.25, image.rows * 0.125, image.cols * 0.75, image.rows * 0.375));
	}
	inline cv::Mat getLowerRect(const cv::Mat &image) { // (6/8 of width and 6/8 of lower half height)
		return image(cv::Rect(image.cols * 0.25, image.rows * 0.5, image.cols * 0.75, image.rows * 0.375));
	}

	FocusResult result;
	bool measureSectors;
	int scorePrecision;
};
