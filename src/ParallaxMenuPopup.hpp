#pragma once
#include <Geode/Geode.hpp>

#include "ParallaxSetup.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
using namespace alpha::prelude;

using namespace geode::prelude;

#include "MyEditorUI.hpp"

class ParallaxMenuPopup : public geode::Popup {
public:
    static ParallaxMenuPopup* create(MyEditorUI* editorUI);

    ParallaxSetup* getSelectedSetup();

    void updateAllUI();
protected:
    bool init(MyEditorUI* editorUI);

    void loadSetupLayerList(ParallaxSetup* setup);

    ParallaxSetupList m_parallaxSetupList;

    void onAddLayerButton(CCObject *);
    void onCleanupTriggersButton(CCObject *);
    void onCreateSetupButton(CCObject *);
    void onFindSetupInEditorButton(CCObject *);

    LevelEditorLayer* m_editorLayer = nullptr;
    MyEditorUI* m_editorUI = nullptr;

    void addLayerNodeToList(ParallaxSetupLayer* layer);//node: this does NOT update the layout
    NineSlice* m_layerListBackground = nullptr;
    CCNode* m_layerListNode = nullptr;
    AdvancedScrollLayer* m_scrollLayer = nullptr;
    AdvancedScrollBar* m_layerListScrollBar = nullptr;

    CCMenuItemSpriteExtra* m_addLayerButton = nullptr;
    Label* m_layerListHint = nullptr;
private:
    void updateAddLayerButton();
    void updateLayerListHint();
};