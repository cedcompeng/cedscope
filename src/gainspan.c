/**
 * \file gainspan.c
 * \brief Communicates with GainSpan module through USART interface
 *
 * Handles Interface to GainSpan module.
 *
 * Additional information can be found in the [GainSpan Interface Guide](\ref GainSpanInterfaceGuide) page.
 *
 */

/**
 * \page GainSpanInterfaceGuide GainSpan Interface Guide
 * 
 * 
 *
 * Defined in \ref gainspan.c
 */


#include <gpio.h>
#include <asf.h>
#include <string.h>


#include "hardware.h"
#include "gainspan.h"
#include "user.h"




/**
 * \fn void gainspan_init(void)
 * \brief Initializes user interface.
 */
void gainspan_init(void)
{
	// Buffer initialization handled in user_init()
}



/**
 * \fn void gainspan_TX(char* buf)
 * \brief Copies buffer to TX buffer.
 * \param buf Buffer of data to be copied
 *
 * Buffer end is marked by 0x00 character.
 */
void gainspan_TX(char* buf)
{
	char ch;
	int i;
	i = 0;
	ch = buf[i++];
	while(ch != 0)
	{
		gainspan_buf_tx[gainspan_head_tx++] = ch;
		ch = buf[i++];
		if (gainspan_head_tx >= HARDWARE_BUFSIZE) gainspan_head_tx = 0;
		if (gainspan_head_tx == gainspan_tail_tx) 
		{
			if (++gainspan_tail_tx >= HARDWARE_BUFSIZE) gainspan_tail_tx = 0;
		}
	}
}


/**
 * \fn void gainspan_TXparam(char* buf)
 * \brief Copies parameter buffer to TX buffer.
 * \param buf Parameter buffer of data to be copied
 *
 * Buffer end is marked by 0x00 character.
 */
void gainspan_TXparam(char* buf)
{
	char ch;
	int i;
	i = 0;
	ch = buf[i++];
	while((ch != 0) && (i <= HARDWARE_BUFSIZE))
	{
		gainspan_buf_tx[gainspan_head_tx++] = ch;
		ch = buf[i++];
		if (gainspan_head_tx >= HARDWARE_BUFSIZE) gainspan_head_tx = 0;
		if (gainspan_head_tx == gainspan_tail_tx) 
		{
			if (++gainspan_tail_tx >= HARDWARE_BUFSIZE) gainspan_tail_tx = 0;
		}
	}
}

/**
 * \fn void gainspan_TXchar(char ch)
 * \brief Adds character to TX buffer.
 * \param ch Character to be added to buffer
 */
void gainspan_TXchar(char ch)
// 
{
	gainspan_buf_tx[gainspan_head_tx++] = ch;
	if (gainspan_head_tx >= HARDWARE_BUFSIZE) gainspan_head_tx = 0;
	if (gainspan_head_tx == gainspan_tail_tx) 
	{	// Overflow
		if (++gainspan_tail_tx >= HARDWARE_BUFSIZE) gainspan_tail_tx = 0;
	}
}


/**
 * \fn void gainspan_TXdata(char* buf)
 * \brief Sends UDP formatted data.
 * \param buf Data buffer to be transmitted
 *
 * Resets TX buffer with new data. Buffer end is marked by 0x00 character.
 */
void gainspan_TXdata(char* buf)
// 
// 
{
	// Send start of data
	gainspan_TXchar(27);
	gainspan_TXchar('U');
	gainspan_TXchar(gainspan_rxesc_cid);
	gainspan_TXparam(gainspan_param_module_ip);
	gainspan_TXchar(':');
	gainspan_TXparam(gainspan_param_module_port);
	gainspan_TXchar(':');
	gainspan_TXparam(buf);
	gainspan_TXchar(27);
	gainspan_TXchar('E');
}




/**
 * \fn void gainspan_RXreset()
 * \brief Reset RX buffer.
 */
void gainspan_RXreset()
{
	gainspan_tail_rx = 0;
	gainspan_head_rx = 0;
	gainspan_rxcr   = 0;
	gainspan_module_adapter = 0;
	gainspan_module_connection = 0;
}



/**
 * \fn uint8_t gainspan_RXresponse()
 * \brief Checks response.
 * \returns Number of <CR> or <ESC> received
 */
uint8_t gainspan_RXresponse() 
{
	if (gainspan_tail_rx != gainspan_head_rx) return gainspan_rxcr;
	return 0;
}




/**
 * \fn uint8_t gainspan_RXequals(char * ans)
 * \brief Checks if received command matches chars in buffer.
 * \param ans String to be found in RX buffer
 * \returns true if match found
 *
 * If match found then command consumed from buffer. Any <ESC> and next two characters are skipped.
 */
uint8_t gainspan_RXequals(char * ans)
// 
// 
{
	char ch1,ch2;
	int rxtail_temp;
	int i;
	rxtail_temp = gainspan_tail_rx;
	i = 0;
	// Do character by character compare til first mismatch or end of string
	do
	{
		ch2 = ans[i++];
		// If end of string then do not read next character from buffer
		if (ch2 != 0)
		{
			ch1 = gainspan_buf_rx[rxtail_temp++];
			if (rxtail_temp >= HARDWARE_BUFSIZE) rxtail_temp = 0;
			if (ch1 == 10) 
			{  // Strip extra <LF>
				ch1 = gainspan_buf_rx[rxtail_temp++];
				if (rxtail_temp >= HARDWARE_BUFSIZE) rxtail_temp = 0;
			}
			// If character is <ESC> then skip this and next two bytes before starting
			// compare
			if (ch1 == 27)
			{	// Skip next three
				rxtail_temp += 2;
				if (rxtail_temp >= HARDWARE_BUFSIZE) rxtail_temp = rxtail_temp-HARDWARE_BUFSIZE;
				ch1 = gainspan_buf_rx[rxtail_temp++];
			}
		}
		// Is character from buffer same as command?
	}
	while ((ch1 == ch2) && (ch2 != 0));
	// End of string?
	if (ch2 == 0) 
	{  // Consume string from buffer
		gainspan_tail_rx = rxtail_temp;
		// Is there a trailing <CR>?
		if (gainspan_buf_rx[gainspan_tail_rx] == 13)
		{  // Consume <CR>
			if (++gainspan_tail_rx > HARDWARE_BUFSIZE) gainspan_tail_rx = 0;
			if (gainspan_rxcr > 0) gainspan_rxcr--;
		}
		return true;
	}
	// Mis-match
	return false;
}




/**
 * \fn uint8_t gainspan_RXconsume()
 * \brief Consumes buffer up to last <CR>.
 * \returns Number of <CR> remaining
 */
uint8_t gainspan_RXconsume()
{
	int ch;
	ch = gainspan_buf_rx[gainspan_tail_rx];
	while((ch != 13) && (gainspan_tail_rx != gainspan_head_rx))
	{
		if (++gainspan_tail_rx >= HARDWARE_BUFSIZE) gainspan_tail_rx = 0;
		ch = gainspan_buf_rx[gainspan_tail_rx];
	}
	// Have we consumed <CR>?
	if (ch == 13)
	{
		if (++gainspan_tail_rx >= HARDWARE_BUFSIZE) gainspan_tail_rx = 0;
		if (gainspan_rxcr > 0) gainspan_rxcr--;
	}
	// Otherwise must have exhausted buffer (should not happen)
	else gainspan_rxcr = 0;   // Should already be zero
	return gainspan_rxcr;
}




/**
 * \fn void gainspan_RXparam(char * param)
 * \brief Copies received data into parameter.
 * \param param Destination parameter for received data
 *
 *  Data must be terminated by <CR> or <LF> or 0x00.
 */
void gainspan_RXparam(char * param)
{
	char ch;
	int i;
	i = 0;
	// Do character by character compare til first mismatch or end of string
	do
	{
		ch = gainspan_buf_rx[gainspan_tail_rx++];
		if (gainspan_tail_rx >= HARDWARE_BUFSIZE) gainspan_tail_rx = 0;
		// Valid character?
		if (ch >= ' ')
		{
			param[i] = ch;
			if (++i >= HARDWARE_BUFSIZESML) i--;
		}
		// Track any swallowed <CR>
		if ((ch == 13) && (gainspan_rxcr > 0)) gainspan_rxcr--;

	}
	while ((ch != 13) && (ch != 0));
	// Terminate string with 0x00 not <CR> or <LF>
	param[i] = 0;
	return;
}




/**
 * \fn uint8_t gainspan_RXdata(char * param)
 * \brief Receives data formatted as UDP.
 * \param param Destination parameter for received data
 * \returns true if data put in buffer
 *
 * Copies received data into parameter encapsulated by <ESC><u><CID><IP><SPACE><PORT><TAB><DATA><ESC><E>
 */
uint8_t gainspan_RXdata(char * param)
{
	char ch;
	if (gainspan_tail_rx == gainspan_head_rx) return false;
	// Do character by character compare til first mismatch or end of string
	do
	{
		ch = gainspan_buf_rx[gainspan_tail_rx++];
		if (gainspan_tail_rx >= HARDWARE_BUFSIZE) gainspan_tail_rx = 0;
		// Got special <ESC> code?
		switch(gainspan_rxesc_data)
		{
			case 0:
				// Waiting for data
				if (ch == 27) gainspan_rxesc_data++;
				break;
			case 1:
				// Have we got <u> for start of data or something else?
				if (ch == 'u') gainspan_rxesc_data++;
				else gainspan_rxesc_data = 99;
				break;
			case 2:
				// Get CID
				gainspan_rxesc_cid = ch;
				gainspan_rxesc_data++;
				// Set i=0 ready for receiving IP Address
				gainspan_rxesc_i = 0;
				break;
			case 3:
				// Catching IP address
				// If <ESC> then abort
				if (ch == 27) gainspan_rxesc_data = 0;
				// If <SPACE> then marks end of IP address and start of Port
				else if (ch == ' ') 
				{	// Done with IP address, ready for port
					gainspan_param_module_ip[gainspan_rxesc_i] = 0;
					gainspan_rxesc_i = 0;
					gainspan_rxesc_data++;
				}
				else if (ch >= ' ')
				{	// Receiving IP address
					gainspan_param_module_ip[gainspan_rxesc_i++] = ch;
					if (gainspan_rxesc_i >= HARDWARE_BUFSIZESML) gainspan_rxesc_i--;
				}
				break;
			case 4:
				// Catching Port
				// If <ESC> then abort
				if (ch == 27) gainspan_rxesc_data = 0;
				// If <TAB> then marks end of Port and start of data
				else if (ch == 9) 
				{	// Done with port, ready for data
					gainspan_param_module_port[gainspan_rxesc_i] = 0;
					gainspan_rxesc_i = 0;
					gainspan_rxesc_data++;
				}
				else if (ch >= ' ')
				{	// Receiving port
					gainspan_param_module_port[gainspan_rxesc_i++] = ch;
					if (gainspan_rxesc_i >= HARDWARE_BUFSIZESML) gainspan_rxesc_i--;
				}
				break;
			case 5:
				// Catching data
				// If <ESC> then finished 
				if (ch == 27) gainspan_rxesc_data++;
				else if (ch >= ' ')
				{	// Receiving data
					param[gainspan_rxesc_i++] = ch;
					if (gainspan_rxesc_i >= HARDWARE_BUFSIZESML) gainspan_rxesc_i--;
				}
				break;
			case 6:
/*
				// This means ok - Got <ESC> at end of data
				// If not received <E> then not proper end
				if (ch != 'E') gainspan_rxesc_data = 0;
				else gainspan_rxesc_data++;
				// Terminate string with 0x00 not <CR> or <LF>
				param[gainspan_rxesc_i] = 0;
				break;
			case 7:
*/
				// Finished
				param[gainspan_rxesc_i] = 0;
				gainspan_rxesc_data = 0;
				return true;
			default:
				// End
				gainspan_rxesc_data = 0;
				break;
		}
	}
	while (gainspan_tail_rx != gainspan_head_rx);
	// Must have found no valid data
	return false;
}




/**
 * \fn uint8_t gainspan_TXexecute(char * cmd, char * param)
 * \brief Sends command and receives any data to param buffer.
 * \param cmd Command buffer
 * \param param Parameter buffer for response
 * \returns 1 if successful otherwise 0
 */
uint8_t gainspan_TXexecute(char * cmd, char * param)
// Will overwrite param even if unsuccessful.
{
	uint16_t wait;
	gainspan_TX(cmd);
	wait = GAINSPAN_COMMAND_WAIT_MS;
	while(wait > 0)
	{
		user_mdelay_tick(1);
		wait--;
		if (gainspan_RXresponse()) 
		{	// Exit if OK or ERROR received.
			if (gainspan_RXequals("OK")) return 1;
			else if (gainspan_RXequals("ERROR")) return 0;
			else gainspan_RXparam(param);
		}
	}
	// Failed
	return 10;
}





