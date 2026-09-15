/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

#include "ParallaxMenuPopup.hpp"

#include "MyEditorUI.hpp"

bool MyEditorUI::init(LevelEditorLayer* p0) {
    if (!EditorUI::init(p0)) return false;

	//add the button next to the layer menu
	if (auto menu = this->getChildByID("editor-buttons-menu")) {
		//create a button that opens the parallax menu
		//auto btn = CCMenuItemToggler::create(
		//	CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
		//	CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
		//	this,
		//	menu_selector(MyEditorUI::onParallaxMenuToggle)
		//);
		//btn->setID("parallax-menu-toggle"_spr);
		//layerMenu->addChild(btn);
		//layerMenu->updateLayout();
		//FLAlertLayer::create("Geode", "this should be opening the parallax menu", "ok?")->show();
    		
		auto btn = Button::createWithSprite("parallaxMenuButton.png"_spr, [this](Button*) {
			ParallaxMenuPopup::create(m_editorLayer,this)->show();
		});

		btn->setContentSize({40, 40});
    	btn->setID("parallax-menu-button");

    	menu->addChild(btn);
    	menu->updateLayout();

		//add it to uiItems so it gets hidden on playtest
		m_uiItems->addObject(btn);
	}

	//init succeeded
	return true;
}

void MyEditorUI::setObjectPosition(GameObject* object,const CCPoint& position){
	moveObject(object,position-object->getPosition());
}


//FLAlertLayer::create(":3","found possible parallax root","ok?")->show();;

/*FLAlertLayer::create("Geode", fmt::format("area move found\nm_targetControlID:{}\nm_hasCenterEffect:{}\nm_centerGroupID:{}\nm_targetGroupID:{}\nm_targetModCenterID:{}\nm_specialTarget:{}\nm_directionType:{}\nm_inbound:{}",
	ego->m_targetControlID,
	ego->m_hasCenterEffect,
	ego->m_centerGroupID,
	ego->m_targetGroupID,
	ego->m_targetModCenterID,
	ego->m_specialTarget,
	ego->m_directionType,
	ego->m_inbound
),"ok?")->show();*/