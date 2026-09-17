#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "ParallaxMenuPopup.hpp"

#include "MyEditorUI.hpp"

bool MyEditorUI::init(LevelEditorLayer* p0) {
    if (!EditorUI::init(p0)) return false;

	if (auto menu = this->getChildByID("editor-buttons-menu")) {

		auto btn = Button::createWithSprite("parallaxMenuButton.png"_spr, [this](Button*) {
			ParallaxMenuPopup::create(this)->show();
		});

		btn->setContentSize({40, 40});

    	menu->addChild(btn);
    	menu->updateLayout();

		//add it to uiItems so it gets hidden on playtest
		m_uiItems->addObject(btn);
	}

	return true;
}
//FLAlertLayer::create(":3","found possible parallax root","ok?")->show();