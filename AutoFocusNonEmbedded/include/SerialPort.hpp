/* Title: Real-time Liquid Lens Focusing            */
/* Program: Non-embedded camera system auto-focus   */
/* Module name: SerialPort                          */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class supporting the Flexiboard     */
/* library. Establishes connection via Serial Port. */

#pragma once

#include <vector>

class SerialPort {
    private:

        int id;
        bool isOpen = false;
    public:

        SerialPort(char* path);
        ~SerialPort();
        void configureSerialPort();
        void writeMessage(std::vector <unsigned char> message);
        unsigned int readMessage(std::vector <unsigned char>* message);
        void closeSerialPort();

        SerialPort(const SerialPort&) = delete;
        SerialPort& operator = (SerialPort&) = delete;
};
