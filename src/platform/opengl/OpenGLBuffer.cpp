#include "pch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

///
/// VertexBuffer
///

lift::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, const uint32_t size) {
	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

lift::OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	glDeleteBuffers(1, &renderer_id_);
}

void lift::OpenGLVertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
}

void lift::OpenGLVertexBuffer::Unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void lift::OpenGLVertexBuffer::SetLayout(const BufferLayout& layout) {
	layout_ = layout;
}

///
/// IndexBuffer
/// 

lift::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, const uint32_t count)
	: count_(count) {
	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_ * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

lift::OpenGLIndexBuffer::~OpenGLIndexBuffer() {
	glDeleteBuffers(1, &renderer_id_);
}

void lift::OpenGLIndexBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
}

void lift::OpenGLIndexBuffer::Unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t lift::OpenGLIndexBuffer::GetCount() const {
	return count_;
}