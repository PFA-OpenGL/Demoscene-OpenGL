//
// Shader.cpp for  in /home/massora/GIT/amd_project/render/ShaderTool
// 
// Made by dorian pinaud
// Login   <pinaud_d@epitech.net>
// 
// Started on  Mon Aug  5 00:37:46 2013 dorian pinaud
// Last update Mon Aug 19 00:15:12 2013 dorian pinaud
//

#include "Utils/OpenGL.hh"

#include "Shader.hh"
#include <assert.h>

namespace OpenGLTools
{

Shader::Shader(void) :
	_progId(0),
	_vertexId(0),
	_fragId(0),
	_geometryId(0),
	_targets(nullptr),
	_textureNumber(0)
{
}

Shader::~Shader(void)
{
	glDetachShader(_progId, _vertexId);
	glDetachShader(_progId, _fragId);
	if (_geometryId != 0)
		glDetachShader(_progId, _geometryId);
	glDeleteProgram(_progId);
	if (_targets)
		delete _targets;
}

bool Shader::init(std::string const &vertex, std::string const &fragment, std::string const &geometry)
{
  if ((_vertexId = addShader(vertex, GL_VERTEX_SHADER)) == 0)
    {
      std::cerr << "Error: vertex shader invalid" << std::endl;
      return (false);
    }
  if ((_fragId = addShader(fragment, GL_FRAGMENT_SHADER)) == 0)
    {
      std::cerr << "Error: fragment shader invalid" << std::endl;
      return (false);
    }
  if (geometry.empty() == false &&
	  (_geometryId = addShader(geometry, GL_GEOMETRY_SHADER)) == 0)
    {
      std::cerr << "Error: geometry shader invalid" << std::endl;
      return (false);
    }
  _progId = glCreateProgram();
  glAttachShader(_progId, _vertexId);
  glAttachShader(_progId, _fragId);
  if (_geometryId != 0)
	  glAttachShader(_progId, _geometryId);
  linkProgram(_progId);

  return (true);
}

void Shader::use(void)
{
  glUseProgram(_progId);
}

/// link the program with the shaders
GLuint Shader::addShader(std::string const &path, GLenum type)
{
  GLuint shaderId;
  std::ifstream file(path.c_str(), std::ios_base::binary);
  GLchar *content;
  GLint fileSize;

  if (file.fail())
    return (0);
  file.seekg(0, file.end);
  fileSize = static_cast<GLint>(file.tellg()) + 1;
  file.seekg(0, file.beg);
  content = new GLchar[fileSize];
  file.read(content, fileSize - 1);
  content[fileSize - 1] = 0;
  shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1,
                 const_cast<const GLchar**>(&content),
                 const_cast<const GLint*>(&fileSize));
  compileShader(shaderId, path);
  delete [] content;
  return (shaderId);
}

void Shader::linkProgram(GLuint progId) const
{
  GLint         linkRet = 0;
  GLsizei       msgLenght;
  GLchar        *errorMsg;

  glLinkProgram(progId);
  glGetProgramiv(progId, GL_LINK_STATUS, &linkRet);
  if (linkRet == GL_FALSE)
    {
      glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &msgLenght);
      errorMsg = new GLchar[msgLenght];
      glGetProgramInfoLog(progId, msgLenght,
			  &msgLenght, errorMsg);
      std::cerr << "Link error on program : " << std::endl;
	  std::cerr << std::string(errorMsg).data() << std::endl;
      delete [] errorMsg;
    }
}

void Shader::compileShader(GLuint shaderId, std::string const &file) const
{
  GLint         compileRet = 0;
  GLsizei       msgLenght;
  GLchar        *errorMsg;

  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileRet);
  if (compileRet == GL_FALSE)
    {
      glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &msgLenght);
      errorMsg = new GLchar[msgLenght];
      glGetShaderInfoLog(shaderId, msgLenght,
			 &msgLenght, errorMsg);
      std::cerr << "Compile error on " << file.data() << ": " << std::endl;
      std::cerr << errorMsg << std::endl;
      delete [] errorMsg;
    } 
}

bool	Shader::bindUniformBlock(std::string const &blockName, UniformBuffer const &buffer)
{
	GLuint	blockId;

	assert(_progId != 0 && "Must initialize the shader...");
	blockId = glGetUniformBlockIndex(_progId, blockName.c_str());
	glUniformBlockBinding(_progId, blockId, buffer.getBindingPoint());
	return (true);
}

// function outdated -> to erase if team is agree
Shader	&Shader::bindActiveTexture(std::string const &uniformName, GLuint activeTexture)
{
	use();
	GLuint	location = glGetUniformLocation(_progId, uniformName.c_str());

	glUniform1i(location, activeTexture);
	if (glGetError() != GL_NO_ERROR)
		std::cerr << "Bind active texture failed for uniform " << uniformName << "." << std::endl;
	return (*this);
}

GLuint	Shader::getId() const
{
	return (_progId);
}

GLenum  *Shader::getTargets() const
{
	return _targets;
}

unsigned int Shader::getTargetsNumber() const
{
	return _targetsList.size();
}

bool Shader::build()
{
	use();

	// if there is no targets defined
	if (_targetsList.size() == 0)
	{
		std::cout << "No targets defined for shader" << std::endl;
		_targetsList.insert(GL_COLOR_ATTACHMENT0);
	}

	if (_targets)
		delete _targets;
	_targets = new GLenum[_targetsList.size()];
	if (!_targets)
		return false;

	unsigned int i = 0;
	for (auto &e : _targetsList)
	{
		_targets[i] = e;
		++i;
	}

	unsigned int destIndex = 0;
	for (auto &e : _layersList)
	{
		GLuint	location = glGetUniformLocation(_progId, std::string("layer" + std::to_string(destIndex)).c_str());

		glUniform1i(location, destIndex);
		if (glGetError() != GL_NO_ERROR)
			std::cerr << "Bind active texture failed for uniform <layer" << e << ">." << std::endl;
		++destIndex;
	}

	for (unsigned int i = 0; i < _textureNumber; ++i)
	{
		GLuint	location = glGetUniformLocation(_progId, std::string("fTexture" + std::to_string(i)).c_str());

		glUniform1i(location, i);
		if (glGetError() != GL_NO_ERROR)
			std::cerr << "Bind active texture failed for uniform <fTexture" << i << ">." << std::endl;
		++destIndex;
	}

	return true;
}

	Shader &Shader::addTarget(GLenum target)
	{
		_targetsList.insert(target);
		return *this;
	}

	Shader &Shader::removeTarget(GLenum target)
	{
		_targetsList.erase(target);
		return *this;
	}

	void Shader::clearTargets()
	{
		_targetsList.clear();
		if (_targets)
			delete _targets;
	}

	Shader &Shader::addLayer(GLenum layer)
	{
		_layersList.insert(layer);
		return *this;
	}

	Shader &Shader::removeLayer(GLenum layer)
	{
		_layersList.erase(layer);
		return *this;
	}

	void Shader::clearLayers()
	{
		_layersList.clear();
	}

}