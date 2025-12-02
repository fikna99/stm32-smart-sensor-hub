/**
 * @file cli.c
 * @brief Simple UART-based command line interface implementation.
 *
 * Implements a line-oriented command interpreter on top of a UART
 * peripheral, providing commands for logging control, power mode
 * management, and system status reporting.
 *
 * @ingroup cli
 */

#include "cli.h"
#include "log.h"
#include "power_manager.h"
#include "app_config.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Maximum length of a single CLI input line (excluding terminator).
 */
#define CLI_MAX_LINE_LENGTH   (64U)

/**
 * @brief UART handle used by the CLI.
 */
static UART_HandleTypeDef *s_cliUart = NULL;

/**
 * @brief Line buffer for accumulating user input.
 */
static char s_lineBuffer[CLI_MAX_LINE_LENGTH];

/**
 * @brief Current write index into the line buffer.
 */
static uint32_t s_lineIndex = 0U;

/**
 * @brief Track whether task logging is currently paused via CLI.
 */
static bool s_logPaused = false;

/**
 * @brief Previous logging state saved when pause was requested.
 */
static bool       s_prevLogEnabled = false;
static LogLevel_t s_prevLogLevel   = LOG_LEVEL_INFO;

/* Forward declarations of internal helper functions. */

/**
 * @brief Process a completed input line and execute the corresponding command.
 *
 * @param line Null-terminated input line string. May be modified in-place.
 */
static void CLI_HandleLine(char *line);

/**
 * @brief Send a null-terminated string over the CLI UART.
 *
 * @param str Pointer to a null-terminated string.
 */
static void CLI_SendString(const char *str);

/**
 * @brief Print the CLI prompt.
 *
 * Typically called after processing a command or when the user
 * presses Enter on an empty line.
 */
static void CLI_PrintPrompt(void);

/**
 * @brief Convert a string to lower case in-place.
 *
 * @param str Pointer to a modifiable null-terminated string.
 */
static void CLI_ToLower(char *str);

/**
 * @brief Trim leading and trailing whitespace from a string in-place.
 *
 * @param str Pointer to a modifiable null-terminated string.
 */
static void CLI_Trim(char *str);

/* ------------------------------------------------------------------------- */

void CLI_Init(UART_HandleTypeDef *huart)
{
    s_cliUart   = huart;
    s_lineIndex = 0U;
    memset(s_lineBuffer, 0, sizeof(s_lineBuffer));

    CLI_SendString("\r\nSmart Sensor Hub CLI ready.\r\n");
    CLI_SendString("Type 'help' for a list of commands.\r\n");
    CLI_PrintPrompt();
}

void CLI_Process(void)
{
    if (s_cliUart == NULL)
    {
        return;
    }

    uint8_t ch;

    /* Poll for all available characters (non-blocking). */
    while (HAL_UART_Receive(s_cliUart, &ch, 1U, 0U) == HAL_OK)
    {
        if ((ch == '\r') || (ch == '\n'))
        {
            if (s_lineIndex > 0U)
            {
                s_lineBuffer[s_lineIndex] = '\0';
                CLI_SendString("\r\n");
                CLI_HandleLine(s_lineBuffer);
                s_lineIndex = 0U;
                memset(s_lineBuffer, 0, sizeof(s_lineBuffer));
            }
            CLI_PrintPrompt();
        }
        else if ((ch == '\b') || (ch == 0x7FU))
        {
            if (s_lineIndex > 0U)
            {
                s_lineIndex--;
                s_lineBuffer[s_lineIndex] = '\0';
                const char bsSeq[] = "\b \b";
                CLI_SendString(bsSeq);
            }
        }
        else if ((ch >= 32U) && (ch < 127U))
        {
            if (s_lineIndex < (CLI_MAX_LINE_LENGTH - 1U))
            {
                s_lineBuffer[s_lineIndex++] = (char)ch;
                (void)HAL_UART_Transmit(s_cliUart, &ch, 1U, HAL_MAX_DELAY);
            }
        }
        else
        {
            /* Ignore non-printable control characters. */
        }
    }
}

void CLI_Print(const char *fmt, ...)
{
    if (s_cliUart == NULL)
    {
        return;
    }

    char buffer[128];

    va_list args;
    va_start(args, fmt);
    (void)vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    CLI_SendString(buffer);
}

/* ------------------------------------------------------------------------- */
/*                          Internal Helper Functions                        */
/* ------------------------------------------------------------------------- */

static void CLI_SendString(const char *str)
{
    if ((s_cliUart == NULL) || (str == NULL))
    {
        return;
    }

    size_t len = strlen(str);
    if (len > 0U)
    {
        (void)HAL_UART_Transmit(s_cliUart,
                                (uint8_t *)str,
                                (uint16_t)len,
                                HAL_MAX_DELAY);
    }
}

static void CLI_PrintPrompt(void)
{
    CLI_SendString("\r\n> ");
}

static void CLI_ToLower(char *str)
{
    if (str == NULL)
    {
        return;
    }

    for (char *p = str; *p != '\0'; ++p)
    {
        if ((*p >= 'A') && (*p <= 'Z'))
        {
            *p = (char)(*p - 'A' + 'a');
        }
    }
}

static void CLI_Trim(char *str)
{
    if (str == NULL)
    {
        return;
    }

    char *start = str;
    while ((*start == ' ') || (*start == '\t'))
    {
        start++;
    }

    char *end = start + strlen(start);
    while ((end > start) && (((unsigned char)*(end - 1U) == ' ') ||
                             ((unsigned char)*(end - 1U) == '\t')))
    {
        end--;
    }
    *end = '\0';

    if (start != str)
    {
        memmove(str, start, (size_t)(end - start) + 1U);
    }
}

static void CLI_HandleLine(char *line)
{
    if (line == NULL)
    {
        return;
    }

    CLI_Trim(line);
    CLI_ToLower(line);

    if (strlen(line) == 0U)
    {
        return;
    }

    if (strcmp(line, "help") == 0)
    {
        CLI_Print("\r\nAvailable commands:\r\n");
        CLI_Print("  help            - Show this help text\r\n");
        CLI_Print("  log off         - Disable all task logging\r\n");
        CLI_Print("  log error       - Task logs at ERROR level only\r\n");
        CLI_Print("  log warn        - Task logs at WARN and above\r\n");
        CLI_Print("  log info        - Task logs at INFO and above\r\n");
        CLI_Print("  log debug       - Task logs at DEBUG and above\r\n");
        CLI_Print("  log pause       - Temporarily pause all task logs\r\n");
        CLI_Print("  log resume      - Resume task logs to previous state\r\n");
        CLI_Print("  pmode active    - Request POWER_MODE_ACTIVE\r\n");
        CLI_Print("  pmode idle      - Request POWER_MODE_IDLE\r\n");
        CLI_Print("  pmode sleep     - Request POWER_MODE_SLEEP\r\n");
        CLI_Print("  pmode stop      - Request POWER_MODE_STOP\r\n");
        CLI_Print("  status          - Show logging and power status\r\n");
        return;
    }

    if (strncmp(line, "log ", 4) == 0)
    {
        char *arg = line + 4;
        CLI_Trim(arg);

        if (strcmp(arg, "off") == 0)
        {
            Log_Enable(false);
            s_logPaused = false; /* explicit off overrides pause logic */
            CLI_Print("\r\nTask logging disabled.\r\n");
        }
        else if (strcmp(arg, "error") == 0)
        {
            Log_SetLevel(LOG_LEVEL_ERROR);
            Log_Enable(true);
            s_logPaused = false;
            CLI_Print("\r\nTask logging enabled, level=ERROR.\r\n");
        }
        else if (strcmp(arg, "warn") == 0)
        {
            Log_SetLevel(LOG_LEVEL_WARN);
            Log_Enable(true);
            s_logPaused = false;
            CLI_Print("\r\nTask logging enabled, level=WARN.\r\n");
        }
        else if (strcmp(arg, "info") == 0)
        {
            Log_SetLevel(LOG_LEVEL_INFO);
            Log_Enable(true);
            s_logPaused = false;
            CLI_Print("\r\nTask logging enabled, level=INFO.\r\n");
        }
        else if (strcmp(arg, "debug") == 0)
        {
            Log_SetLevel(LOG_LEVEL_DEBUG);
            Log_Enable(true);
            s_logPaused = false;
            CLI_Print("\r\nTask logging enabled, level=DEBUG.\r\n");
        }
        else if (strcmp(arg, "pause") == 0)
        {
            if (!s_logPaused)
            {
                s_prevLogEnabled = Log_IsEnabled();
                s_prevLogLevel   = Log_GetLevel();
                Log_Enable(false);
                s_logPaused = true;
                CLI_Print("\r\nTask logging paused. Use 'log resume' to restore.\r\n");
            }
            else
            {
                CLI_Print("\r\nTask logging is already paused.\r\n");
            }
        }
        else if (strcmp(arg, "resume") == 0)
        {
            if (s_logPaused)
            {
                Log_SetLevel(s_prevLogLevel);
                Log_Enable(s_prevLogEnabled);
                s_logPaused = false;
                CLI_Print("\r\nTask logging resumed.\r\n");
            }
            else
            {
                CLI_Print("\r\nTask logging is not paused.\r\n");
            }
        }
        else
        {
            CLI_Print("\r\nUnknown log option '%s'. Type 'help'.\r\n", arg);
        }

        return;
    }

    if (strncmp(line, "pmode ", 6) == 0)
    {
        char *arg = line + 6;
        CLI_Trim(arg);

        PowerMode_t requested = POWER_MODE_ACTIVE;
        bool valid = true;

        if (strcmp(arg, "active") == 0)
        {
            requested = POWER_MODE_ACTIVE;
        }
        else if (strcmp(arg, "idle") == 0)
        {
            requested = POWER_MODE_IDLE;
        }
        else if (strcmp(arg, "sleep") == 0)
        {
            requested = POWER_MODE_SLEEP;
        }
        else if (strcmp(arg, "stop") == 0)
        {
            requested = POWER_MODE_STOP;
        }
        else
        {
            valid = false;
        }

        if (valid)
        {
            PowerManager_RequestMode(requested);
            CLI_Print("\r\nRequested power mode change: %s\r\n", arg);
        }
        else
        {
            CLI_Print("\r\nUnknown power mode '%s'. Type 'help'.\r\n", arg);
        }

        return;
    }

    if (strcmp(line, "status") == 0)
    {
        PowerMode_t mode   = PowerManager_GetCurrentMode();
        LogLevel_t  level  = Log_GetLevel();
        bool        enable = Log_IsEnabled();

        uint32_t period_ms = 0U;
        switch (mode)
        {
            case POWER_MODE_ACTIVE:
                period_ms = SENSOR_PERIOD_ACTIVE_MS;
                break;
            case POWER_MODE_IDLE:
                period_ms = SENSOR_PERIOD_IDLE_MS;
                break;
            case POWER_MODE_SLEEP:
                period_ms = SENSOR_PERIOD_SLEEP_MS;
                break;
            case POWER_MODE_STOP:
            default:
                period_ms = SENSOR_PERIOD_STOP_MS;
                break;
        }

        CLI_Print("\r\nStatus:\r\n");
        CLI_Print("  Task logging: %s\r\n", enable ? "ENABLED" : "DISABLED");
        CLI_Print("  LogLevel: %d (0=DEBUG,1=INFO,2=WARN,3=ERROR)\r\n", (int)level);
        CLI_Print("  PowerMode: %d (0=ACTIVE,1=IDLE,2=SLEEP,3=STOP)\r\n", (int)mode);
        CLI_Print("  Sensor sample period: %lu ms\r\n", (unsigned long)period_ms);
        return;
    }

    CLI_Print("\r\nUnknown command '%s'. Type 'help'.\r\n", line);
}

/**
 * @brief Redraw the current CLI prompt and input line after external output.
 *
 * Called from the logging subsystem after each log line so the CLI input
 * line behaves like a fixed dashboard prompt at the bottom of the console.
 */
void CLI_OnExternalOutput(void)
{
    if (s_cliUart == NULL)
    {
        return;
    }

    /* Move cursor to beginning and reprint prompt + partial command. */
    CLI_SendString("\r> ");

    if (s_lineIndex > 0U)
    {
        (void)HAL_UART_Transmit(s_cliUart,
                                (uint8_t *)s_lineBuffer,
                                (uint16_t)s_lineIndex,
                                HAL_MAX_DELAY);
    }
}
