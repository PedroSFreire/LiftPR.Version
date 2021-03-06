#pragma once

#include "core/utilities.h"

namespace vulkan {
class Device;

class CommandPool final {
public:
    CommandPool(const Device& device, uint32_t queue_family_index, bool allow_reset);
    ~CommandPool();

    [[nodiscard]] VkCommandPool handle() const { return command_pool_; }
    [[nodiscard]] const class Device& device() const { return device_; }

private:
    const class Device& device_;
    VkCommandPool command_pool_ {};
};

}  // namespace vulkan
