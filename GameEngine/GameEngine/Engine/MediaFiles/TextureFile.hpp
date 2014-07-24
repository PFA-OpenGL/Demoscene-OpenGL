#ifndef  __TEXTURE_FILE_HPP__
# define __TEXTURE_FILE_HPP__

#include <memory>
#include <MediaFiles/MediaFile.hpp>
#include <Utils/Containers/Vector.hpp>
#include <glm/glm.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
//#include <cereal/types/memory.hpp>
#include <Utils/GlmSerialization.hpp>
#include <OpenGL/Key.hh>
#include <OpenGL/Texture.hh>

struct TextureFile : public MediaFile<TextureFile>
{
	TextureFile();
	virtual ~TextureFile();
	TextureFile(const TextureFile &o);
	TextureFile &operator=(const TextureFile &o);


	void save(cereal::PortableBinaryOutputArchive &ar) const;
	void load(cereal::PortableBinaryInputArchive &ar);

	gl::Key<gl::Texture> const &getTexture() const;

	AGE::Vector<GLbyte> datas;
	GLint		width, height;
	GLint		components;
	GLenum		format;
	GLenum		wrap;
	GLenum		minFilter;
	GLenum		magFilter;
	gl::Key<gl::Texture> key;

};

#endif   //__TEXTURE_FILE_HPP__
