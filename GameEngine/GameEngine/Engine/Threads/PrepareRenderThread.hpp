#pragma once

#include "Thread.hpp"
#include "QueuePusher.hpp"
#include "QueueOwner.hpp"
#include <Threads/RenderScene.hpp>
#include <Components/DirectionalLightComponent.hh>
#include <Components/SpotLight.hh>
#include <Components/Light.hh>
#include <Components/CameraComponent.hpp>
#include <vector>
#include <memory>
#include <thread>

namespace AGE
{
	class PrepareRenderThread : public Thread, public QueuePusher, public QueueOwner
	{
	public:
		virtual bool init();
		virtual bool release();
		bool update();
		virtual bool launch();
		virtual bool stop();

		void setCameraInfos(CameraData const &data, const PrepareKey &key);
		PrepareKey addCamera();
		void updateGeometry(
			const PrepareKey &key
			, const std::vector<SubMeshInstance> &meshs
			, const std::vector<MaterialInstance> &materials);
		void updateRenderMode(
			const PrepareKey &key
			, const RenderModeSet &renderModes);
		PrepareKey addMesh();
		PrepareKey addPointLight();
		PrepareKey addSpotLight();
		PrepareKey addDirectionalLight();
		void setPointLight(PointLightData const &data, const PrepareKey &key);
		void setDirectionalLight(DirectionalLightData const &data, PrepareKey const &key);
		void setSpotLight(SpotLightData const &data, const PrepareKey &key);
	private:
		PrepareRenderThread();
		virtual ~PrepareRenderThread();
		PrepareRenderThread(const PrepareRenderThread &) = delete;
		PrepareRenderThread(PrepareRenderThread &&) = delete;
		PrepareRenderThread &operator=(const PrepareRenderThread &) = delete;
		PrepareRenderThread &operator=(PrepareRenderThread &&) = delete;

		void _createRenderScene(AScene *scene);
		void _destroyRenderScene(AGE::RenderScene *scene);
		RenderScene *_getRenderScene(const AScene *scene);

		AGE::Vector < std::unique_ptr<AGE::RenderScene> > _scenes;
		AGE::RenderScene *_activeScene;

		std::thread _threadHandle;
		std::atomic_bool _insideRun;
		bool _run;

		friend class ThreadManager;
	};
}