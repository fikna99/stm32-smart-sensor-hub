/**
 * @file log.h
 * @brief Simple logging interface for the Smart Sensor Hub project.
 *
 * This module provides formatted log output over UART, including
 * optional metadata such as timestamp, file name, line number,
 * and function name. It is intended to make code flow traceable
 * during development and debugging.
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
 */
void Log_Init(UART_HandleTypeDef *huart);

void Log_SetLevel(LogLevel_t level);
LogLevel_t Log_GetLevel(void);
void Log_Enable(bool enable);
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
 */
void Log_Print(LogLevel_t level,
               const char *file,
               uint32_t line,
               const char *func,
               const char *fmt,
               ...);

/* Convenience macros for logging with automatic metadata. */
#define LOG_DEBUG(fmt, ...)  Log_Print(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)   Log_Print(LOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)   Log_Print(LOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)  Log_Print(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
