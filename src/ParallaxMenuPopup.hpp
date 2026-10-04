#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <alphalaneous.alphas-ui-pack/include/API.hpp>
using namespace alpha::prelude;

#include "ParallaxSetup.hpp"

#include "Utils/TriggerButton.hpp"
#include "Utils/CustomNumberInput.hpp"

class ParallaxMenuPopup : public geode::Popup {
public:
    static ParallaxMenuPopup* create();

    ParallaxSetup* getSelectedSetup();
    void scrollToLayerIndex(int index);
    
    ~ParallaxMenuPopup();
protected:

    ParallaxSetupList m_parallaxSetupList;

    //layer list label
    Label* m_layerListLabel = nullptr;
    //layer list menus
    Button* m_addLayerButton = nullptr;
    Button* m_sortButton = nullptr;
    //setup actions
    CCMenu* m_setupActionMenu = nullptr;
    std::vector<Button*> m_setupActionButtons;//buttons that get disabled when theres no setup
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
    CustomNumberInput* m_durationInput = nullptr;
    //setup groupids
    TriggerButton* m_setupRootIDButton = nullptr;
    TriggerButton* m_setupFollowIDButton = nullptr;

    int m_selectedSetupIndex = 0;

    //allows for reuse of nodes to avoid crashes when deleting them
    int m_nextLayerListNode = 0;

    ListenerHandle* m_upListener = nullptr;
    ListenerHandle* m_downListener = nullptr;

    bool init();
    void initDevButtons();//this one can be commented out to hide the dev buttons
    void initSetupSwitcher();
    void initInfoButtons();
    void initLayerList();

    //callbacks
    void onAddLayerButton(CCObject *);
    void onCleanupTriggersButton(CCObject *);
    void onDeleteSetupButton(CCObject *);
    void onFindCenterButton(CCObject *);
    void onCreateSetupButton(CCObject *);
    void onFindSetupInEditorButton(CCObject *);
    void onMakeDurationInfiniteButton(CCObject *);
    void onDuplicateAndLayerButton(CCObject *);
    void onCreateQuickGradientButton(CCObject *);

    //layer list methods
    void loadSetupLayerList(ParallaxSetup* setup);
    void addLayerNodeToList(ParallaxSetupLayer* layer);//note: this does NOT update the layout
    size_t getFocusedLayer();//returns SIZE_MAX if no layer is selected
    void changeFocusedLayer(int indexOffset);


    void updateAllUI();

    //the current setup can probably be passed into a lot of these so it only has to be gotten once

    void updateSetupActionButtons();
    void updateLayerListHint();
    void updateSetupSelector();
    void updateSetupDurationInput();//this only really needs to be called when switching setups
    void updateRootIDButton();
    void updateFollowIDButton();
};