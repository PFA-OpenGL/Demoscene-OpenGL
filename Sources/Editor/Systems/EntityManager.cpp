#include "EntityManager.hpp"
#include <imgui/imgui.h>
#include <Components/EntityRepresentation.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Components/ComponentRegistrationManager.hpp>
#include <ComponentsCore/MeshRenderer.hh>
#include <AssetManagement/AssetManager.hh>
#include <EditorConfiguration.hpp>
#include <ComponentsCore/CameraComponent.hpp>
#include <ComponentsCore/FreeFlyComponent.hh>
#include <Entity/Archetype.hpp>
#include <Managers/ArchetypesEditorManager.hpp>
#include <EntityHelpers/EntityImgui.hpp>
#include <Entities/EntityReadablePacker.hpp>
#include <Entities/ReadableEntityPack.hpp>
#include <Entity/EntityBinaryPacker.hpp>
#include <Entity/BinaryEntityPack.hpp>
#include <Core/Inputs/Input.hh>
#include <Components/ArchetypeComponent.hpp>

namespace AGE
{
	namespace WE
	{
			EntityManager::EntityManager(AScene *scene)
				: System(std::move(scene))
				, _filter(std::move(scene))
				, _meshRenderers(std::move(scene))
				, _selectedEntity(nullptr)
				, _selectedEntityIndex(0)
				, _graphNodeDisplay(false)
				, _selectParent(false)
				, _lastFrameSelectedEntity(nullptr)
			{
				//
				auto name = "\0";
				strcpy_s(_sceneName, name);
				_meshRenderers.requireComponent<MeshRenderer>();
				_displayWindow = true;
				generateBasicEntities();

				scene->getInstance<AGE::AssetsManager>()->pushNewCallback("Gizmo/gizmo.sage", scene,
					std::function<void()>([=](){
					_gizmoMesh = scene->getInstance<AGE::AssetsManager>()->getMesh("Gizmo/gizmo.sage");
				}));
				scene->getInstance<AGE::AssetsManager>()->pushNewCallback("Gizmo/gizmo.mage", scene,
					std::function<void()>([=](){
					_gizmoMaterial = scene->getInstance<AGE::AssetsManager>()->getMaterial("Gizmo/gizmo.mage");
				}));

				_scene->getInstance<AGE::AssetsManager>()->loadMesh(OldFile("Gizmo/gizmo.sage"), "Gizmo/gizmo.sage");
				_scene->getInstance<AGE::AssetsManager>()->loadMaterial(OldFile("Gizmo/gizmo.mage"), "Gizmo/gizmo.mage");
			}
			EntityManager::~EntityManager(){}

			void EntityManager::updateMenu()
			{
				if (ImGui::MenuItem("Show", nullptr, &_displayWindow)) {}
				if (ImGui::MenuItem("Graphnode display", nullptr, &_graphNodeDisplay, _displayWindow)) {}
				if (_cam != nullptr && ImGui::BeginMenu("Render options"))
				{
					static char const *pipelineNames[RenderType::TOTAL] =
					{ "Debug deferred rendering"
					, "Deferred rendering" };

					for (auto i = 0; i < RenderType::TOTAL; ++i)
					{
						auto enabled = _cam->getPipeline() != RenderType(i);
						if (ImGui::MenuItem(pipelineNames[i], nullptr, nullptr, enabled) && enabled)
						{
							_pipelineToSet = i;
						}
					}

					ImGui::EndMenu();
				}
				ImGui::Separator();
				bool saveSceneEnabled = _sceneName[0] != '\0';
				if (ImGui::MenuItem("Save scene", "CTRL+S", nullptr, saveSceneEnabled))
				{
					_saveScene = true;
				}
				if (ImGui::BeginMenu("Open scene"))
				{
					if (ImGui::ListBox("Scenes", &WE::EditorConfiguration::getSelectedSceneIndex(), WE::EditorConfiguration::getScenesName().data(), static_cast<int>(WE::EditorConfiguration::getScenesName().size())))
					{
						_reloadScene = true;
					}
					ImGui::EndMenu();
				}
			}

			void EntityManager::updateBegin(float time)
			{}

			void EntityManager::updateEnd(float time)
			{}

			void EntityManager::mainUpdate(float time)
			{
				if (_reloadScene)
				{
					_scene->clearAllEntities();

					generateBasicEntities();

					auto sceneFileName = WE::EditorConfiguration::getSelectedScenePath() + ".raw_scene";

					strcpy_s(_sceneName, WE::EditorConfiguration::getSelectedSceneName().c_str());

					ReadableEntityPack pack;
					pack.scene = _scene;
					pack.loadFromFile(sceneFileName);
					_reloadScene = false;
					_selectedEntity = nullptr;
				}
				{
					EntityFilter::Lock lock(_meshRenderers);
					_meshRenderers.requireComponent<MeshRenderer>();
					for (auto mesh : _meshRenderers.getCollection()) {
						auto r = mesh->getComponent<MeshRenderer>();
						r->reload_material();
					}
				}

				if (_saveScene)
				{
					((WE::ArchetypeEditorManager*)(_scene->getInstance<IArchetypeManager>()))->save();

					//auto parent = _gizmoEntity.getLinkPtr()->getParent();
					//_gizmoEntity.getLinkPtr()->detachParent();

					ReadableEntityPack pack;
					{
						CreateReadableEntityPack(pack, _entities);
						pack.saveToFile(WE::EditorConfiguration::GetEditedSceneDirectory() + std::string(_sceneName) + ".raw_scene");
					}
					{
						BinaryEntityPack binaryPack = pack.toBinary();
						binaryPack.saveToFile(WE::EditorConfiguration::GetExportedSceneDirectory() + std::string(_sceneName) + ".scene");
					}

					//if (parent)
					//{
					//	_gizmoEntity.getLinkPtr()->attachParent(parent);
					//}

					_saveScene = false;
				}

				if (_cam && _pipelineToSet > -1)
				{
					_cam->setPipeline(RenderType(_pipelineToSet));
					_pipelineToSet = -1;
				}

				if (_displayWindow == false)
				{
					return;
				}
				ImGui::Begin("Entity list", nullptr, ImGuiWindowFlags_MenuBar);

				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Options"))
					{
						updateMenu();
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				ImGui::Checkbox("Graphnode display", &_graphNodeDisplay);

				EntityFilter::Lock lock(_filter);

				// Disgusting but fuck it ! :)
				_entityNames.clear();
				_entities.clear();
				{
					EntityFilter::Lock lock(_filter);
					for (auto &e : _filter.getCollection())
					{
						auto representation = e->getComponent<AGE::WE::EntityRepresentation>();
						if (representation->editorOnly)
						{
							_filter.manuallyRemoveEntity(e);
							continue;
						}

						if (_graphNodeDisplay)
						{
							// if it's not attached to root
							if (e->getLink().hasParent())
							{
								continue;
							}
						}
						_entityNames.push_back(representation->name);
						_entities.push_back(e);
					}
				}
				if (_selectedEntityIndex >= _entities.size())
				{
					_selectedEntityIndex = 0;
				}

				if (_entities.size() > 0 && !_graphNodeDisplay)
				{
					ImGui::PushItemWidth(-1);
					//ImGui::ListBoxHeader("##empty");
					if (ImGui::ListBox("##empty", &_selectedEntityIndex, &(_entityNames.front()), (int)(_entityNames.size())))
					{
						if (_entities.size() > 0 && _selectedEntityIndex < _entities.size())
						{
							_selectedEntity = _entities[_selectedEntityIndex].getPtr();
						}
						else
						{
							_selectedEntity = nullptr;
						}
					}
					//ImGui::ListBoxFooter();
					ImGui::PopItemWidth();
				}
				else
				{
					for (auto &e : _entities)
					{
						recursiveDisplayList(e, _selectedEntity, _selectParent);
					}
				}

				ImGui::Separator();

				ImGui::BeginChild("Edit entity");

				if (_entities.size() > 0 && _selectedEntity != nullptr)
				{
					auto entity = *_selectedEntity;
					displayEntity(entity, _scene);

					ImGui::Separator();

					auto representation = entity->getComponent<AGE::WE::EntityRepresentation>();
					auto archetypeCpt = entity->getComponent<AGE::ArchetypeComponent>();

					if (archetypeCpt == nullptr)
					{
						auto &types = ComponentRegistrationManager::getInstance().getExposedType();
						auto &creationFn = ComponentRegistrationManager::getInstance().getCreationFunctions();

						if (ImGui::Button("Add component..."))
							ImGui::OpenPopup("add_component");

						if (ImGui::BeginPopup("add_component"))
						{
							for (auto &t : types)
							{
								if (!entity->haveComponent(t.second))
								{
									if (ImGui::Selectable(ComponentRegistrationManager::getInstance().getComponentName(t.second).c_str()))
									{
										auto cptPtr = creationFn.at(t.first)(&entity);
									}
								}
							}
							ImGui::EndPopup();
						}
					}

					auto isAnArchetype = archetypeCpt != nullptr;
					auto isAnArchetypeChild = isAnArchetype && archetypeCpt->parentIsAnArchetype;

					ImGui::SameLine();
					if (!isAnArchetypeChild && ImGui::Button("Delete entity"))
					{
						ImGui::OpenPopup("Delete entity ?");
					}
					if (ImGui::BeginPopupModal("Delete entity ?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Are you sure to delete entity ?\nThis operation cannot be undone!\n\n");
						ImGui::Separator();

						static bool dont_ask_me_next_time = false;
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
						ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
						ImGui::PopStyleVar();

						if (ImGui::Button("Delete", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
							auto destroyAllHierarchy = archetypeCpt != nullptr;
							_scene->destroy(entity, destroyAllHierarchy);
							_selectedEntity = nullptr;
							_selectedEntityIndex = 0;
						}
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}

					ImGui::SameLine();
					static bool duplicateModalIsOpen = false;
					if (isAnArchetype == false && isAnArchetypeChild == false && ImGui::Button("Duplicate"))
					{
						ImGui::OpenPopup("Duplicate entity ?");
					}
					if (ImGui::BeginPopupModal("Duplicate entity ?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						static Entity duplicate;
						AGE::WE::EntityRepresentation *copyRepresentationCpt = nullptr;
						if (duplicateModalIsOpen == false)
						{
							auto archetypeCpt = entity->getComponent<ArchetypeComponent>();
							duplicate = Entity();
							if (archetypeCpt == nullptr)
							{
								_scene->copyEntity(entity, duplicate, true, false);
							}
							else
							{
								duplicate = _scene->createEntity();
								_scene->getInstance<IArchetypeManager>()->spawn(duplicate, archetypeCpt->archetypeName);
							}
							duplicateModalIsOpen = true;
						}

						copyRepresentationCpt = duplicate->getComponent<AGE::WE::EntityRepresentation>();

						ImGui::Text("Duplicate entity");
						ImGui::Separator();
						ImGui::InputText("Entity name", copyRepresentationCpt->name, ENTITY_NAME_LENGTH);

						if (ImGui::Button("Okay", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
							duplicateModalIsOpen = false;
						}
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
							_scene->destroy(duplicate, true);
							duplicateModalIsOpen = false;
						}
						ImGui::EndPopup();
					}

					if (isAnArchetypeChild == false)
					{
						ImGui::InputText("Archetype name", _archetypeName, MAX_SCENE_NAME_LENGTH);
						if (ImGui::Button("Convert to Archetype"))
						{
							//auto parent = _gizmoEntity.getLinkPtr()->getParent();
							//_gizmoEntity.getLinkPtr()->detachParent();

							auto manager = _scene->getInstance<AGE::WE::ArchetypeEditorManager>();
							manager->addOne(_archetypeName, *_selectedEntity);

							//if (parent)
							//{
							//	_gizmoEntity.getLinkPtr()->attachParent(parent);
							//}
						}
					}
				}

				ImGui::SameLine();
				static bool createEntityModalIsOpen = false;
				if (ImGui::Button("Create entity"))
				{
					ImGui::OpenPopup("Create entity ?");
				}
				if (ImGui::BeginPopupModal("Create entity ?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static Entity created;
					AGE::WE::EntityRepresentation *newRepresentation = nullptr;
					if (createEntityModalIsOpen == false)
					{
						created = _scene->createEntity();
						createEntityModalIsOpen = true;
					}

					newRepresentation = created->getComponent<AGE::WE::EntityRepresentation>();

					ImGui::Text("Create entity");
					ImGui::Separator();
					if (newRepresentation)
					{
						ImGui::InputText("Entity name", newRepresentation->name, ENTITY_NAME_LENGTH);
					}

					if (ImGui::Button("Okay", ImVec2(120, 0)))
					{
						ImGui::CloseCurrentPopup();
						createEntityModalIsOpen = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(120, 0)))
					{
						ImGui::CloseCurrentPopup();
						_scene->destroy(created, true);
						createEntityModalIsOpen = false;
					}
					ImGui::EndPopup();
				}

				_entities.clear();
				{
					EntityFilter::Lock lock(_filter);
					for (auto &e : _filter.getCollection())
					{
						// if it's not attached to root
						if (e->getLink().hasParent())
						{
							continue;
						}
						_entities.push_back(e);
					}
				}
				ImGui::InputText("Scene name", _sceneName, MAX_SCENE_NAME_LENGTH, ImGuiInputTextFlags_CharsNoBlank);
				ImGui::SameLine();
				if (_sceneName[0] && ImGui::Button("Save"))
				{
					_saveScene = true;
				}

				ImGui::EndChild();

				ImGui::End();

				auto input = _scene->getInstance<Input>();

				auto ctrl = input->getPhysicalKeyPressed(AgeKeys::AGE_LCTRL);
				ctrl |= input->getPhysicalKeyPressed(AgeKeys::AGE_RCTRL);

				auto sKey = input->getPhysicalKeyPressed(AgeKeys::AGE_s);

				if (ctrl && sKey && _sceneName[0])
				{
					_saveScene = true;
				}

				if (_lastFrameSelectedEntity != _selectedEntity)
				{
					//if (_lastFrameSelectedEntity != nullptr)
					//{
					//	_gizmoEntity->removeComponent<MeshRenderer>();
					//	_gizmoEntity.getLinkPtr()->detachParent();
					//}
					//if (_selectedEntity != nullptr)
					//{
					//	_gizmoEntity.getLinkPtr()->attachParent(_selectedEntity->getLinkPtr());
					//	_gizmoEntity->addComponent<MeshRenderer>(_gizmoMesh, _gizmoMaterial);
					//}
					_lastFrameSelectedEntity = _selectedEntity;
				}

			}

			bool EntityManager::initialize()
			{
				return true;
			}

			void EntityManager::generateBasicEntities()
			{
				//auto camera = _scene->createEntity();
				//_cam = camera->addComponent<CameraComponent>();
				//camera->getLink().setPosition(glm::vec3(0, 3, 5));
				//camera->getLink().setForward(glm::vec3(0, 0, 0));
				//camera->addComponent<FreeFlyComponent>();
				//camera->addComponent<AGE::WE::EntityRepresentation>()->editorOnly = true;

				//_gizmoEntity = _scene->createEntity();
				//_gizmoEntity->addComponent<AGE::WE::EntityRepresentation>()->editorOnly = true;
			}
	}
}