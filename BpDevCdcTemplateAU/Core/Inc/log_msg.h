/*
 * log_msg.h
 */
#ifndef INC_LOG_MSG_H_
#define INC_LOG_MSG_H_

#include "stdbool.h"

/*##############################################################################
## API FUNCTIONS
##############################################################################*/

void LogMsg_Init(void);

void LogMsg_WaitUsbReady(void);

void LogMsg_SendPendingMessages(void);

// Function to log CLI Header messages
void LogMsg_Hdr(const char* fmt, ...);

// Function to log CLI Data messages
void LogMsg_Cli(const char* fmt, ...);

// Function to log Info messages
void LogMsg_Inf(const char* fmt, ...);

// Function to log Warning messages
void LogMsg_Wrn(const char* fmt, ...);

// Function to log Error messages
void LogMsg_Err(const char* fmt, ...);

// Function to log Debug messages
void LogMsg_Dbg(const char* fmt, ...);

// Function to de-/activate Debug messages
void LogMsg_DbgSelect(bool dbg_msg);


#endif /* INC_LOG_MSG_H_ */
