#include "display_server.h"

#include <set>

#include "input_server.h"

namespace Flint {

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

DisplayServer::DisplayServer() {
    glfwInit();

    create_instance();
}

void DisplayServer::cleanup() {
    vkDestroyCommandPool(device, command_pool, nullptr);

    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwTerminate();
}

void DisplayServer::initialize_after_surface_creation(VkSurfaceKHR surface) {
    if (initialized) {
        return;
    }

    // Pick a suitable GPU.
    pick_physical_device(surface);

    // Create a logical device.
    create_logical_device(surface);

    create_command_pool(surface);

    create_graphics_queues(surface, graphicsQueue);

    initialized = true;
}

void DisplayServer::create_command_pool(VkSurfaceKHR surface) {
    QueueFamilyIndices qf_indices = findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = qf_indices.graphicsFamily.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // So we can reset command buffers.

    if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

GLFWwindow *DisplayServer::create_window(Vec2I size, const std::string &title) {
    // Do not create an OpenGL context (as we're using Vulkan).
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Enable window resizing.
    // Resizing requires swap chain recreation.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Hide window upon creation as we need to center the window before showing it.
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // Get monitor position (used to correctly center the window in a multi-monitor scenario).
    int monitors_count;
    GLFWmonitor **monitors = glfwGetMonitors(&monitors_count);

    const GLFWvidmode *video_mode = glfwGetVideoMode(monitors[0]);

    int monitor_x, monitor_y;
    glfwGetMonitorPos(monitors[0], &monitor_x, &monitor_y);

    // Get DPI scale.
    float dpi_scale_x, dpi_scale_y;
    glfwGetMonitorContentScale(monitors[0], &dpi_scale_x, &dpi_scale_y);

    GLFWwindow *glfw_window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

    // Center window.
    glfwSetWindowPos(
        glfw_window, monitor_x + (video_mode->width - size.x) / 2, monitor_y + (video_mode->height - size.y) / 2);

    // Show window.
    glfwShowWindow(glfw_window);

    // Assign this to window user, so we can fetch it when the window size changes.
    glfwSetWindowUserPointer(glfw_window, this);
    //        glfwSetFramebufferSizeCallback(glfw_window, framebufferResizeCallback);

    InputServer::get_singleton()->initialize_callbacks(glfw_window);

    return glfw_window;
}

void DisplayServer::create_logical_device(VkSurfaceKHR surface) {
    QueueFamilyIndices qf_indices = findQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    std::set<uint32_t> unique_queue_families = {qf_indices.graphicsFamily.value(), qf_indices.presentFamily.value()};

    float queue_priority = 1.0f;

    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        create_info.queueFamilyIndex = queue_family;
        create_info.queueCount = 1;
        create_info.pQueuePriorities = &queue_priority;

        queue_create_infos.push_back(create_info);
    }

    // Describing the fine-grained features that can be supported by an implementation.
    VkPhysicalDeviceFeatures device_features{};
    // Specifies whether anisotropic filtering is supported.
    // If this feature is not enabled, the anisotropyEnable member of the VkSamplerCreateInfo structure must be
    // VK_FALSE.
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();

    device_create_info.pEnabledFeatures = &device_features;

    device_create_info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    device_create_info.ppEnabledExtensionNames = deviceExtensions.data();

    // enabledLayerCount and ppEnabledLayerNames are deprecated and ignored.
    if (enableValidationLayers) {
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        device_create_info.ppEnabledLayerNames = validationLayers.data();
    } else {
        device_create_info.enabledLayerCount = 0;
    }

    // A logical device is created as a connection to a physical device.
    if (vkCreateDevice(physicalDevice, &device_create_info, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }
}

void DisplayServer::create_graphics_queues(VkSurfaceKHR surface, VkQueue &graphicsQueue) {
    QueueFamilyIndices qf_indices = findQueueFamilies(physicalDevice, surface);

    // Get a queue handle from a device.
    vkGetDeviceQueue(device, qf_indices.graphicsFamily.value(), 0, &graphicsQueue);
}

void DisplayServer::create_present_queues(VkSurfaceKHR surface, VkQueue &presentQueue) {
    QueueFamilyIndices qf_indices = findQueueFamilies(physicalDevice, surface);

    // Get a queue handle from a device.
    vkGetDeviceQueue(device, qf_indices.presentFamily.value(), 0, &presentQueue);
}

void DisplayServer::create_instance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    // Specifying application information.
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Flint";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine"; // Name of the engine (if any) used to create the application.
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.pNext = nullptr;

    // Specifying parameters of a newly created instance.
    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.flags = 0;

    auto extensions = getRequiredExtensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enableValidationLayers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        create_info.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debug_create_info);
        // Pointer to a structure extending this structure.
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;

        create_info.pNext = nullptr;
    }

    // Create a new Vulkan instance.
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance!");
    }
}

void DisplayServer::setupDebugMessenger() {
    if (!enableValidationLayers) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}

void DisplayServer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void DisplayServer::pick_physical_device(VkSurfaceKHR surface) {
    // Enumerates the physical devices accessible to a Vulkan instance.
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
        throw std::runtime_error("Failed to find a GPU with Vulkan support!");
    }

    // Call again to get the physical devices.
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    // Pick a suitable one among the physical devices.
    for (const auto &d : devices) {
        if (isDeviceSuitable(d, surface)) {
            physicalDevice = d;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

QueueFamilyIndices DisplayServer::findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices qfIndices;

    // Reports properties of the queues of the specified physical device.
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    // Structure providing information about a queue family.
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        // queueFlags is a bitmask of VkQueueFlagBits indicating capabilities of the queues in this queue family.
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            qfIndices.graphicsFamily = i;
        }

        // Query if presentation is supported.
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport) {
            qfIndices.presentFamily = i;
        }

        // If both graphics family and present family acquired.
        if (qfIndices.isComplete()) {
            break;
        }

        i++;
    }

    return qfIndices;
}

bool DisplayServer::isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

SwapChainSupportDetails DisplayServer::querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool DisplayServer::checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VkFormat DisplayServer::findSupportedFormat(const std::vector<VkFormat> &candidates,
                                            VkImageTiling tiling,
                                            VkFormatFeatureFlags features) const {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

uint32_t DisplayServer::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties mem_properties;
    // Reports memory information for the specified physical device.
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

VkFormat DisplayServer::findDepthFormat() const {
    return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace Flint
