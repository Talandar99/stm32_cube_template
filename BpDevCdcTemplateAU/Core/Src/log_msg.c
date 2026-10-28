/*
 * log_msg.c
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "stdbool.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "usbd_cdc_if.h"
#include "CliTask.h"
#include "log_msg.h"

/*##############################################################################
## Macros and Constants Definitions
##############################################################################*/

//----------------------------------------------------------------------------
// ANSI Color Escape Codes
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define YELLOW  "\x1b[33m"

#define LIGHT_GREEN   "\x1b[92m"
#define GREEN   "\x1b[32m"

#define LIGHT_BLUE    "\x1b[94m"
#define BLUE    "\x1b[34m"

//----------------------------------------------------------------------------
#define LOG_MSG_BUF_SIZE 512
#define TEMP_BUFFER_SIZE 384  // Temporary buffer for formatted string
#define FIFO_SIZE 32  // Max number of messages in the FIFO buffer

//----------------------------------------------------------------------------
char log_msg_buf[LOG_MSG_BUF_SIZE];

/*##############################################################################
## TYPES definitions
##############################################################################*/

// FIFO buffer structure
typedef struct {
    char buffer[FIFO_SIZE][LOG_MSG_BUF_SIZE];  // FIFO buffer array
    int head;  // Points to the next message to read
    int tail;  // Points to the next empty slot for writing
    int count; // Number of messages in the buffer
} fifo_t;

fifo_t log_msg_fifo; // Global FIFO buffer

// Mock variable to indicate USB readiness (1 = ready, 0 = not ready)
//volatile bool usb_ready = false;  // Start as not ready
volatile bool usb_ready = true;  // Start as ready

/*##############################################################################
## API FUNCTIONS
##############################################################################*/

//----------------------------------------------------------------------------
void LogMsg_WaitUsbReady(void){
	if(!usb_ready) {
		while(CliTask_Is_Usb_Ready() == false) {
			osDelay(5);   // wait [ms] for USB
		}
		usb_ready = true;
	}
}

//----------------------------------------------------------------------------
// Initialize the FIFO buffer
void LogMsg_FifoInit(fifo_t* fifo) {
    fifo->head = 0;
    fifo->tail = 0;
    fifo->count = 0;
}

//----------------------------------------------------------------------------
// Add a message to the FIFO buffer
int LogMsg_FifoEnqueue(fifo_t* fifo, const char* message) {
    if (fifo->count < FIFO_SIZE) {
        strncpy(fifo->buffer[fifo->tail], message, LOG_MSG_BUF_SIZE - 1);
        fifo->buffer[fifo->tail][LOG_MSG_BUF_SIZE - 1] = '\0';  // Ensure null-termination
        fifo->tail = (fifo->tail + 1) % FIFO_SIZE;
        fifo->count++;
        return 0;  // Success
    }
    return -1;  // FIFO is full, message dropped
}

//----------------------------------------------------------------------------
// Retrieve a message from the FIFO buffer
int LogMsg_FifoDequeue(fifo_t* fifo, char* message) {
    if (fifo->count > 0) {
        strncpy(message, fifo->buffer[fifo->head], LOG_MSG_BUF_SIZE);
        fifo->head = (fifo->head + 1) % FIFO_SIZE;
        fifo->count--;
        return 0;  // Success
    }
    return -1;  // FIFO is empty
}

//----------------------------------------------------------------------------
// Send all messages from the FIFO buffer when USB is ready
void LogMsg_SendPendingMessages(void) {
	if(usb_ready & (log_msg_fifo.count > 0)) {
		char message[LOG_MSG_BUF_SIZE];

		while (LogMsg_FifoDequeue(&log_msg_fifo, message) == 0) {
			osDelay(25);
			CDC_Transmit_FS((uint8_t*)message, strlen(message));
		}
	}
}

//----------------------------------------------------------------------------
#ifdef LOG_MSG_ENABLE_TIMESTAMP
// Get timestamp based on FreeRTOS/CMSIS-RTOS ticks
void get_timestamp(char* buffer, size_t len) {
    uint32_t ticks = osKernelGetTickCount();  // Get the current tick count
    uint32_t freq = osKernelGetTickFreq();    // Get the tick frequency

    uint32_t seconds = ticks / freq;
    uint32_t milliseconds = (ticks % freq) * 1000 / freq;

    snprintf(buffer, len, "[%lu.%03lu]", seconds, milliseconds);  // Format as [seconds.milliseconds]
}
#endif	//LOG_MSG_ENABLE_TIMESTAMP

//----------------------------------------------------------------------------
// Generic log function that handles printing and buffering
void LogMsg_Message(const char* level, const char* color, const char* fmt, va_list args) {
    char temp_buffer[TEMP_BUFFER_SIZE];  // Temporary buffer for formatted log message

#ifdef LOG_MSG_ENABLE_TIMESTAMP
    char timestamp[16];
    get_timestamp(timestamp, sizeof(timestamp));
#else
    const char* timestamp = "";  // No timestamp if disabled
#endif	//LOG_MSG_ENABLE_TIMESTAMP

    int n = vsnprintf(temp_buffer, TEMP_BUFFER_SIZE, fmt, args);  // Format the message into a temporary buffer

    if (n < 0) {
        snprintf(log_msg_buf, LOG_MSG_BUF_SIZE, "\n\r%s%s%s> Error formatting message%s", timestamp, color, level, RESET);
    } else if (n >= TEMP_BUFFER_SIZE) {
        snprintf(log_msg_buf, LOG_MSG_BUF_SIZE - 1, "\n\r%s%s%s> %s... [Message truncated]%s", timestamp, color, level, temp_buffer, RESET);
    } else {
        // Prepend timestamp, color, and level, append reset, and format into the main buffer
        snprintf(log_msg_buf, LOG_MSG_BUF_SIZE, "\n\r%s%s%s %s%s", timestamp, color, level, temp_buffer, RESET);
    }

    // If USB is ready, transmit immediately
    if (usb_ready) {
        CDC_Transmit_FS((uint8_t*)log_msg_buf, strlen(log_msg_buf));
        osDelay(5);
    } else {
        // If USB is not ready, enqueue the message into FIFO
        if (LogMsg_FifoEnqueue(&log_msg_fifo, log_msg_buf) != 0) {
            // Handle FIFO full situation (e.g., message dropped)
            // Optionally, print an error or take other action
        }
    }
}

//----------------------------------------------------------------------------
// Function to log CLI Headers
void LogMsg_Hdr(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMsg_Message(">>", LIGHT_BLUE, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
// Function to log CLI messages
void LogMsg_Cli(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMsg_Message(">", LIGHT_GREEN, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
// Function to log info messages
void LogMsg_Inf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMsg_Message("(i)", GREEN, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
// Function to log warning messages
void LogMsg_Wrn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMsg_Message("(w)", YELLOW, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
// Function to log error messages
void LogMsg_Err(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMsg_Message("(e)", RED, fmt, args);
    va_end(args);
}

//----------------------------------------------------------------------------
// Function to log debug messages
bool log_msg_dbg = false;
void LogMsg_Dbg(const char* fmt, ...) {
#ifdef LOG_MSG_ENABLE_DEBUG
	if(log_msg_dbg) {
		va_list args;
		va_start(args, fmt);
		LogMsg_Message("(d)", LIGHT_BLUE, fmt, args);
		va_end(args);
	}
#endif //LOG_MSG_ENABLE_TIMESTAMP
}

//----------------------------------------------------------------------------
void LogMsg_DbgSelect(bool dbg_msg) {
	log_msg_dbg = dbg_msg;
}

//----------------------------------------------------------------------------
// Function to initialize the log message module
void LogMsg_Init(void) {
    // Log is ready to print
    LogMsg_WaitUsbReady();
    // Transmit pending messages
    LogMsg_SendPendingMessages();
/*
    LogMsg_Dbg("This is a Debug Message");
	LogMsg_Inf("This is an Information");
	LogMsg_Wrn("This is a Warning");
	LogMsg_Err("This is an Error");
	LogMsg_Hdr("This is a CLI Header");
	LogMsg_Cli("This is a CLI Data");
*/
}

