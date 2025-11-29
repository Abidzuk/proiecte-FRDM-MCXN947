/*
 * oled.h
 *
 *  Created on: Aug 9, 2024
 *      Author: nxf89430
 */

#ifndef OLED_H_
#define OLED_H_

#include <stdint.h>
#include "stdbool.h"
#include "stdarg.h"
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"

/***************************************************************************************************
 Constants and Macros
***************************************************************************************************/
#define EXAMPLE_I2C_MASTER_BASE (LPI2C2_BASE)
#define LPI2C_MASTER_CLOCK_FREQUENCY CLOCK_GetLPFlexCommClkFreq(2u)
#define EXAMPLE_I2C_MASTER ((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)

#define OLED_COMMAND 0x00
#define OLED_DATA 0x40
#define OLED_ADDRESS 0x3CU
#define I2C_OLED LPI2C2
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern uint8_t currCharLoc[2];
extern const char font[256][6];
extern uint8_t frame[1024];
extern uint8_t img_NXP_Panda_unu[1024];
extern uint8_t img_NXP_Panda_doi[1024];
extern uint8_t img_NXP_Panda_trei[1024];
extern uint8_t img_NXP_Panda_patru[1024];
extern uint8_t img_NXP_Panda_cinci[1024];
extern uint8_t img_NXP_Panda_sase[1024];
extern uint8_t img_NXP_Panda_sapte[1024];
extern uint8_t img_NXP_Panda_opt[1024];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void sendOLED(uint8_t* buffer, uint16_t size, uint8_t CD);
extern void initOLED();
extern void writeChar(uint8_t character, bool inverted);
extern void printfOLED(const char* format, ...);
extern void setPage(uint8_t page);
extern void setSeg(uint8_t seg);
extern void fillOLED(uint8_t data);
extern void lineWrap(void);
extern void printVar(char* formatting, int32_t var, bool inverted, uint8_t seg, uint8_t page);
extern void resetOLED(void);
extern void fillPage(uint8_t data);
extern void scrollOLED(uint8_t rows);
extern void testFont(uint8_t startChar, uint8_t endChar);
extern void showImage(const uint8_t* image_buffer);

uint8_t min(uint8_t num1, uint8_t num2);
uint8_t max(uint8_t num1, uint8_t num2);


#endif /* OLED_H_ */
