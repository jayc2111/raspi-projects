
/*********************************************
* $Revision: 2 $  $Date: 2017-09-29 13:52:04 +0200 (Fr, 29 Sep 2017) $
* -------------
* Revision log
* -------------
* Version     Date        Comment
*    1        18-02-2023  first version
*
*********************************************/
#include <iostream>
#include <cstdarg>

#include "easy_log.h"
#include "MH-Z19C.hpp"
    

/*********************************************
** MAIN
*********************************************/
int main (int argc, const char** argv) {
    int rv = -1;
    
    auto sensor = co2_sensor();
    
    if (sensor.init_serial() != 0)
        ERR_L("init failed");
    else {
        if ( sensor.set_self_calibration() < 0 )
            ERR_L("failed to set self calibration");
        else
            if ( (rv = sensor.read_co2_concentration()) > 0 ) {
                std::cout << rv << " ppm" << std::endl;
                rv = 0;
            }
    
    }
    return rv;
} /**** main ****/


