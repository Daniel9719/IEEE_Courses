#include<stdbool.h>
#include<stdint.h>
#include"IEEE_I2C.h"
#include"Acel_ADXL345.h"
//#include"inc/tm4c1294ncpdt.h"
#include"inc/tm4c123gh6pm.h"
#include"driverlib/sysctl.h"

//------------- DIRECCCIONES ESCLAVO TIVA -------------------//
static short Esclavo_ADD1=0x7A;
static short Esclavo_ADD2=0x11;

int Rx;
int Vector1[3], Vector2[3];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACIÓN DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//                      SCL: PB2    SDA: PB3
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x2;            //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}

    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C1294NCPDT)
//    GPIO_PORTB_AHB_AFSEL_R|=0xC;         //Función alterna a PB2 y PB3
//    GPIO_PORTB_AHB_PCTL_R|=0x2200;       //Función I2C a PB2 y PB3
//    GPIO_PORTB_AHB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
//    GPIO_PORTB_AHB_DIR_R|=0xC;           //PB2 y PB3 como salida Tx
//    GPIO_PORTB_AHB_DEN_R|=0xC;           //Hab función digital PB2 y PB3

    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C123GH6PM)
    GPIO_PORTB_AFSEL_R|=0xC;         //Función alterna a PB2 y PB3
    GPIO_PORTB_PCTL_R|=0x3300;       //Función I2C a PB2 y PB3
    GPIO_PORTB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
    GPIO_PORTB_DIR_R|=0xC;           //PB2 y PB3 como salida Tx
    GPIO_PORTB_DEN_R|=0xC;           //Hab función digital PB2 y PB3

    //SYSTICK
    NVIC_ST_CTRL_R|=0x4;         //Uso del reloj a 16 [Mhz]
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
    I2C0_MIMR_R|=0x1;                         //Hab interrupción cuando se complete una transacción
    NVIC_EN0_R|=(((1<<8)) & 0xFFFFFFFF);
    NVIC_PRI2_R=(NVIC_PRI2_R&0xFFFFFF00)|0x00000000;  //Prioridad 0 (Mayor)
}

////-------------------------------------------------------
////%%%%%%%%%%%%%%%%%%%%    SYSTICK    %%%%%%%%%%%%%%%%%%%%
////-------------------------------------------------------
//void SysTick(Retraso){
//    NVIC_ST_RELOAD_R=Retraso;
//    NVIC_ST_CURRENT_R=0x0;
//    NVIC_ST_CTRL_R|=0x1;
//    while((NVIC_ST_CTRL_R&0x10000)==0){}
//    NVIC_ST_CTRL_R&=~0x1;
//}

//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ESCLAVO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Esclavo_LeerDato(void){
    SysTick(26666);                                 //Output Data Rate 800 [Hz]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_HighSpeed();
    I2C_StartR(Esclavo_ADD1);
    Rx=I2C_Leer_Byte();
    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_HighSpeed();
    I2C_StartR(Esclavo_ADD2);
    Rx=I2C_Leer_Byte();
    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

}

//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ACELERÓMETRO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_LeerDato(void){
    int i, Aux;
////----------------------- PRIMER SENSOR ---------------------------
    SysTick(26666);                                 //Output Data Rate 800 [Hz]
////    SysTick(20000);                                 //Output Data Rate 800 [Hz]
//    //Eje X
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD0, ACE_X_LSB);
//    I2C_StartR(ACE_ADD0);
//    Vector1[0]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD0, ACE_X_MSB);
//    I2C_StartR(ACE_ADD0);
//    Vector1[0]|=(I2C_Leer_Byte()<<8);
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    //Eje Y
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD0, ACE_Y_LSB);
//    I2C_StartR(ACE_ADD0);
//    Vector1[1]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD0, ACE_Y_MSB);
//    I2C_StartR(ACE_ADD0);
//    Vector1[1]|=(I2C_Leer_Byte()<<8);
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    //Eje Z
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD0, ACE_Z_LSB);
//    I2C_StartR(ACE_ADD0);
//    Vector1[2]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD0, ACE_Z_MSB);
//    I2C_StartR(ACE_ADD0);
//    Vector1[2]|=(I2C_Leer_Byte()<<8);
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    for(i=0;i<3;i++){
//        if((Vector1[i] & 0x8000)==0x8000){
//            Vector1[i]=-((Vector1[i]^0xFFFF)+1);              //Conversión a Complem 2
//        }
//    }

//----------------------- SEGUNDO SENSOR ---------------------------

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ACE_ADD1, ACE_X_LSB);
    I2C_StartR(ACE_ADD1);
    Vector2[0]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ACE_ADD1, ACE_X_MSB);
    I2C_StartR(ACE_ADD1);
    Vector2[0]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    //Eje Y
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD1, ACE_Y_LSB);
//    I2C_StartR(ACE_ADD1);
//    Vector2[1]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD1, ACE_Y_MSB);
//    I2C_StartR(ACE_ADD1);
//    Vector2[1]|=(I2C_Leer_Byte()<<8);
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    //Eje Z
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD1, ACE_Z_LSB);
//    I2C_StartR(ACE_ADD1);
//    Vector2[2]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ACE_ADD1, ACE_Z_MSB);
//    I2C_StartR(ACE_ADD1);
//    Vector2[2]|=(I2C_Leer_Byte()<<8);
//    I2C_Stop();
//    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    for(i=0;i<3;i++){
       if((Vector2[i] & 0x8000)==0x8000){
           Vector2[i]=-((Vector2[i]^0xFFFF)+1);              //Conversión a Complem 2
       }
    }
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%    INTERRUPCIÓN DEL I2C0    %%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Inter_I2C0(void){
    if(I2C0_MCS_R&0x10){                   //Si se ha perdido el arbitraje
        while(I2C0_MCS_R&0x40){}            //Esperar mientras el bus esté ocupado
    }
    I2C0_MICR_R|=0x1;
}

//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
//    SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_320), 40000000); //Generar señal de 40 [MHz]
//    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    Config_puertos();
    Config_I2C0();
//    I2C_GlitchSupp();
//    Acel_Config(ACE_ADD0);
    Acel_Config(ACE_ADD1);
    while(1){
//        Esclavo_LeerDato();
        Acel_LeerDato();
    }
}
