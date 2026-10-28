/*
 * Command Handler
 *
 *  Created on: Jan 15, 2026
 *      Author: AU
 */
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "cmsis_os2.h"
#include "log_msg.h"
#include "usb_ring_buffer.h"
#include "CliTask.h"

/*##############################################################################
## DEFINITIONS & MACROS
##############################################################################*/

// Dimensions the buffer into which input characters are placed
#define CLI_MAX_CMD_INPUT_SIZE   (128)

// default curent used, if nothig was set by 'cur' command
#define CLI_TASK_DEFAULT_CURRENT (200)	//[mA]

// cmd-feedback message to be send back by most of commands
#define CLI_CMD_FEEDBACK() sprintf( pcWriteBuffer," ");

/*##############################################################################
## EXTERNAL & GLOBAL VARIABLES
##############################################################################*/

// usbRxRing Ring buffer contains data received over USB-CDC
extern RingBuffer_t usbRxRing;

// CLI Current & Last commands' buffers
static char cli_cur_cmd[CLI_MAX_CMD_INPUT_SIZE];
static char cli_lst_cmd[CLI_MAX_CMD_INPUT_SIZE];

// CLI common strings
const char * const cli_welcome_msg = "CLI Ready: type Help to view registered commands...\n\r";
const char * const cli_cmd_repeat_msg = "Press [ENTER] to repeat the previous command\n\r";
static const char * const cli_new_line_msg = "";

// Mock variable to indicate USB readiness (1 = ready, 0 = not ready)
//volatile bool usb_ready = false;  // Start as not ready

bool CliTask_Is_Usb_Ready(void) {
	// add code to detect USB CDC connection is fully established
	return true;
}

/*==============================================================================
 // Implements Version command
==============================================================================*/
BaseType_t CliCmdVer( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	// This function assumes xWriteBufferLen is large enough!
	( void ) xWriteBufferLen;
	uint16_t sw_ver;

	sw_ver = 1; // CmdUtil_GetSwVersion();
	// *** Execute Version ***
	LogMsg_Inf("Logger SW Version: %d.%d", (sw_ver >> 8), (sw_ver & 0xFF));
	return pdFALSE;
}

/*==============================================================================
 // Implements of the debug messages de-/activate command
==============================================================================*/
BaseType_t CliCmdDbg( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *cmd_par;
	BaseType_t par_str_len;
	uint32_t dbg_state = 0;

	// This function assumes xWriteBufferLen is large enough!
	( void ) xWriteBufferLen;
	cmd_par = FreeRTOS_CLIGetParameter( pcCommandString, 1, &par_str_len);
	dbg_state = (uint32_t)strtol(cmd_par, NULL, 10); // Read activation state
	CLI_CMD_FEEDBACK();

	// *** Execute Command ***
	if(dbg_state == 0) {
		LogMsg_DbgSelect(false);   // Disable
		LogMsg_Wrn("Debug messages disabled!");
	}
	else {   // Enable
		LogMsg_DbgSelect(true);
		LogMsg_Wrn("Debug messages enabled!");
	}
	return pdFALSE;
}

//============================================================================
static const CLI_Command_Definition_t CliCmd_Dbg = {
    "dbg",
    "dbg <0 or 1> e.g. [dbg 0] disables and [dbg 1] enables debug messages\n\r",
	CliCmdDbg,
    1
};

//============================================================================
static const CLI_Command_Definition_t CliCmd_Ver = {
    "ver",
    "ver: Displays SW Version\n\r",
    CliCmdVer,
    0
};

//============================================================================
void CliTaskHandler(void *argument) {
	signed char rx_char;
	uint8_t idx = 0;
	char *rsp_msg;
	BaseType_t cli_result;

	// init code for USB_DEVICE
	MX_USB_DEVICE_Init();

	LogMsg_WaitUsbReady();
	LogMsg_Init();

	// Drop anything in Communication Buffer
    //ComBuf_Flush(&usbd_cdc_com_buf_rx);

	/* Obtain the address of the output buffer.  Note there is no mutual exclusion
	 on this buffer as it is assumed only one command console interface will be used at any one time. */
	rsp_msg = FreeRTOS_CLIGetOutputBuffer();

	/*
	 * Register the command passed in using the pxCommandToRegister parameter.
	 * Registering a command adds the command to the list of commands that are
	 * handled by the command interpreter.  Once a command has been registered it
	 * can be executed from the command line.
	 */
	FreeRTOS_CLIRegisterCommand( &CliCmd_Ver );
	FreeRTOS_CLIRegisterCommand( &CliCmd_Dbg );

	LogMsg_Hdr(cli_welcome_msg);
	bool buf_empty = true;

    for(;;) {
        // Transmit any pending messages
        LogMsg_SendPendingMessages();

      buf_empty = RingBuffer_IsEmpty(&usbRxRing);
      if(!buf_empty) {
   		RingBuffer_Get(&usbRxRing, (void *)&rx_char);

    	  // Echo the character back is optimized and done immediately
    	  // in usbd_cdc_if.c CDC_Receive_FS()
          // do { result = CDC_Transmit_FS((uint8_t*)&rx_char, 1); } while(result != USBD_OK);

    		// Was it the end of the line?
    		if( rx_char == '\n' || rx_char == '\r' ) {
    			// Just to space the output from the input.
    			LogMsg_Cli(cli_new_line_msg);

    			// CMD empty: so the last command is to be executed again.
    			if( idx == 0 ) {
    				// Copy the last command back into the input string
    				strcpy( cli_cur_cmd, cli_lst_cmd );
    			}
    			// Pass the received command to the command interpreter.  The
    			//command interpreter is called repeatedly until it returns
    			//pdFALSE	(indicating there is no more output) as it might
    			//generate more than one string
    			do {
    				// Get the next output string from the command interpreter
    				cli_result = FreeRTOS_CLIProcessCommand( cli_cur_cmd, rsp_msg, CLI_CMD_OUTPUT_BUF_SIZE );
    				// Write the generated string to the USB/UART
    				LogMsg_Cli(rsp_msg);

    			} while( cli_result != pdFALSE );

    			// All the strings generated by the input command have been sent.  Clear the input string ready to receive the next command.
    			//   Remember the command that was just processed first in case it is to be processed again
    			strcpy( cli_lst_cmd, cli_cur_cmd );
    			idx = 0;
    			memset( cli_cur_cmd, 0x00, CLI_MAX_CMD_INPUT_SIZE );
		        LogMsg_Hdr(cli_cmd_repeat_msg);
    		}
    		else {
    			if(rx_char == '\r') {
    				// Ignore the character
    			}
    			else if(rx_char == '\e') { // ESC pressed-: Erase ALL characters the current cmd
    				idx = 0;
    				cli_cur_cmd[idx] = '\0';
    				LogMsg_Cli(cli_new_line_msg);
    			}
    			else if((rx_char == '\b') || (rx_char == (0x7F)) ) {
    				// BackSpace (0x7F) was pressed: Erase the last character in the string if any
    				if( idx > 0 )
    				{
    					idx--;
    					cli_cur_cmd[idx] = '\0';
    				}
    			}
    			else
    			{
    				// A character was entered.  Add it to the string entered so far.
    				// When a \n is entered the complete string will be passed to the command interpreter
    				if( ( rx_char >= ' ' ) && ( rx_char <= '~' ) )
    				{
    					if(idx < CLI_MAX_CMD_INPUT_SIZE)
    					{
    						cli_cur_cmd[idx] = rx_char;
    						idx++;
    					}
    				}
    			}
    		}
    	} //if
      // wait for the message to print

      osDelay(5);
    } //for(;;)
}

