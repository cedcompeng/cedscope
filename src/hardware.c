/**
 * \file hardware.c
 * \brief Initializes hardware
 *
 */
/**
 * \page HardwarePinouts Hardware Pinouts
 * Hardware Pinouts
 *-----------------
 * - `ADC0` --> Pin 40: `VREF` 3.0V reference (not fitted)
 * - `ADC1` --> Pin 41: `ADC1` analog input channel 1
 * - `ADC2` --> Pin 42: `ADC2` analog input channel 2
 * - `ADC3` --> Pin 43: `DIN1` digital input or `ADC3` analog input channel 3
 * - `ADC4` --> Pin 44: `DIN2` digital input or `ADC4` analog input channel 4
 * - `DAC0` --> Pin 6:  `DAC1` digital to analog converter channel 1
 * - `DAC1` --> Pin 7:  `DAC2` digital to analog converter channel 2
 * - `PB0`  --> Pin 4:  `DOUT1` digital output 1
 * - `PB1`  --> Pin 5:  `DOUT2` digital output 2 
 * - `PD0`  --> Pin 20: `LED1` digital output
 * - `PA5`  --> Pin 1:  `SW` digital input
 * - `PE0`  --> Pin 28: `ENTXS` enables logic level translator to GainSpan module
 * - `PD1`  --> Pin 21: `EN3V3` enables 3.3V supply to peripherals
 * - `TXE0` --> Pin 33: UART `TX0` connected to GainSpan module
 * - `RXE0` --> Pin 32: UART `RX0` connected to GainSpan module
 * - `TXD0` --> Pin 23: UART `TX1` connected to FTDI USB serial
 * - `RXD0` --> Pin 22: UART `RX1` connected to FTDI USB serial 
 *
 * Defined in \ref hardware.c
 */

#include <asf.h>
#include <string.h>

#include "board.h"
#include "compiler.h"
#include "user_board.h"
#include "conf_board.h"
#include "conf_usart_serial.h"
#include "ioport.h"
#include "adc.h"

#include "hardware.h"


// ADC Configuration structures
struct adc_config			adc_conf;	/**< ADC config structure */
struct adc_channel_config	adcch_conf; /**< ADC channel config structure */

// DAC Configuration structures
struct dac_config			dac_conf;	/**< DAC config structure */


/**
 * \fn void hardware_init(void)
 * \brief Creates I/O ports
 *
 * Sets the ports used by the hardware.
 *
 * The hardware pinouts are defined here for the CEDSCOPE-0 PCB.
 *
 * NOTE `ENTXS` should never be `ON` when `EN3V3` is off because GaiSpan module must not have 
 * voltage on inputs when supply is turned off.
 *
 * Additional information can be found in the [Hardware Pinouts](\ref HardwarePinouts) page.
 *
 */
void hardware_init(void)
{
	
	// Initialize the LED on PORTD 
	ioport_configure_pin(LED1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	// Initialize USARTD0
	ioport_configure_pin(TXD0, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(RXD0, IOPORT_DIR_INPUT);
	
	// Initialize USARTE0
	ioport_configure_pin(TXE0, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(RXE0, IOPORT_DIR_INPUT);
	
	// Initialize I/O control for peripherals 3.3V supply and translator
	ioport_configure_pin(EN3V3, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ENTXS, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	// Initialize the Digital Outputs
	ioport_configure_pin(DOUT1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(DOUT2, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(SWITCH, IOPORT_DIR_INPUT);
	
	// Initialize ADC
	adc_read_configuration(&ADCA, &adc_conf);
	// Channel 0 - pin 1
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE, 1);
	adc_write_configuration(&ADCA, &adc_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
	// Channel 1 - pin 2
	adcch_read_configuration(&ADCA, ADC_CH1, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN2, ADCCH_NEG_NONE, 1);
	adc_write_configuration(&ADCA, &adc_conf);
	adcch_write_configuration(&ADCA, ADC_CH1, &adcch_conf);
	// Channel 2 - pin 3
	adcch_read_configuration(&ADCA, ADC_CH2, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN3, ADCCH_NEG_NONE, 1);
	adc_write_configuration(&ADCA, &adc_conf);
	adcch_write_configuration(&ADCA, ADC_CH2, &adcch_conf);
	
	/*
	// Setup DAC clock
	// Use the sysclk service to enable the clock to the event system
	//sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	// Connect TCC0 overflow to event channel 3
	//EVSYS.CH3MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	// Enable the clock to the DAC trigger timer
	tc_enable(&TCC0);
	// Configure the DAC trigger timer in normal Waveform Generator mode
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	// Configure the DAC trigger timer period to overflow at the specified rate of conversion in Hz
	tc_write_period(&TCC0, (sysclk_get_per_hz() / 500) - 1);
	// Configure the DAC trigger timer clock source to use the peripheral bus frequency
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
	*/
	
		
	// Configuration of DAC
	// Read configuration
	dac_read_configuration(&DACB, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_AVCC, DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, DAC_CH0 | DAC_CH1);
	dac_write_configuration(&DACB, &dac_conf);
	
	
	// Setup USARTD0
	usart_serial_init(USART_USER, &usart_user_options);
	// Setup USARTE0
	usart_serial_init(USART_GAINSPAN, &usart_gainspan_options);
	
	// Setup start conditions
	// ENTXS must always be off before EN3V3 turns off because GainSpan module
	// should not have power to I/O pins when turned off.
	OUT_ENTXS_OFF;
	OUT_EN3V3_OFF;
	OUT_LED1_ON;
	
	// Enable the ADC
	adc_enable(&ADCA);
	// Enable the DAC
	dac_enable(&DACB);
	 
}



/**
 * \fn uint16_t hardware_read_adc(uint8_t ch)
 * \brief Read ADC pin and returns ADC pin value.
 * \param ch ADC channel (ASCII character) '0', '1' or '2' (default)
 * \returns ADC reading
 */
uint16_t hardware_read_adc(uint8_t ch)
{	// Read channel
	if (ch == '0')
	{	
		adc_start_conversion(&ADCA, ADC_CH0);
		adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
		return adc_get_result(&ADCA, ADC_CH0);
	}
	else if (ch == '1')
	{
		adc_start_conversion(&ADCA, ADC_CH1);
		adc_wait_for_interrupt_flag(&ADCA, ADC_CH1);
		return adc_get_result(&ADCA, ADC_CH1);
	}
	else
	{
		adc_start_conversion(&ADCA, ADC_CH2);
		adc_wait_for_interrupt_flag(&ADCA, ADC_CH2);
		return adc_get_result(&ADCA, ADC_CH2);
	}
}	
	
	
/**
 * \fn void hardware_write_dac(uint8_t ch,uint16_t val)
 * \brief Write to DAC channel.
 * \param ch Channel (ASCII character) '0' or '1'
 * \param val Value to write to DAC
 */
void hardware_write_dac(uint8_t ch, uint16_t val)
{
	if (ch == '0')
	{
		dac_wait_for_channel_ready(&DACB, DAC_CH0);
		dac_set_channel_value(&DACB, DAC_CH0, val);	
	}
	else
	{
		dac_wait_for_channel_ready(&DACB, DAC_CH1);
		dac_set_channel_value(&DACB, DAC_CH1, val);
	}	
}


/**
 * \fn static void hardware_mdelay(uint16_t ms)
 * \brief Loops for required delay in milliseconds
 * \param ms Number milliseconds delay
 */
void hardware_mdelay(uint16_t ms)
{
	uint32_t count;

	// Approximate the number of loop iterations needed.
	count = sysclk_get_cpu_hz() / 6;
	count *= ms;
	count /= 1000;

	do 
	{
		asm("");
	} 
	while (--count);
}
