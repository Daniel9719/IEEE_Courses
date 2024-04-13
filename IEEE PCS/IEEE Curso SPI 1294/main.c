#include <Acel_ADXL345_v1.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

typedef union
{
    struct {
        uint16_t HSB : 8;
        uint16_t LSB : 8;
    } Bits;
    uint16_t Dato;
} Eje;

static struct {
    Eje EjeX;
    Eje EjeY;
    Eje EjeZ;
} Acel_Qi;

// uint32_t Vector[3];
static float Acel[3];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACION DE PUERTOS ASOCIADOS AL SPI0  %%%%%%%%%%%
//       CLK: PA2    CS/FSS: PA3    TX/MOSI: PA4    RX/MISO: PA5
//---------------------------------------------------------------------
void Config_puertos(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Hab del reloj del puerto GPIOA

    GPIOPinConfigure(GPIO_PA2_SSI0CLK);             //Funcion SPI a PA2-PA5
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    /* Funcion alterna y digital PA2-PA5 */
    GPIOPinTypeSSI(GPIO_PORTA_AHB_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
}


//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACION SSI0    %%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_SPI0(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);     //Hab del reloj del módulo SSI
    SSIDisable(SSI0_BASE);                          //Deshab puerto QSSI            
// ui32SysClock
    /*  SCR=1   SPH:1   SPO:1  FRF:Freescale SPI Format (0)   Hab modo Maestro   DSS:16 bit data */
    SSIConfigSetExpClk(SSI0_BASE, 160000000, SSI_FRF_MOTO_MODE_3,
                       SSI_MODE_MASTER, 5000000, 16);    //SysCtlClockGet()   160000000
    SSIEnable(SSI0_BASE);                           //Hab puerto QSSI

}


//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ACELER�METRO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_LeerDato_SPI(void){
    int i;
    uint32_t  SPI_Data_Temp1 = 0;
    uint32_t  SPI_Data_Temp2 = 0;

//    SysCtlDelay(6667);                                 //Output Data Rate 800 [Hz]
    SysCtlDelay(1667);                                    //Output Data Rate 800 [Hz]                          //Libertad del bus entre STOP y START 1.3 [us]
    //Eje X
    // SPI_Data_Temp1 = (ACE_X_LSB)<<8|0xC000;
    SPI_Data_Temp1 = (ACE_X_LSB)<<8|0x8000;
    SSIDataPut(SSI0_BASE, SPI_Data_Temp1);
    SSIDataGet(SSI0_BASE,&Acel_Qi.EjeX.Dato);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]                                  //Libertad del bus entre STOP y START 1.3 [us]

    SPI_Data_Temp1 = (ACE_X_MSB)<<8|0x8000;
    SSIDataPut(SSI0_BASE, SPI_Data_Temp1);
    SSIDataGet(SSI0_BASE,&SPI_Data_Temp2);
    Acel_Qi.EjeX.Bits.HSB=((SPI_Data_Temp2)<<0);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Y
    SPI_Data_Temp1 = (ACE_Y_LSB)<<8|0x8000;
    SSIDataPut(SSI0_BASE, SPI_Data_Temp1);
    SSIDataGet(SSI0_BASE,&Acel_Qi.EjeY.Dato);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]                                  //Libertad del bus entre STOP y START 1.3 [us]

    SPI_Data_Temp1 = (ACE_Y_MSB)<<8|0x8000;
    SSIDataPut(SSI0_BASE, SPI_Data_Temp1);
    SSIDataGet(SSI0_BASE,&SPI_Data_Temp2);
    Acel_Qi.EjeY.Bits.LSB=((SPI_Data_Temp2)<<0);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Z
    SPI_Data_Temp1 = (ACE_Z_LSB)<<8|0x8000;
    SSIDataPut(SSI0_BASE, SPI_Data_Temp1);
    SSIDataGet(SSI0_BASE,&Acel_Qi.EjeZ.Dato);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]                                  //Libertad del bus entre STOP y START 1.3 [us]

    SPI_Data_Temp1 = (ACE_Z_MSB)<<8|0x8000;
    SSIDataPut(SSI0_BASE, SPI_Data_Temp1);
    SSIDataGet(SSI0_BASE,&SPI_Data_Temp2);
    Acel_Qi.EjeZ.Bits.HSB=((SPI_Data_Temp2)<<0);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    if(Acel_Qi.EjeX.Dato>=512){
        Acel_Qi.EjeX.Dato=-((Acel_Qi.EjeX.Dato^0x3FF)+1);       //Conversi�n a Complem 2
    }
    Acel[0]=Acel_Qi.EjeX.Dato*0.003906;                    //Factor de escala 3.9 mg/LSB de hoja de datos para +-2g
    if(Acel_Qi.EjeY.Dato>=512){
        Acel_Qi.EjeY.Dato=-((Acel_Qi.EjeY.Dato^0x3FF)+1);       //Conversi�n a Complem 2
    }
    Acel[1]=Acel_Qi.EjeY.Dato*0.003906;                    //Factor de escala 3.9 mg/LSB de hoja de datos para +-2g
    if(Acel_Qi.EjeZ.Dato>=512){
        Acel_Qi.EjeZ.Dato=-((Acel_Qi.EjeZ.Dato^0x3FF)+1);       //Conversi�n a Complem 2
    }
    Acel[2]=Acel_Qi.EjeZ.Dato*0.003906;                    //Factor de escala 3.9 mg/LSB de hoja de datos para +-2g
                                                        //Dato en Q8 Punto fijo

}


//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
    Config_puertos();
    Config_SPI0();
    Acel_Config_SPI();
    while(1){
        Acel_LeerDato_SPI();
    }
}
