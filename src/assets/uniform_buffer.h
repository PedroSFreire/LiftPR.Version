#pragma once

#include "core/glm.h"
#include "lights.h"
#include <memory>

namespace vulkan {
class Buffer;
class Device;
class DeviceMemory;
}  // namespace vulkan

namespace assets {
struct alignas(16) UniformBufferObject {
    glm::mat4 modelView {};
    glm::mat4 projection {};
    glm::mat4 model_view_inverse {};
    glm::mat4 projection_inverse {};
    float x {};
    //float focus_distance {};
    float fov {};
    float y {};
    uint32_t number_of_samples {};
    uint32_t number_of_bounces {};
    uint32_t seed {};
    float z {};  // bool
    uint32_t gamma_correction {};  // bool
    uint32_t tone_map {};  // bool
    float exposure {};
    uint32_t has_sky {};           // bool
    uint32_t frame {};
    uint32_t debug_normals {};  // bool
    uint32_t debug_radiance {};  // bool
    float resX {};
    uint32_t total_number_of_bounces {};
};

class UniformBuffer {
public:
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer& operator=(UniformBuffer&&) = delete;

    explicit UniformBuffer(const vulkan::Device& device);
    UniformBuffer(UniformBuffer&& other) noexcept;
    ~UniformBuffer();

    [[nodiscard]] const vulkan::Buffer& buffer() const { return *buffer_; }

    void setValue(const UniformBufferObject& ubo);

private:
    std::unique_ptr<vulkan::Buffer> buffer_;
    std::unique_ptr<vulkan::DeviceMemory> memory_;
};

}  // namespace assets