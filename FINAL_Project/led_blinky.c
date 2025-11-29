/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "app.h"
#include <stdio.h>
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "oled.h"
#include "fsl_common.h" 
#include "fsl_lpadc.h" 
#include "fsl_gpio.h"

// DIP SWITCHES
#define BOARD_DIP1_GPIO GPIO2
#define BOARD_DIP1_PIN  0U

#define BOARD_DIP2_GPIO GPIO1
#define BOARD_DIP2_PIN  22U

#define BOARD_DIP4_GPIO GPIO0
#define BOARD_DIP4_PIN  24U

#define BOARD_DIP8_GPIO GPIO0
#define BOARD_DIP8_PIN  25U

// ROTARY ENCODER
#define ROTARY_SW_GPIO  GPIO0
#define ROTARY_SW_PIN   31U
#define ROTARY_A_GPIO   GPIO3
#define ROTARY_A_PIN    19U
#define ROTARY_B_GPIO   GPIO3
#define ROTARY_B_PIN    21U

// JOYSTICK
#define NAV_UP_GPIO     GPIO3
#define NAV_UP_PIN      17U

#define NAV_DOWN_GPIO   GPIO0
#define NAV_DOWN_PIN    30U

#define NAV_LEFT_GPIO   GPIO3
#define NAV_LEFT_PIN    18U

#define NAV_RIGHT_GPIO  GPIO1
#define NAV_RIGHT_PIN   23U


// VARIABILE GLOBALE
volatile uint8_t direction = 1;  
volatile int rotary_diff = 0;    

typedef struct {
    GPIO_Type *gpio;
    uint32_t pin;
} LED_TypeDef_t;

// Array LED-uri
LED_TypeDef_t LEDs[] = {
        {GPIO4, SHIELD_LED1_GPIO_PIN}, 
        {GPIO0, SHIELD_LED2_GPIO_PIN}, 
        {GPIO0, SHIELD_LED3_GPIO_PIN}, 
        {GPIO0, SHIELD_LED4_GPIO_PIN}, 
        {GPIO2, SHIELD_LED5_GPIO_PIN}, 
        {GPIO2, SHIELD_LED6_GPIO_PIN}, 
        {GPIO2, SHIELD_LED7_GPIO_PIN}, 
        {GPIO2, SHIELD_LED8_GPIO_PIN}, 
};

const uint8_t num_leds = sizeof(LEDs) / sizeof(LED_TypeDef_t); 

// Flag-uri Intreruperi
volatile uint32_t sw1_int_flag = 0;
volatile uint32_t sw2_int_flag = 0;

// State Machine
typedef enum {
    STATE_BOOT_LOGO,
    STATE_MAIN_MENU,
    STATE_APP_1, // Rotating LEDs
    STATE_APP_2, // Thermistor & Light
    STATE_APP_3, // Rotary Encoder
    STATE_APP_4, // Joystick
    STATE_ERROR  // Error state
} AppState_t;

AppState_t currentState = STATE_BOOT_LOGO;
AppState_t previousState = STATE_ERROR;

// INTERRUPT HANDLERS

// Handler SW1 (GPIO4)
void GPIO4_INT_0_IRQHANDLER(void) {
  uint32_t pin_flags0 = GPIO_GpioGetInterruptChannelFlags(GPIO4, 0U);
  sw1_int_flag = 1; // 1 = clockwise
  GPIO_GpioClearInterruptChannelFlags(GPIO4, pin_flags0, 0U);
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

// Handler Rotary Encoder (GPIO3)
void GPIO3_INT_1_IRQHANDLER(void) {
  uint32_t pin_flags1 = GPIO_GpioGetInterruptChannelFlags(GPIO3, 1U);
  if (pin_flags1 & (1U << ROTARY_A_PIN)) 
  {
        uint32_t val_a = GPIO_PinRead(ROTARY_A_GPIO, ROTARY_A_PIN);
        uint32_t val_b = GPIO_PinRead(ROTARY_B_GPIO, ROTARY_B_PIN);
        if (val_a != val_b) {
            rotary_diff = -1; // Stanga
        } else {
            rotary_diff = 1;  // Dreapta
        }
  }
  GPIO_GpioClearInterruptChannelFlags(GPIO3, pin_flags1, 1U);
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

// FUNCTIONS
void ctimer_match_callback(uint32_t flags)
{
    sw2_int_flag = 1;
}

void turnOffAllLEDs(void) {
    for(int i=0; i < sizeof(LEDs) / sizeof(LED_TypeDef_t); i++) {
        GPIO_PinWrite(LEDs[i].gpio, LEDs[i].pin, 0);
    }
}

uint16_t readADC(uint32_t triggerID) {
    lpadc_conv_result_t result;
    LPADC_DoSoftwareTrigger(ADC0, 1U << triggerID);
    LPADC_GetConvResultBlocking(ADC0, &result, 0U);
    return result.convValue;
}


// MAIN APP
int main(void) {

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif
    
    // Initializare Pini(Shield)
    BOARD_InitLEDsPins();
    BOARD_InitBUTTONsPins();
    SHIELD_InitLEDsPins();
    SHIELD_InitBUTTONsPins();
    SHIELD_DIPSwitchPins();
    SHIELD_ADCPins(); 
    SHIELD_RotaryPins(); 
    SHIELD_NAVSwitchPins();

    // Configurare I2C Clock
     CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u);
     CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    initOLED();

    // BOOT SEQUENECE (PANDA)
    showImage(img_NXP_Panda_unu);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_doi);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_trei);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_patru);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_cinci);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_sase);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_sapte);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    showImage(img_NXP_Panda_opt);
    SDK_DelayAtLeastUs(500000, SystemCoreClock);
    fillOLED(0x00);

    // Variabile Locale
    uint8_t current_led = 0; 
    uint8_t old_led = 0;  
    ctimer_match_config_t matchConfig = CTIMER0_Match_0_config; 
    uint16_t pot_value = 0;

    uint16_t light_value = 0;
    uint16_t therm_value = 0;

    bool rotary_mode_all = false; 
    int rotary_led_count = 1;     
    int rotary_single_idx = 0;    

    CTIMER_StartTimer(CTIMER0_PERIPHERAL);  

    currentState = STATE_MAIN_MENU;
    previousState = STATE_BOOT_LOGO;

    while(1)
    {      
        // Citire DIP Switches
        uint32_t dp1 = GPIO_PinRead(BOARD_DIP1_GPIO, BOARD_DIP1_PIN);
        uint32_t dp2 = GPIO_PinRead(BOARD_DIP2_GPIO, BOARD_DIP2_PIN);
        uint32_t dp4 = GPIO_PinRead(BOARD_DIP4_GPIO, BOARD_DIP4_PIN);
        uint32_t dp8 = GPIO_PinRead(BOARD_DIP8_GPIO, BOARD_DIP8_PIN); 
        
        // STATE SELECTION
        if (currentState == STATE_ERROR) {
            if (dp1 && dp2 && dp4 && dp8) {
                currentState = STATE_MAIN_MENU;
            }
        }
        else {
            if (!dp1 && dp2 && dp4 && dp8)      currentState = STATE_APP_1; 
            else if (dp1 && !dp2 && dp4 && dp8) currentState = STATE_APP_2; 
            else if (dp1 && dp2 && !dp4 && dp8) currentState = STATE_APP_3; 
            else if (dp1 && dp2 && dp4 && !dp8) currentState = STATE_APP_4; 
            else if (dp1 && dp2 && dp4 && dp8)  currentState = STATE_MAIN_MENU; 
            else                                currentState = STATE_ERROR; 
        }

        // DISPLAY & STATE INIT
        if (currentState != previousState) {
            fillOLED(0x00); 
            resetOLED();    
            turnOffAllLEDs();

            if (currentState == STATE_MAIN_MENU) {
                printfOLED("Main Menu:\n");
                printfOLED("DP1: LED Ring\n");
                printfOLED("DP2: Sensors\n");
                printfOLED("DP4: Rotary Enc\n");
                printfOLED("DP8: Joystick\n");
            }
            else if (currentState == STATE_APP_1) {
                printfOLED("App1: LED Ring\n");
                printfOLED("----------------\n");
            }
            else if (currentState == STATE_APP_2) {
                printfOLED("App2: Sensors\n");
                printfOLED("----------------\n");
            }
            else if (currentState == STATE_APP_3) {
                printfOLED("App3: Rotary Enc\n");
                printfOLED("----------------\n");
                rotary_mode_all = false;
                rotary_single_idx = 0;
                GPIO_PinWrite(LEDs[0].gpio, LEDs[0].pin, 1);
            }
            else if (currentState == STATE_APP_4) {
                printfOLED("App4: Joystick:\n");
                printfOLED("----------------\n");
            }
            else if (currentState == STATE_ERROR) {
                printfOLED("ERROR: Invalid SW\n");
                printfOLED("Configuration!\n\n");
                printfOLED("Please Reset All\n");
                printfOLED("DP Switches.");
            }

            previousState = currentState;
        }

        // APP 1: LED RING
        if (currentState == STATE_APP_1) {
            lpadc_conv_result_t result; 
            if(sw1_int_flag == 1) {
                direction = !direction; 
                sw1_int_flag = 0;   
            }
            if(sw2_int_flag == 1) {
                LPADC_DoSoftwareTrigger(ADC0, 1U);
                LPADC_GetConvResultBlocking(ADC0, &result, 0);
                pot_value = result.convValue >> 3; 
                if (pot_value < 400) { 
                    pot_value = 400; 
                }
                matchConfig.matchValue = pot_value << 12; 
                CTIMER_SetupMatch(CTIMER0_PERIPHERAL, CTIMER0_MATCH_0_CHANNEL, &matchConfig);

                GPIO_PinWrite(LEDs[old_led].gpio, LEDs[old_led].pin, 0);
                GPIO_PinWrite(LEDs[current_led].gpio, LEDs[current_led].pin, 1);

                if(direction) {
                    old_led = current_led;
                    current_led = (current_led + 1) % num_leds;
                } else {
                    old_led = current_led;
                    current_led = (current_led == 0) ? (num_leds - 1) : (current_led - 1);
                }
                sw2_int_flag = 0;
            }
        }

        // APP 2: SENSORS
        else if (currentState == STATE_APP_2) {
            light_value = readADC(2) >> 3; 
            therm_value = readADC(1) >> 3; 
            printVar("Therm: %d    ", therm_value, false, 0, 3);
            printVar("Light: %d    ", light_value, false, 0, 4);
            SDK_DelayAtLeastUs(200000U, CLOCK_GetCoreSysClkFreq());
        }

        // APP 3: ROTARY ENCODER
        else if (currentState == STATE_APP_3) {
            if (GPIO_PinRead(ROTARY_SW_GPIO, ROTARY_SW_PIN) == 0) {
                rotary_mode_all = true;
                rotary_led_count = 8;
                SDK_DelayAtLeastUs(200000U, CLOCK_GetCoreSysClkFreq()); 
            }
            if (rotary_diff != 0) {
                turnOffAllLEDs(); 
                if (rotary_mode_all) {
                    if (rotary_diff == -1) { 
                        if (rotary_led_count > 1) rotary_led_count--;
                        else { rotary_mode_all = false; rotary_single_idx = 0; }
                    }
                    else if (rotary_diff == 1) { 
                        if (rotary_led_count < 8) rotary_led_count++;
                    }
                } else {
                    if (rotary_diff == 1) { 
                        rotary_single_idx++;
                        if (rotary_single_idx >= num_leds) rotary_single_idx = 0;
                    } else if (rotary_diff == -1) { 
                        if (rotary_single_idx == 0) rotary_single_idx = num_leds - 1; 
                        else rotary_single_idx--;
                    }
                }
                rotary_diff = 0; 
            }
            if (rotary_mode_all) {
                for (int i = 0; i < rotary_led_count; i++) GPIO_PinWrite(LEDs[i].gpio, LEDs[i].pin, 1);
            } else {
                GPIO_PinWrite(LEDs[rotary_single_idx].gpio, LEDs[rotary_single_idx].pin, 1);
            }
        }

        // APP 4: JOYSTICK
        else if (currentState == STATE_APP_4) {

            uint32_t up    = !GPIO_PinRead(NAV_UP_GPIO, NAV_UP_PIN);
            uint32_t down  = !GPIO_PinRead(NAV_DOWN_GPIO, NAV_DOWN_PIN);
            uint32_t left  = !GPIO_PinRead(NAV_LEFT_GPIO, NAV_LEFT_PIN);
            uint32_t right = !GPIO_PinRead(NAV_RIGHT_GPIO, NAV_RIGHT_PIN);

            turnOffAllLEDs();


            if (up && right) {
                GPIO_PinWrite(LEDs[1].gpio, LEDs[1].pin, 1); 
            }
            else if (down && right) {
                GPIO_PinWrite(LEDs[3].gpio, LEDs[3].pin, 1); 
            }
            else if (down && left) {
                GPIO_PinWrite(LEDs[5].gpio, LEDs[5].pin, 1); 
            }
            else if (up && left) {
                GPIO_PinWrite(LEDs[7].gpio, LEDs[7].pin, 1); 
            }
            else if (up) {
                GPIO_PinWrite(LEDs[0].gpio, LEDs[0].pin, 1); 
            }
            else if (right) {
                GPIO_PinWrite(LEDs[2].gpio, LEDs[2].pin, 1); 
            }
            else if (down) {
                GPIO_PinWrite(LEDs[4].gpio, LEDs[4].pin, 1); 
            }
            else if (left) {
                GPIO_PinWrite(LEDs[6].gpio, LEDs[6].pin, 1); 
            }
        }
        // ERROR STATE 
        else if (currentState == STATE_ERROR) {
            SDK_DelayAtLeastUs(100000U, CLOCK_GetCoreSysClkFreq());
        }
        else {
            sw1_int_flag = 0;
            sw2_int_flag = 0;
        }
    }
    return 0 ;
}