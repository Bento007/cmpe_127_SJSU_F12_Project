/**
 * @file  handles.h
 * @brief This file should contain all FreeRTOS Handles and data
 *          that shall be shared across all FreeRTOS tasks.
 */
#ifndef HANDLES_H_
#define HANDLES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"



/**
 * Structure that holds FreeRTOS Queues, Tasks, and Semaphore Handles
 * Do not instantiate these structs, go through getHandles() to get
 * single instance of this structure.
 */
typedef struct
{
    struct {
        const xSemaphoreHandle spi;
    }Sem;

    struct {

    }Queue;

    struct {
        xTaskHandle terminal;
    }Task;
}HandlesType;



/**
 * Get the instance of System Handles Structure
 */
inline HandlesType* getHandles()
{
    static HandlesType handles = { {xSemaphoreCreateMutex()} };
    return &handles;
}



#ifdef __cplusplus
}
#endif
#endif /* HANDLES_H_ */
