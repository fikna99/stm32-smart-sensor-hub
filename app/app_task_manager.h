/**
 * @file app_task_manager.h
 * @brief Cooperative task manager for Smart Sensor Hub.
 *
 * This module implements a simple tick-based scheduler that
 * periodically calls registered tasks based on their configured
 * execution period. It is intended as a stepping stone toward
 * a full RTOS-based design in later phases.
 *
 * @ingroup scheduler
 */

#ifndef APP_TASK_MANAGER_H
#define APP_TASK_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @defgroup scheduler Cooperative Task Scheduler
 * @brief Simple cooperative scheduler for periodic application tasks.
 * @ingroup app
 * @{
 */

/**
 * @brief Function pointer type for a scheduled task.
 */
typedef void (*AppTaskFunction_t)(void);

/**
 * @brief Descriptor for a single scheduled task.
 *
 * Each task descriptor defines the function to call, how often it
 * should be executed, and the last time it was run. Instances of
 * this structure are registered with the task manager and remain
 * valid for the lifetime of the application (typically in static
 * or global storage).
 */
typedef struct
{
    const char        *name;       /**< Human-readable task name.          */
    AppTaskFunction_t  function;   /**< Pointer to the task function.      */
    uint32_t           period_ms;  /**< Period of execution in milliseconds. */
    uint32_t           lastRun_ms; /**< Last time the task was executed.   */
} AppTaskDescriptor_t;

/**
 * @brief Initializes the task manager.
 *
 * This function must be called once at startup, before the scheduler
 * is used. It resets all internal tracking variables and clears
 * the task list.
 *
 * @return None.
 */
void AppTaskManager_Init(void);

/**
 * @brief Registers a task with the scheduler.
 *
 * Tasks must be registered before they will be executed. This
 * function should typically be called once for each task during
 * application initialization.
 *
 * @param task Pointer to a task descriptor. The descriptor must remain
 *             valid (in static or global storage) for the lifetime
 *             of the application.
 *
 * @return 0 on success, non-zero if the task list is full or the task
 *         pointer is invalid.
 */
int AppTaskManager_RegisterTask(AppTaskDescriptor_t *task);

/**
 * @brief Executes any tasks that are due to run.
 *
 * This function should be called frequently from the main loop. It
 * checks each registered task and executes it if its period has
 * elapsed since the last run.
 *
 * @return None.
 */
void AppTaskManager_RunOnce(void);

/** @} */ /* end of scheduler group */

#ifdef __cplusplus
}
#endif

#endif /* APP_TASK_MANAGER_H */
