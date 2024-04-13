#include<stdbool.h>
#include<stdint.h>
#include"IEEE_I2C.h"
#include<inc/tm4c123gh6pm.h>

int Rx;

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACI�N DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//                      SCL: PB2    SDA: PB3
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x2;            //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}

    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C123GH6PM)
    GPIO_PORTB_AFSEL_R|=0xC;         //Funci�n alterna a PB2 y PB3
    GPIO_PORTB_PCTL_R|=0x3300;       //Funci�n I2C a PB2 y PB3
    GPIO_PORTB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
    GPIO_PORTB_DEN_R|=0xC;           //Hab funci�n digital PB2 y PB3
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACI�N I2C0    %%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_I2C0(void){
    //COMUNICACI�N I2C0 (Aceler�metro)    I2C6    PB2:SCL (Sin OD)    PB3:SDA (Con OD)
    SYSCTL_RCGCI2C_R=0x1;                   //Reloj M�dulo I2C0
    while((SYSCTL_PRI2C_R & 0x1)==0){}      //Reloj I2C
    I2C0_MCR_R=0x20;                        //Hab modo Esclavo del I2C
    I2C0_SCSR_R|=0x1;                       //Hab modo Esclavo del I2C
    I2C0_SIMR_R|=0x1;                       //Hab interrupci�n cuando se reciba un dato
    NVIC_EN0_R|=(((1<<8)) & 0xFFFFFFFF);
    NVIC_PRI2_R=(NVIC_PRI2_R&0xFFFFFF00)|0x00000000;  //Prioridad 0 (Mayor)
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%    INTERRUPCI�N DEL I2C0    %%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Inter_I2C0(void){
    if(I2C0_SCSR_R&0x1){                   //Solicitud de Recepci�n Esclavo
        Rx=I2C0_SDR_R;
    }
    if(I2C0_SCSR_R&0x2){                   //Solicitud de Transmisi�n Esclavo
        if(I2C0_SCSR_R&0x8){
            I2C0_SDR_R=0x33;               //Dato de la direcci�n 2
        }else{
            I2C0_SDR_R=0x55;               //Dato de la direcci�n 1
        }
    }
    I2C0_SICR_R|=0x1;
}

//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
    //    SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_320), 40000000); //Generar se�al de 40 [MHz] (TM4C1294NCPDT)
    // Si se requiere operar el esclavo en HS se debe cambiar la frecuencia de reloj del microcontrolador
    Config_puertos();
    Config_I2C0();
    I2CS_Direccion(0x7A, 0x11);            //Direcci�n 1 y direcci�n 2 de esclavo
    while(1){
    }
}
