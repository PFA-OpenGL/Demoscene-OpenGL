#include <Render/Pipelining/Render/ScreenRender.hh>

namespace AGE
{

	ScreenRender::ScreenRender(glm::uvec2 const &screenSize, std::shared_ptr<PaintingManager> painterManager) :
		ARender(painterManager),
		viewport(screenSize)
	{

	}

	ScreenRender::ScreenRender(ScreenRender &&move) :
		ARender(std::move(move))
	{

	}

	IRender & ScreenRender::render(const DRBCameraDrawableList &infos)
	{
		SCOPE_profile_gpu_i("ScreenRender pass");
		SCOPE_profile_cpu_i("RenderTimer", "ScreenRender pass");

		glViewport(0, 0, viewport.x, viewport.y);
		renderPass(infos);
		if (_nextPass != nullptr)
			_nextPass->render(infos);
		return (*this);
	}

}