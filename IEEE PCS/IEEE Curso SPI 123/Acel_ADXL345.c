#include<stdbool.h>
#include<stdint.h>
#include"inc/tm4c1294ncpdt.h"
#include"Acel_ADXL345.h"
#include"driverlib/sysctl.h"

#include "driverlib/ssi.h"
#include "inc/hw_memmap.h"


//--------------------------------------------------------------------
//%%%%%%%%%%%%%%    CONFIGURACI�N DEL ACELER�METRO    %%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_Config_SPI(void){
    uint32_t Temp;

    //Apagado del sensor
    Temp = (ACE_POWER_CTL)<<8|0x0;
    SSIDataPut(SSI0_BASE,Temp);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Uso de +-2g 10 bits
    Temp = (ACE_DATA_FORMAT)<<8|0x0;
    SSIDataPut(SSI0_BASE,Temp);
    SysCtlDelay(54);
    //Configuraci�n del Baud Rate a 3200 [Hz]
    // Temp = (ACE_BW_RATE)<<8|0xD;                    //Tasa de 800 [Hz]
    Temp = (ACE_BW_RATE)<<8|0xF;                    //Tasa de 3200 [Hz]
    SSIDataPut(SSI0_BASE,Temp);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Uso de FIFO en modo Stream
    Temp = (ACE_FIFO_CTL)<<8|0x80;
    SSIDataPut(SSI0_BASE,Temp);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]                                //Libertad del bus entre STOP y START 1.3 [us]
    //Modo de medici�n encendido
    Temp = (ACE_POWER_CTL)<<8|0x8;
    SSIDataPut(SSI0_BASE,Temp);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
}



