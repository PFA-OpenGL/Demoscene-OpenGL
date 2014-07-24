#include <OpenGL/ShadingManager.hh>
#include <OpenGL/Shader.hh>
#include <OpenGL/Texture.hh>
#include <OpenGL/RenderPass.hh>
#include <iostream>
#include <string>
#include <Core/Drawable.hh>

namespace gl
{
	ShadingManager::ShadingManager()
	{
	}

	ShadingManager::~ShadingManager()
	{
	}

	Key<Shader> ShadingManager::addShader(std::string const &compute)
	{
		Key<Shader> key;
		Shader shader(compute);

		if (!shader.isValid())
			DEBUG_MESSAGE("Warning", "ShadingManager-addShader(string const &compute)", "compute invalid", Key<Shader>(KEY_DESTROY))
		_shaders[key] = shader;
		return (key);
	}

	Key<Shader> ShadingManager::addShader(std::string const &vertex, std::string const &frag)
	{
		Key<Shader> key;
		Shader shader(vertex, frag);

		if (!shader.isValid())
			DEBUG_MESSAGE("Warning", "ShadingManager-addShader(string const &frag, string const &vertex)", "frag and vertex invalid", Key<Shader>(KEY_DESTROY))
		_shaders[key] = shader;
		return (key);
	}

	Key<Shader> ShadingManager::addShader(std::string const &geo, std::string const &vertex, std::string const &frag)
	{
		Key<Shader> key;
		Shader shader(vertex, frag, geo);

		if (!shader.isValid())
			DEBUG_MESSAGE("Warning", "ShadingManager-addShader(string const &frag, string const &vertex, string const &geo)", "frag, vertex or geo invalid", Key<Shader>(KEY_DESTROY))
		_shaders[key] = shader;
		return (key);
	}

	ShadingManager &ShadingManager::rmShader(Key<Shader> &key)
	{
		if (getShader(key, "rmShader()") == NULL)
			return (*this);
		_shaders.erase(key);
		key.destroy();
		return (*this);
	}

	Key<Shader> ShadingManager::getShader(size_t target) const
	{
		if (target >= _shaders.size())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp-getShader(size_t target)", "the target is out of range", Key<Shader>(KEY_DESTROY));
		auto &element = _shaders.begin();
		for (size_t index = 0; index < target; ++index)
			++element;
		return (element->first);
	}

	ShadingManager &ShadingManager::updateMemoryShader(Key<Shader> const &key)
	{
		Shader *shader;
		if ((shader = getShader(key, "updateMemoryShader")) == NULL)
			return (*this);
		shader->updateMemory();
		return (*this);
	}

	Key<Uniform> ShadingManager::getShaderUniform(Key<Shader> const &key, size_t target)
	{
		Shader const *shader;
		if ((shader = getShader(key, "getShaderUniform()")) == NULL)
			return (Key<Uniform>(KEY_DESTROY));
		return (shader->getUniform(target));
	}

	Key<Uniform> ShadingManager::addShaderUniform(Key<Shader> const &key, std::string const &flag)
	{
		Shader *shader;
		if ((shader = getShader(key, "addShaderUniform(mat4)")) == NULL)
			return (Key<Uniform>(KEY_DESTROY));
		return (shader->addUniform(flag));
	}

	Key<Uniform> ShadingManager::addShaderUniform(Key<Shader> const &key, std::string const &flag, glm::mat4 const &value)
	{
		Shader *shader;
		if ((shader = getShader(key, "addShaderUniform(mat4)")) == NULL)
			return (Key<Uniform>(KEY_DESTROY));
		return (shader->addUniform(flag, value));
	}

	Key<Uniform> ShadingManager::addShaderUniform(Key<Shader> const &key, std::string const &flag, glm::mat3 const &value)
	{
		Shader *shader;
		if ((shader = getShader(key, "addShaderUniform(mat3)")) == NULL)
			return (Key<Uniform>(KEY_DESTROY));
		return (shader->addUniform(flag, value));
	}

	Key<Uniform> ShadingManager::addShaderUniform(Key<Shader> const &key, std::string const &flag, glm::vec4 const &value)
	{
		Shader *shader;
		if ((shader = getShader(key, "addShaderUniform(vec4)")) == NULL)
			return (Key<Uniform>(KEY_DESTROY));
		return (shader->addUniform(flag, value));
	}

	Key<Uniform> ShadingManager::addShaderUniform(Key<Shader> const &key, std::string const &flag, float value)
	{
		Shader *shader;
		if ((shader = getShader(key, "addShaderUniform(float)")) == NULL)
			return (Key<Uniform>(KEY_DESTROY));
		return (shader->addUniform(flag, value));
	}

	ShadingManager &ShadingManager::setShaderUniform(Key<Shader> const &keyShader, Key<Uniform> const &key, glm::mat4 const &mat4)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "setShaderUniform(mat4)")) == NULL)
			return (*this);
		shader->setUniform(key, mat4);
		return (*this);
	}

	ShadingManager &ShadingManager::setShaderUniform(Key<Shader> const &keyShader, Key<Uniform> const &key, glm::vec4 const &vec4)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "setShaderUniform(vec4)")) == NULL)
			return (*this);
		shader->setUniform(key, vec4);
		return (*this);
	}

	ShadingManager &ShadingManager::setShaderUniform(Key<Shader> const &keyShader, Key<Uniform> const &key, float v)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "setShaderUniform(float)")) == NULL)
			return (*this);
		shader->setUniform(key, v);
		return (*this);
	}

	ShadingManager &ShadingManager::setShaderUniform(Key<Shader> const &keyShader, Key<Uniform> const &key, glm::mat3 const &mat3)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "setShaderUniform(mat3)")) == NULL)
			return (*this);
		shader->setUniform(key, mat3);
		return (*this);
	}

	Key<Sampler> ShadingManager::addShaderSampler(Key<Shader> const &keyShader, std::string const &flag)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "addShaderSampler()")) == NULL)
			return (Key<Sampler>(KEY_DESTROY));
		return (shader->addSampler(flag));
	}

	Key<Sampler> ShadingManager::getShaderSampler(Key<Shader> const &keyShader, size_t target)
	{
		Shader const *shader;
		if ((shader = getShader(keyShader, "getShaderSampler()")) == NULL)
			return (Key<Sampler>(KEY_DESTROY));
		return (shader->getSampler(target));
	}

	ShadingManager &ShadingManager::setShaderSampler(Key<Shader> const &keyShader, Key<Sampler> const &keySampler, Key<Texture> const &keyTexture)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "setShaderSampler()")) == NULL)
			return (*this);
		Texture *texture;
		if ((texture = getTexture(keyTexture, "setShaderSampler()", GL_NONE)) == NULL)
			return (*this);
		shader->setSampler(keySampler, *texture);
		return (*this);
	}

	Key<UniformBlock> ShadingManager::addUniformBlock(size_t nbrElement, size_t *sizeElement)
	{
		Key<UniformBlock> key;

		_uniformBlock[key] = UniformBlock(nbrElement, sizeElement);
		return (key);
	}

	ShadingManager &ShadingManager::rmUniformBlock(Key<UniformBlock> &key)
	{
		if (getUniformBlock(key, "rmUniformBlock") == NULL)
			return (*this);
		_uniformBlock.erase(key);
		key.destroy();
		return (*this);
	}

	Key<UniformBlock> ShadingManager::getUniformBlock(size_t target) const
	{
		if (target >= _uniformBlock.size())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp-getUniformBlock(size_t target)", "the target is out of range", Key<UniformBlock>(KEY_DESTROY));
		auto &element = _uniformBlock.begin();
		for (size_t index = 0; index < target; ++index)
			++element;
		return (element->first);
	}

	Key<InterfaceBlock> ShadingManager::addShaderInterfaceBlock(Key<Shader> const &keyShader, std::string const &flag, Key<UniformBlock> const &keyUniformBlock)
	{
		Shader *shader;
		if ((shader = getShader(keyShader, "addShaderInterfaceBlock()")) == NULL)
			return (Key<InterfaceBlock>(KEY_DESTROY));
		UniformBlock *uniformBlock;
		if ((uniformBlock = getUniformBlock(keyUniformBlock, "addShaderInterfaceBlock()")) == NULL)
			return (Key<InterfaceBlock>(KEY_DESTROY));
		return (shader->addInterfaceBlock(flag, *uniformBlock));
	}

	Key<InterfaceBlock> ShadingManager::getShaderInterfaceBlock(Key<Shader> const &keyShader, size_t target)
	{
		Shader const *shader;
		if ((shader = getShader(keyShader, "getShaderInterfaceBlock()")) == NULL)
			return (Key<InterfaceBlock>(KEY_DESTROY));
		return (shader->getInterfaceBlock(target));
	}

	Key<Texture> ShadingManager::addTexture2D(GLenum internalFormat, GLsizei width, GLsizei height, bool mipmapping)
	{
		Key<Texture> key;

		_textures[key] = new Texture2D(internalFormat, width, height, mipmapping);
		return (key);
	}

	Key<Texture> ShadingManager::addTextureMultiSample(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSampleLocation)
	{
		Key<Texture> key;

		_textures[key] = new TextureMultiSample(samples, internalFormat, width, height, fixedSampleLocation);
		return (key);
	}

	ShadingManager &ShadingManager::rmTexture(Key<Texture> &key)
	{
		Texture *texture;
		if ((texture = getTexture(key, "rmTexture()", GL_NONE)) == NULL)
			return (*this);
		delete texture;
		_textures.erase(key);
		key.destroy();
		return (*this);
	}

	Key<Texture> ShadingManager::getTexture(size_t target) const
	{
		if (target >= _textures.size())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp-getTexture(size_t target)", "the target is out of range", Key<Texture>(KEY_DESTROY));
		auto &element = _textures.begin();
		for (size_t index = 0; index < target; ++index)
			++element;
		return (element->first);
	}

	GLenum ShadingManager::getTypeTexture(Key<Texture> const &key)
	{
		Texture const *texture;
		if ((texture = getTexture(key, "getTypeTexture()", GL_NONE)) == NULL)
			return (GL_NONE);
		return (texture->getType());
	}

	Shader *ShadingManager::getShader(Key<Shader> const &key, std::string const &in)
	{
		if (!key)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "key destroy", NULL);
		if (_shaders.size() == 0)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "no shader present in pool", NULL);
		if (key == _optimizeShaderSearch.first)
			return (_optimizeShaderSearch.second);
		auto &shader = _shaders.find(key);
		if (shader == _shaders.end())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "shader not find", NULL);
		_optimizeShaderSearch.first = key;
		_optimizeShaderSearch.second = &shader->second;
		return (&shader->second);
	}

	Texture *ShadingManager::getTexture(Key<Texture> const &key, std::string const &in, GLenum type)
	{
		if (!key)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "key destroy", NULL);
		if (_textures.size() == 0)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "no texture present in pool", NULL);
		if (key == _optimizeTextureSearch.first)
			return (_optimizeTextureSearch.second);
		auto &texture = _textures.find(key);
		if (texture == _textures.end())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "texture not find", NULL);
		if (texture->second->getType() != type && type != GL_NONE)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "error cast on texture", NULL);
		_optimizeTextureSearch.first = key;
		_optimizeTextureSearch.second = texture->second;
		return (texture->second);
	}

	UniformBlock *ShadingManager::getUniformBlock(Key<UniformBlock> const &key, std::string const &in)
	{
		if (!key)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "key destroy", NULL);
		if (_uniformBlock.size() == 0)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "no uniformBlock present in pool", NULL);
		if (key == _optimizeUniformBlockSearch.first)
			return (_optimizeUniformBlockSearch.second);
		auto &uniformBlock = _uniformBlock.find(key);
		if (uniformBlock == _uniformBlock.end())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "uniformBlock not find", NULL);
		_optimizeUniformBlockSearch.first = key;
		_optimizeUniformBlockSearch.second = &uniformBlock->second;
		return (&uniformBlock->second);
	}

	RenderPass *ShadingManager::getRenderPass(Key<RenderPass> const &key, std::string const &in)
	{
		if (!key)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "key destroy", NULL);
		if (_renderPass.size() == 0)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "no uniformBlock present in pool", NULL);
		if (key == _optimizeRenderPassSearch.first)
			return (_optimizeRenderPassSearch.second);
		auto &renderPass = _renderPass.find(key);
		if (renderPass == _renderPass.end())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "uniformBlock not find", NULL);
		_optimizeRenderPassSearch.first = key;
		_optimizeRenderPassSearch.second = &renderPass->second;
		return (&renderPass->second);
	}

	Material *ShadingManager::getMaterial(Key<Material> const &key, std::string const &in)
	{
		if (!key)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "key destroy", NULL);
		if (_materials.size() == 0)
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "no uniformBlock present in pool", NULL);
		if (key == _optimizeMaterialSearch.first)
			return (_optimizeMaterialSearch.second);
		auto &material = _materials.find(key);
		if (material == _materials.end())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - " + in, "uniformBlock not find", NULL);
		_optimizeMaterialSearch.first = key;
		_optimizeMaterialSearch.second = &material->second;
		return (&material->second);
	}

	ShadingManager &ShadingManager::wrapTexture2D(Key<Texture> const &key, GLint param)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "wrapTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->wrap(param);
		return (*this);
	}

	ShadingManager &ShadingManager::filterTexture2D(Key<Texture> const &key, GLint param)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "filterTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->filter(param);
		return (*this);
	}
	
	ShadingManager &ShadingManager::filterTexture2D(Key<Texture> const &key, GLint minFilter, GLint magFilter)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "filterTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->filter(minFilter, magFilter);
		return (*this);
	}

	ShadingManager &ShadingManager::storageTexture2D(Key<Texture> const &key, GLint param)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "storageTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->storage(param);
		return (*this);
	}

	ShadingManager &ShadingManager::storageTexture2D(Key<Texture> const &key, GLint pack, GLint unpack)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "storageTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->storage(pack, unpack);
		return (*this);
	}

	ShadingManager &ShadingManager::setOptionTransferTexture2D(Key<Texture> const &key, GLint level, GLenum format, GLenum type)
	{
		Texture2D *texture;
		if ((texture = (Texture2D *)getTexture(key, "setOptionTransferTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->setOptionTransfer(level, format, type);
		return (*this);
	}

	ShadingManager &ShadingManager::generateMipMapTexture2D(Key<Texture> const &key)
	{
		Texture2D *texture;
		if ((texture = (Texture2D *)getTexture(key, "generateMipMapTexture2D", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->generateMipMap();
		return (*this);
	}

	ShadingManager &ShadingManager::writeTexture(Key<Texture> const &key, void *write)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "writeTexture", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->write(write);
		return (*this);
	}

	ShadingManager &ShadingManager::readTexture(Key<Texture> const &key, void *read)
	{
		Texture2D const *texture;
		if ((texture = (Texture2D *)getTexture(key, "readTexture", GL_TEXTURE_2D)) == NULL)
			return (*this);
		texture->read(read);
		return (*this);
	}

	ShadingManager &ShadingManager::bindTexture(Key<Texture> const &key)
	{
		Texture const *texture;
		if ((texture = getTexture(key, "bindTexture", GL_NONE)) == NULL)
			return (*this);
		texture->bind();
		return (*this);
	}

	ShadingManager &ShadingManager::unbindTexture(Key<Texture> const &key) 
	{
		Texture const *texture;
		if ((texture = getTexture(key, "unbindTexture", GL_NONE)) == NULL)
			return (*this);
		texture->unbind();
		return (*this);
	}

	std::uint8_t ShadingManager::getMaxLevelMipMapTexture2D(Key<Texture> const &key)
	{
		Texture2D *texture;
		if ((texture = (Texture2D *)getTexture(key, "getMaxLevelMipMapTexture2D", GL_TEXTURE_2D)) == NULL)
			return (-1);
		return (texture->getMaxLevelMipMap());
	}

	Key<RenderPass> ShadingManager::addRenderPass(Key<Shader> const &keyShader)
	{
		Key<RenderPass> key;
		Shader *shader;

		if ((shader = getShader(keyShader, "addRenderPass")) == NULL)
			return (Key<RenderPass>(KEY_DESTROY));
		_renderPass[key] = RenderPass(*shader, geometryManager);
		return (key);
	}

	ShadingManager &ShadingManager::rmRenderPass(Key<RenderPass> &key)
	{
		if (getRenderPass(key, "rmRenderPass") == NULL)
			return (*this);
		_renderPass.erase(key);
		key.destroy();
		return (*this);
	}

	Key<RenderPass> ShadingManager::getRenderPass(size_t target) const
	{
		if (target >= _renderPass.size())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp - getRenderPass(size_t target)", "the target is out of range", Key<RenderPass>(KEY_DESTROY));
		auto &element = _renderPass.begin();
		for (size_t index = 0; index < target; ++index)
			++element;
		return (element->first);
	}

	ShadingManager &ShadingManager::pushClearTaskRenderPass(Key<RenderPass> const &key, bool color, bool depth, bool stencil)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setClearOptionRenderPass")) == NULL)
			return (*this);
		renderPass->pushClearTask(color, depth, stencil);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetClearValueTaskRenderPass(Key<RenderPass> const &key, glm::vec4 const &color, float depth, uint8_t stencil)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setClearValueRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetClearValueTask(color, depth, stencil);
		return (*this);
	}
	
	ShadingManager &ShadingManager::pushSetColorMaskTaskRenderPass(Key<RenderPass> const &key, glm::bvec4 const &color, GLuint index)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setColorMaskRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetColorMaskTask(index, color);
		return (*this);
	}
	
	ShadingManager &ShadingManager::pushSetStencilMaskTaskRenderPass(Key<RenderPass> const &key, uint8_t front, uint8_t back)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setDepthStencilMaskRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilMaskTask(front, back);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetDepthMaskTaskRenderPass(Key<RenderPass> const &key, bool depth)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setDepthStencilMaskRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetDepthMaskTask(depth);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetTestTaskRenderPass(Key<RenderPass> const &key, bool scissor, bool stencil, bool depth)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setDepthStencilMaskRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetTestTask(scissor, stencil, depth);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetScissorTaskRenderPass(Key<RenderPass> const &key, glm::ivec4 const &area)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setDepthStencilMaskRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetScissorTask(area);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetStencilFunctionFrontFaceTaskRenderPass(Key<RenderPass> const &key, GLenum func, int ref, uint8_t mask)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setStencilFunctionFrontFaceRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilFunctionFrontFaceTask(func, ref, mask);
		return (*this);
	}
	ShadingManager &ShadingManager::pushSetStencilOperationFrontFaceTaskRenderPass(Key<RenderPass> const &key, GLenum opStencilFail, GLenum opDepthFail, GLenum opDepthPass)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setStencilOperationFrontFaceRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilOperationFrontFaceTask(opStencilFail, opDepthFail, opDepthPass);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetStencilFunctionBackFaceTaskRenderPass(Key<RenderPass> const &key, GLenum func, int ref, uint8_t mask)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setStencilFunctionBackFaceRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilFunctionBackFaceTask(func, ref, mask);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetStencilOperationBackFaceTaskRenderPass(Key<RenderPass> const &key, GLenum opStencilFail, GLenum opDepthFail, GLenum opDepthPass)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setStencilOperationBackFaceRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilOperationBackFaceTask(opStencilFail, opDepthFail, opDepthPass);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetStencilFunctionTaskRenderPass(Key<RenderPass> const &key, GLenum func, int ref, uint8_t mask)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setStencilFunctionRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilFunctionTask(func, ref, mask);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetStencilOperationTaskRenderPass(Key<RenderPass> const &key, GLenum opStencilFail, GLenum opDepthFail, GLenum opDepthPass)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setStencilOperationRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetStencilOperationTask(opStencilFail, opDepthFail, opDepthPass);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetBlendEquationTaskRenderPass(Key<RenderPass> const &key, GLenum colorMode, GLenum alphaMode)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setBlendEquationRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetBlendEquationTask(colorMode, alphaMode);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetBlendEquationTaskRenderPass(Key<RenderPass> const &key, GLenum mode)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setBlendEquationRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetBlendEquationTask(mode);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetBlendFuncTaskRenderPass(Key<RenderPass> const &key, GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setBlendFuncRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetBlendFuncTask(srcRGB, destRGB, srcAlpha, destAlpha);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetBlendFuncTaskRenderPass(Key<RenderPass> const &key, GLenum src, GLenum dest)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setBlendFuncRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetBlendFuncTask(src, dest);
		return (*this);
	}

	ShadingManager &ShadingManager::pushSetBlendConstantTaskRenderPass(Key<RenderPass> const &key, glm::vec4 const &blendColor)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "setBlendConstantRenderPass")) == NULL)
			return (*this);
		renderPass->pushSetBlendConstantTask(blendColor);
		return (*this);
	}

	ShadingManager &ShadingManager::popTaskRenderPass(Key<RenderPass> const &key)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "popTaskRenderPass")) == NULL)
			return (*this);
		renderPass->popTask();
		return (*this);
	}

	ShadingManager &ShadingManager::draw(GLenum mode, Key<RenderPass> const &key, AGE::Drawable const *objectRender, size_t nbrObjectRender)
	{
		RenderPass *renderPass;
		if ((renderPass = getRenderPass(key, "popTaskRenderPass")) == NULL)
			return (*this);
		renderPass->draw(mode, objectRender, nbrObjectRender);
		return (*this);
	}

	Key<Material> ShadingManager::addMaterial()
	{
		Key<Material> key;

		_materials[key];
		return (key);
	}

	ShadingManager &ShadingManager::rmMaterial(Key<Material> &key)
	{
		Material *material;
		if ((material = getMaterial(key, "rmMaterial()")) == NULL)
			return (*this);
		_materials.erase(key);
		key.destroy();
		return (*this);
	}

	Key<Material> ShadingManager::getMaterial(size_t target) const
	{
		if (target >= _materials.size())
			DEBUG_MESSAGE("Warning", "ShadingManager.cpp-getMaterial(size_t target)", "the target is out of range", Key<Material>(KEY_DESTROY));
		auto &element = _materials.begin();
		for (size_t index = 0; index < target; ++index)
			++element;
		return (element->first);
	}

}