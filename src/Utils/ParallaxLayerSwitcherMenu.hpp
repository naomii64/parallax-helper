#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ParallaxLayerSwitcherMenu : public CCMenu {
public:
    static ParallaxLayerSwitcherMenu* create(const cocos2d::CCSize &size);
protected:
    bool init(const cocos2d::CCSize &size);

};