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
        int video_fd;
        struct v4l2_buffer bufferinfo;
        struct pollfd * mPFd;
        void* buffer_start;
        Trigger trigger;

        void create_file_descriptor();
        void set_trigger_mode();
        void check_video_capture();
        void set_caps();
        void request_buffer();
        void inform_about_buffer(v4l2_buffer * bufferinfo);
        void map_buffer();
        void activate_streaming(int type);
        void deactivate_streaming(int type);
        void queue_buffer(v4l2_buffer *bufferinfo);
        void dequeue_buffer(v4l2_buffer *bufferinfo);
        
    public:
        VideoCapture();
        ~VideoCapture();
        
        cv::Mat get_image();
};
