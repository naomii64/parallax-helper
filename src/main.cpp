#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

#include "ParallaxMenuPopup.hpp"
#include "Utils/ParallaxLayerSwitcherMenu.hpp"



#include <Geode/modify/EditorUI.hpp>
class $modify(MyEditorUI, EditorUI) {

	struct Fields {
		//create a setup list based on the level here for now
		//testing here because of scope issues
		ParallaxSetupList m_parallaxSetupList = ParallaxSetupList();
	};

	
	bool init(LevelEditorLayer* p0) {
	    if (!EditorUI::init(p0)) return false;

		if (auto menu = this->getChildByID("editor-buttons-menu")) {

			//create a setup list based on the level here for now
			m_fields->m_parallaxSetupList.scanEditorForSetups(editor::layer());


			auto btn = Button::createWithSprite("parallaxMenuButton.png"_spr, [this](Button*) {

				ParallaxMenuPopup::create(&m_fields->m_parallaxSetupList)->show();
			});

			btn->setContentSize({40, 40});
			btn->setID("parallax-menu-button"_spr);

	    	menu->addChild(btn);
	    	menu->updateLayout();

			//add it to uiItems so it gets hidden on playtest
			m_uiItems->addObject(btn);
		}

		//now setup the parallax layer switcher
		constexpr CCSize LAYER_SWITCHER_SIZE = {200.f,22.f};

		auto parallaxLayerSwitcher = ParallaxLayerSwitcherMenu::create(LAYER_SWITCHER_SIZE);
		
		parallaxLayerSwitcher->setScale(.9f);
		parallaxLayerSwitcher->setID("layer-switcher"_spr);
		parallaxLayerSwitcher->setPosition(344.,193);//MOVE THIS LATER BEFORE RELEASING

		this->addChild(parallaxLayerSwitcher);
		m_uiItems->addObject(parallaxLayerSwitcher);

		updateLayout();
		return true;
	}
};


//placeholder test function
bool shouldbeselectedfunction(CCObject* obj){
	return true;
	//lowk just only show move triggers for now
	//return static_cast<GameObject*>(obj)->m_objectID == editor::trigger::MOVE_TRIGGER;
}
//name this better later and of course move it once i develop this more
void filterObjectArray(CCArray* arr){
	for(auto& obj : arr->asExt()){
		if(!shouldbeselectedfunction(obj))
			arr->removeObject(obj);
	}
}

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(MyLevelEditorLayer, LevelEditorLayer) {
	void updateVisibility(float dt) {	
		LevelEditorLayer::updateVisibility(dt);
		
		if(editor::isPlaytesting()) return;

		//copied from robtop
		constexpr int ALPHA_WHEN_NOT_ON_LAYER = 50;
		constexpr float NOT_ON_LAYER_ALPHA_FRACTION = ((float)ALPHA_WHEN_NOT_ON_LAYER/(float)255);

		/*
			FOR ANYONE READING/REFERENCING THIS CODE

			the way that this works makes it so its hard to have compatibility with other mods that have their own layer filtering systems
			
			unless there is a way to make this compatible that i missed/didnt think of
			if any other mod wants to do this there might have to be some sort of editor layering/editor filtering api created
		*/

		//this isnt fully compatible with other mods that do this sadly but uhm
		for (size_t i = 0; i < m_activeObjectsCount; i++) {
			GameObject* obj = m_activeObjects[i];
			
			bool isVisibleEditorLayer = (obj->m_editorLayer == m_currentLayer || ((obj->m_editorLayer2 == m_currentLayer) && (obj->m_editorLayer2 != 0)/*because 0 isnt a valid l2 layer*/) || m_currentLayer == editor::constants::ALL_LAYERS);

			if(!isVisibleEditorLayer) continue;

			if(!shouldbeselectedfunction(obj)){
				//make the object less visible
				obj->setOpacity(obj->getOpacity() * NOT_ON_LAYER_ALPHA_FRACTION);
			}
		}
	}

	CCArray* objectsAtPosition(CCPoint position) {
		CCArray* ret = LevelEditorLayer::objectsAtPosition(position);
		
		filterObjectArray(ret);

		return ret;
	}

	CCArray* objectsInRect(CCRect rect, bool ignoreGroups) {
		CCArray* ret = LevelEditorLayer::objectsInRect(rect, ignoreGroups);
		
		filterObjectArray(ret);
		
		return ret;
	}


	// void addSpecial(GameObject* object) {
	// 	if(!m_initializing){
	// 		geode::log::info("[LevelEditorLayer]: obj created: id={}",object->m_objectID);
	// 	}
		
	// 	LevelEditorLayer::addSpecial(object);
	// }

	// void removeSpecial(GameObject* object) {
	// 	if(!m_initializing){
	// 		geode::log::info("[LevelEditorLayer]: obj removed: id={}",object->m_objectID);
	// 	}

	// 	LevelEditorLayer::removeSpecial(object);
	// }

};

$on_mod(Loaded) {
    nwo5::settings::SettingsManager::get()->load();
}