#include "../include/scheduler.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Scheduler::Scheduler() : running(false) {
}

Scheduler::~Scheduler() {
    stop();
}

bool Scheduler::addTask(const std::string& id, const std::string& name,
                        std::chrono::system_clock::time_point scheduledTime,
                        ScheduleFrequency frequency, std::function<void()> taskFunction) {
    std::lock_guard<std::mutex> lock(tasksMutex);
    
    if (tasks.find(id) != tasks.end()) {
        return false; // Task ID already exists
    }
    
    auto task = std::make_shared<ScheduledTask>(id, name, scheduledTime, frequency, taskFunction);
    tasks[id] = task;
    
    // Wake up the scheduler thread to recalculate next task
    wakeCondition.notify_one();
    
    return true;
}

bool Scheduler::removeTask(const std::string& id) {
    std::lock_guard<std::mutex> lock(tasksMutex);
    
    auto it = tasks.find(id);
    if (it == tasks.end()) {
        return false; // Task not found
    }
    
    tasks.erase(it);
    
    // Wake up the scheduler thread to recalculate next task
    wakeCondition.notify_one();
    
    return true;
}

bool Scheduler::taskExists(const std::string& id) const {
    std::lock_guard<std::mutex> lock(tasksMutex);
    return tasks.find(id) != tasks.end();
}

std::vector<std::shared_ptr<ScheduledTask>> Scheduler::getAllTasks() const {
    std::lock_guard<std::mutex> lock(tasksMutex);
    
    std::vector<std::shared_ptr<ScheduledTask>> result;
    for (const auto& pair : tasks) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::shared_ptr<ScheduledTask> Scheduler::getTask(const std::string& id) const {
    std::lock_guard<std::mutex> lock(tasksMutex);
    
    auto it = tasks.find(id);
    if (it == tasks.end()) {
        return nullptr;
    }
    
    return it->second;
}

void Scheduler::start() {
    if (running) {
        return;
    }
    
    running = true;
    schedulerThread = std::thread(&Scheduler::schedulerLoop, this);
}

void Scheduler::stop() {
    if (!running) {
        return;
    }
    
    running = false;
    wakeCondition.notify_one();
    
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
}

bool Scheduler::isRunning() const {
    return running;
}

void Scheduler::schedulerLoop() {
    while (running) {
        std::chrono::system_clock::time_point nextTaskTime;
        std::shared_ptr<ScheduledTask> nextTask;
        
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            
            // Find the task with the earliest execution time
            nextTaskTime = std::chrono::system_clock::time_point::max();
            
            for (const auto& pair : tasks) {
                if (pair.second->scheduledTime < nextTaskTime) {
                    nextTaskTime = pair.second->scheduledTime;
                    nextTask = pair.second;
                }
            }
            
            if (!nextTask) {
                // No tasks scheduled, wait until a new task is added
                wakeCondition.wait(lock);
                continue;
            }
            
            auto now = std::chrono::system_clock::now();
            if (nextTaskTime > now) {
                // Wait until the next task is due or a new task is added
                wakeCondition.wait_until(lock, nextTaskTime);
                continue;
            }
            
            // Execute the task outside the lock
        }
        
        // Now that we're outside the lock, execute the task
        if (nextTask) {
            executeTask(nextTask);
        }
    }
}

void Scheduler::executeTask(std::shared_ptr<ScheduledTask> task) {
    try {
        task->taskFunction();
        task->executed = true;
        
        // For recurring tasks, calculate the next execution time
        if (task->frequency != ScheduleFrequency::ONCE) {
            std::lock_guard<std::mutex> lock(tasksMutex);
            
            // Update the scheduled time for the next execution
            task->scheduledTime = calculateNextExecutionTime(*task);
            task->executed = false;
        } else {
            // For one-time tasks, remove them after execution
            removeTask(task->id);
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in task " << task->id << ": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in task " << task->id << std::endl;
    }
}

std::chrono::system_clock::time_point Scheduler::calculateNextExecutionTime(const ScheduledTask& task) {
    auto timePoint = task.scheduledTime;
    auto now = std::chrono::system_clock::now();
    
    // Convert time_point to time_t to work with calendar time
    auto timeT = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* timeInfo = std::localtime(&timeT);
    
    switch (task.frequency) {
        case ScheduleFrequency::DAILY:
            // Next day, same time
            timeInfo->tm_mday += 1;
            break;
            
        case ScheduleFrequency::WEEKLY:
            // Next week, same day and time
            timeInfo->tm_mday += 7;
            break;
            
        case ScheduleFrequency::MONTHLY:
            // Next month, same day and time
            timeInfo->tm_mon += 1;
            if (timeInfo->tm_mon > 11) {
                timeInfo->tm_mon = 0;
                timeInfo->tm_year += 1;
            }
            break;
            
        default:
            // Default behavior (shouldn't reach here for ONCE frequency)
            break;
    }
    
    // Convert back to time_point
    timeT = std::mktime(timeInfo);
    auto nextTimePoint = std::chrono::system_clock::from_time_t(timeT);
    
    // Ensure the next execution time is in the future
    if (nextTimePoint <= now) {
        // If not, try adding another period
        return calculateNextExecutionTime(ScheduledTask(task.id, task.name, nextTimePoint, 
                                                      task.frequency, task.taskFunction));
    }
    
    return nextTimePoint;
}

std::string Scheduler::frequencyToString(ScheduleFrequency frequency) {
    switch (frequency) {
        case ScheduleFrequency::ONCE:
            return "Once";
        case ScheduleFrequency::DAILY:
            return "Daily";
        case ScheduleFrequency::WEEKLY:
            return "Weekly";
        case ScheduleFrequency::MONTHLY:
            return "Monthly";
        default:
            return "Unknown";
    }
}

std::string Scheduler::timePointToString(const std::chrono::system_clock::time_point& timePoint) {
    auto timeT = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* timeInfo = std::localtime(&timeT);
    
    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}