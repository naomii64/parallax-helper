#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

#include "ParallaxMenuPopup.hpp"

#include <Geode/modify/EditorUI.hpp>
class $modify(MyEditorUI, EditorUI) {
	bool MyEditorUI::init(LevelEditorLayer* p0) {
	    if (!EditorUI::init(p0)) return false;

		if (auto menu = this->getChildByID("editor-buttons-menu")) {

			auto btn = Button::createWithSprite("parallaxMenuButton.png"_spr, [](Button*) {
				ParallaxMenuPopup::create()->show();
			});

			btn->setContentSize({40, 40});

	    	menu->addChild(btn);
	    	menu->updateLayout();

			//add it to uiItems so it gets hidden on playtest
			m_uiItems->addObject(btn);
		}

		return true;
	}
};

$on_mod(Loaded) {
    nwo5::settings::SettingsManager::get()->load();
}