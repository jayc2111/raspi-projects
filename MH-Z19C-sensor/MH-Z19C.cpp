#include <iostream>
#include <cstring>

// linux headers
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "MH-Z19C.hpp"
#include "easy_log.h"


co2_sensor::co2_sensor(const char* device) :
dev(device)
{
}


co2_sensor::~co2_sensor() {
    close(fd);
}


int8_t co2_sensor::init_serial(void) {
    int8_t rv = -1;
    struct termios tty;
    
    if ( (fd = open(dev, O_RDWR)) < 0 )
        ERR("Error %d from open %s", errno, std::strerror(errno));
    else if ( tcgetattr(fd, &tty) != 0 )
        ERR("Error %d from tcgetattr %s", errno, std::strerror(errno));
    else {
        tty.c_cflag &= ~PARENB;  // clear parity bit
        tty.c_cflag &= ~CSTOPB;  // clear stop bit
        tty.c_cflag &= ~CRTSCTS; // disable hardware flow ctrl
        tty.c_cflag &= ~CSIZE;   // clear size and set to 8
        tty.c_cflag |= CS8;

        tty.c_cflag |= CREAD ;   // disable carrier detect
        tty.c_cflag |= CLOCAL;   // allow reading
        tty.c_lflag &= ~ICANON;  // Disable canonical
        tty.c_lflag &= ~ECHO;    // Disable echo
        tty.c_lflag &= ~ECHOE;   // Disable erasure
        tty.c_lflag &= ~ECHONL;  // Disable new-line echo
        tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP
        
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
        tty.c_oflag &= ~OPOST;
        tty.c_oflag &= ~ONLCR;
        tty.c_cc[VTIME] = 10;
        tty.c_cc[VMIN] = answer_bytes;
        
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);
        
        if (tcsetattr(fd, TCSANOW, &tty) != 0)
            ERR("Error %d from tcsetattr %s", errno, std::strerror(errno));
        else
            rv = 0;
    }
    
    return rv;
}


int16_t co2_sensor::read_co2_concentration(void) {
    const char read_cmd[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    
    // send command to UART
    if (write(fd, read_cmd, sizeof(read_cmd)) > 0)
        return read_check();
    else
        return -1;
    
}


int8_t co2_sensor::set_self_calibration(void) {
    const char read_cmd[] = {0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6}; // ON
//     char read_cmd[] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86}; // OFF

//     read_cmd[8] = checksum(&read_cmd[0]);
    
    // send command to UART
    return write(fd, read_cmd, sizeof(read_cmd));
}

int8_t co2_sensor::checksum(char* read_buf) {
    int8_t chksum = 0;
    char* buf_ptr = read_buf;
    
    // skip first index
    buf_ptr++;
    
    // sum the following bytes
    for(char i = 7; i--;) {
            chksum += *buf_ptr;
            buf_ptr++;
    }
    
    // invert and add "1"
    chksum = ~chksum + 0x01;
    DEBUG("chksum = 0x%02x ", (0xFF && chksum));
    
    return chksum;
}


int16_t co2_sensor::read_check(void) {
    uint16_t rv = -1;
    char read_buf[answer_bytes];
    
    // expect 9 bytes
    rv = read(fd, &read_buf, sizeof(read_buf));
    
    if (rv > 0) {
        
#ifdef DEBUG
        char log_buf[256] = { 0 };
        snprintf(log_buf, sizeof(log_buf), "-> ");
        for (auto idx = 0; idx < 9; idx++)
            snprintfcat(log_buf, sizeof(log_buf), "0x%02x ", read_buf[idx]);
        
//         DEBUG(log_buf, NULL);
#endif
        
        if ( (int8_t)read_buf[8] != checksum(&read_buf[0]) ) {
            ERR_L("failed to verify checksum");
            rv = -1;
        }
        else {
            rv = (read_buf[2]<<8) + read_buf[3];
        }
    }
    
    return rv;
}




