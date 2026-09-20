#pragma once
#include <Geode/Geode.hpp>

#include "ParallaxSetup.hpp"

using namespace geode::prelude;

class ParallaxMenuLayerNode : public CCMenu {
public:
    static ParallaxMenuLayerNode* create(const cocos2d::CCSize &size,ParallaxSetupLayer* layer);
    void setLayer(ParallaxSetupLayer* layer);//changes the layer this node corresponds to
protected:
    bool init(const cocos2d::CCSize &size,ParallaxSetupLayer* layer);
    void updateDepthLabelColor();

    Label* m_layerGroupIDLabel = nullptr;
    Label* m_depthLabel = nullptr;
    TextInput* m_depthInput = nullptr;

    ParallaxSetupLayer* m_layerPtr = nullptr;
};