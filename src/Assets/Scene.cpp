#include "scene.h"
#include "model.h"
#include "sphere.h"
#include "texture.h"
#include "texture_image.h"
#include "platform/vulkan/buffer.h"
#include "platform/vulkan/command_pool.h"
#include "platform/vulkan/image_view.h"
#include "platform/vulkan/sampler.h"
#include <cstring>
#include <memory>

namespace assets {

template<class T>
void copyFromStagingBuffer(vulkan::CommandPool& command_pool,
						   vulkan::Buffer& dst_buffer,
						   const std::vector<T>& content) {
	const auto& device = command_pool.device();
	const auto content_size = sizeof(content[0]) * content.size();

	// Create a temporary host-visible staging buffer.
	auto staging_buffer = std::make_unique<vulkan::Buffer>(device, content_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	auto staging_buffer_memory =
		staging_buffer->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// Copy the host data into the staging buffer.
	const auto data = staging_buffer_memory.map(0, content_size);
	std::memcpy(data, content.data(), content_size);
	staging_buffer_memory.unmap();

	// Copy the staging buffer to the device buffer.
	dst_buffer.copyFrom(command_pool, *staging_buffer, content_size);

	// Delete the buffer before the memory
	staging_buffer.reset();
}

template<class T>
void createDeviceBuffer(
	vulkan::CommandPool& command_pool,
	const VkBufferUsageFlags usage,
	const std::vector<T>& content,
	std::unique_ptr<vulkan::Buffer>& buffer,
	std::unique_ptr<vulkan::DeviceMemory>& memory) {
	const auto& device = command_pool.device();
	const auto content_size = sizeof(content[0]) * content.size();

	buffer = std::make_unique<vulkan::Buffer>(device, content_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage);
	memory = std::make_unique<vulkan::DeviceMemory>(buffer->allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	copyFromStagingBuffer(command_pool, *buffer, content);
}

Scene::Scene(vulkan::CommandPool& command_pool,
			 std::vector<Model>&& models,
			 std::vector<Texture>&& textures,
			 bool used_for_ray_tracing) :
	models_(std::move(models)),
	textures_(std::move(textures)) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Material> materials;
	std::vector<glm::vec4> procedurals;
	std::vector<std::pair<glm::vec3, glm::vec3>> aabbs;
	std::vector<glm::uvec2> offsets;

	for (const auto& model : models_) {
		const auto index_offset = static_cast<uint32_t>(indices.size());
		const auto vertex_offset = static_cast<uint32_t>(vertices.size());
		const auto material_offset = static_cast<uint32_t>(materials.size());

		offsets.emplace_back(index_offset, vertex_offset);

		vertices.insert(vertices.end(), model.vertices().begin(), model.vertices().end());
		indices.insert(indices.end(), model.indices().begin(), model.indices().end());
		materials.insert(materials.end(), model.materials().begin(), model.materials().end());

		for (size_t i = vertex_offset; i != vertices.size(); ++i) {
			vertices[i].materialIndex += material_offset;
		}

		const auto sphere = dynamic_cast<const Sphere*>(model.procedural());
		if (sphere != nullptr) {
			aabbs.push_back(sphere->boundingBox());
			procedurals.emplace_back(sphere->center, sphere->radius);
		} else {
			aabbs.emplace_back();
			procedurals.emplace_back();
		}
	}

	const auto flag = used_for_ray_tracing ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : 0;

	createDeviceBuffer(command_pool,
					   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | flag,
					   vertices,
					   vertex_buffer_,
					   vertex_buffer_memory_);
	createDeviceBuffer(command_pool,
					   VK_BUFFER_USAGE_INDEX_BUFFER_BIT | flag,
					   indices,
					   index_buffer_,
					   index_buffer_memory_);
	createDeviceBuffer(command_pool,
					   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					   materials,
					   material_buffer_,
					   material_buffer_memory_);
	createDeviceBuffer(command_pool,
					   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					   offsets,
					   offset_buffer_,
					   offset_buffer_memory_);

	createDeviceBuffer(command_pool, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, aabbs, aabb_buffer_, aabb_buffer_memory_);
	createDeviceBuffer(command_pool,
					   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					   procedurals,
					   procedural_buffer_,
					   procedural_buffer_memory_);

	// Upload all textures
	texture_images_.reserve(textures_.size());
	texture_image_view_handles_.resize(textures_.size());
	texture_sampler_handles_.resize(textures_.size());

	for (size_t i = 0; i != textures_.size(); ++i) {
		texture_images_.emplace_back(new TextureImage(command_pool, textures_[i]));
		texture_image_view_handles_[i] = texture_images_[i]->imageView().handle();
		texture_sampler_handles_[i] = texture_images_[i]->sampler().handle();
	}
}

Scene::~Scene() {
	texture_sampler_handles_.clear();
	texture_image_view_handles_.clear();
	texture_images_.clear();
	procedural_buffer_.reset();
	procedural_buffer_memory_.reset();
	aabb_buffer_.reset();
	aabb_buffer_memory_.reset();
	offset_buffer_.reset();
	offset_buffer_memory_.reset();
	material_buffer_.reset();
	material_buffer_memory_.reset();
	index_buffer_.reset();
	index_buffer_memory_.reset();
	vertex_buffer_.reset();
	vertex_buffer_memory_.reset();
}

}