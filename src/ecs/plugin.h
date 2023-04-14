#ifndef FLINT_PLUGIN_H
#define FLINT_PLUGIN_H

#include <string>
#include <typeinfo>

namespace Flint::Ecs {

class App;

/// A collection of Bevy app logic and configuration.
///
/// Plugins configure an [`App`]. When an [`App`] registers a plugin,
/// the plugin's [`Plugin::build`] function is run. By default, a plugin
/// can only be added once to an [`App`].
///
/// If the plugin may need to be added twice or more, the function [`is_unique()`](Self::is_unique)
/// should be overridden to return `false`. Plugins are considered duplicate if they have the same
/// [`name()`](Self::name). The default `name()` implementation returns the type name, which means
/// generic plugins with different type parameters will not be considered duplicates.
class Plugin {
public:
    /// Configures the [`App`] to which this plugin is added.
    virtual void build(App& app) = 0;

    /// Runs after all plugins are built, but before the app runner is called.
    /// This can be useful if you have some resource that other plugins need during their build step,
    /// but after build you want to remove it and send it to another thread.
    virtual void setup(App& app) {
        // do nothing
    }

    /// Configures a name for the [`Plugin`] which is primarily used for checking plugin
    /// uniqueness and debugging.
    std::string name() {
        return std::string(typeid(this).name());
    }

    /// If the plugin can be meaningfully instantiated several times in an [`App`](crate::App),
    /// override this method to return `false`.
    virtual bool is_unique() {
        return true;
    }
};

} // namespace Flint::Ecs

#endif // FLINT_PLUGIN_H
