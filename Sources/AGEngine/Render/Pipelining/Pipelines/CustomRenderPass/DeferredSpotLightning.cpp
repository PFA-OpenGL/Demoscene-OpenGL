#include <Render/Pipelining/Pipelines/CustomRenderPass/DeferredSpotLightning.hh>

#include <Render/Textures/Texture2D.hh>
#include <Render/OpenGLTask/OpenGLState.hh>
#include <Render/GeometryManagement/Painting/Painter.hh>
#include <Render/ProgramResources/Types/Uniform/Mat4.hh>
#include <Render/ProgramResources/Types/Uniform/Sampler/Sampler2D.hh>
#include <Render/ProgramResources/Types/Uniform/Vec3.hh>
#include <Render/ProgramResources/Types/Uniform/Vec1.hh>
#include <Threads/RenderThread.hpp>
#include <Threads/ThreadManager.hpp>
#include <Core/ConfigurationManager.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Core/Engine.hh>

#include "Graphic\DRBCameraDrawableList.hpp"
#include "Graphic/DRBSpotLightData.hpp"

#define DEFERRED_SHADING_SPOT_LIGHT_VERTEX "deferred_shading/deferred_shading_spot_light.vp"
#define DEFERRED_SHADING_SPOT_LIGHT_FRAG "deferred_shading/deferred_shading_spot_light.fp"

namespace AGE
{
	enum Programs
	{
		PROGRAM_LIGHTNING,
		PROGRAM_NBR
	};

	DeferredSpotLightning::DeferredSpotLightning(glm::uvec2 const &screenSize, std::shared_ptr<PaintingManager> painterManager,
		std::shared_ptr<Texture2D> normal,
		std::shared_ptr<Texture2D> depth,
		std::shared_ptr<Texture2D> specular,
		std::shared_ptr<Texture2D> lightAccumulation,
		std::shared_ptr<Texture2D> shinyAccumulation) :
		FrameBufferRender(screenSize.x, screenSize.y, painterManager)
	{
		push_storage_output(GL_COLOR_ATTACHMENT0, lightAccumulation);
		push_storage_output(GL_COLOR_ATTACHMENT1, shinyAccumulation);
		push_storage_output(GL_DEPTH_STENCIL_ATTACHMENT, depth);

		_normalInput = normal;
		_depthInput = depth;
		_specularInput = specular;

		_programs.resize(PROGRAM_NBR);
		auto confManager = GetEngine()->getInstance<ConfigurationManager>();
		auto shaderPath = confManager->getConfiguration<std::string>("ShadersPath");
		// you have to set shader directory in configuration path
		AGE_ASSERT(shaderPath != nullptr);
		auto vertexShaderPath = shaderPath->getValue() + DEFERRED_SHADING_SPOT_LIGHT_VERTEX;
		auto fragmentShaderPath = shaderPath->getValue() + DEFERRED_SHADING_SPOT_LIGHT_FRAG;

		_programs[PROGRAM_LIGHTNING] = std::make_shared<Program>(Program(std::string("program_spot_light"),
		{
			std::make_shared<UnitProg>(vertexShaderPath, GL_VERTEX_SHADER),
			std::make_shared<UnitProg>(fragmentShaderPath, GL_FRAGMENT_SHADER)
		}));
		GetRenderThread()->getQuadGeometry(_quad, _quadPainter);

	}

	void DeferredSpotLightning::renderPass(const DRBCameraDrawableList &infos)
	{
		//@PROUT
		SCOPE_profile_gpu_i("DeferredSpotLightning render pass");
		SCOPE_profile_cpu_i("RenderTimer", "DeferredSpotLightning render pass");

		glm::vec3 cameraPosition = -glm::transpose(glm::mat3(infos.cameraInfos.view)) * glm::vec3(infos.cameraInfos.view[3]);

		{
			SCOPE_profile_gpu_i("Overhead pipeline");
			SCOPE_profile_cpu_i("RenderTimer", "Overhead pipeline");
			_programs[PROGRAM_LIGHTNING]->use();
			_programs[PROGRAM_LIGHTNING]->get_resource<Mat4>("projection_matrix").set(infos.cameraInfos.data.projection);
			_programs[PROGRAM_LIGHTNING]->get_resource<Mat4>("view_matrix").set(infos.cameraInfos.view);
			_programs[PROGRAM_LIGHTNING]->get_resource<Sampler2D>("normal_buffer").set(_normalInput);
			_programs[PROGRAM_LIGHTNING]->get_resource<Sampler2D>("specular_buffer").set(_specularInput);
			_programs[PROGRAM_LIGHTNING]->get_resource<Sampler2D>("depth_buffer").set(_depthInput);
			_programs[PROGRAM_LIGHTNING]->get_resource<Vec3>("eye_pos").set(cameraPosition);
		}

		OpenGLState::glDisable(GL_CULL_FACE);
		OpenGLState::glDisable(GL_DEPTH_TEST);
		OpenGLState::glEnable(GL_STENCIL_TEST);
		OpenGLState::glStencilFunc(GL_EQUAL, 0, 0xFF);
		OpenGLState::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// And we set the blend mode to additive
		OpenGLState::glEnable(GL_BLEND);
		OpenGLState::glBlendFunc(GL_ONE, GL_ONE);

		auto painter = _painterManager->get_painter(_quadPainter);
		//@CESAR_OPTIMIZE
		for (auto &spot : infos.spotLights)
		{
			auto &spotlight = (std::shared_ptr<DRBSpotLightData>&)(spot->spotLight);

			// @PROUT todo to add in properties
			_programs[PROGRAM_LIGHTNING]->get_resource<Sampler2D>("shadow_map").set(spotlight->shadowMap);

			painter->uniqueDrawBegin(_programs[PROGRAM_LIGHTNING]);
			painter->uniqueDraw(GL_TRIANGLES, _programs[PROGRAM_LIGHTNING], spotlight->globalProperties, _quad);
			painter->uniqueDrawEnd();
		}
	}
}