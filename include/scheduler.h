#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

enum class ScheduleFrequency {
    ONCE,
    DAILY,
    WEEKLY,
    MONTHLY
};

struct ScheduledTask {
    std::string id;
    std::string name;
    std::chrono::system_clock::time_point scheduledTime;
    ScheduleFrequency frequency;
    std::function<void()> taskFunction;
    bool executed;
    
    ScheduledTask(const std::string& id, const std::string& name,
                 std::chrono::system_clock::time_point time,
                 ScheduleFrequency freq, std::function<void()> func)
        : id(id), name(name), scheduledTime(time), frequency(freq), 
          taskFunction(func), executed(false) {}
};

class Scheduler {
private:
    std::map<std::string, std::shared_ptr<ScheduledTask>> tasks;
    std::thread schedulerThread;
    mutable std::mutex tasksMutex; 
    std::condition_variable wakeCondition;
    std::atomic<bool> running;
    
    void schedulerLoop();
    void executeTask(std::shared_ptr<ScheduledTask> task);
    std::chrono::system_clock::time_point calculateNextExecutionTime(const ScheduledTask& task);
    
public:
    Scheduler();
    ~Scheduler();
    
    // Prevent copying and moving
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;
    
    // Task management
    bool addTask(const std::string& id, const std::string& name,
                std::chrono::system_clock::time_point scheduledTime,
                ScheduleFrequency frequency, std::function<void()> taskFunction);
    
    bool removeTask(const std::string& id);
    bool taskExists(const std::string& id) const;
    
    // Getters
    std::vector<std::shared_ptr<ScheduledTask>> getAllTasks() const;
    std::shared_ptr<ScheduledTask> getTask(const std::string& id) const;
    
    // Controls
    void start();
    void stop();
    bool isRunning() const;
    
    // Utility functions
    static std::string frequencyToString(ScheduleFrequency frequency);
    static std::string timePointToString(const std::chrono::system_clock::time_point& timePoint);
};

#endif // SCHEDULER_H