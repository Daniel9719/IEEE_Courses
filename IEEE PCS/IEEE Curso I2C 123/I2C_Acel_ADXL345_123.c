#include<stdbool.h>
#include<stdint.h>
#include"IEEE_I2C.h"
#include"Acel_ADXL345.h"
#include"inc/tm4c123gh6pm.h"

int Vector[3];
float Acel[3];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACIÓN DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//                      SCL: PB2    SDA: PB3
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x2;            //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}
    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C123GH6PM)
    GPIO_PORTB_AFSEL_R|=0xC;         //Función alterna a PB2 y PB3
    GPIO_PORTB_PCTL_R|=0x3300;       //Función I2C a PB2 y PB3
    GPIO_PORTB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
    GPIO_PORTB_DEN_R|=0xC;           //Hab función digital PB2 y PB3
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
                                            //SCL_freq=400 [kHz]
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ACELERÓMETRO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_LeerDato(uint8_t ADD){
    int i;
    SysTick(26666);                                 //Output Data Rate 800 [Hz]
//    SysTick(20000);                                 //Output Data Rate 800 [Hz]
    //Eje X
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ADD, ACE_X_LSB);
    I2C_StartR(ADD);
    Vector[0]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ADD, ACE_X_MSB);
    I2C_StartR(ADD);
    Vector[0]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Y
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ADD, ACE_Y_LSB);
    I2C_StartR(ADD);
    Vector[1]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ADD, ACE_Y_MSB);
    I2C_StartR(ADD);
    Vector[1]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Z
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ADD, ACE_Z_LSB);
    I2C_StartR(ADD);
    Vector[2]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(ADD, ACE_Z_MSB);
    I2C_StartR(ADD);
    Vector[2]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    for(i=0;i<3;i++){
       if(Vector[i]>2045){
           Vector[i]=-((Vector[i]^0xFFFF)+1);       //Conversión a Complem 2
       }
       Acel[i]=Vector[i]*0.0039;                    //Factor de escala 3.9 mg/LSB de hoja de datos para +-2g
    }
}


//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
    Config_puertos();
    Config_I2C0();
    Acel_Config(ACE_ADD1);
    while(1){
        Acel_LeerDato(ACE_ADD1);
    }
}


