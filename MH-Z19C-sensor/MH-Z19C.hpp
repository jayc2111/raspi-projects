
#ifndef MH_Z19C_H
#define MH_Z19C_H

class co2_sensor {

public:
    co2_sensor(const char* device = "/dev/ttyS0");
    ~co2_sensor();
    
    int8_t  init_serial(void);
    int8_t  checksum(char*);
    int8_t  set_self_calibration(void);
    int16_t read_co2_concentration(void);

private:
    int16_t read_check(void);
    
    const char*   dev;
    int32_t       fd;
    const uint8_t answer_bytes = 9u;
    
};

#endif /* MH_Z19C_H */
