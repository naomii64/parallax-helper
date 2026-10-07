#pragma once

#include "constants.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ph::Utils{
    enum class LayerSortingType : int {
    	BY_DEPTH=0,
    	BY_GROUPID,
    	_count
    };
    
    LayerSortingType getSortingType();

    geode::ZStringView getTriggerSprite(int objectID);

    void replaceIDinObjects(int oldID,int newID);//replaces an id with another in objects that have it

    int getNextFreeGroupID();

    //used for setup actions
    Button* createButtonWithALittleIconNextToTheText(geode::Button::ButtonCallback callback,const std::string& labelText,float width, float height = 30.0f,const std::string& iconSpritePath = "",const std::string& backgroundSprite = "GJ_button_01.png");

    void scaleToPixels(CCNode* node,const cocos2d::CCSize &sizePixels);

    void enableButton(Button* item);
    void disableButton(Button* item);    
    CCMenu* createTwoButtonMenu(std::function<void(bool)> callback,const char* btn1Title = "Cancel", const char* btn2Title = "Confirm");
};
using namespace ph;