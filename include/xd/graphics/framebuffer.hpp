#ifndef H_XD_GRAPHICS_FRAMEBUFFER
#define H_XD_GRAPHICS_FRAMEBUFFER

#include <xd/vendor/glew/glew.h>
#include <xd/ref_counted.hpp>
#include <xd/asset_serializer.hpp>
#include <xd/graphics/texture.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <tuple>
#include <string>

namespace xd
{
	class XD_API framebuffer : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<framebuffer> ptr;

		framebuffer();
		virtual ~framebuffer();

		void bind() const;
		void unbind() const;

		void attach_color_texture(xd::texture::ptr texture, int slot) const;
		void attach_depth_texture(xd::texture::ptr texture) const;

		static bool extension_supported() {
			return GLEW_EXT_framebuffer_object;
		}

		std::tuple<bool, std::string> check_complete() const;

	private:
		GLuint m_buffer_id;

		void attach_texture(xd::texture::ptr texture, int slot, bool is_depth) const;
	};

}

#endif
