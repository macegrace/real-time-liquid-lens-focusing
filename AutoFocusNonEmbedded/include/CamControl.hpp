/* Title: Real-time Liquid Lens Focusing                */
/* Program: Non-embedded camera system auto-focus       */
/* Module name: CamControl                              */
/* Module author: IDS Imaging Development Systems GmbH  */
/* Date: 4/2022                                         */
/* Description: Module downloaded from                  */
/* https://en.ids-imaging.com/ as part of IDS Peak      */

#pragma once

#ifndef CAMCONTROL_H
#define CAMCONTROL_H

// camera resolution, 0 denotes the maximal resoution
#define RES_WIDTH 0
#define RES_HEIGHT 0

#include <iostream>
#include <ueye.h>

#include <opencv2/opencv.hpp>


class idsCamera{

	private:
		int displWidth = RES_WIDTH;
		int displHeight = RES_HEIGHT;


		int memID = 0;
		char* camMem;

		CAMINFO camInfo;
		SENSORINFO senInfo;

		void initResolution();


	public:
		//Get any camera
		idsCamera();
		//Get camera by id
		idsCamera(int id);

		~idsCamera();

		
		void getResolution(int* wid, int* hei);
		bool setDefaultCam();
        void setAutoGain(bool* enable);

		void getFrame(cv::Mat *frame);

		HIDS camId = 0;

		double fps;

};

#endif
