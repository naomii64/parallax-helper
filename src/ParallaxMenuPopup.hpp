#pragma once
#include <Geode/Geode.hpp>

#include "ParallaxSetup.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
using namespace alpha::prelude;

using namespace geode::prelude;

#include "MyEditorUI.hpp"

class ParallaxMenuPopup : public geode::Popup {
public:
    //TODO: theres no need to pass and store BOTH of these, this only needs to pass and store the ui
    static ParallaxMenuPopup* create(LevelEditorLayer* editor,MyEditorUI* editorUI);

    ParallaxSetup* getSelectedSetup();
protected:
    bool init(LevelEditorLayer* editor,MyEditorUI* editorUI);

    ParallaxSetupList m_parallaxSetupList;

    void onAddLayerButton(CCObject *);
    void onCleanupTriggersButton(CCObject *);
    void onCreateSetupButton(CCObject *);
    

    LevelEditorLayer* m_editorLayer = nullptr;
    MyEditorUI* m_editorUI = nullptr;

    void addLayerNodeToList(ParallaxSetupLayer* layer);//node: this does NOT update the layout
    CCNode* m_layerListNode;
    alpha::ui::AdvancedScrollLayer* m_scrollLayer;
};