#pragma once

#include "TriggerButton.hpp"
#include "CustomNumberInput.hpp"
#include "../ParallaxSetup.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ParallaxLayerSwitcherMenu : public CCMenu {
public:
    static ParallaxLayerSwitcherMenu* create(const cocos2d::CCSize &size,ParallaxSetupList* setupList);
    ParallaxSetupLayer* getSelectedLayer();
protected:
    bool init(const cocos2d::CCSize &size,ParallaxSetupList* setupList);
    void selectAllLayer();
    void selectLastLayer();
    void changeSelectedLayer(int amount);
    
    void updateUI();

    void updateObjectLayerVisibility();

    ParallaxSetupList* m_parallaxSetupList = nullptr;

    Button* m_prevLayerButton = nullptr;
    Button* m_nextLayerButton = nullptr;

    Button* m_allLayersButton = nullptr;
    Button* m_lastLayerButton = nullptr;

    NineSlice* m_mainBG = nullptr;
    TriggerButton* m_triggerButton = nullptr;

    CustomNumberInput* m_depthInput = nullptr;
    Label* m_allLabel = nullptr;

    static constexpr int LAYER_ALL = -1;

    int m_selectedLayerIndex = LAYER_ALL;
};