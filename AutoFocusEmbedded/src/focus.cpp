#include <stdio.h>
#include <string>
#include <sstream>
#include <errno.h>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>
#include <linux/i2c-dev.h>

#define I2C_ADDR 0x23
#define V4L2_TRIG_CTRL_ID 0x0199e208

using namespace cv;

int create_file_descriptor() {
    int fd = 0;
    if((fd = open("/dev/video0", O_RDWR, 1080, 1920)) < 0){
        perror("open");
        exit(1);
    }
    return fd;
}

void set_trigger_mode(int fd) {
    struct v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));

    ctrl.id = V4L2_TRIG_CTRL_ID;
    ctrl.value = true;

    ioctl(fd, VIDIOC_S_CTRL, &ctrl);
}   

void check_video_capture(int fd) {
    struct v4l2_capability cap;
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){
        perror("VIDIOC_QUERYCAP");
        exit(1);
    }

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
        fprintf(stderr, "The device does not handle single-planar video capture.\n");
        exit(1);
    }
    return;
}

void set_caps(int fd) {
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    format.fmt.pix.width = 1920;
    format.fmt.pix.height = 1080;

    if(ioctl(fd, VIDIOC_S_FMT, &format) < 0){
        perror("VIDIOC_S_FMT");
        exit(1);
    }
}

void request_buffer(int fd) {
    struct v4l2_requestbuffers bufrequest;
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = 1;
    
    if(ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0){        
	perror("VIDIOC_REQBUFS");
        exit(1);
    }
}

void activate_streaming(int fd, int type) {
    if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
        perror("VIDIOC_STREAMON");
        exit(1);
    }
}

void deactivate_streaming(int fd, int type) {
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
        perror("VIDIOC_STREAMOFF");
        exit(1);
    }
}

// returns a fd of a file to write the value to
int setup_trigger() {
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if(fd == -1) {
	    perror("Trigger: Unable to open file for pin export");
	    exit(1);
    }
    if(write(fd, "17", 2) != 2) {
	    perror("Trigger: Unable to write to a file for pin export");
	    exit(1);
    }
    close(fd);

    fd = open("/sys/class/gpio/gpio17/direction", O_WRONLY);
    if(fd == -1) {
	    perror("Trigger: Unable to open a direction file");
        exit(1);
    }

    if(write(fd, "out", 3) != 3) {
	    perror("Trigger: Unable to write to a direction file");
	    exit(1);
    }
    close(fd);

    fd = open("/sys/class/gpio/gpio17/value", O_WRONLY);
    if(fd == -1) {
	    perror("Trigger: Unable to open a value file");
    	exit(1);
    }

    return fd;
}

void trigger(int value_fd, int ms) {
    if(write(value_fd, "1", 1) != 1) {
	perror("Trigger: Unable to write to a value file");
	exit(1);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    
    if(write(value_fd, "0", 1) != 1) {
	perror("Trigger: Unable to write to a value file");
	exit(1);
    }
}

void destroy_trigger() {
    int unexport_fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if(unexport_fd == -1) {
	perror("Trigger: Unable to open unexport file");
	exit(1);
    }
    
    if(write(unexport_fd, "17", 2) != 2) {
	perror("Trigger: Unable to write to unexport file");
	exit(1);
    }
}

void inform_about_buffer(int fd,v4l2_buffer * bufferinfo) {
    memset(bufferinfo, 0, sizeof(bufferinfo));
     
    bufferinfo->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo->memory = V4L2_MEMORY_MMAP;
    bufferinfo->index = 0;
     
    if(ioctl(fd, VIDIOC_QUERYBUF, bufferinfo) < 0){
        perror("VIDIOC_QUERYBUF");
        exit(1);
    }
}

int setup_alt_I2C(int adapter_addr, int device_addr) {
    int file;
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", adapter_addr);
    
    file = open(filename, O_RDWR);
    if(file < 0) {
	perror("I2C : Unable to open adapter file");
        exit(1);
    }
    
    if(ioctl(file, I2C_SLAVE, device_addr) < 0) {
	perror("I2C : Invalid ioctl call");
	exit(1);
    }
    
    char buf = 0;
    if(write(file, "0", 1) != 1) {
	perror("I2C : Failed to write to a value file");
	exit(1);
    }

    return file;
}

void i2c_write(int file, char* data) {
    if(write(file, data, 1) != 1) {
	perror("I2C : Failed to write to a value file");
	exit(1);
    }
}

void queue_buffer(int fd, v4l2_buffer *bufferinfo) {
    // Put the buffer in the incoming queue.
    if(ioctl(fd, VIDIOC_QBUF, bufferinfo) < 0){
        perror("VIDIOC_QBUF");
        exit(1);
    }
}

void dequeue_buffer(int fd, v4l2_buffer *bufferinfo) {
    // The buffer is waiting in the outgoing queue.
    if( ioctl(fd, VIDIOC_DQBUF, bufferinfo) < 0) {
        perror("VIDIOC_DQBUF");
        exit(1);
    }
}

int main(void){
    
    char write_data = 0, max_data = 0;
    int frames = 0, fd = 0;
    double max_mean = 0.0;

    int trig_fd = setup_trigger();
    int i2c_file = setup_alt_I2C(0x01, 0x23);

    fd = create_file_descriptor();
    set_trigger_mode(fd);
    
    check_video_capture(fd);
    set_caps(fd);
    request_buffer(fd);
   
    struct v4l2_buffer bufferinfo;
    inform_about_buffer(fd, &bufferinfo);
    
    void* buffer_start = mmap(NULL, bufferinfo.length, PROT_READ | PROT_WRITE,MAP_SHARED, fd, bufferinfo.m.offset);
    if(buffer_start == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    int type = bufferinfo.type;
    activate_streaming(fd, type); 
    
    trigger(trig_fd, 30);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    while(1) {
	
	if(frames == 26) {
	    i2c_write(i2c_file, &max_data);
	    std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}

	queue_buffer(fd, &bufferinfo);
    trigger(trig_fd, 2);

	// TODO: parallelize image loading to remove this sleep
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	dequeue_buffer(fd, &bufferinfo);
	
	Mat cvmat = Mat(1080, 1920, CV_8U, buffer_start);
	Mat cropped = cvmat(Rect(810, 390, 300, 300));

	if(frames == 26) { 
	    auto rect = Rect(810, 390, 300, 300);
	    rectangle(cvmat, rect, Scalar(255,255,255), 5);
	    imwrite("focused.pgm", cvmat);
	    imshow("focused", cvmat);
	    waitKey(0);
	    break;
	}

	//Mat canny;
	//Canny(cropped, canny, 10, 30, 3);
	
	Mat sobel;
	Sobel(cropped, sobel, CV_8UC1, 1, 0);
	
	Scalar temp = mean(sobel);
	float mean1 = temp.val[0];
	
	Sobel(cropped, sobel, CV_8UC1, 0, 1);
	Scalar temp2 = mean(sobel);
	float mean2 = temp2.val[0];

	float mean = mean1 + mean2;
	printf("\nData: %i -- mean : %f", write_data, mean);

	if(mean > max_mean) {
	    max_mean = mean;
	    max_data = write_data;
	    if(max_data < 0)
		max_data = 0;
	    if(max_data > 250)
		max_data = 250;
	}
	
	std::ostringstream os;
	os << "file" << frames << ".pgm";

	std::string s = os.str();
	const char * filename = s.c_str();

	frames++;
	write_data = frames * 10;

	if(frames < 26) {
	    i2c_write(i2c_file, &write_data);
	    //std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
	
	// Save image via OpenCV
	//imwrite(filename, cvmat);
    }
    
    destroy_trigger();
    deactivate_streaming(fd, type);
   
    close(i2c_file); 
    close(fd);
    close(trig_fd);
    return EXIT_SUCCESS;
}
