#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "ParallaxSetup.hpp"


class ParallaxMenuLayerNode : public CCMenu {
public:
    static ParallaxMenuLayerNode* create(const cocos2d::CCSize &size,ParallaxSetupLayer* layer);
    void setLayer(ParallaxSetupLayer* layer);//changes the layer this node corresponds to

    void defocus();
    void focus();
    bool getFocused();
protected:
    bool init(const cocos2d::CCSize &size,ParallaxSetupLayer* layer);
    void updateDepthLabelColor();

    Label* m_layerGroupIDLabel = nullptr;
    Label* m_depthLabel = nullptr;
    TextInput* m_depthInput = nullptr;

    ParallaxSetupLayer* m_layerPtr = nullptr;
};