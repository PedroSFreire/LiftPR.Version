#include "ray_tracing_pipeline.h"

#include "assets/scene.h"
#include "assets/uniform_buffer.h"
#include "device_procedures.h"
#include "tlas.h"
#include "vulkan/buffer.h"
#include "vulkan/descriptor_binding.h"
#include "vulkan/descriptor_set_manager.h"
#include "vulkan/descriptor_sets.h"
#include "vulkan/device.h"
#include "vulkan/image_view.h"
#include "vulkan/pipeline_layout.h"
#include "vulkan/shader_module.h"
#include "vulkan/swap_chain.h"

namespace vulkan {

RayTracingPipeline::RayTracingPipeline(const DeviceProcedures& device_procedures,
                                       const SwapChain& swap_chain,
                                       const TopLevelAccelerationStructure& acceleration_structure,
                                       const ImageView& output_image_view,
                                       const ImageView& accumulation_image_view,
                                       const std::vector<assets::UniformBuffer>& uniform_buffers,
                                       const assets::Scene& scene,
                                       const Algorithm algorithm)
    : swap_chain_(swap_chain), algorithm_(algorithm) {
    // Create descriptor pool/sets.
    const auto& device = swap_chain.device();
    const std::vector<DescriptorBinding> descriptor_bindings = {
        // Top level acceleration structure.
        {0,
         1,
         VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
         VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

        // Output & Accumulation Image
        {1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR},
        {2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR},

        // Scene & Camera (Uniform Buffer)
        {3,
         1,
         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
         VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

        // Vertex buffer
        {4, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
        // Index buffer
        {5, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
        // Material buffer
        {6, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
        // Lights buffer,
        {7, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_RAYGEN_BIT_KHR},
        // Offset buffer
        {8, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

        // Textures and image samplers
        {9,
         static_cast<uint32_t>(scene.textureSamplers().size()),
         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
         VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

        // The Procedural buffer (Spheres).
        {10,
         1,
         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
         VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR}};

    descriptor_set_manager_ =
        std::make_unique<DescriptorSetManager>(device, descriptor_bindings, uniform_buffers.size());

    auto& descriptor_sets = descriptor_set_manager_->descriptorSets();

    for (uint32_t i = 0; i != swap_chain.images().size(); ++i) {
        // Top level acceleration structure.
        const auto acceleration_structure_handle = acceleration_structure.handle();
        VkWriteDescriptorSetAccelerationStructureKHR structure_info = {};
        structure_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        structure_info.pNext = nullptr;
        structure_info.accelerationStructureCount = 1;
        structure_info.pAccelerationStructures = &acceleration_structure_handle;

        // Output image
        VkDescriptorImageInfo output_image_info = {};
        output_image_info.imageView = output_image_view.handle();
        output_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        // Accumulation image
        VkDescriptorImageInfo accumulation_image_info = {};
        accumulation_image_info.imageView = accumulation_image_view.handle();
        accumulation_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        // Uniform buffer
        VkDescriptorBufferInfo uniform_buffer_info = {};
        uniform_buffer_info.buffer = uniform_buffers[i].buffer().handle();
        uniform_buffer_info.range = VK_WHOLE_SIZE;

        // Vertex buffer
        VkDescriptorBufferInfo vertex_buffer_info = {};
        vertex_buffer_info.buffer = scene.vertexBuffer().handle();
        vertex_buffer_info.range = VK_WHOLE_SIZE;

        // Index buffer
        VkDescriptorBufferInfo index_buffer_info = {};
        index_buffer_info.buffer = scene.indexBuffer().handle();
        index_buffer_info.range = VK_WHOLE_SIZE;

        // Material buffer
        VkDescriptorBufferInfo material_buffer_info = {};
        material_buffer_info.buffer = scene.materialBuffer().handle();
        material_buffer_info.range = VK_WHOLE_SIZE;

        // Light buffer
        VkDescriptorBufferInfo light_buffer_info = {};
        light_buffer_info.buffer = scene.lightBuffer().handle();
        light_buffer_info.range = VK_WHOLE_SIZE;

        // Offsets buffer
        VkDescriptorBufferInfo offsets_buffer_info = {};
        offsets_buffer_info.buffer = scene.offsetsBuffer().handle();
        offsets_buffer_info.range = VK_WHOLE_SIZE;

        // Image and texture samplers.
        std::vector<VkDescriptorImageInfo> image_infos(scene.textureSamplers().size());

        for (size_t t = 0; t != image_infos.size(); ++t) {
            auto& image_info = image_infos[t];
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = scene.textureImageViews()[t];
            image_info.sampler = scene.textureSamplers()[t];
        }

        std::vector<VkWriteDescriptorSet> descriptor_writes = {
            descriptor_sets.bind(i, 0, structure_info),
            descriptor_sets.bind(i, 1, output_image_info),
            descriptor_sets.bind(i, 2, accumulation_image_info),
            descriptor_sets.bind(i, 3, uniform_buffer_info),
            descriptor_sets.bind(i, 4, vertex_buffer_info),
            descriptor_sets.bind(i, 5, index_buffer_info),
            descriptor_sets.bind(i, 6, material_buffer_info),
            descriptor_sets.bind(i, 7, light_buffer_info),
            descriptor_sets.bind(i, 8, offsets_buffer_info),
            descriptor_sets.bind(i, 9, *image_infos.data(), static_cast<uint32_t>(image_infos.size())),
        };

        // Procedural buffer (optional)
        VkDescriptorBufferInfo procedural_buffer_info = {};

        if (scene.hasProcedurals()) {
            procedural_buffer_info.buffer = scene.proceduralBuffer().handle();
            procedural_buffer_info.range = VK_WHOLE_SIZE;

            descriptor_writes.push_back(descriptor_sets.bind(i, 10, procedural_buffer_info));
        }

        descriptor_sets.updateDescriptors(descriptor_writes);
    }

    pipeline_layout_ = std::make_unique<class PipelineLayout>(device, descriptor_set_manager_->descriptorSetLayout());

    // Load shaders.
    std::string rgen_path = "../resources/shaders/path.rgen.spv";
    std::string chit_path = "../resources/shaders/path.rchit.spv";
    if (algorithm == Algorithm::BDPT) {
        rgen_path = "../resources/shaders/bdpt.rgen.spv";
        //        chit_path = "../resources/shaders/bdpt.rchit.spv";
    }
    else if (algorithm == Algorithm::VCM) {
        rgen_path = "../resources/shaders/vcm.rgen.spv";
    }
    else if (algorithm == Algorithm::VM) {
        rgen_path = "../resources/shaders/vm.rgen.spv";
    } else if (algorithm == Algorithm::VC) {
        rgen_path = "../resources/shaders/vc.rgen.spv";
    }

    const ShaderModule ray_gen_shader(device, rgen_path);
    const ShaderModule miss_shader(device, "../resources/shaders/path.rmiss.spv");
    const ShaderModule shadow_miss_shader(device, "../resources/shaders/shadow.rmiss.spv");
    const ShaderModule closest_hit_shader(device, chit_path);
    const ShaderModule procedural_closest_hit_shader(device, "../resources/shaders/path.procedural.rchit.spv");
    const ShaderModule procedural_intersection_shader(device, "../resources/shaders/path.procedural.rint.spv");

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
        ray_gen_shader.createShaderStage(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
        miss_shader.createShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
        shadow_miss_shader.createShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
        closest_hit_shader.createShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
        procedural_closest_hit_shader.createShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
        procedural_intersection_shader.createShaderStage(VK_SHADER_STAGE_INTERSECTION_BIT_KHR),
    };

    // Shader groups
    VkRayTracingShaderGroupCreateInfoKHR ray_gen_group_info = {};
    ray_gen_group_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ray_gen_group_info.pNext = nullptr;
    ray_gen_group_info.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ray_gen_group_info.generalShader = 0;
    ray_gen_group_info.closestHitShader = VK_SHADER_UNUSED_KHR;
    ray_gen_group_info.anyHitShader = VK_SHADER_UNUSED_KHR;
    ray_gen_group_info.intersectionShader = VK_SHADER_UNUSED_KHR;
    ray_gen_index_ = 0;

    VkRayTracingShaderGroupCreateInfoKHR miss_group_info = {};
    miss_group_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    miss_group_info.pNext = nullptr;
    miss_group_info.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    miss_group_info.generalShader = 1;
    miss_group_info.closestHitShader = VK_SHADER_UNUSED_KHR;
    miss_group_info.anyHitShader = VK_SHADER_UNUSED_KHR;
    miss_group_info.intersectionShader = VK_SHADER_UNUSED_KHR;
    miss_index_ = 1;

    VkRayTracingShaderGroupCreateInfoKHR shadow_miss_group_info = {};
    shadow_miss_group_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    shadow_miss_group_info.pNext = nullptr;
    shadow_miss_group_info.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    shadow_miss_group_info.generalShader = 2;
    shadow_miss_group_info.closestHitShader = VK_SHADER_UNUSED_KHR;
    shadow_miss_group_info.anyHitShader = VK_SHADER_UNUSED_KHR;
    shadow_miss_group_info.intersectionShader = VK_SHADER_UNUSED_KHR;
    shadow_miss_index_ = 2;

    VkRayTracingShaderGroupCreateInfoKHR triangle_hit_group_info = {};
    triangle_hit_group_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    triangle_hit_group_info.pNext = nullptr;
    triangle_hit_group_info.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    triangle_hit_group_info.generalShader = VK_SHADER_UNUSED_KHR;
    triangle_hit_group_info.closestHitShader = 3;
    triangle_hit_group_info.anyHitShader = VK_SHADER_UNUSED_KHR;
    triangle_hit_group_info.intersectionShader = VK_SHADER_UNUSED_KHR;
    triangle_hit_group_index_ = 3;

    VkRayTracingShaderGroupCreateInfoKHR procedural_hit_group_info = {};
    procedural_hit_group_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    procedural_hit_group_info.pNext = nullptr;
    procedural_hit_group_info.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
    procedural_hit_group_info.generalShader = VK_SHADER_UNUSED_KHR;
    procedural_hit_group_info.closestHitShader = 4;
    procedural_hit_group_info.anyHitShader = VK_SHADER_UNUSED_KHR;
    procedural_hit_group_info.intersectionShader = 5;
    procedural_hit_group_index_ = 4;

    std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups = {
        ray_gen_group_info,
        miss_group_info,
        shadow_miss_group_info,
        triangle_hit_group_info,
        procedural_hit_group_info,
    };

    // Create raytracing pipeline
    VkRayTracingPipelineCreateInfoKHR pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    pipeline_info.pNext = nullptr;
    pipeline_info.flags = 0;
    pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.groupCount = static_cast<uint32_t>(groups.size());
    pipeline_info.pGroups = groups.data();
    pipeline_info.maxPipelineRayRecursionDepth = 2;
    pipeline_info.layout = pipeline_layout_->handle();
    pipeline_info.basePipelineHandle = nullptr;
    pipeline_info.basePipelineIndex = 0;

    vulkanCheck(
        device_procedures
            .vkCreateRayTracingPipelinesKHR(device.handle(), nullptr, nullptr, 1, &pipeline_info, nullptr, &pipeline_),
        "create ray tracing pipeline");
}

RayTracingPipeline::~RayTracingPipeline() {
    if (pipeline_ != nullptr) {
        vkDestroyPipeline(swap_chain_.device().handle(), pipeline_, nullptr);
        pipeline_ = nullptr;
    }

    pipeline_layout_.reset();
    descriptor_set_manager_.reset();
}

VkDescriptorSet RayTracingPipeline::descriptorSet(uint32_t index) const {
    return descriptor_set_manager_->descriptorSets().handle(index);
}

}  // namespace vulkan
