#include <Utils/Containers/Vector.hpp>
#include <map>

//CONVERTOR
#include <Convertor/SkeletonLoader.hpp>
#include <Convertor/AnimationsLoader.hpp>
#include <Convertor/MeshLoader.hpp>
#include <Convertor/MaterialConvertor.hpp>
#include <Convertor/ImageLoader.hpp>

//SKINNING
#include <Skinning/Animation.hpp>
#include <Skinning/AnimationChannel.hpp>
#include <Skinning/AnimationInstance.hpp>
#include <Skinning/AnimationKey.hpp>
#include <Skinning/Bone.hpp>
#include <Skinning/Skeleton.hpp>

//CORE
#include <Threads/ThreadManager.hpp>
#include <Core/Engine.hh>
#include <Core/Timer.hh>
#include <Core/AssetsManager.hpp>
#include <Threads/RenderThread.hpp>
#include <Utils/Age_Imgui.hpp>
#include <Core/Tasks/Basics.hpp>
#include <Threads/TaskScheduler.hpp>

//SCENE
#include <Scenes/AssetsEditorScene.hpp>
#include <Scenes/SceneSelectorScene.hpp>

int			main(int ac, char **av)
{
	AGE::InitAGE();
	auto engine = AGE::CreateEngine();

	engine.lock()->launch(std::function<bool()>([&]()
	{
		AGE::GetThreadManager()->setAsWorker(false, false, false);
		engine.lock().get()->setInstance<AGE::Timer>();
		engine.lock().get()->setInstance<AGE::AssetsManager>();

		AGE::GetRenderThread()->getQueue()->emplaceFutureTask<AGE::Tasks::Basic::BoolFunction, bool>([=](){
			AGE::Imgui::getInstance()->init(engine.lock().get());
			return true;
		}).get();

#ifdef RENDERING_ACTIVATED
		if (!loadAssets(engine.lock().get()))
			return false;
#endif
		engine.lock()->addScene(std::make_shared<AGE::AssetsEditorScene>(engine), "Assets Convertor");
		engine.lock()->addScene(std::make_shared<AGE::SceneSelectorScene>(engine), "SceneSelector");

		if (!engine.lock()->initScene("Assets Convertor"))
			return false;
		if (!engine.lock()->initScene(AGE::SceneSelectorScene::Name))
			return false;
		engine.lock()->enableScene("Assets Convertor", 1);
		engine.lock()->enableScene(AGE::SceneSelectorScene::Name, 1000);
		return true;
	}));

	return (0);
}