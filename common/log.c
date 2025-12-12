/**
 * @file log.c
 * @brief Logging implementation for Smart Sensor Hub.
 *
 * Provides formatted UART logging with timestamps and source metadata.
 *
 * @ingroup logging
 */

#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

uint32_t Log_GetTimestampMs(void)
{
    return HAL_GetTick();
}


/**
 * @brief UART handle used for log output.
 *
 * This handle is set in Log_Init() and used internally by
 * Log_Print() to send data over serial.
 */
static UART_HandleTypeDef *s_logUart = NULL;

/**
 * @brief Current minimum log level.
 */
static LogLevel_t s_minLevel = LOG_LEVEL_INFO;

/**
 * @brief Global enable flag for task logging.
 *
 * When false, Log_Print() returns immediately. CLI output is not affected.
 */
static bool s_enabled = false;

/**
 * @brief Maximum length of a single log line (excluding prefix).
 */
#define LOG_MAX_MESSAGE_LENGTH   (256U)

/**
 * @brief Converts a LogLevel_t to a short string tag.
 *
 * @param level Logging level.
 * @return Pointer to a static string.
 */
static const char *Log_LevelToString(LogLevel_t level);

/* ------------------------------------------------------------------------- */

void Log_Init(UART_HandleTypeDef *huart)
{
    s_logUart = huart;
}

/**
 * @brief Weak hook for external output notification.
 *
 * The CLI module overrides this function to redraw its prompt and
 * input line each time a log message is printed, keeping the UART
 * console looking like a dashboard.
 */
__attribute__((weak)) void CLI_OnExternalOutput(void)
{
    /* Default implementation: do nothing. */
}

void Log_Print(LogLevel_t level,
               const char *file,
               uint32_t line,
               const char *func,
               const char *fmt,
               ...)
{
    /* If logging is not initialized, do nothing. */
    if (s_logUart == NULL)
    {
        return;
    }

    /* Apply global filters: enabled flag and min log level. */
    if ((!s_enabled) || (level < s_minLevel))
    {
        return;
    }

    /* Apply global filters: enabled flag and min log level. */
    if ((!s_enabled) || (level < s_minLevel))
    {
        return;
    }

    /* 🔴 THIS IS THE CRITICAL FILTER 🔴 */
     if ((s_logUart == NULL) || (!s_enabled) || (level < s_minLevel))
     {
         return;
     }

    char buffer[LOG_MAX_MESSAGE_LENGTH];
    char logPrefix[96];

    /* Obtain system tick count for basic timestamping. */
    uint32_t timestamp_ms = HAL_GetTick();

    /* Compose a short prefix with timestamp, level, file, line, and function. */
    (void)snprintf(logPrefix,
                   sizeof(logPrefix),
                   "[%08lu ms][%s][%s:%lu][%s] ",
                   (unsigned long)timestamp_ms,
                   Log_LevelToString(level),
                   file,
                   (unsigned long)line,
                   func);

    /* Format the main log message using the variable arguments. */
    va_list args;
    va_start(args, fmt);
    int msgLen = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (msgLen < 0)
    {
        return;
    }

    /* Move to column 0, then print prefix and message. */
    const char cr = '\r';
    (void)HAL_UART_Transmit(s_logUart,
                            (uint8_t *)&cr,
                            1U,
                            HAL_MAX_DELAY);

    /* Transmit prefix, then message, then newline. */
    (void)HAL_UART_Transmit(s_logUart,
                            (uint8_t *)logPrefix,
                            (uint16_t)strlen(logPrefix),
                            HAL_MAX_DELAY);

    (void)HAL_UART_Transmit(s_logUart,
                            (uint8_t *)buffer,
                            (uint16_t)strlen(buffer),
                            HAL_MAX_DELAY);

    const char newline[] = "\r\n";
    (void)HAL_UART_Transmit(s_logUart,
                            (uint8_t *)newline,
                            (uint16_t)strlen(newline),
                            HAL_MAX_DELAY);

    /* Notify any interested module (e.g., CLI) that new output occurred. */
    CLI_OnExternalOutput();
}

/**
 * @brief Map a log level to a compact string tag.
 */
static const char *Log_LevelToString(LogLevel_t level)
{
    switch (level)
    {
        case LOG_LEVEL_DEBUG: return "DBG";
        case LOG_LEVEL_INFO:  return "INF";
        case LOG_LEVEL_WARN:  return "WRN";
        case LOG_LEVEL_ERROR: return "ERR";
        default:              return "UNK";
    }
}

void Log_SetLevel(LogLevel_t level)
{
    s_minLevel = level;
}

LogLevel_t Log_GetLevel(void)
{
    return s_minLevel;
}

void Log_Enable(bool enable)
{
    s_enabled = enable;
}

bool Log_IsEnabled(void)
{
    return s_enabled;
}
