#ifndef MYTHICPLUS_SCHEDULER_H
#define MYTHICPLUS_SCHEDULER_H

#include "MpLogger.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"
#include <chrono>

enum MP_SCHEDULE_GROUP
{
    MP_WORLD_TASK_GROUP = 100
};

/**
 * Global scheduler specifically for handling events fired inside of mythic
 * scripts
 *
 * Example usage:
 *  void ScheduleWorldTask(std::chrono::nanoseconds const& time, std::function<void (TaskContext)> task) {
 *       sMpScheduler.Schedule(time, MP_WORLD_TASK_GROUP, [](TaskContext ctx) {
 *          // do the things you want for Mp Task Group
 *          return;
 *      });
 *  }
 */
class MpScheduler
{
public:
    static MpScheduler* instance () {
        static MpScheduler instance;
        return &instance;
    }

    MpScheduler(const MpScheduler&) = delete;
    MpScheduler& operator=(const MpScheduler&) = delete;

    TaskScheduler& GetWorldScheduler() {
        return _worldScheduler;
    }

private:
    MpScheduler() {}
    ~MpScheduler() {}

    TaskScheduler _worldScheduler;
};

#define sMpScheduler MpScheduler::instance()
#endif // MYTHICPLUS_SCHEDULER_H

// Attach the world scheduler to listen to world events
class MpScheduler_WorldScript : public WorldScript
{
    public:
        MpScheduler_WorldScript() : WorldScript("MpScheduler_GlobalScript") { }

    void OnUpdate(uint32 diff) override {
        sMpScheduler->GetWorldScheduler().Update(diff);
    }
};

