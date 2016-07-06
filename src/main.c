/**
 * \file main.c
 *
 * \brief Main file for CEDSCOPE Project
 *
 * \mainpage Main Project Page
 * \section title CEDSCOPE
 *
 * This project still under development.
 *
 * Some important project information is contained in the following pages:
 * - [Hardware Pinouts](\ref HardwarePinouts) - pinouts for the CEDSCOPE-0 PCB.
 * - [User Interface Guide](\ref UserInterfaceGuide) - user command and mode description.
 *
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <stdio.h>
#include <string.h>

#include "hardware.h"
#include "user.h"
#include "conf_usart_serial.h"
#include "gainspan.h"

#define VERSION			"\r\nCedScope v1.0.06\r\n\0"

// Choose one of the following wifi connections
//#define USE_NO_WIFI
//#define USE_WIFI_JRRSFT
#define USE_WIFI_AP_CEDRIC



/**
 * \fn static void mdelay(uint16_t ms)
 * \brief Loops for required delay in milliseconds
 * \param ms Number milliseconds delay
 */
static void mdelay(uint16_t ms)
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



int main (void)
{
	
	uint32_t msec;
	
	uint8_t oknext;
	uint16_t val;
	char ch;
	uint8_t retry;
	uint8_t connected;
	uint16_t switch_debounce;
	
	uint16_t i;
	
	char buf[32];
	
	board_init();
	sysclk_init();
	pmic_init();
	cpu_irq_enable();
	
	hardware_init();
	user_init();


	
	// Initialize variables
	msec = 0;
	switch_debounce = 0;
	
	// Enable supply to GainSpan module first then I/O
	OUT_LED1_OFF;
	OUT_EN3V3_ON;
	mdelay(500);
	OUT_ENTXS_ON;
	OUT_LED1_ON;
	

	user_mdelay_tick(500);
	user_TX(VERSION);
	gainspan_RXreset();
	OUT_LED1_OFF;

	// Wait for module to startup
	user_mdelay_tick(2000);	

#ifndef USE_NO_WIFI	
	// Get version info
	oknext = 0;
	oknext += gainspan_TXexecute("ATI0\r\n", gainspan_param_module_i0);
	oknext += gainspan_TXexecute("ATI1\r\n", gainspan_param_module_i1);
	oknext += gainspan_TXexecute("ATI2\r\n", gainspan_param_module_i2);
	
	// Did it work?
	if (oknext == 3)
	{	// Show version data
		user_TX(gainspan_param_module_i0);
		user_TX("\r\n");
		user_TX(gainspan_param_module_i1);
		user_TX("\r\n");
		user_TX(gainspan_param_module_i2);
		user_TX("\r\n");
		user_TX("SUCCESS!!\r\n");
	}
	else
	{
		user_TX("FAILED!!\r\n");
		if (oknext >= 10) user_TX("NO RESPONSE!\r\n");
	}

	// Start connect sequence
	retry = 3;
	connected = false;
	do 
	{
		// Pause
		user_mdelay_tick(1000);
		// Reset buffer
		gainspan_RXreset();

		OUT_LED1_ON;
		oknext = 0;
		// Start commands
#ifdef USE_WIFI_JRRSFT
		user_TX("WIFI_JRRSFT\r\n");
		oknext += gainspan_TXexecute("AT+WWPA=onestationlane\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+WM=0\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+NDHCP=1\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+WA=jrrsft\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+NSTAT=?\r\n", gainspan_param_module);
		if (oknext == 5) oknext += gainspan_TXexecute("AT+NSUDP=8888\r\n", gainspan_param_module);
		if (oknext > 5)	connected = true;
#endif
#ifdef USE_WIFI_AP_CEDRIC
		user_TX("WIFI_AP_CEDRIC\r\n");
		oknext += gainspan_TXexecute("AT+WRXACTIVE=1\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+WSEC=1\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+WM=2\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+DHCPSRVR=1\r\n", gainspan_param_module);
		oknext += gainspan_TXexecute("AT+WA=Cedric\r\n", gainspan_param_module);
		if (oknext == 5) oknext += gainspan_TXexecute("AT+NSUDP=8888\r\n", gainspan_param_module);
		if (oknext > 5)	connected = true;
#endif
		
		OUT_LED1_OFF;
	} 
	while ((!connected) && (--retry > 0));
#else
	user_TX("USE_NO_WIFI\r\n");
#endif
	
	
	while (1)
	{
		mdelay(1);
		user_tick();
		
#ifdef USE_NO_WIFI
		msec++;
		if (msec < 900) 
		{
			OUT_LED1_ON;
			OUT_DOUT1_ON;
			OUT_DOUT2_OFF;
		}	
		else
		{
			OUT_LED1_OFF;
			OUT_DOUT1_OFF;
			OUT_DOUT2_ON;
		}	
		if (msec >= 1000)
		{
			msec = 0;
			i = val;
			val = hardware_read_adc('0');
			sprintf(buf,"ADC:%d", val);
			user_TX(buf);
			val = hardware_read_adc('1');
			sprintf(buf,":%d", val);
			user_TX(buf);
			val = hardware_read_adc('2');
			sprintf(buf,":%d\r\n", val);
			user_TX(buf);
			
			val = i += 100;
			if (val > 4095) val = 0;
			hardware_write_dac('0', val);
			hardware_write_dac('1', (4095 - val));
		}	
		
#else
		if (user_command_ready)
		{
			// Send to Gainspan
			user_buf_rx[user_i_rx++] = 10;
			user_buf_rx[user_i_rx++] = 13;
			user_buf_rx[user_i_rx] = 0;
			gainspan_TXexecute(user_buf_rx, gainspan_param_module);
	
			// Reset input buffer
			user_i_rx = 0;
			// Clear user command indicator
			user_command_ready = false;
		}
		// Flash LED
		if (connected)
		{
			
			msec++;
			if (msec < 20) OUT_LED1_ON;
			else OUT_LED1_OFF;
			if (msec > 1000) msec = 0;
			
			// Any UDP data?
			if (gainspan_RXdata(gainspan_param_module))
			{	// Received UDP data from WIFI link
				user_TX(gainspan_param_module);
				user_TX("\r\n");
				// Process data
				if (gainspan_param_module[0] == '@')
				{	
					if (strncmp(gainspan_param_module, "@adc", 4) == 0)
					{	// Read from ADC
						ch = gainspan_param_module[4];
						val = hardware_read_adc((int)(ch));
						sprintf(buf,"ADC%c:%d",ch,val);
						gainspan_TXdata(buf);
						user_TX(buf);
						user_TX("\r\n");
					}
					else if (strncmp(gainspan_param_module, "@dac", 4) == 0)
					{	// Set DAC
						ch = gainspan_param_module[4];
						val = (int)(gainspan_param_module[6]);
						if (val > 'a' && val < 'z') 
						{
							val -= (int)('a')*10;
							hardware_write_dac(ch, val);
							sprintf(buf,"DAC%c:%d",ch, val);
							user_TX(buf);
							user_TX("\r\n");
						}
					}
					else if (strncmp(gainspan_param_module, "@echo", 5) == 0)
					{	// Echo test message
						gainspan_TXdata("ECHO");
						user_TX("ECHO\r\n");
					}
				}
			}
			
			// Button pressed?
			if (IN_SWITCH_DOWN)
			{	// Send when button released
				switch_debounce = 100;
			}
			else
			{	// Button released
				if (switch_debounce > 0)
				{
					switch_debounce--;
					if (switch_debounce == 0)
					{	// Send UDP packet
						gainspan_TXdata("SWITCH");
						user_TX("Switch\r\n");
					}
				}
			}
		}
		else OUT_LED1_OFF;		
#endif
	}	
}
