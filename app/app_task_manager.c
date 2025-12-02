/**
 * @file app_task_manager.c
 * @brief Cooperative task manager implementation.
 * @ingroup scheduler
 */

#include "app_task_manager.h"
#include "stm32f4xx_hal.h"
#include "log.h"

/**
 * @brief Maximum number of tasks that can be registered.
 */
#define APP_MAX_TASKS   (8U)

/**
 * @brief Static array of pointers to registered tasks.
 *
 * Each entry points to an @ref AppTaskDescriptor_t instance that
 * is managed by the application. Unused slots are set to NULL.
 */
static AppTaskDescriptor_t *s_tasks[APP_MAX_TASKS] = {0};

/**
 * @brief Number of tasks currently registered.
 */
static uint32_t s_taskCount = 0U;

void AppTaskManager_Init(void)
{
    /* Clear all task entries. */
    for (uint32_t i = 0U; i < APP_MAX_TASKS; ++i)
    {
        s_tasks[i] = NULL;
    }
    s_taskCount = 0U;

    LOG_INFO("Task Manager initialized (max tasks = %lu)", (unsigned long)APP_MAX_TASKS);
}

int AppTaskManager_RegisterTask(AppTaskDescriptor_t *task)
{
    if (task == NULL)
    {
        LOG_ERROR("Attempted to register a NULL task");
        return -1;
    }

    if (s_taskCount >= APP_MAX_TASKS)
    {
        LOG_WARN("Task list is full, cannot register task '%s'", task->name);
        return -2;
    }

    task->lastRun_ms = HAL_GetTick();
    s_tasks[s_taskCount] = task;
    s_taskCount++;

    LOG_INFO("Registered task '%s' with period %lu ms",
             task->name,
             (unsigned long)task->period_ms);

    return 0;
}

void AppTaskManager_RunOnce(void)
{
    uint32_t now_ms = HAL_GetTick();

    for (uint32_t i = 0U; i < s_taskCount; ++i)
    {
        AppTaskDescriptor_t *task = s_tasks[i];
        if (task == NULL)
        {
            continue;
        }

        uint32_t elapsed = now_ms - task->lastRun_ms;
        if (elapsed >= task->period_ms)
        {
            LOG_DEBUG("Running task '%s' (elapsed: %lu ms)",
                      task->name,
                      (unsigned long)elapsed);

            task->lastRun_ms = now_ms;
            task->function();
        }
    }
}
