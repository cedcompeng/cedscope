/**
 * \file hardware.h
 * \brief Defines hardware ports
 *
 */


#include <gpio.h>

#ifndef HARDWARE_H
#define HARDWARE_H
	
	
#define HARDWARE_BUFSIZE		32		/**< def User buffer size */

char* hardware_gainspan_buf_tx[HARDWARE_BUFSIZE];	/**< Serial TX circular buffer */

uint8_t hardware_gainspan_head_tx;	/**< Head index in TX circular buffer */
uint8_t hardware_gainspan_tail_tx;	/**< Tail Index in TX circular buffer */

char* hardware_gainspan_buf_rx[HARDWARE_BUFSIZE];	/**< Serial RX buffer */
uint8_t hardware_gainspan_i_rx;		/**< Index in RX buffer */


uint16_t hardware_adc[6];	/**< ADC channel results array */

	
// Define pins	
#define LED1		IOPORT_CREATE_PIN(PORTD,0)
#define EN3V3		IOPORT_CREATE_PIN(PORTD,1)
#define RXD0		IOPORT_CREATE_PIN(PORTD,2)
#define TXD0		IOPORT_CREATE_PIN(PORTD,3)
#define ENTXS		IOPORT_CREATE_PIN(PORTE,0)
#define RXE0		IOPORT_CREATE_PIN(PORTE,2)
#define TXE0		IOPORT_CREATE_PIN(PORTE,3)
#define SWITCH		IOPORT_CREATE_PIN(PORTA,5)
#define DOUT1		IOPORT_CREATE_PIN(PORTB,0)
#define DOUT2		IOPORT_CREATE_PIN(PORTB,1)

// Define macros to operate pins
#define OUT_LED1_OFF		gpio_set_pin_low(LED1)
#define OUT_LED1_ON			gpio_set_pin_high(LED1)
#define OUT_EN3V3_OFF		gpio_set_pin_low(EN3V3)
#define OUT_EN3V3_ON		gpio_set_pin_high(EN3V3)
#define OUT_ENTXS_OFF		gpio_set_pin_low(ENTXS)
#define OUT_ENTXS_ON		gpio_set_pin_high(ENTXS)
#define OUT_DOUT1_OFF		gpio_set_pin_low(DOUT1)
#define OUT_DOUT1_ON		gpio_set_pin_high(DOUT1)
#define OUT_DOUT2_OFF		gpio_set_pin_low(DOUT2)
#define OUT_DOUT2_ON		gpio_set_pin_high(DOUT2)
#define IN_SWITCH_DOWN		gpio_pin_is_low(SWITCH)


/*
#define BOARD_XOSC_HZ          16000000
#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
#define BOARD_XOSC_STARTUP_US  500000
*/



#define HARDWARE_BUFSIZE		250		/**< def USART buffer size */
#define HARDWARE_BUFSIZESML		32



/**
 * \fn void hardware_init(void)
 * \brief Creates I/O ports.
 */
void hardware_init(void);



/**
 * \fn uint16_t hardware_read_adc(uint8_t ch)
 * \brief Read ADC channel.
 */
uint16_t hardware_read_adc(uint8_t ch);


/**
 * \fn void hardware_write_dac(uint8_t ch, uint16_t val)
 * \brief Write to DAC channel.
 */
void hardware_write_dac(uint8_t ch, uint16_t val);


/**
 * \fn void hardware_mdelay(uint16_t ms)
 * \brief Loops for required delay in milliseconds
 */
void hardware_mdelay(uint16_t ms);

#endif // HARDWARE_H
