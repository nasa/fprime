/*
 * TaskRunner.cpp
 *
 *  Created on: Feb 28, 2019
 *      Author: lestarch
 */
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>
#include <Os/Baremetal/TaskRunner/TaskRunner.hpp>
#include <Os/Baremetal/TaskRunner/BareTaskHandle.hpp>
namespace Os {

TaskRunner::TaskRunner() :
    m_index(0),
    m_cont(true)
{
    for (U32 i = 0; i < TASK_REGISTRY_CAP; i++) {
        this->m_task_table[i] = 0;
    }
    Task::registerTaskRegistry(this);
}
TaskRunner::~TaskRunner() {}

void TaskRunner::addTask(Task* task) {
    FW_ASSERT(m_index < TASK_REGISTRY_CAP);
    this->m_task_table[m_index] = task;
    m_index++;
}

void TaskRunner::removeTask(Task* task) {
    bool found = false;
    //Squash that existing task
    for (U32 i = 0; i < TASK_REGISTRY_CAP; i++) {
        found = found | (task == this->m_task_table[i]);
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
            this->m_task_table[i] = nullptr;
        }
    }
}

void TaskRunner::stop() {
    m_cont = false;
}

void TaskRunner::run() {
    U32 i = 0;
    if (!m_cont) {
        return;
    }
    //Loop through full table
    for (i = 0; i < TASK_REGISTRY_CAP; i++) {
        //Break at end of table
        if (m_task_table[i] == nullptr) {
            break;
        }
        //Get bare task or break
        BareTaskHandle* handle = reinterpret_cast<BareTaskHandle*>(m_task_table[i]->getRawHandle());
        if (handle == nullptr || handle->m_routine == nullptr || !handle->m_enabled) {
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
