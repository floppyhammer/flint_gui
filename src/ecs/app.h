#ifndef FLINT_ECS_APP_H
#define FLINT_ECS_APP_H

#include "plugin.h"
#include "schedule.h"
#include "world.h"

namespace Flint::Ecs {

class App {
public:
    App(RenderServer *render_server) {
        world = std::make_shared<World>(render_server);
    }

    /// Advances the execution of the [`Schedule`] by one cycle.
    ///
    /// This method also updates sub apps.
    /// See [`insert_sub_app`](Self::insert_sub_app) for more details.
    ///
    /// The schedule run by this method is determined by the [`main_schedule_label`](App) field.
    /// By default this is [`Main`].
    ///
    /// # Panics
    ///
    /// The active schedule of the app must be set before this method is called.
    void update() {
        //            world.run_schedule_ref(&*self.main_schedule_label);
    }

    /// Starts the application by calling the app's [runner function](Self::set_runner).
    ///
    /// Finalizes the [`App`] configuration. For general usage, see the example on the item
    /// level documentation.
    ///
    /// # `run()` might not return
    ///
    /// Calls to [`App::run()`] might never return.
    ///
    /// In simple and *headless* applications, one can expect that execution will
    /// proceed, normally, after calling [`run()`](App::run()) but this is not the case for
    /// windowed applications.
    ///
    /// Windowed apps are typically driven by an *event loop* or *message loop* and
    /// some window-manager APIs expect programs to terminate when their primary
    /// window is closed and that event loop terminates – behavior of processes that
    /// do not is often platform dependent or undocumented.
    ///
    /// By default, *Bevy* uses the `winit` crate for window creation. See
    /// [`WinitSettings::return_from_run`](https://docs.rs/bevy/latest/bevy/winit/struct.WinitSettings.html#structfield.return_from_run)
    /// for further discussion of this topic and for a mechanism to require that [`App::run()`]
    /// *does* return – albeit one that carries its own caveats and disclaimers.
    ///
    /// # Panics
    ///
    /// Panics if called from `Plugin::build()`, because it would prevent other plugins to properly build.
    void run() {
        setup();

        while (true) {
            update();
        }
    }

    /// Run [`Plugin::setup`] for each plugin. This is usually called by [`App::run`], but can
    /// be useful for situations where you want to use [`App::update`].
    void setup() {
        for (auto &plugin : plugin_registry) {
            plugin->setup(*this);
        }
    }

    /// Adds a system to the given schedule in this app's [`Schedules`].
    void add_system(Schedule schedule, void *system) {
        //        world.
        //        let mut schedules = self.world.resource_mut::<Schedules>();
        //
        //        if let Some(schedule) = schedules.get_mut(&schedule) {
        //            schedule.add_systems(systems);
        //            }
        //        else {
        //            let mut new_schedule = Schedule::new ();
        //            new_schedule.add_systems(systems);
        //            schedules.insert(schedule, new_schedule);
        //        }
    }

    void add_startup_system(void *system) {
        //        self.add_systems(Startup, system)
    }

    /// Adds a new `schedule` to the [`App`] under the provided `label`.
    ///
    /// # Warning
    /// This method will overwrite any existing schedule at that label.
    /// To avoid this behavior, use the `init_schedule` method instead.
    void add_schedule(std::shared_ptr<Schedule> schedule) {
        //    let mut schedules = self.world.resource_mut::<Schedules>();
        //    schedules.insert(label, schedule);
    }

private:
    /// The main ECS [`World`] of the [`App`].
    /// This stores and provides access to all the main data of the application.
    /// The systems of the [`App`] will run using this [`World`].
    /// If additional separate [`World`]-[`Schedule`] pairs are needed, you can use [`sub_app`](App::insert_sub_app)s.
    std::shared_ptr<World> world;

    std::vector<std::shared_ptr<Plugin>> plugin_registry;
    std::set<std::string> plugin_name_added;

    std::vector<std::shared_ptr<Schedule>> schedules;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_APP_H
