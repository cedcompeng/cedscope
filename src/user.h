/**
 * \file user.h
 * \brief Handles the interface to the user
 *
 */

#ifndef USER_H
#define USER_H
	

enum user_modes 
{
	USER_MODE_NORMAL,
	USER_MODE_GAINSPAN
};	/**< User modes enumerations */


enum user_commands
{
	USER_COMMAND_NONE,
	USER_COMMAND_GAINSPAN,
	USER_COMMAND_EXIT,
	USER_COMMAND_INVALID
};	/**< User command enumerations */

enum user_modes user_mode;	/**< Current user mode */
enum user_commands user_command; /**< Current user command */

uint8_t user_param;	/**< Current user parameter entered with last command */
uint16_t user_value;	/**< Current user parameter value entered with last command */

bool user_command_ready;	/**< User detected <CR> - ready to be processed when TRUE */

char user_buf_tx[HARDWARE_BUFSIZE];	/**< Serial TX circular buffer */
uint8_t user_head_tx;	/**< Head index in TX circular buffer */
uint8_t user_tail_tx;	/**< Tail Index in TX circular buffer */
char user_buf_rx[HARDWARE_BUFSIZE];	/**< Serial RX buffer */
uint8_t user_i_rx;		/**< Index in RX buffer */



/**
 * \fn void user_init(void)
 * \brief Initializes serial port for user interface
 */
void user_init(void);


/**
 * \fn void user_tick(void)
 * \brief Checks for serial RX and TX from buffer
 */
void user_tick(void);


/**
 * \fn void user_mdelay_tick(uint16_t ms)
 * \brief Checks for serial RX and TX from buffer and delays.
 */
void user_mdelay_tick(uint16_t ms);


/**
 * \fn void user_TX(char* buf)
 * \brief Writes character buffer to serial port
 * \param buf Buffer containing characters terminated by 0x00
 */
void user_TX(char* buf);



/**
 * \fn void user_process(void)
 * \brief Processes user command
 */
void user_process(void);


#endif // USER_H
