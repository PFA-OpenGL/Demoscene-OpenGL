#ifndef		MESHRENDERER_HH_
#define		MESHRENDERER_HH_

#include <map>
#include <set>

#include "AComponent.hh"
#include "Utils/SmartPointer.hh"
#include "ResourceManager/SharedMesh.hh"
#include "OpenGL/Shader.hh"

namespace Resources
{
	class Texture;
};

class Material;

namespace Components
{

class MeshRenderer : public AComponent
{
private:
	typedef std::map<unsigned int, std::pair<std::string, SmartPointer<Resources::Texture> > > textureMap;
	typedef textureMap::iterator textureMapIt;

	SmartPointer<Resources::SharedMesh>	_mesh;

	std::string							_shader;
	textureMap                          _textures;

	MeshRenderer						*_next;
	std::set<std::string>                _materials;

	MeshRenderer();
	MeshRenderer(MeshRenderer const &);
	MeshRenderer	&operator=(MeshRenderer const &);
public:
	MeshRenderer(std::string const &name, std::string const &resource);
	virtual ~MeshRenderer(void);

	virtual void	start();
	virtual void	update();
	virtual void	stop();

	bool				setShader(std::string const &name);
	std::string const	&getShader() const;

	void addTexture(const std::string &textureName, unsigned int priority = 0);
	void removeTexture(unsigned int priority);
	void bindTextures(OpenGLTools::Shader *shader) const;
	void unbindTextures() const;
	void addMaterial(SmartPointer<Material> material);
	void removeMaterial(SmartPointer<Material> material);
	void addMaterial(const std::string &material);
	void removeMaterial(const std::string &material);

	void			setNext(MeshRenderer *next);
	MeshRenderer	*getNext() const;

	SmartPointer<Resources::SharedMesh>	const &getMesh() const;
};

}

#endif