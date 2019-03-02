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
namespace Os {

TaskRunner::TaskRunner(Task::taskRoutine callback) :
    m_index(0),
    m_callback(callback),
    m_cont(false)
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
    this->m_task_table[m_index] = task;
    m_index++;
}
/**
 * Remove a task to the registry. These tasks will no-longer be run.
 * \param Task* task: task to be removed
 */
void TaskRunner::removeTask(Task* task) {
    bool found = false;
    //Squash that existing task
    for (U32 i = 0; i < TASK_REGISTRY_CAP; i++) {
        found = found | task == this->m_task_table[i];
        //If not found, keep looking
        if (!found) {
            continue;
        }
        //If we are less than the end of the array, shift variables over
        else if (i < TASK_REGISTRY_CAP - 1) {
            this->m_task_table[i] = this->m_task_table[i+1];
        }
        //If the last element, mark NULL
        else {
            this->m_task_table[i] = NULL;
        }
    }
}
/**
 * Stop the task runner
 */
void TaskRunner::stop() {
    m_cont = false;
}
/**
 * Program loop, used to run all tasks forever
 */
void TaskRunner::run() {
    m_cont = true;
    //Run until asked to stop, and all subtasks cease
    while (m_cont) {
        U32 i = 0;
        //Loop through full table
        for (i = 0; i < TASK_REGISTRY_CAP; i++) {
            //Break at end of table
            if (m_task_table[i] == NULL) {
                break;
            }
            //Get bare task or break
            BareTaskHandle* handle = reinterpret_cast<BareTaskHandle*>(m_task_table[i]->getRawHandle());
            if (handle == NULL || handle->m_routine == NULL || !handle->m_enabled) {
                continue;
            }
            //Run-it!
            handle->m_routine(handle->m_argument);
            //Disable tasks that have "exited" or stopped
            handle->m_enabled = m_task_table[i]->isStarted();
        }
        //Check if no tasks, and stop
        if (i == 0) {
            m_cont = false;
        }
    }
}
}
