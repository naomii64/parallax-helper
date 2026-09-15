#pragma once
#include <Geode/Geode.hpp>

#include "ParallaxSetup.hpp"

using namespace geode::prelude;

class ParallaxMenuLayerNode : public CCMenu {
public:
    static ParallaxMenuLayerNode* create(const cocos2d::CCSize &size,ParallaxSetupLayer* layer);

protected:
    bool init(const cocos2d::CCSize &size,ParallaxSetupLayer* layer);
    void updateDepthLabelColor();

    geode::Label* m_depthLabel = nullptr;
    geode::TextInput* m_depthInput = nullptr;

    ParallaxSetupLayer* m_layerPtr = nullptr;
    ////these get stored
    //EffectGameObject* m_followTriggerPtr = nullptr;
    //TransformTriggerGameObject* m_scaleTriggerPtr = nullptr;
};