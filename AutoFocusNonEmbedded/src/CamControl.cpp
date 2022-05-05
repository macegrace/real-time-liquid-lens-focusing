/* Title: Real-time Liquid Lens Focusing                */
/* Program: Non-embedded camera system auto-focus       */
/* Module name: CamControl                              */
/* Module author: IDS Imaging Development Systems GmbH  */
/* Date: 4/2022                                         */
/* Description: Module downloaded from                  */
/* https://en.ids-imaging.com/ as part of IDS Peak      */
/* API. Provides control over IDS cameras.              */

#include "CamControl.hpp"

/* Initialization of cammera resolution
 * resolution is defined in camControl.h
 */
void idsCamera::initResolution(){
	is_GetCameraInfo(this->camId, &(this->camInfo));
	is_GetSensorInfo(this->camId, &(this->senInfo));

	if(this->displWidth == 0)
		this->displWidth = this->senInfo.nMaxWidth;

	if(this->displHeight == 0)
		this->displHeight = this->senInfo.nMaxHeight;

}

bool idsCamera::setDefaultCam(){
	int retVal;
	double newfps = 40.0;

	//retVal = is_PixelClock(this->camId, IS_PIXELCLOCK_CMD_SET, (void*)&fps, sizeof(fps));
	retVal = is_SetFrameRate(this->camId, newfps, &(this->fps));

	if(retVal != IS_SUCCESS)
		std::cerr << "Error to set clock on camera" << std::endl;

	retVal = is_SetColorMode(this->camId, IS_CM_MONO8);


	retVal = is_SetDisplayMode(this->camId, IS_SET_DM_DIB);
		
	//Set frame memory for camera
	is_AllocImageMem(this->camId, displWidth, displHeight, 8,&(this->camMem), &(this->memID));

	return true;
}

idsCamera::idsCamera(){

	int nRet = 0;

	nRet = is_InitCamera(&(this->camId), NULL);
	if(nRet != IS_SUCCESS){
		std::cerr << "Init cammera error id:" << nRet << std::endl;
	}
	
	this->initResolution();

}

idsCamera::idsCamera(int id){

	
	this->initResolution();

}

idsCamera::~idsCamera(){
	
	is_ExitCamera(this->camId);
}

void idsCamera::getResolution(int* wid, int* hei){

	*wid = this->displWidth;
	*hei = this->displHeight;

	return;
}

void idsCamera::setAutoGain(bool *enable) {
    double param1 = *enable ? 1.0 : 0.0;
    double param2 = 0;

    if (IS_SUCCESS != is_SetAutoParameter(this->camId, IS_SET_ENABLE_AUTO_GAIN, &param1, &param2)) {
        param1 = 0;
        is_SetAutoParameter(this->camId, IS_SET_ENABLE_AUTO_GAIN, &param1, &param2);
        *enable = false;
    }

}

void idsCamera::getFrame(cv::Mat *frame){

	
	is_SetImageMem(this->camId, this->camMem, this->memID);
	is_FreezeVideo(this->camId, IS_WAIT);

	void* pMem;
	if(is_GetImageMem(this->camId, &pMem) != IS_SUCCESS)
		std::cerr << "Frame error" << std::endl;

	memcpy(frame->ptr(), pMem, displWidth*displHeight);
}
