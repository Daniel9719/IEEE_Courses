#include<stdbool.h>
#include<stdint.h>
#include<IEEE_I2C.h>
//#include<inc/tm4c1294ncpdt.h>
#include<inc/tm4c123gh6pm.h>

//------------- DIRECCCIONES TERMÓMETRO DIGITAL -------------------//
static uint8_t TEMP_ADD0=0x48;              //8 bits inferiores Acelerómetro Eje X
static uint8_t TEMP_ADD1=0x49;              //8 bits inferiores Acelerómetro Eje X
static uint8_t TEMP_ADD2=0x4A;              //8 bits inferiores Acelerómetro Eje Y
static uint8_t TEMP_ADD3=0x4B;              //8 bits superiores Acelerómetro Eje Y
static uint8_t TEMP_ADD4=0x4C;              //8 bits inferiores Acelerómetro Eje Z
static uint8_t TEMP_ADD5=0x4D;              //8 bits superiores Acelerómetro Eje Z
static uint8_t TEMP_ADD6=0x4E;              //8 bits superiores Acelerómetro Eje Z
static uint8_t TEMP_ADD7=0x4F;              //8 bits superiores Acelerómetro Eje Z

int Temp[8];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACIÓN DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//                      SCL: PB2    SDA: PB3
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x2;            //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}

//    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C1294NCPDT)
//    GPIO_PORTB_AHB_AFSEL_R|=0xC;         //Función alterna a PB2 y PB3
//    GPIO_PORTB_AHB_PCTL_R|=0x2200;       //Función I2C a PB2 y PB3
//    GPIO_PORTB_AHB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
//    GPIO_PORTB_AHB_DIR_R|=0xC;           //PB2 y PB3 como salida Tx
//    GPIO_PORTB_AHB_DEN_R|=0xC;           //Hab función digital PB2 y PB3

    //PUERTO B  I2C (PB2 SCL y PB3 SDA)  (TM4C123GH6PM)
    GPIO_PORTB_AFSEL_R|=0xC;         //Función alterna a PB2 y PB3
    GPIO_PORTB_PCTL_R|=0x3300;       //Función I2C a PB2 y PB3
    GPIO_PORTB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
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
                                            //SCL_freq=400 [kHz]
//    I2C0_MIMR_R|=0x1;                         //Hab interrupción cuando se complete una transacción
    NVIC_EN0_R|=(((1<<8)) & 0xFFFFFFFF);
    NVIC_PRI2_R=(NVIC_PRI2_R&0xFFFFFF00)|0x00000000;  //Prioridad 0 (Mayor)
}

//-------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    SYSTICK    %%%%%%%%%%%%%%%%%%%%
//-------------------------------------------------------
void SysTick(Retraso){
    NVIC_ST_RELOAD_R=Retraso;
    NVIC_ST_CURRENT_R=0x0;
    NVIC_ST_CTRL_R|=0x1;
    while((NVIC_ST_CTRL_R&0x10000)==0){}
    NVIC_ST_CTRL_R&=~0x1;
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ACELERÓMETRO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Temp_LeerDato(void){
    int i;
    SysTick(1360000);                                 //Output Data Rate 11.8 [Hz]
//    SysTick(20000);                                 //Output Data Rate 800 [Hz]
    //Sensor de Temperatura 0
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartR(TEMP_ADD0);
    Temp[0]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
//
//    //Sensor de Temperatura 1
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD1);
//    Temp[1]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
//
//    //Sensor de Temperatura 2
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD2);
//    Temp[2]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
//
//    //Sensor de Temperatura 3
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD3);
//    Temp[3]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
//
//    //Sensor de Temperatura 4
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD4);
//    Temp[4]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
//
//    //Sensor de Temperatura 5
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD5);
//    Temp[5]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
//
//    //Sensor de Temperatura 6
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD6);
//    Temp[6]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
//
//    //Sensor de Temperatura 7
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartR(TEMP_ADD7);
//    Temp[7]=I2C_Leer_Byte();
//    I2C_Stop();
//    SysTick(21);
}

//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
    Config_puertos();
    Config_I2C0();
    while(1){
        Temp_LeerDato();
    }
}
