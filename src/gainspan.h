/**
 * \file gainspan.h
 * \brief Handles the interface to the GainSpan modules
 *
 */

#ifndef GAINSPAN_H
#define GAINSPAN_H


#define GAINSPAN_COMMAND_WAIT_MS	20000	/**< Wait in milliseconds for command response */



char gainspan_buf_tx[HARDWARE_BUFSIZE];	/**< Serial TX circular buffer */
uint8_t gainspan_head_tx;	/**< Head index in TX circular buffer */
uint8_t gainspan_tail_tx;	/**< Tail Index in TX circular buffer */
char gainspan_buf_rx[HARDWARE_BUFSIZE];	/**< Serial RX buffer */
uint8_t gainspan_head_rx;	/**< Head index in RX circular buffer */
uint8_t gainspan_tail_rx;	/**< Tail Index in RX circular buffer */

uint8_t gainspan_rxesc_data;
uint8_t gainspan_rxesc_i;
uint8_t gainspan_rxesc_cid;
uint8_t gainspan_rxcr;


// Parameters
char gainspan_param_debug[HARDWARE_BUFSIZESML];
char gainspan_param_module[HARDWARE_BUFSIZESML];
char gainspan_param_module_i0[HARDWARE_BUFSIZESML];
char gainspan_param_module_i1[HARDWARE_BUFSIZESML];
char gainspan_param_module_i2[HARDWARE_BUFSIZESML];
// UDP parameters
char gainspan_param_module_port[HARDWARE_BUFSIZESML];	/**< UDP port of connection */
char gainspan_param_module_ip[HARDWARE_BUFSIZESML];		/**< IP address for UDP communications */

uint8_t gainspan_module_adapter;		/**< Adapter is first digit from CONNECT string */
uint8_t gainspan_module_connection;	/**< Connection is second digit from CONNECT string */




/**
 * \fn void gainspan_init(void)
 * \brief Initializes serial port for user interface
 */
void gainspan_init(void);



/**
 * \fn void gainspan_TX(char* buf)
 * \brief Copies const buffer to TX buffer.
 */
void gainspan_TX(char* buf);


/**
 * \fn void gainspan_TXparam(char* buf)
 * \brief Copies parameter buffer to TX buffer.
 */
void gainspan_TXparam(char* buf);

/**
 * \fn void gainspan_TXchar(char ch)
 * \brief Adds character to TX buffer.
 */
void gainspan_TXchar(char ch);

/**
 * \fn void gainspan_TXdata(char* buf)
 * \brief Sends UDP formatted data.
 */
void gainspan_TXdata(char* buf);

/**
 * \fn void gainspan_RXreset()
 * \brief Reset RX buffer.
 */
void gainspan_RXreset();

/**
 * \fn uint8_t gainspan_RXresponse()
 * \brief Checks response.
 */
uint8_t gainspan_RXresponse();


/**
 * \fn uint8_t gainspan_RXequals(char * ans)
 * \brief Checks if received command matches chars in buffer.
 */
uint8_t gainspan_RXequals(char * ans);



/**
 * \fn uint8_t gainspan_RXconsume()
 * \brief Consumes buffer up to last <CR>.
 */
uint8_t gainspan_RXconsume();


/**
 * \fn void gainspan_RXparam(char * param)
 * \brief Copies received data into parameter.
 */
void gainspan_RXparam(char * param);


/**
 * \fn uint8_t gainspan_RXdata(char * param)
 * \brief Receives data formatted as UDP.
 */
uint8_t gainspan_RXdata(char * param);


/**
 * \fn uint8_t gainspan_TXexecute(char * cmd, char * param)
 * \brief Sends command and receives any data to param buffer.
 */
uint8_t gainspan_TXexecute(char * cmd, char * param);


#endif // GAINSPAN_H
