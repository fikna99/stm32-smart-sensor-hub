/**
 * @file log.h
 * @brief Simple logging interface for the Smart Sensor Hub project.
 *
 * This module provides formatted log output over UART, including
 * optional metadata such as timestamp, file name, line number,
 * and function name. It is intended to make code flow traceable
 * during development and debugging.
 *
 * @ingroup logging
 */

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

/**
 * @defgroup logging Logging Subsystem
 * @brief Formatted UART logging for the Smart Sensor Hub.
 * @ingroup common
 * @{
 */

/**
 * @enum LogLevel_t
 * @brief Logging severity levels.
 */
typedef enum
{
    LOG_LEVEL_DEBUG = 0U,  /**< Detailed debug information. */
    LOG_LEVEL_INFO,        /**< High-level informational messages. */
    LOG_LEVEL_WARN,        /**< Warnings about potential issues. */
    LOG_LEVEL_ERROR        /**< Errors that require attention. */
} LogLevel_t;

/**
 * @brief Initializes the logging module.
 *
 * This function must be called once at startup, after the UART
 * peripheral used for logging has been initialized.
 *
 * @param huart Pointer to the UART handle for log output.
 *
 * @return None.
 */
void Log_Init(UART_HandleTypeDef *huart);

void Log_SetLevel(LogLevel_t level);
LogLevel_t Log_GetLevel(void);
void Log_Enable(bool enable);
bool Log_IsEnabled(void);


/**
 * @brief Set the minimum log level.
 *
 * Messages below this level will be filtered out.
 *
 * @param level New minimum log level.
 *
 * @return None.
 */
void Log_SetLevel(LogLevel_t level);

/**
 * @brief Get the current minimum log level.
 *
 * @return The active minimum log level.
 */
LogLevel_t Log_GetLevel(void);

/**
 * @brief Enable or disable logging globally.
 *
 * When disabled, Log_Print() returns immediately without writing
 * anything to the UART. CLI output is not affected by this flag.
 *
 * @param enable true to enable logging, false to disable.
 *
 * @return None.
 */
void Log_Enable(bool enable);

/**
 * @brief Query whether logging is currently enabled.
 *
 * @return true if logging is enabled, false otherwise.
 */
bool Log_IsEnabled(void);

/**
 * @brief Set the minimum log level.
 *
 * Messages below this level will be filtered out.
 *
 * @param level New minimum log level.
 *
 * @return None.
 */
void Log_SetLevel(LogLevel_t level);

/**
 * @brief Get the current minimum log level.
 *
 * @return The active minimum log level.
 */
LogLevel_t Log_GetLevel(void);

/**
 * @brief Enable or disable logging globally.
 *
 * When disabled, Log_Print() returns immediately without writing
 * anything to the UART. CLI output is not affected by this flag.
 *
 * @param enable true to enable logging, false to disable.
 *
 * @return None.
 */
void Log_Enable(bool enable);

/**
 * @brief Query whether logging is currently enabled.
 *
 * @return true if logging is enabled, false otherwise.
 */
bool Log_IsEnabled(void);

/**
 * @brief Prints a formatted log message.
 *
 * This is the generic logging function used internally by the
 * LOG_* macros. It attaches timestamp and source location
 * information to each message.
 *
 * @param level   Logging level (e.g. LOG_LEVEL_INFO).
 * @param file    Source file name (__FILE__).
 * @param line    Line number in source file (__LINE__).
 * @param func    Function name (__func__).
 * @param fmt     printf-style format string.
 * @param ...     Arguments for the format string.
 *
 * @return None.
 */
void Log_Print(LogLevel_t level,
               const char *file,
               uint32_t line,
               const char *func,
               const char *fmt,
               ...);

/**
 * @brief Hook called after each log line is printed.
 *
 * Implemented as a weak symbol in log.c and optionally overridden
 * by the CLI module to redraw the prompt and input line so that
 * the CLI behaves like a clean dashboard.
 */
void CLI_OnExternalOutput(void);

/* Convenience macros for logging with automatic metadata. */
#define LOG_DEBUG(fmt, ...)  Log_Print(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)   Log_Print(LOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)   Log_Print(LOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)  Log_Print(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

/**
 * @brief Get the current log timestamp in milliseconds.
 *
 * This helper is used by modules that want a timestamp compatible
 * with the logging system.
 *
 * @return Current timestamp in milliseconds.
 */
uint32_t Log_GetTimestampMs(void);

/** @} */ /* end of logging group */

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
