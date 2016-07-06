/**
 * \file user.c
 * \brief Communicates with user through USART interface
 *
 * Handles User Interface to hardware.
 *
 * Additional information can be found in the [User Interface Guide](\ref UserInterfaceGuide) page.
 *
 */

/**
 * \page UserInterfaceGuide User Interface Guide
 * 
 * - `gainspan` enters GainSpan mode where input is echoed directly to GainSpan module
 * - `normal` returns to normal mode when not in GainSpan mode
 *
 * Defined in \ref user.c
 */


#include <gpio.h>
#include <asf.h>
#include <string.h>


#include "conf_usart_serial.h"
#include "hardware.h"
#include "user.h"
#include "gainspan.h"



/**
 * \fn void user_init(void);
 * \brief Initializes user interface.
 */
void user_init(void)
{
	
	// Set mode and command.
	user_mode = USER_MODE_NORMAL;
	user_command = USER_COMMAND_NONE;
	
	// No commands ready to be processed.
	user_command_ready = false;
	
	// Setup empty buffers.
	user_i_rx = 0;
	user_head_tx = 0;
	user_tail_tx = 0;
	gainspan_head_tx = 0;
	gainspan_tail_tx = 0;
	gainspan_head_rx = 0;
	gainspan_tail_rx = 0;
	
	user_command_ready = false;
}




/**
 * \fn void user_tick(void)
 * \brief Checks for serial RX and TX from buffer.
 */
void user_tick(void)
{
	// Byte received from USART.
	uint8_t ch;
	uint8_t i;
	
	// Any serial input?
	for(i = 0; i < 10; i++)
	{	
		if (usart_rx_is_complete(USART_USER))
		{
			usart_serial_getchar(USART_USER, &ch);
			// Has <CR> been received (if not GainSpan command)?
			if (ch == 13) user_command_ready = true;
			// Store command in buffer.
			if (user_i_rx < HARDWARE_BUFSIZE-1) user_buf_rx[user_i_rx++] = ch;
		}	
		// Check GainSpan for input
		if (usart_rx_is_complete(USART_GAINSPAN))
		{
			usart_serial_getchar(USART_GAINSPAN, &ch);
			// Append to head of circular buffer.
			user_buf_tx[user_head_tx++] = ch;
			gainspan_buf_rx[gainspan_head_rx++] = ch;
			// Wrap around but do not check for overflow.
			if (user_head_tx >= HARDWARE_BUFSIZE) user_head_tx = 0;
			if (gainspan_head_rx >= HARDWARE_BUFSIZE) gainspan_head_rx = 0;
			// Any <CR>?
			if (ch == 13) gainspan_rxcr++;
		}
	}	
	// Any serial output?
	if (user_head_tx != user_tail_tx)
	{	// Some data in buffer.
		// Get character from tail (first in, first out)
		ch = user_buf_tx[user_tail_tx++];
		// Send to USART
		usart_serial_putchar(USART_USER, ch);
		// Wrap around buffer?
		if (user_tail_tx >= HARDWARE_BUFSIZE) user_tail_tx = 0;
	}

	if (gainspan_head_tx != gainspan_tail_tx)
	{	// Some data in buffer.
		// Get character from tail (first in, first out)
		ch = gainspan_buf_tx[gainspan_tail_tx++];
		// Send to USART
		usart_serial_putchar(USART_GAINSPAN, ch);
		// Wrap around buffer?
		if (gainspan_tail_tx >= HARDWARE_BUFSIZE) gainspan_tail_tx = 0;
	}
}



/**
 * \fn void user_mdelay_tick(uint16_t ms)
 * \brief Checks for serial RX and TX from buffer and delays.
 * \param ms Number milliseconds delay
 *
 * Loops for required delay in milliseconds
 */
void user_mdelay_tick(uint16_t ms)
{
	uint16_t t;
	for(t = 0; t < ms; t++)
	{
		user_tick();
		hardware_mdelay(1);
	}
}	



/**
 * \fn void user_TX(char* buf)
 * \brief Writes character buffer to serial port.
 * \param buf Buffer containing characters terminated by 0.
 */
void user_TX(char* buf)
{
	int i;
	// Add to TX circular buffer head.
	for(i = 0; i < HARDWARE_BUFSIZE; i++)
	{
		// Not end of string?
		if (buf[i] != 0)
		{	// Append to head of circular buffer.
			user_buf_tx[user_head_tx++] = buf[i];
			// Wrap around but do not check for overflow.
			if (user_head_tx >= HARDWARE_BUFSIZE) user_head_tx = 0;
		}
		else break;
	}
}



/**
 * \fn void user_get_param_value()
 * \brief Gets the parameter and value pair from the serial RX buffer.
 *
 * Separates parameter value pair where '=' sign found.
 */
void user_get_param_value()
{
	int i;
	char ch;
	user_param = 0;
	user_value = 0;
	i = 1;
	// Build param number
	do
	{	
		ch = user_buf_rx[i++];
		if ((ch >= '0') && (ch <= '9')) user_param = (user_param*10) + (ch - '0');
	}
	while((ch != '=') && (ch != 13) && (i < HARDWARE_BUFSIZE) && (i < user_i_rx));
	// Build value number if '=' exists.
	if ((ch == '=') && (i < user_i_rx))
	{
		do
		{
			ch = user_buf_rx[i++];
			if ((ch >= '0') && (ch <= '9')) user_value = (user_value*10) + (ch - '0');
		}
		while((ch != 13) && (i < HARDWARE_BUFSIZE) && (i < user_i_rx));
	} 
	// Both param and value now contain input data.	
}



/**
 * \fn void user_process(void)
 * \brief Processes user command
 *
 * Called when a command termination character has been received and main loop has free time.
 * Parses the input buffer to determine what action to take.
 * 
 *
 */
void user_process(void)
{
	int i;
	char ch;
	// Clear last user command.
	user_command = USER_COMMAND_NONE;
	
	// Look for command
	if (strncmp(user_buf_rx, "gainspan", 1) == 0) user_command = USER_COMMAND_GAINSPAN;
	else if (strncmp(user_buf_rx, "normal", 1) == 0) user_command = USER_COMMAND_EXIT;
	else user_command = USER_COMMAND_INVALID;
	
	// Reset input buffer
	user_i_rx = 0;
	// Clear user command indicator
	user_command_ready = false;
	
}