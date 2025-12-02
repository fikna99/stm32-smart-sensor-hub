/**
 * @file cli.h
 * @brief Simple UART-based command line interface (CLI).
 *
 * The CLI provides runtime control for logging and power management,
 * and a human-friendly way to interact with the firmware.
 *
 * @ingroup cli
 */

#ifndef CLI_H
#define CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/**
 * @defgroup cli Command Line Interface
 * @brief UART-based interactive interface for the Smart Sensor Hub.
 * @ingroup common
 * @{
 */

/**
 * @brief Initialize the CLI module.
 *
 * Must be called once after the UART handle is configured. Prints a
 * welcome banner and prompt.
 *
 * @param huart Pointer to the UART handle used for CLI I/O.
 *
 * @return None.
 */
void CLI_Init(UART_HandleTypeDef *huart);

/**
 * @brief Periodic CLI processing function.
 *
 * Should be called frequently (e.g. every 20 ms) from a scheduled task.
 * Polls the UART for incoming characters, builds command lines, and
 * executes commands when a full line is received.
 *
 * @return None.
 */
void CLI_Process(void);

/**
 * @brief Print a formatted message to the CLI UART.
 *
 * Used for command responses and help text. This is independent of the
 * logging subsystem and is always visible, even if task logs are disabled.
 *
 * @param fmt printf-style format string.
 * @param ... Arguments for the format string.
 *
 * @return None.
 */
void CLI_Print(const char *fmt, ...);

/**
 * @brief Called after asynchronous output (e.g. log line) to redraw prompt.
 *
 * Implemented in cli.c and invoked from log.c (weak hook) to maintain a
 * dashboard-like CLI where the input line stays clean at the bottom.
 *
 * @return None.
 */
void CLI_OnExternalOutput(void);

/** @} */ /* end of cli group */

#ifdef __cplusplus
}
#endif

#endif /* CLI_H */
