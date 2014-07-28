#pragma once

#include <string>
#include <glm/glm.hpp>
#include <Utils/GlmSerialization.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <OpenGL/Material.hh>

namespace AGE
{
	struct MaterialData
	{
		glm::vec4 diffuse;
		glm::vec4 ambient;
		glm::vec4 emissive;
		glm::vec4 reflective;
		glm::vec4 specular;

		std::string diffuseTexPath;
		std::string ambientTexPath;
		std::string emissiveTexPath;
		std::string reflectiveTexPath;
		std::string specularTexPath;
		std::string normalTexPath;

		template <class Archive>
		void serialize(Archive &ar)
		{
			ar(diffuse, ambient, emissive, reflective, specular, diffuseTexPath, ambientTexPath, emissiveTexPath, reflectiveTexPath, specularTexPath, normalTexPath);
		}
	};

	struct MaterialDataSet
	{
		AGE::Vector<MaterialData> collection;

		template <class Archive>
		void serialize(Archive &ar)
		{
			ar(collection);
		}
	};

	typedef gl::Key<gl::Material> MaterialInstance;

	struct MaterialSetInstance
	{
		AGE::Vector<MaterialInstance> datas;
	};
}