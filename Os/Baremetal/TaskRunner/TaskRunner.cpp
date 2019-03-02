/*
 * TaskRunner.cpp
 *
 *  Created on: Feb 28, 2019
 *      Author: lestarch
 */
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/Baremetal/TaskRunner/TaskRunner.hpp>
#include <Os/Baremetal/TaskRunner/BareTaskHandle.hpp>
#include <stdio.h>
namespace Os {

TaskRunner::TaskRunner(Task::taskRoutine callback) :
    m_index(0),
    m_callback(callback)
{
    for (U32 i = 0; i < TASK_REGISTRY_CAP; i++) {
        this->m_task_table[i] = 0;
    }
    Task::registerTaskRegistry(this);
}
TaskRunner::~TaskRunner() {}
/**
 * Add a task to the registry. These tasks will be run on a bare-metal
 * loop. The function used in this task may be overridden.
 * \param Task* task: task to be added
 */
void TaskRunner::addTask(Task* task) {
    FW_ASSERT(m_index < TASK_REGISTRY_CAP);
    printf("Registered task: 0x%p\n", task);
    this->m_task_table[m_index] = task;
    m_index++;
}
/**
 * Remove a task to the registry. These tasks will no-longer be run.
 * \param Task* task: task to be removed
 */
void TaskRunner::removeTask(Task* task) {
    //You are the paragon of Lazy developers
    FW_ASSERT(0);
}
/**
 * Program loop, used to run all tasks forever
 */
void TaskRunner::run_forever() {
    //We loop forever, woe to those who come after
    while (1) {
        m_callback(NULL);
        //Loop through full table
        for (U32 i = 0; i < TASK_REGISTRY_CAP; i++) {
            //Break at end of table
            if (m_task_table[i] == NULL) {
                break;
            }
            //Get bare task or break
            BareTaskHandle* handle = reinterpret_cast<BareTaskHandle*>(m_task_table[i]->getRawHandle());
            //printf("Attempting task: 0x%p,0x%x Arg: 0x%p\n", handle, handle->m_routine, handle->m_argument);
            if (handle == NULL || handle->m_routine == NULL) {
                continue;
            }
            //Run-it!
            handle->m_routine(handle->m_argument);
        }
    }
}
}
