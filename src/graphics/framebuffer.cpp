#include <stdexcept>
#include <xd/graphics/framebuffer.hpp>


xd::framebuffer::framebuffer()
{
	if (extension_supported())
		glGenFramebuffersEXT(1, &m_buffer_id);
}

xd::framebuffer::~framebuffer()
{
	if (extension_supported())
		glDeleteFramebuffersEXT(1, &m_buffer_id);
}

void xd::framebuffer::bind() const
{
	if (extension_supported())
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_buffer_id);
}

void xd::framebuffer::unbind() const
{
	if (extension_supported())
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void xd::framebuffer::attach_color_texture(xd::texture::ptr texture, int slot) const
{
	attach_texture(texture, slot, false);
}

void xd::framebuffer::attach_depth_texture(xd::texture::ptr texture) const
{
	attach_texture(texture, 0, true);
}

void xd::framebuffer::attach_texture(xd::texture::ptr texture, int slot, bool is_depth) const
{
	if (!extension_supported())
		return;
	GLenum attachment;
	if (is_depth)
		attachment = GL_DEPTH_ATTACHMENT_EXT;
	else
		attachment = GL_COLOR_ATTACHMENT0_EXT + slot;

	bind();

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment,
		GL_TEXTURE_2D, texture->texture_id(), 0);

	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		throw std::exception("Could not attach texture to framebuffer object");
	}

	GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0_EXT + slot };
	glDrawBuffers(1, draw_buffers);
}


std::tuple<bool, std::string> xd::framebuffer::check_complete() const
{
	auto result = std::make_tuple(true, std::string(""));
	if (!extension_supported())
		return result;
	GLenum status = glGetError();
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT && status != GL_NO_ERROR) {
		std::get<0>(result) = false;
		std::string error = "Unknown";
		switch (status) {
		case GL_INVALID_OPERATION:
			error = "Operation is invalid";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			error = "Attachment is incomplete";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			error = "An attachment is missing";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			error = "Framebuffer format is not supported";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			error = "Attachments have different formats";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			error = "Attachments have different dimensions";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			error = "Read buffer is missing";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			error = "Draw buffer is missing";
			break;
		}
		std::get<1>(result) = error;
	}
	return result;
}

