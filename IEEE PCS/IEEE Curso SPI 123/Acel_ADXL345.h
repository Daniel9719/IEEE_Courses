#ifndef ACEL_ADXL345_H_
#define ACEL_ADXL345_H_

//------------- DIRECCCIONES ACELER�METRO -------------------//
static short ACE_DATA_FORMAT=0x31;        //Formato de los datos
static short ACE_X_LSB=0x32;              //8 bits inferiores Aceler�metro Eje X
static short ACE_X_MSB=0x33;              //8 bits inferiores Aceler�metro Eje X
static short ACE_Y_LSB=0x34;              //8 bits inferiores Aceler�metro Eje Y
static short ACE_Y_MSB=0x35;              //8 bits superiores Aceler�metro Eje Y
static short ACE_Z_LSB=0x36;              //8 bits inferiores Aceler�metro Eje Z
static short ACE_Z_MSB=0x37;              //8 bits superiores Aceler�metro Eje Z
static short ACE_ADD0=0x53;                //Direcci�n del Esclavo del Aceler�metro
static short ACE_ADD1=0x1D;                //Direcci�n del Esclavo del Aceler�metro
static short ACE_POWER_CTL=0x2D;            //Tasa de datos y control de modo de encendido
static short ACE_FIFO_CTL=0x38;            //Tasa de datos y control de modo de encendido
static short ACE_BW_RATE=0x2C;            //Tasa de datos y control de modo de encendido
static short ACE_OFFX=0x1E;               //Offset Eje X
static short ACE_OFFY=0x1F;               //Offset Eje Y
static short ACE_OFFZ=0x20;               //Offset Eje Z

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%    CONFIGURACI�N DEL ACELER�METRO    %%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_Config_I2C(int ADD);
void Acel_Config_SPI(void);

#endif /* ACEL_ADXL345_H_ */
