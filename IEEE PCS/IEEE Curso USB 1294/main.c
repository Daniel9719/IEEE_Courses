#include <stdint.h>
#include <stdbool.h>
// #include "driverlib/pin_map.h"
// #include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "DAP_config.h"
#include "DAP.h"
#include "USB_HID.h"

void main(void){
    /* Run from the PLL at CPU_CLOCK MHz. */
    MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
            SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
            SYSCTL_CFG_VCO_240), CPU_CLOCK);
    DAP_Setup();                          // DAP Setup 
    USB_init();

    // Create DAP Thread
    DAP_Thread();

    // // Create SWO Thread
    // SWO_ThreadId = osThreadNew(SWO_Thread, NULL, &SWO_ThreadAttr);
}