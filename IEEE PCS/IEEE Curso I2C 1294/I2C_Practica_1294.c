#include<stdbool.h>
#include<stdint.h>
#include"IEEE_I2C.h"
#include"Acel_ADXL345.h"
#include"inc/tm4c1294ncpdt.h"
#include"driverlib/sysctl.h"

int Rx[2];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACIÓN DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//                      SCL: PB2    SDA: PB3
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x2;                  //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}

    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C1294NCPDT)
    GPIO_PORTB_AHB_AFSEL_R|=0xC;            //Función alterna a PB2 y PB3
    GPIO_PORTB_AHB_PCTL_R|=0x2200;          //Función I2C a PB2 y PB3
    GPIO_PORTB_AHB_ODR_R|=0x8;              //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
    GPIO_PORTB_AHB_DEN_R|=0xC;              //Hab función digital PB2 y PB3
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACIÓN I2C0    %%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_I2C0(void){
    //COMUNICACIÓN I2C0 (Acelerómetro)    I2C6    PB2:SCL (Sin OD)    PB3:SDA (Con OD)
    SYSCTL_RCGCI2C_R=0x1;                   //Reloj Módulo I2C0
    while((SYSCTL_PRI2C_R & 0x1)==0){}      //Reloj I2C
    I2C0_MCR_R=0x10;                        //Hab modo Maestro del I2C
    I2C0_MTPR_R=0x1;                        //TPR: Timer Period=2    Fclk=16 [MHz]
                                            //SCL_PRD=2*(1+TPR)*(SCL_LP+SCL_HP)/Fclk;   SCL_LP=6 y SCL_HP=4
                                            //TPR=Fclk/(SCL_Freq*(SCL_LP+SCL_HP)*2)-1
                                            //SCL_freq=400 [kHz]
//    I2C0_MIMR_R|=0x1;                         //Hab interrupción cuando se complete una transacción
    NVIC_EN0_R|=(((1<<8)) & 0xFFFFFFFF);
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ESCLAVO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Esclavo_LeerDato(void){
    SysTick(26666);                         //Output Data Rate 800 [Hz]

    while(I2C0_MCS_R&0x40){};               //Mientras el bus no esté en idle
//    I2C_HighSpeed();
    I2C_StartR(0x7A);
    Rx[0]=I2C_Leer_Byte();
    I2C_Stop();
//    SysTick(21);                          //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};               //Mientras el bus no esté en idle
//    I2C_HighSpeed();
    I2C_StartR(0x11);
    Rx[1]=I2C_Leer_Byte();
    I2C_Stop();
//    SysTick(21);                          //Libertad del bus entre STOP y START 1.3 [us]

}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%    INTERRUPCIÓN DEL I2C0    %%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Inter_I2C0(void){
    if(I2C0_MCS_R&0x10){                    //Si se ha perdido el arbitraje
        while(I2C0_MCS_R&0x40){}            //Esperar mientras el bus esté ocupado
    }
    I2C0_MICR_R|=0x1;                       //Limpieza de bandera de interrupción
}

//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
//    SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_320), 40000000); //Generar señal de 40 [MHz] (TM4C1294NCPDT)
    Config_puertos();
    Config_I2C0();
//    I2C_GlitchSupp();
    while(1){
        Esclavo_LeerDato();
    }
}
