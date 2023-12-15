#include "FightTimesPlugin.h"

#include "Controller/Controller.h"
#include "Task/ProcessTask.h"

bool asst::FightTimesPlugin::verify(AsstMsg msg, const json::value& details) const
{
    if (msg != AsstMsg::SubTaskStart || details.get("subtask", std::string()) != "ProcessTask") {
        return false;
    }

    return !inited && details.at("details").at("task").as_string().ends_with("StartButton1");
}

bool asst::FightTimesPlugin::_run()
{
    const static std::string FightSeriesOpenTask = "FightSeries-Indicator";
    const static std::string FightSeriesValidTask = "FightSeries-Icon";
    auto img = ctrler()->get_image();
    // 目前也可以做成subtask，但是给以后糊屎留个地方
    auto check_status_task = ProcessTask(*this, { FightSeriesOpenTask, FightSeriesValidTask });
    check_status_task.set_reusable_image(img).set_retry_times(0);

    if (!check_status_task.run()) { // 认为是外服，无法使用连续战斗
        inited = true;
        return true;
    }

    if (check_status_task.get_last_task_name() == FightSeriesValidTask) { // 连续战斗次数选择列表未打开
        auto task = ProcessTask(*this, { "FightSeries-Open" });
        task.set_reusable_image(img);
        if (!task.run()) {
            return false;
        }
    }
    ProcessTask(*this, { "FightSeries-List-1" }).run();

    inited = true;
    return true;
}