
#include "device.h"
#include "enumerate.h"
#include "instance.h"
#include "surface.h"
#include <algorithm>
#include <core.h>
#include <set>
#include <vulkan/vulkan.hpp>

namespace vulkan {

std::vector<VkQueueFamilyProperties>::const_iterator
findQueue(const std::vector<VkQueueFamilyProperties>& queue_families,
          const std::string& name,
          const VkQueueFlags required_bits,
          const VkQueueFlags excluded_bits) {
    const auto family = std::find_if(queue_families.begin(),
                                     queue_families.end(),
                                     [required_bits, excluded_bits](const VkQueueFamilyProperties& queue_family) {
                                         return queue_family.queueCount > 0 &&
                                                queue_family.queueFlags & required_bits &&
                                                !(queue_family.queueFlags & excluded_bits);
                                     });

    LF_ASSERT(family != queue_families.end(), "found no matching {0} queue", name);

    return family;
}

Device::Device(VkPhysicalDevice physical_device,
               const Surface& surface,
               const std::vector<const char*>& required_extensions,
               const VkPhysicalDeviceFeatures& device_features,
               const void* next_device_features)
    : physical_device_(physical_device), surface_(surface) {

    checkRequiredExtensions(physical_device, required_extensions);

    const auto queue_families = getEnumerateVector(physical_device, vkGetPhysicalDeviceQueueFamilyProperties);

    // Find the graphics queue.
    const auto graphics_family = findQueue(queue_families, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
    const auto compute_family = findQueue(queue_families, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
    const auto transfer_family =
        findQueue(queue_families, "transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

    // Find the presentation queue (usually the same as graphics queue).
    const auto present_family =
        std::find_if(queue_families.begin(), queue_families.end(), [&](const VkQueueFamilyProperties& queue_family) {
            VkBool32 present_support = false;
            const uint32_t i = static_cast<uint32_t>(&*queue_families.cbegin() - &queue_family);
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface.handle(), &present_support);
            return queue_family.queueCount > 0 && present_support;
        });

    LF_ASSERT(present_family != queue_families.end(), "Found no presentation Queue");

    graphics_family_index_ = static_cast<uint32_t>(graphics_family - queue_families.begin());
    compute_family_index_ = static_cast<uint32_t>(compute_family - queue_families.begin());
    present_family_index_ = static_cast<uint32_t>(present_family - queue_families.begin());
    transfer_family_index_ = static_cast<uint32_t>(transfer_family - queue_families.begin());

    // Queues can be the same
    const std::set<uint32_t> unique_queue_families = {graphics_family_index_,
                                                      compute_family_index_,
                                                      present_family_index_,
                                                      transfer_family_index_};

    // Create queues
    float queue_priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    for (uint32_t queue_family_index : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family_index;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        queue_create_infos.push_back(queue_create_info);
    }

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext = next_device_features;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledLayerCount = static_cast<uint32_t>(surface_.instance().validationLayers().size());
    create_info.ppEnabledLayerNames = surface_.instance().validationLayers().data();
    create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();

    vulkanCheck(vkCreateDevice(physical_device, &create_info, nullptr, &device_), "create logical device");

    vkGetDeviceQueue(device_, graphics_family_index_, 0, &graphics_queue_);
    vkGetDeviceQueue(device_, compute_family_index_, 0, &compute_queue_);
    vkGetDeviceQueue(device_, present_family_index_, 0, &present_queue_);
    vkGetDeviceQueue(device_, transfer_family_index_, 0, &transfer_queue_);
}

Device::~Device() {
    if (device_ != nullptr) {
        vkDestroyDevice(device_, nullptr);
        device_ = nullptr;
    }
}

void Device::waitIdle() const {
    vulkanCheck(vkDeviceWaitIdle(device_), "wait for device idle");
}

void Device::checkRequiredExtensions(VkPhysicalDevice physical_device,
                                     const std::vector<const char*>& required_extensions) {
    const auto available_extensions =
        getEnumerateVector(physical_device, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
    std::set<std::string> required(required_extensions.begin(), required_extensions.end());

    for (const auto& extension : available_extensions) { required.erase(extension.extensionName); }

    if (!required.empty()) {
        bool first = true;
        std::string extensions;

        for (const auto& extension : required) {
            if (!first) {
                extensions += ", ";
            }

            extensions += extension;
            first = false;
        }

        LF_ERROR("missing required extensions: {0}", extensions);
    }
}

}  // namespace vulkan
