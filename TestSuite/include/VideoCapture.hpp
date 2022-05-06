/* Program: Test suite for sharpness score          */
/* assesment methods                                */
/* Module name: VideoCapture                        */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control over  */
/* the embedded systems camera                      */

#pragma once

#include <sys/mman.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>

#include "Trigger.hpp"
#include "ADM00931.hpp"

class VideoCapture {
    private:
        int videoFD;
        struct v4l2_buffer bufferInfo;
        struct pollfd * mPFd;
        void* bufferStart;
        Trigger trigger;

        void createFileDescriptor();
        void setTriggerMode();
        void checkVideoCapture();
        void setCaps();
        void requestBuffer();
        void informAboutBuffer(v4l2_buffer * bufferinfo);
        void mapBuffer();
        void activateStreaming(int type);
        void deactivateStreaming(int type);
        void queueBuffer(v4l2_buffer *bufferinfo);
        void dequeueBuffer(v4l2_buffer *bufferinfo);
        
    public:
        VideoCapture();
        ~VideoCapture();
        
        cv::Mat getImage();
};
