#include	<Systems/BlitFinalRender.hh>
#include	<Core/AScene.hh>
#include	<Components/CameraComponent.hpp>
#include	<Core/Renderer.hh>

BlitFinalRender::BlitFinalRender(std::weak_ptr<AScene> &&scene) :
					System(std::move(scene)),
					_cameraFilter(std::move(scene))
{
}

BlitFinalRender::~BlitFinalRender()
{
}

bool	BlitFinalRender::initialize()
{
	_cameraFilter.requireComponent<Component::CameraComponent>();
	_scene.lock()->getInstance<Renderer>()->addShader("fboToScreenMultisampled", "../../Shaders/fboToScreen.vp", "../../Shaders/fboToScreenMultisampled.fp");
	_scene.lock()->getInstance<Renderer>()->addShader("fboToScreen", "../../Shaders/fboToScreen.vp", "../../Shaders/fboToScreen.fp");
	_quad.init(_scene);
	return true;
}

void	BlitFinalRender::mainUpdate(double time)
{
	Component::CameraComponent *camera = nullptr;
	auto scene = _scene.lock();
	for (auto c : _cameraFilter.getCollection())
	{
		camera = scene->getComponent<Component::CameraComponent>(c);
		if (camera->blitOnScreen)
		{
			OpenGLTools::Framebuffer	&current = camera->frameBuffer.isMultisampled() ? camera->downSampling : camera->frameBuffer;

			glViewport(camera->viewport.x, camera->viewport.y, camera->viewport.z, camera->viewport.w);

			// Blit final result on back buffer:
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			glDepthFunc(GL_ALWAYS);
			_quad.draw(current[GL_COLOR_ATTACHMENT0]->getId(), current.getSampleNbr(), current.getSize());
		}
	}
}