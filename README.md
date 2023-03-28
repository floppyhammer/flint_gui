# Flint

This is an experimental vector-based GUI framework.

**Warning: this is not intended for production use and never will be.**

Features:

* Scene tree.
* Vulkan backend.
* High quality anti-aliasing.
* Multiple window support.
* Cross-platform.
* I18N support.

The development work has been mostly done on Windows, but it should be easy to make
it run on other platforms.

![Example 1](screenshot_1.png)

![Example 2](screenshot_2.png)

![Example 2](screenshot_3.gif)

## How to run examples

### Shader generation

* Run `src/shaders/compile.ps1` to generate shader headers. You should have Vulkan SDK installed first.
