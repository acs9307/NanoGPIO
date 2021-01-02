#ifndef NANOGPIO_PIN_IS_DEFINED
#define NANOGPIO_PIN_IS_DEFINED

#include <bool.h>
#include <stdio.h>

/***** Defines ******/
#define GET_PHY_PIN(pinNum) SysPins[pinNum-1]
/********************/

/***** Enums *****/
typedef enum
{
	NANOGPIO_INPUT 	= 0,
	NANOGPIO_OUTPUT = 1,
}NanoPinDir;

typedef enum
{
	NANOGPIO_LOW 	= 0,
	NANOGPIO_HIGH 	= 1,	
}NanoPinDigitalValue;
/*****************/

/***** Structures *****/
/* Class that holds details on the pin. */
typedef struct
{
	/* Physical pin number. */
	size_t id;

	/* SysFS GPIO number.  Negative numbers means the pin is not a GPIO. */	
	int gpioID;	

	/* The human readable name. */
	const char* name;

	bool isInitialized;
}NanoPin;
/**********************/

/***** Globals *****/
/* TODO: Will need #defines in here to check for various system types, i.e. if we want
 * this to work with anything other than NVIDIA Jetson Nano. */

/* NVIDIA Jetson Nano Pinout */
NanoPin SysPins[40] = 
{
	{ 1, 	-1, 	"3.3V DC", 		false },
	{ 2, 	-1, 	"5V DC",		false },
	{ 3, 	-1, 	"I2C_2_SDA",	false },
	{ 4, 	-1, 	"5V DC",		false },
	{ 5, 	-1, 	"I2C_2_SCL",	false },
	{ 6, 	-1, 	"GNC", 			false },
	{ 7, 	216, 	"AUDIO_MCLK", 	false },
	{ 8, 	-1,		"UART_2_TX", 	false },
	{ 9, 	-1, 	"GNC",			false },
	{ 10,	-1,		"UART_2_RX",	false },
	{ 11, 	50,		"UART_2_RTS",	false },
	{ 12, 	79, 	"I2S_4_CLK",	false },
	{ 13,	14,		"SPI_2_SCK",	false },
	{ 14, 	-1,		"GNC",			false },
	{ 15,	194,	"LCD_TE",		false },
	{ 16, 	232,	"SPI_2_CS1",	false },
	{ 17, 	-1, 	"3.3V DC", 		false },
	{ 18, 	15,		"SPI_2_CS0",	false },
	{ 19, 	16,		"SPI_1_MOSI",	false },
	{ 20, 	-1, 	"GND",			false },
	{ 21, 	17,		"SPI_1_MISO", 	false },
	{ 22, 	13, 	"SPI_2_MISO", 	false },
	{ 23, 	18,		"SPI_1_SCK",	false },
	{ 24, 	19,		"SPI_2_CS0", 	false },
	{ 25, 	-1, 	"GNC", 			false },
	{ 26, 	20,		"SPI_2_CS1",	false },
	{ 27, 	-1, 	"I2C_1_SDA", 	false },
	{ 28, 	-1, 	"I2C_1_SCL", 	false },
	{ 29, 	149, 	"CAM_AF_EN", 	false },
	{ 30, 	-1, 	"GNC", 			false },
	{ 31, 	200,	"GPIO_PZO", 	false },
	{ 32, 	168,	"LCD_BL_PWM", 	false },
	{ 33, 	38, 	"GPIO_PE6", 	false },
	{ 34, 	-1, 	"GND", 			false },
	{ 35, 	76, 	"I2S_4_LRCK", 	false },
	{ 36, 	51, 	"UART_2_CTS", 	false },
	{ 37, 	12, 	"SPI_2_MOSI", 	false },
	{ 38, 	77,		"I2S_4_SDIN", 	false },
	{ 39,	-1,		"GND",			false },
	{ 40, 	78,		"I2S_4_SDOUT", 	false },
};

NanoPin* GPIOPins[] = 
{
	&SysPins[6],
	&SysPins[10],
	&SysPins[11],
	&SysPins[12],
	&SysPins[14],
	&SysPins[15],
	&SysPins[18],
	&SysPins[20],
	&SysPins[21],
	&SysPins[22],
	&SysPins[23],
	&SysPins[25],
	&SysPins[28],
	&SysPins[30],
	&SysPins[31],
	&SysPins[32],
	&SysPins[34],
	&SysPins[35],
	&SysPins[36],
	&SysPins[37],
	&SysPins[39],
}
/*******************/

/***** Functions *****/
/* Returns the initialized pin. 
 * If the pin has not been initialized, then the required setup will be done so the
 * pin is ready to be used.  If it has already been initialized, then it will simply
 * return a pointer to the instance of the pin.
 *
 * Note that the instance is statically stored in memory and should not be freed by the
 * caller.
 *
 * Parameters:
 * 		gpioID: ID of the GPIO pin. 
 *
 * Returns:
 * 		Pointer to the global pin.  This is statically allocated and should not 
 * 		be freed by the caller. */
NanoPin* NanoPin_Init(int gpioID)
{
	FILE* fp;
	NanoPin** rval = GPIOPins;
	NanoPin** gpioEnd = GPIOPins + (sizeof(GPIOPins) / sizeof(*GPIOPins));
	for(; rval < gpioEnd; ++rval)
	{
		if(*rval->gpioID == gpioID)
			break;
	}

	/* Check if we looked through all the pins. */
	if(rval >= gpioEnd)
		goto f_error;

	if(!*rval->isInitialized)
	{
		/* Initialize the pin if it needs to be initialized. */
		fp = fopen("/sys/class/gpio/export", "w");
		if(!fp)
			goto f_error;

		fprintf(fp, "%d\n", gpioID);
		fclose(fp);
		
		*rval->isInitialized = true;
	}

	return(*rval);
f_error:
	return(NULL);
}

/* Deinitializes the pin.  
 * This should be called after the pin is no longer required. 
 *
 * Parameters:
 * 		pin: NanoPin to deinitialize. */
void NanoPin_Deinit(NanoPin* pin)
{
	FILE* fp;

	if(!pin)return;

	/* Initialize the pin if it needs to be initialized. */
	fp = fopen("/sys/class/gpio/unexport", "w");
	if(!fp)
		goto f_error;

	fprintf(fp, "%d\n", pin->gpioID);
	fclose(fp);
	
	pin->isInitialized = false;
}

/* Sets the direction for the pin. 
 *
 * Parameters:
 * 		pin: Instance of the pin to modify.
 * 		dir: Direction of the pin. 
 *
 * Returns:
 * 		True on success, false otherwise. */
bool NanoPin_SetDirection(NanoPin* pin, NanoPinDir dir)
{
	if(!pin)return(false);

	char fpathBuff[256];
	FILE* fp;

	snprintf(fpathBuff, sizeof(fpathBuff), "/sys/class/gpio/gpio%d/direction", pin->gpioID);

	fp = fopen(fpathBuff, "w");
	if(!fp)return(false);

	if(dir == NANOGPIO_OUTPUT)
		fprintf(fp, "out");
	else
		fprintf(fp, "in");	
	fclose(fp);
	return(true);
}

/* Writes to the digital pin. 
 *
 * Parameters:
 * 		pin: Pin to write to.
 * 		val: Value to write to the pin.
 *
 * Returns:
 * 		True on success, false otherwise. */
bool NanoPin_DigitalWrite(NanoPin* pin, NanoPinDigitalValue val)
{
	if(!pin)return(false);

	char fpathBuff[256];
	FILE* fp;

	snprintf(fpathBuff, sizeof(fpathBuff), "/sys/class/gpio/gpio%d/value", pin->gpioID);
	
	fp = fopen(fpathBuff, "w");
	if(!fp)return(false);

	fprintf(fp, "%d", val);
	fclose(fp);
	return(true);
}
/*********************/

#endif
