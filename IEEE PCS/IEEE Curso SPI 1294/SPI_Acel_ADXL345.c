#include<stdbool.h>
#include<stdint.h>
#include"inc/tm4c1294ncpdt.h"
#include"IEEE_SPI.h"
#include"Acel_ADXL345.h"
#include"driverlib/sysctl.h"

int Vector[3];
float Acel[3];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACIÓN DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//       CLK: PA2    CS/FSS: PA3    TX/MOSI: PA4    RX/MISO: PA5
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x1;            //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x1)==0){}

    //PUERTO A  SPI (TM4C1294NCPDT)
    GPIO_PORTA_AHB_AFSEL_R|=0x3C;           //Función alterna a PA2-PA5
    GPIO_PORTA_AHB_PCTL_R|=0xFFFF00;        //Función SPI a PA2-PA4
    GPIO_PORTA_AHB_DEN_R|=0x3C;             //Hab función digital PA2-PA4
    GPIO_PORTA_AHB_PUR_R|=0x4;              //Pull up PA2 (16 [kOhms])
}


//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACIÓN SPI0    %%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_SPI0(void){
    //COMUNICACIÓN SPI0 (Acelerómetro ADXL345)
    SYSCTL_RCGCSSI_R=0x1;                   //Reloj Módulo SPI0
    while((SYSCTL_PRSSI_R & 0x1)==0){}      //Reloj SPI
    SSI0_CR1_R=0x0;                         //Hab modo Maestro, Deshab puerto QSSI
    SSI0_CPSR_R=0x2;                        //Fclk=16 [MHz]   CPSDVSR=2
                                            //SPI_BR=SysClk/(CPSDVSR*(1+SCR))
                                            //CLK_freq=4 [MHz]
    SSI0_CR0_R=0x1CF;                       //SCR=1
                                            //SPH:1   SPO:1   DSS:16 bit data (0xF)
                                            //FRF:Freescale SPI Format (0)
//    SSI0_CR0_R = (0x0100 | SSI_CR0_FRF_MOTO | SSI_CR0_DSS_16) & ~(SSI_CR0_SPO | SSI_CR0_SPH);
    SSI0_CR1_R|=0x2;                        //Hab puerto QSSI
}


//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ACELERÓMETRO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_LeerDato_SPI(void){
    int i;
//    SysCtlDelay(6667);                                 //Output Data Rate 800 [Hz]
    SysCtlDelay(1667);                                    //Output Data Rate 800 [Hz]                          //Libertad del bus entre STOP y START 1.3 [us]
    //Eje X
    Vector[0]=SPI_Leer(ACE_X_LSB);
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]                                  //Libertad del bus entre STOP y START 1.3 [us]

    Vector[0]|=((SPI_Leer(ACE_X_MSB)&0x3)<<8);
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Y
    Vector[1]=SPI_Leer(ACE_Y_LSB);
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    Vector[1]|=((SPI_Leer(ACE_Y_MSB)&0x3)<<8);
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Z
    Vector[2]=SPI_Leer(ACE_Z_LSB);
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    Vector[2]|=((SPI_Leer(ACE_Z_MSB)&0x3)<<8);
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]

    for(i=0;i<3;i++){
       if(Vector[i]>=512){
           Vector[i]=-((Vector[i]^0x3FF)+1);       //Conversión a Complem 2
       }
       Acel[i]=Vector[i]*0.003906;                    //Factor de escala 3.9 mg/LSB de hoja de datos para +-2g
                                                    //Dato en Q8 Punto fijo
    }
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
