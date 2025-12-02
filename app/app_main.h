/**
 * @file app_main.h
 * @brief Application entry points for Smart Sensor Hub.
 * @ingroup app
 */

#ifndef APP_MAIN_H
#define APP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup app Application Core
 * @brief High-level application control for the Smart Sensor Hub.
 *
 * The application core is responsible for:
 * - Initializing shared subsystems (logging, power, sensors, CLI, scheduler)
 * - Registering periodic tasks with the task manager
 * - Driving the cooperative scheduler from the main loop
 */

/**
 * @brief Initializes the application.
 *
 * This function sets up application-specific modules such as the
 * task manager and individual tasks, and wires up the power manager,
 * sensor interface, logging, and CLI subsystems.
 *
 * @return None.
 */
void App_MainInit(void);

/**
 * @brief Executes one iteration of the application main loop.
 *
 * This function should be called repeatedly from the main() loop.
 * It typically delegates to the task manager to run any tasks
 * that are due.
 *
 * @return None.
 */
void App_MainLoop(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_MAIN_H */
