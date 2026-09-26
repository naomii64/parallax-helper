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
    void scrollToLayerIndex(int index);
    
    ~ParallaxMenuPopup();
protected:
    bool init(MyEditorUI* editorUI);
    void init_createSetupSwitcher();
    void init_createInfoButtons();



    //callbacks
    void onAddLayerButton(CCObject *);
    void onCleanupTriggersButton(CCObject *);
    void onDeleteSetupButton(CCObject *);
    void onFindCenterButton(CCObject *);
    void onCreateSetupButton(CCObject *);
    void onFindSetupInEditorButton(CCObject *);
    void onMakeDurationInfiniteButton(CCObject *);
    void onDuplicateAndLayerButton(CCObject *);

    //layer list methods
    void loadSetupLayerList(ParallaxSetup* setup);
    void addLayerNodeToList(ParallaxSetupLayer* layer);//node: this does NOT update the layout
    size_t getFocusedLayer();//returns SIZE_MAX if no layer is selected
    void changeFocusedLayer(int indexOffset);

    ParallaxSetupList m_parallaxSetupList;

    //editor pointers (hopefully these wont be needed with the use of api)
    LevelEditorLayer* m_editorLayer = nullptr;
    MyEditorUI* m_editorUI = nullptr;


    //layer list label
    Label* m_layerListLabel = nullptr;
    //layer list menus
    CCMenuItemSpriteExtra* m_addLayerButton = nullptr;
    //setup actions
    Button* m_findCenterButton = nullptr;
    Button* m_findSetupButton = nullptr;
    Button* m_cleanSetupButton = nullptr;
    Button* m_deleteSetupButton = nullptr;
    //layer list
    NineSlice* m_layerListBackground = nullptr;
    CCMenu* m_layerListMenu = nullptr;
    AdvancedScrollLayer* m_scrollLayer = nullptr;
    AdvancedScrollBar* m_layerListScrollBar = nullptr;
    Label* m_layerListHint = nullptr;
    //setup switcher
    Button* m_setupSwitcherPrevButton = nullptr;
    Button* m_setupSwitcherNextButton = nullptr;
    Label* m_setupSelectorLabel = nullptr;
    //duration input
    TextInput* m_durationInput = nullptr;

    int m_selectedSetupIndex = 0;

    //allows for reuse of nodes to avoid crashes when deleting them
    int m_nextLayerListNode = 0;

    ListenerHandle* m_upListener = nullptr;
    ListenerHandle* m_downListener = nullptr;
private:
    void updateAllUI();

    //the current setup can probably be passed into a lot of these so it only has to be gotten once

    void updateSetupActionButtons();
    void updateLayerListHint();
    void updateSetupSelector();
    void updateSetupDurationInput();//this only really needs to be called when switching setups
};