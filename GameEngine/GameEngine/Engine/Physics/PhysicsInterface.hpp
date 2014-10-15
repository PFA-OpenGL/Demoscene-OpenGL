#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "EngineType.hpp"
#include "../Utils/DependenciesInjector.hpp"

namespace AGE
{
	namespace Physics
	{
		class WorldInterface;

		class PhysicsInterface : public Dependency < PhysicsInterface >
		{
		public:
			// Constructors
			PhysicsInterface(void) = default;

			PhysicsInterface(const PhysicsInterface &) = delete;

			// Assignment Operators
			PhysicsInterface &operator=(const PhysicsInterface &) = delete;

			// Destructor
			virtual ~PhysicsInterface(void) = default;

			// Methods
			bool startup(const std::string &assetDirectory);

			void shutdown(const std::string &assetDirectory);

			WorldInterface *getWorld(void);

			const WorldInterface *getWorld(void) const;

			void destroyWorld(void);

			// Virtual Methods
			virtual EngineType getPluginType(void) const = 0;

		protected:
			// Attributes
			WorldInterface *world = nullptr;

			// Virtual Methods
			virtual bool initialize(void) = 0;

			virtual void finalize(void) = 0;

			virtual WorldInterface *createWorld(void) = 0;
		};
	}
}

#include "PhysicsInterface.inl"