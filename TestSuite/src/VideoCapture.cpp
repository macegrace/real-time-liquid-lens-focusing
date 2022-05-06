/* Title: Real-time Liquid Lens Focusing            */
/* Program: AutoFocusEmbedded                       */
/* Module name: VideoCapture                        */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: This library provides control over  */
/* the embedded systems camera                      */

#include <iostream>
#include <sys/poll.h>
#include "VideoCapture.hpp"

#define HEIGHT 1080
#define WIDTH  1920
#define V4L2_TRIG_CTRL_ID 0x0199e208
#define V4L2_TRIG_REG 0x0199e217

using namespace std;

VideoCapture::VideoCapture() {
    try {
        createFileDescriptor();
        //setTriggerMode();
        checkVideoCapture();
        setCaps();
        requestBuffer();
        informAboutBuffer(&bufferInfo);
        mapBuffer();
        activateStreaming(bufferInfo.type);
        mPFd = (struct pollfd*)malloc(sizeof(struct pollfd));
        mPFd->fd = videoFD;
        mPFd->events = POLLIN | POLLRDNORM;
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}

VideoCapture::~VideoCapture() {
    close(videoFD);
}

cv::Mat VideoCapture::getImage() {
    
    // get image
    queueBuffer(&bufferInfo);
    dequeueBuffer(&bufferInfo);

    cv::Mat image = cv::Mat(HEIGHT, WIDTH, CV_8U, bufferStart);
    return image;
}

void VideoCapture::mapBuffer() {
    bufferStart = mmap(NULL, bufferInfo.length, PROT_READ | PROT_WRITE,MAP_SHARED, videoFD, bufferInfo.m.offset);
    if(bufferStart == MAP_FAILED)
        throw(std::runtime_error("VideoCapture : Failed to mmap a buffer"));
}

void VideoCapture::createFileDescriptor() {
    videoFD = 0;
    if((videoFD = open("/dev/video0", O_RDWR, HEIGHT, WIDTH)) < 0)
        throw(std::runtime_error("VideoCapture : Failed to create file descriptor"));
}
 
void VideoCapture::setTriggerMode() {
    struct v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));

    ctrl.id = V4L2_TRIG_CTRL_ID;
    ctrl.value = true;

    if(ioctl(videoFD, VIDIOC_S_CTRL, &ctrl) < 0)
        throw(std::runtime_error("VideoCapture : Failed to set trigger mode"));
}

// check if camera is capable of video capture
void VideoCapture::checkVideoCapture() {
    struct v4l2_capability cap;
    if(ioctl(videoFD, VIDIOC_QUERYCAP, &cap) < 0){
         perror("VIDIOC_QUERYCAP");
        exit(1);
    }

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        throw(std::runtime_error("VideoCapture : The camera does not support video capture"));
    return;
}

void VideoCapture::setCaps() {
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;

    if(ioctl(videoFD, VIDIOC_S_FMT, &format) < 0)
        throw(std::runtime_error("VideoCapture : Failed to set caps"));
}

void VideoCapture::requestBuffer() {
    struct v4l2_requestbuffers bufrequest;
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = 1;

    if(ioctl(videoFD, VIDIOC_REQBUFS, &bufrequest) < 0)
        throw(std::runtime_error("VideoCapture : Failed while requesting a buffer"));
}

void VideoCapture::informAboutBuffer(v4l2_buffer * bufferInfo) {
    memset(bufferInfo, 0, sizeof(bufferInfo));
 
    bufferInfo->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferInfo->memory = V4L2_MEMORY_MMAP;
    bufferInfo->index = 0;
 
    if(ioctl(videoFD, VIDIOC_QUERYBUF, bufferInfo) < 0)
        throw(std::runtime_error("VideoCapture : Failed to inform about buffer"));
}

void VideoCapture::queueBuffer(v4l2_buffer *bufferInfo) {
    try {
        int retval = -1; 
        while(retval < 0) {
            retval = ioctl(videoFD, VIDIOC_QBUF, bufferInfo);
            if(retval < 0) {
                sleep(10);
                throw(std::runtime_error("VideoCapture : Failed to queue a buffer"));
            }
        }
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}

void VideoCapture::dequeueBuffer(v4l2_buffer *bufferInfo) {
    try {
        /*do {
            if(poll(mPFd, 1, 250) <= 0)
                break;
        }
        while(errno = 0, ioctl(videoFD, VIDIOC_DQBUF, bufferInfo) < 0 && (errno == EINVAL || errno == EAGAIN));*/
        errno = 0;
        if(ioctl(videoFD, VIDIOC_DQBUF, bufferInfo) < 0)
            throw(std::runtime_error("VideoCapture : Failed to dequeue a buffer"));
        //else
            //cout << "BUFFER queued successfully" << endl;
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}

void VideoCapture::activateStreaming(int type) {
    if(ioctl(videoFD, VIDIOC_STREAMON, &type) < 0)
        throw(std::runtime_error("VideoCapture : Failed to activate streaming"));
}
 
void VideoCapture::deactivateStreaming(int type) {
    try {
        if(ioctl(videoFD, VIDIOC_STREAMOFF, &type) < 0)
            throw(std::runtime_error("VideoCapture : Failed to deactivate streaming"));
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}
