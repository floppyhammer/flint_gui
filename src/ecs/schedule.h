#ifndef FLINT_ECS_SCHEDULE_H
#define FLINT_ECS_SCHEDULE_H

#include <memory>
#include <string>
#include <vector>

namespace Flint::Ecs {

class Schedule {
    std::string name;
};

/// The schedule that contains the app logic that is evaluated each tick of [`App::update()`].
///
/// By default, it will run the following schedules in the given order:
///
/// On the first run of the schedule (and only on the first run), it will run:
/// * [`PreStartup`]
/// * [`Startup`]
/// * [`PostStartup`]
///
/// Then it will run:
/// * [`First`]
/// * [`PreUpdate`]
/// * [`StateTransition`]
/// * [`RunFixedUpdateLoop`]
///     * This will run [`FixedUpdate`] zero to many times, based on how much time has elapsed.
/// * [`Update`]
/// * [`PostUpdate`]
/// * [`Last`]
class ScheduleMain : public Schedule {};

/// Resource that stores [`Schedule`]s mapped to [`ScheduleLabel`]s.
struct R_Schedules {
    std::vector<std::shared_ptr<Schedule>> inner;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_SCHEDULE_H
