/* Title: Real-time Liquid Lens Focusing            */
/* Program: Non-embedded camera system auto-focus   */
/* Module name: SerialPort                          */
/* Moduel author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Module downloaded from              */
/* https://en.ids-imaging.com/ as part of IDS Peak  */
/* API. Provides control over IDS cameras.          */

#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "SerialPort.hpp"

const unsigned int VTIME_VAL = 10;
const unsigned int VMIN_VAL = 0;

SerialPort::SerialPort(char* path) {
    id = open(path, O_RDWR);
    
    if (id < 0) {
        throw std::runtime_error(strerror(errno));
    }
    else
        isOpen = true;
}

SerialPort::~SerialPort() {
    try {
        closeSerialPort();
    }
    catch(std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
}

void SerialPort::closeSerialPort() {
    if (isOpen) {
        if (close(id) < 0)
            throw std::runtime_error(strerror(errno));
        else
            isOpen = false;
    }
}

void SerialPort::configureSerialPort() { 
    struct termios tty;

    if (tcgetattr(id, &tty) != 0)
        throw std::runtime_error(strerror(errno));

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;
    tty.c_cc[VTIME] = VTIME_VAL;
    tty.c_cc[VMIN] = VMIN_VAL;
    
    if (cfsetspeed(&tty, B57600) != 0)
        throw std::runtime_error(strerror(errno));

    if (tcsetattr(id, TCSANOW, &tty) != 0)
        throw std::runtime_error(strerror(errno));
}

void SerialPort::writeMessage(std::vector <unsigned char> message) {

    if (write(id, message.data(), message.size()) != message.size())
        throw std::runtime_error(strerror(errno));
}

unsigned int SerialPort::readMessage(std::vector <unsigned char> *readBuf) {

    int retVal = read(id, static_cast<void*>(&readBuf->at(0)), readBuf->size());
    if (retVal <= 0)
        throw std::runtime_error(strerror(errno));

    return retVal;
}
