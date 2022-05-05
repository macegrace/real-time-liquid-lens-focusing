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

using namespace std;

VideoCapture::VideoCapture() {
    try {
        create_file_descriptor();
        set_trigger_mode();
        check_video_capture();
        set_caps();
        request_buffer();
        inform_about_buffer(&bufferinfo);
        map_buffer();
        activate_streaming(bufferinfo.type);
        mPFd = (struct pollfd*)malloc(sizeof(struct pollfd));
        mPFd->fd = video_fd;
        mPFd->events = POLLIN | POLLRDNORM;
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}

VideoCapture::~VideoCapture() {
    close(video_fd);
}

cv::Mat VideoCapture::get_image() {
    
    // get image
    queue_buffer(&bufferinfo);
    trigger.send_trigger(5);
    dequeue_buffer(&bufferinfo);

    cv::Mat image = cv::Mat(HEIGHT, WIDTH, CV_8U, buffer_start);
    return image;
}

void VideoCapture::map_buffer() {
    buffer_start = mmap(NULL, bufferinfo.length, PROT_READ | PROT_WRITE,MAP_SHARED, video_fd, bufferinfo.m.offset);
    if(buffer_start == MAP_FAILED)
        throw(std::runtime_error("VideoCapture : Failed to mmap a buffer"));
}

void VideoCapture::create_file_descriptor() {
    video_fd = 0;
    if((video_fd = open("/dev/video0", O_RDWR | O_NONBLOCK, HEIGHT, WIDTH)) < 0)
        throw(std::runtime_error("VideoCapture : Failed to create file descriptor"));
}
 
void VideoCapture::set_trigger_mode() {
    struct v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));

    ctrl.id = V4L2_TRIG_CTRL_ID;
    ctrl.value = true;

    if(ioctl(video_fd, VIDIOC_S_CTRL, &ctrl) < 0)
        throw(std::runtime_error("VideoCapture : Failed to set trigger mode"));
}

// check if camera is capable of video capture
void VideoCapture::check_video_capture() {
    struct v4l2_capability cap;
    if(ioctl(video_fd, VIDIOC_QUERYCAP, &cap) < 0){
         perror("VIDIOC_QUERYCAP");
        exit(1);
    }

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        throw(std::runtime_error("VideoCapture : The camera does not support video capture"));
    return;
}

void VideoCapture::set_caps() {
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;

    if(ioctl(video_fd, VIDIOC_S_FMT, &format) < 0)
        throw(std::runtime_error("VideoCapture : Failed to set caps"));
}

void VideoCapture::request_buffer() {
    struct v4l2_requestbuffers bufrequest;
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = 1;

    if(ioctl(video_fd, VIDIOC_REQBUFS, &bufrequest) < 0)
        throw(std::runtime_error("VideoCapture : Failed while requesting a buffer"));
}

void VideoCapture::inform_about_buffer(v4l2_buffer * bufferinfo) {
    memset(bufferinfo, 0, sizeof(bufferinfo));
 
    bufferinfo->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo->memory = V4L2_MEMORY_MMAP;
    bufferinfo->index = 0;
 
    if(ioctl(video_fd, VIDIOC_QUERYBUF, bufferinfo) < 0)
        throw(std::runtime_error("VideoCapture : Failed to inform about buffer"));
}

void VideoCapture::queue_buffer(v4l2_buffer *bufferinfo) {
    try {
        int retval = -1; 
        while(retval < 0) {
            retval = ioctl(video_fd, VIDIOC_QBUF, bufferinfo);
            if(retval < 0) {
                cout << "QBUF retval = " << retval << endl;
                throw(std::runtime_error("VideoCapture : Failed to queue a buffer"));
            }
        }
        //cout << "BUFFER QUEUED" << endl;
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}

void VideoCapture::dequeue_buffer(v4l2_buffer *bufferinfo) {
    //try {
        do {
            if(poll(mPFd, 1, 5000) <= 0)
                break;
        }
        while(errno = 0, ioctl(video_fd, VIDIOC_DQBUF, bufferinfo) < 0 && (errno == EINVAL || errno == EAGAIN));
        //if(errno)
        //    cout << "dequeue failed" << endl;
        //else
        //    cout << "DEQUEUE BUFFER" << endl;

        /*if( ioctl(video_fd, VIDIOC_DQBUF, bufferinfo) < 0)
            switch(errno) {
                case EAGAIN:
                    cout << "EAGAIN" << endl;
                    break;
            }
            throw(std::runtime_error("VideoCapture : Failed to dequeue a buffer"));
        }
        catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
        */
    //}
}

void VideoCapture::activate_streaming(int type) {
    if(ioctl(video_fd, VIDIOC_STREAMON, &type) < 0)
        throw(std::runtime_error("VideoCapture : Failed to activate streaming"));
}
 
void VideoCapture::deactivate_streaming(int type) {
    try {
        if(ioctl(video_fd, VIDIOC_STREAMOFF, &type) < 0)
            throw(std::runtime_error("VideoCapture : Failed to deactivate streaming"));
    }
    catch(std::exception &ex) {
        std::cout << ex.what() << "\n";
    }
}
