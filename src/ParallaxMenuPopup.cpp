#include "ParallaxMenuPopup.hpp"


#include "ParallaxMenuLayerNode.hpp"

#include "constants.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

constexpr float popupWidth = 400;
constexpr float popupHeight = 290;

constexpr float padAmount = 10;
constexpr float layerListWidth = 200.0f;
constexpr float layerListHeight = 240.0f;

constexpr float layerListX = popupWidth-padAmount;

constexpr float LAYER_NODE_HEIGHT = 40.0f;
constexpr float LAYER_LIST_SCROLL_LAYER_HEIGHT = 260.0f;
//idk where else to put this function
template <typename T>
void enableNode(T* item){
    item->setEnabled(true);
    item->setColor(ccWHITE);
    item->setOpacity(255);
}
template <typename T>
void disableNode(T* item){
    item->setEnabled(false);
    item->setColor(constants::ui::disabledColor);
    item->setOpacity(constants::ui::disabledAlpha);
}

void ParallaxMenuPopup::loadSetupLayerList(ParallaxSetup *setup)
{

    //reset the nodes because we cant just delete them apparently
    m_nextLayerListNode = 0;
    auto nodes = m_layerListMenu->getChildrenExt();
    for(auto node : nodes){
        auto layerNode = static_cast<ParallaxMenuLayerNode*>(node);
        layerNode->setVisible(false);
        layerNode->defocus();
    }

    if(setup){
        auto sortedLayers = setup->sortDepth();
        for(auto layerPtr : sortedLayers){
            addLayerNodeToList(layerPtr);
        }
    }

    //update everything in the right order
    m_layerListMenu->updateLayout();
    m_scrollLayer->updateLayout();
    m_layerListBackground->updateLayout();
}

void ParallaxMenuPopup::init_createSetupSwitcher()
{
    //create the setup swticher (maybe make this a custom node later?)
    constexpr float setupSwitcherWidth = 90;
    constexpr float setupSwitcherHeight = 20;
    constexpr float setupSwitcherButtonScale = 0.8f;
    auto setupSwitcherMenu = CCMenu::create();
    setupSwitcherMenu->setPosition({padAmount,popupHeight-padAmount-setupSwitcherHeight});
    setupSwitcherMenu->setScale(0.8f);

    setupSwitcherMenu->setContentSize({setupSwitcherWidth,setupSwitcherHeight});
    m_setupSwitcherPrevButton = Button::createWithSpriteFrameName("GJ_arrow_01_001.png", [this](Button*) {
        m_selectedSetupIndex--;
        this->loadSetupLayerList(getSelectedSetup());
        this->updateAllUI();
    });
    m_setupSwitcherNextButton = Button::createWithSpriteFrameName("GJ_arrow_01_001.png", [this](Button*) {
        m_selectedSetupIndex++;
        this->loadSetupLayerList(getSelectedSetup());
        this->updateAllUI();
	});
    auto nextButtonSprite = m_setupSwitcherNextButton->getChildByType<CCSprite>();
    nextButtonSprite->setFlipX(true);

    m_setupSwitcherPrevButton->setPosition({0.0,setupSwitcherHeight/2});
    m_setupSwitcherNextButton->setPosition({setupSwitcherWidth,setupSwitcherHeight/2});
    m_setupSwitcherPrevButton->setScale(setupSwitcherButtonScale);
    m_setupSwitcherNextButton->setScale(setupSwitcherButtonScale);
    setupSwitcherMenu->addChild(m_setupSwitcherPrevButton);
    setupSwitcherMenu->addChild(m_setupSwitcherNextButton);

    m_setupSelectorLabel = Label::create("","bigFont.fnt");
    setupSwitcherMenu->addChild(m_setupSelectorLabel);
    m_setupSelectorLabel->setPosition({setupSwitcherWidth/2,setupSwitcherHeight/2});


    auto newSetupButton = Button::createWithSpriteFrameName("GJ_plusBtn_001.png", [this](Button* btn) {
        this->onCreateSetupButton(btn);
    });
    newSetupButton->setPosition(120,setupSwitcherHeight/2);
    newSetupButton->setScale(0.6f);
    setupSwitcherMenu->addChild(newSetupButton);
    newSetupButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Create New Setup"));

    m_mainLayer->addChild(setupSwitcherMenu);
}

std::string getKeybindStringForInfo(const std::vector<geode::Keybind>& keybinds){
    std::string ret = "";

    for(size_t i = 0;i<keybinds.size();i++){
        const auto& keyBind = keybinds[i];
        //if this is the last one we dont need to add a comma
        if(i>=(keybinds.size()-1)){
            ret+=keyBind.toString();
            continue;
        }
        //else if its the second to last one add an or
        if(i==(keybinds.size()-2)){
            ret+=keyBind.toString()+", or ";

            continue;
        }
        //otherwise just add a comma
        ret+=keyBind.toString()+", ";
    }

    return ret;
}

void ParallaxMenuPopup::init_createInfoButtons()
{
    //this might be able to be moved where the label is created
    auto layerListInfoButton = Button::createWithSpriteFrameName("GJ_infoIcon_001.png", [this](Button*) {
        auto keybindsUp = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("keybind-layerlist-up");
        auto keybindsDown = Mod::get()->getSettingValue<std::vector<geode::Keybind>>("keybind-layerlist-down");
        
        //build the string line by line
        //i know this is slower but its way more readable
        std::string infoStr = fmt::format(
        "Each layer has a <cg>GroupID</c> as shown in the two circles.\n"
        "You can also change the <cd>Depth</c> of each layer.\n"
        "<cr>positive</c> numbers go into the background.\n"
        "<cj>Negative</c> numbers go into the foreground.\n"
        "Note that the max depth is 1, representing the given layer being infinitely far into the background.\n"
        //"The color of the \"Depth:\" text indicates if a given layer is behind or in front of the player.\n"
        "You can use the <cs>{}</c> and <cs>{}</c> keys to move between layers without needing to move your mouse cursor.",
        getKeybindStringForInfo(keybindsUp),getKeybindStringForInfo(keybindsDown));
        //infoStr+="\nYou can also press the <cs>N</c> key to invert numbers but thats a vanilla feature.";

        auto alert = FLAlertLayer::create(
            nullptr,
            "Info",
            infoStr,
            "ok",
            nullptr,
            450
        );

        alert->show();
    });
    layerListInfoButton->setPosition(92,20);
    layerListInfoButton->setScale(0.5f);

    m_layerListLabel->addChild(layerListInfoButton);
}

#include "CustomNumberInput.hpp"

//todo: clean up this init ui function
bool ParallaxMenuPopup::init(MyEditorUI *editorUI)
{

    if (!Popup::init({popupWidth, popupHeight})) return false;
    //disable the popup animation
    m_noElasticity = true;

    //save these pointers
    m_editorUI = editorUI;
    m_editorLayer = editorUI->m_editorLayer;
    
    //initialize the tootip
    auto tooltip = nwo5::ui::Tooltip::create("chatFont.fnt");
    tooltip->setScale(0.5f);//scale this down
    m_mainLayer->addChild(tooltip);
    

    m_layerListBackground = NineSlice::create("square02b_001.png");
    m_layerListBackground->setColor({0, 0, 0});
    m_layerListBackground->setOpacity(44);
    m_layerListBackground->setAnchorPoint({1.0f,0.0f});

    m_layerListBackground->setPosition({layerListX,padAmount+1.5});/*+1.5 to account for the shadow*/
    m_layerListBackground->setContentSize({layerListWidth,layerListHeight});

    m_layerListBackground->setLayout(
        AxisLayout::create()
        ->setPadding({5.0f,5.0f,5.0f,5.0f})
    );

    m_mainLayer->addChild(m_layerListBackground);

    m_layerListMenu = CCMenu::create();
    m_layerListMenu->setLayout(
        AxisLayout::create(Axis::Column)
        ->setAutoGrowAxis({0.0f})
        ->setGap(0.0f)
        ->setAxisReverse(true)//so 0 is at the top
    );
    

    m_scrollLayer = AdvancedScrollLayer::create({layerListWidth,LAYER_LIST_SCROLL_LAYER_HEIGHT});
    m_scrollLayer->setAnchorPoint({0.0f,0.0f});
    m_scrollLayer->setLayout(AxisLayout::create());
    m_scrollLayer->setKeyboardEnabled(false);//could make this be enabled as long as there is no text input selected later
    
    m_layerListScrollBar = AdvancedScrollBar::create(m_scrollLayer,ScrollOrientation::VERTICAL);

    m_layerListBackground->addChild(m_scrollLayer);
    m_layerListBackground->addChild(m_layerListScrollBar);
    m_scrollLayer->addChild(m_layerListMenu);
    
    //create the parallax setup list
	m_parallaxSetupList.scanEditorForSetups(m_editorLayer);
    
    auto setup = getSelectedSetup();
    loadSetupLayerList(setup);

    /*
        CREATE THE SORT BUTTON FOR THE LAYER LIST
    */
    auto sortMenu = CCMenu::create();
    sortMenu->setAnchorPoint({1.0f,1.0f});
    sortMenu->setPosition({layerListX,popupHeight-padAmount});
    sortMenu->setScale(0.75f);
    sortMenu->setContentSize({200.0f,30.0f});
    sortMenu->setLayout(
        AxisLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
    );
    auto sortButton = CCMenuItemSpriteExtra::create(
        EditorButtonSprite::createWithSpriteFrameName("GJ_sortIcon_001.png"), 
        this, 
        nullptr
    );
    disableNode(sortButton);//disable this cuz its not implemented yet

    sortMenu->addChild(sortButton);
    sortMenu->updateLayout();
    m_mainLayer->addChild(sortMenu);
    /*
        CREATE THE ADD LAYER BUTTON
    */
    auto addLayerMenu = CCMenu::create();
    addLayerMenu->setAnchorPoint({0.0f,1.0f});
    addLayerMenu->setPosition({layerListX-layerListWidth,popupHeight-padAmount});
    addLayerMenu->setScale(0.75f);
    addLayerMenu->setContentSize({200.0f,30.0f});
    addLayerMenu->setLayout(
        AxisLayout::create()
        ->setAxisAlignment(AxisAlignment::Start)
    );
    
    m_addLayerButton = CCMenuItemSpriteExtra::create(
        EditorButtonSprite::createWithSpriteFrameName("edit_addCBtn_001.png"), 
        this, 
        menu_selector(ParallaxMenuPopup::onAddLayerButton)
    );
    m_addLayerButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Add Layer"));
    
    addLayerMenu->addChild(m_addLayerButton);
    addLayerMenu->updateLayout();
    m_mainLayer->addChild(addLayerMenu);

    /*
        CREATE THE LABEL FOR THE LAYER LIST
    */
    m_layerListLabel = Label::create("Layers","goldFont.fnt");
    //place it right above the layer list
    //try to line the y up with the sort button
    m_layerListLabel->setPosition(popupWidth-padAmount-(layerListWidth/2),layerListHeight+padAmount+3.0f);
    m_layerListLabel->setAnchorPoint({0.5f,0.0f});

    m_mainLayer->addChild(m_layerListLabel);
    

    //create the menu for test stuff (give this nicer ui later)
    //auto actionButtonMenu = CCMenu::create();
    //
    //actionButtonMenu->setLayout(AxisLayout::create()
    //    ->setAxis(Axis::Column)
    //    ->setAxisAlignment(AxisAlignment::Start)
    //    ->setCrossAxisLineAlignment(AxisAlignment::Start)
    //);
    //actionButtonMenu->setAnchorPoint({0.0f,0.0f});
    //actionButtonMenu->setPosition({padAmount,padAmount});
    //actionButtonMenu->setScale(0.5f);
    //
    ////this one just works whenever
    //auto dev_createSetupButton = CCMenuItemSpriteExtra::create(
    //    ButtonSprite::create("Create Parallax Setup"),
    //    this,
    //    menu_selector(ParallaxMenuPopup::onCreateSetupButton)
    //);
    // 
    //auto dev_stupidLabel = Label::create("TEMPORARY DEV BUTTONS","goldFont.fnt");
    //actionButtonMenu->addChild(dev_createSetupButton);
    //actionButtonMenu->addChild(dev_stupidLabel);
    //actionButtonMenu->updateLayout();

    //m_mainLayer->addChild(actionButtonMenu);

    //add a label for the layer list
    m_layerListHint = Label::create("","bigFont.fnt");
    m_mainLayer->addChild(m_layerListHint);
    m_layerListHint->setPosition(popupWidth-(padAmount*1.25f)-layerListWidth+(m_layerListScrollBar->getPositionX()/2),padAmount+(layerListHeight/2));
    m_layerListHint->setScale(0.3f);
    m_layerListHint->setColor(constants::ui::disabledColor);
    m_layerListHint->setOpacity(constants::ui::disabledAlpha);


    constexpr float setupOptionsBackgroundWidth = 170.0f;
    constexpr float setupOptionsWidth = setupOptionsBackgroundWidth-(2*padAmount);

    auto setupOptionsBackground = NineSlice::create("square02b_001.png");
    setupOptionsBackground->setColor({0, 0, 0});
    setupOptionsBackground->setOpacity(44);
    setupOptionsBackground->setAnchorPoint({0.0f,0.5f});
    setupOptionsBackground->setPosition(padAmount,popupHeight/2);
    m_mainLayer->addChild(setupOptionsBackground);


    /*
        add the duration input
    */
    constexpr float durationInputScale = 0.5f;
    auto durationInputMenu = CCMenu::create();
    setupOptionsBackground->addChild(durationInputMenu);

    m_durationInput = CustomNumberInput::create(100.0f);

    auto durationInputLabel = Label::create("Duration:","bigFont.fnt");
        
    m_durationInput->setCallback([this](const std::string&){
        auto setup = getSelectedSetup();
        if(!setup) return;
        
        //calls whenever the text is changed
        //get the input value as a float
        std::string durationInputString = m_durationInput->getString();
        auto durationResult = geode::utils::numFromString<float>(durationInputString);
        float inputDuration = 0.0f;
        if (durationResult) {
            inputDuration = durationResult.unwrap();
        }

        setup->setDuration(inputDuration);
    });

    auto makeDurationInfiniteButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Inf."),
        this,
        menu_selector(ParallaxMenuPopup::onMakeDurationInfiniteButton)
    );
    
    durationInputMenu->addChild(durationInputLabel);
    durationInputMenu->addChild(m_durationInput);
    durationInputMenu->addChild(makeDurationInfiniteButton);
    durationInputMenu->setLayout(
        AxisLayout::create()
        ->setAxis(Axis::Row)
    );
    durationInputMenu->setPosition(padAmount,padAmount);
    durationInputMenu->setAnchorPoint({0.0f,0.0f});
    durationInputMenu->setContentSize({setupOptionsWidth/durationInputScale,1.0f});
    durationInputMenu->setScale(durationInputScale);
    durationInputMenu->updateLayout();

    
    //the actions
    constexpr float setupActionButtonSize = 33.0f;
    constexpr float actionButtonScale = 0.75f;

    auto setupActionMenu = CCMenu::create();
    setupActionMenu->setLayout(
        AxisLayout::create()
        ->setAxis(Axis::Row)
    );
    setupActionMenu->setAnchorPoint({0.0f,0.0f});
    setupActionMenu->setPosition(padAmount,30);
    setupActionMenu->setContentSize({setupOptionsWidth/actionButtonScale,setupActionButtonSize});
    setupActionMenu->setScale(actionButtonScale);
    setupOptionsBackground->addChild(setupActionMenu);

    m_findSetupButton = Button::createWithSpriteFrameName("gj_findBtn_001.png", [this](Button* btn) {
        this->onFindSetupInEditorButton(btn);
    });
    m_findSetupButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Find Setup In Editor"));
    setupActionMenu->addChild(m_findSetupButton);
    m_cleanSetupButton = Button::createWithSprite("CleanUpButton.png"_spr, [this](Button* btn) {
        this->onCleanupTriggersButton(btn);
    });
    m_cleanSetupButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Clean Up Setup"));
    setupActionMenu->addChild(m_cleanSetupButton);
    m_deleteSetupButton = Button::createWithSprite("deleteButton.png"_spr, [this](Button* btn) {
        this->onDeleteSetupButton(btn);
    });
    m_deleteSetupButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Delete Setup"));
    setupActionMenu->addChild(m_deleteSetupButton);
    

    setupActionMenu->updateLayout();

    //just make sure this matches up
    m_findSetupButton->setContentSize({setupActionButtonSize,setupActionButtonSize});
    m_cleanSetupButton->setContentSize({setupActionButtonSize,setupActionButtonSize});

    //resize the background
    setupOptionsBackground->setContentSize({setupOptionsBackgroundWidth,30+(setupActionButtonSize*actionButtonScale)+padAmount});

    init_createSetupSwitcher();
    init_createInfoButtons();

    updateAllUI();
    updateLayout();

    //the inputs
    m_upListener = listenForKeybindSettingPresses("keybind-layerlist-up", [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down) {
            this->changeFocusedLayer(-1);
        }
    });
	m_downListener = listenForKeybindSettingPresses("keybind-layerlist-down", [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down) {
            this->changeFocusedLayer(1);
        }
    });

    return true;
}
void ParallaxMenuPopup::onMakeDurationInfiniteButton(CCObject *){
    auto setup = getSelectedSetup();
    if(!setup) return;
    m_durationInput->setString("-1");
    setup->setDuration(-1.0f);
}

void ParallaxMenuPopup::updateSetupActionButtons()
{
    auto setup = getSelectedSetup();
    if(setup){
        enableNode(m_addLayerButton);

        enableNode(m_cleanSetupButton);
        enableNode(m_findSetupButton);
        enableNode(m_deleteSetupButton);
    }else{
        disableNode(m_addLayerButton);
        
        disableNode(m_cleanSetupButton);
        disableNode(m_findSetupButton);
        disableNode(m_deleteSetupButton);
    }
}

void ParallaxMenuPopup::updateLayerListHint()
{
    auto setup = getSelectedSetup();
    if(setup){
        if(setup->m_layers.empty()){
            m_layerListHint->setVisible(true);
            m_layerListHint->setText("(Press add to create layers)");
        }else{
            m_layerListHint->setVisible(false);
        }
    }else{
        m_layerListHint->setVisible(true);
        m_layerListHint->setText("(Create a setup to add layers)");
    }
}

void ParallaxMenuPopup::updateSetupSelector()
{    
    //exit if there is no setup selector
    if(!m_setupSelectorLabel) return;

    bool enablePrev = m_selectedSetupIndex>0;
    bool enableNext = (m_selectedSetupIndex+1)<(m_parallaxSetupList.m_setups.size());

    if(enablePrev) enableNode(m_setupSwitcherPrevButton);
    else disableNode(m_setupSwitcherPrevButton);

    if(enableNext) enableNode(m_setupSwitcherNextButton);
    else disableNode(m_setupSwitcherNextButton);
    

    int setupCount = m_parallaxSetupList.m_setups.size();
    int setupIndex = (setupCount==0) ? 0 : m_selectedSetupIndex+1;

    m_setupSelectorLabel->setText(fmt::format("{}/{}",setupIndex,setupCount));
}
void ParallaxMenuPopup::updateSetupDurationInput()
{
    auto setup = getSelectedSetup();
    if(!setup) return; 
        
    m_durationInput->setString(setup->getDurationString());
}


//TODO: clean this up
//to avoid explicit in copy-initialization build issues on android create a set of empty groups for finding a group ID like this
//this could be replaced with nwo api editor::nextFreeGroup(); however that doesnt update if i give the group to an object
const gd::unordered_set<int> noExcludeGroups{};

void ParallaxMenuPopup::onAddLayerButton(CCObject *){

    if(!m_editorLayer) return;

    auto setup = getSelectedSetup();
    if(!setup) return;

    auto newTriggersPosition = setup->getPositionForNewLayerTriggers();

    //first add the triggers
    auto newScaleTrigger = static_cast<TransformTriggerGameObject*>(m_editorLayer->createObject(trigger::SCALE_TRIGGER,newTriggersPosition,false));
    auto newFollowTrigger = static_cast<EffectGameObject*>(m_editorLayer->createObject(trigger::FOLLOW_TRIGGER,newTriggersPosition+CCPoint{editor::constants::GRID_SIZE,0.0f},false));
    //give them the correct groups
    int newLayerGroupID = m_editorLayer->getNextFreeGroupID(noExcludeGroups);
    //set the target gid
    newFollowTrigger->m_targetGroupID = newLayerGroupID;
    newScaleTrigger->m_targetGroupID = newLayerGroupID;
    //have the follow trigger follow the followid
    newFollowTrigger->m_centerGroupID = setup->m_followID;
    //the scale can be either the follow or center, im gonna use the center here
    //this should probably be able to be changed in settings later
    newScaleTrigger->m_centerGroupID = setup->m_rootID;
    //set the lengths
    //TODO: have this get the value in the duration input for the first trigger
    trigger::setDuration(newScaleTrigger,0.0f);
    trigger::setDuration(newFollowTrigger,setup->getDuration());
    
    //make sure theyre on the right editor layer
    

    //create a new layer object
    auto newLayer = setup->addLayer(newScaleTrigger,newFollowTrigger);
    newLayer->setTriggerValuesByDepth(0.0f);

    //next add it to the ui
    addLayerNodeToList(newLayer);
    m_layerListMenu->updateLayout();
    m_scrollLayer->updateLayout();

    //update the group id
    LevelEditorLayer::updateObjectLabel(newScaleTrigger);
    LevelEditorLayer::updateObjectLabel(newFollowTrigger);

    updateAllUI();
    
    //scroll to the index of the new
    scrollToLayerIndex(setup->getLayerCount()-1);
}
void ParallaxMenuPopup::onCleanupTriggersButton(CCObject *)
{
    //PROBABLY ADD A CONFIRMATION SCREEN

    //but uhh for now just do it
    auto setup = getSelectedSetup();
    if(!setup) return;

    auto basePosition = setup->m_areaMoveTriggerPtr->getPosition();
    //use the area move as the base
    //the follow trigger goes next to the area move trigger
    editor::object::move(setup->m_advancedFollowTriggerPtr,{basePosition.x+editor::constants::GRID_SIZE,basePosition.y});

    //for now just go through all the layers (maybe sort them later though)
    float y = basePosition.y;
    for(auto& layer : setup->m_layers){
        y+=editor::constants::GRID_SIZE;

        editor::object::move(layer.m_scaleTriggerPtr,{basePosition.x,y});
        editor::object::move(layer.m_followTriggerPtr,{basePosition.x+editor::constants::GRID_SIZE,y});
    }

    //move the objects below the move trigger
    //there should only be 1 of each but this wont account for that yet
    auto rootFollowObjPos = basePosition-CCPoint{0.0f,editor::constants::GRID_SIZE};

    auto rootObjs = editor::objectsWithGroup(setup->m_rootID);
    auto followObjs = editor::objectsWithGroup(setup->m_followID);

    for(auto obj : CCArrayExt<GameObject*>(rootObjs)) editor::object::move(obj,rootFollowObjPos);
    for(auto obj : CCArrayExt<GameObject*>(followObjs)) editor::object::move(obj,rootFollowObjPos);
}

void ParallaxMenuPopup::onDeleteSetupButton(CCObject *)
{
    createQuickPopup(
        "Warning!",
        "This will delete this setup along with all of its layers and center objects.\n"
        "Are you sure you want to delete this setup?",
        "Cancel",
        "Delete",
        420.0f,
        [this](FLAlertLayer*,bool confirmed){
            if(confirmed){
                auto setup = getSelectedSetup();
                if(!setup) return;

                setup->deleteAllLayersAndLayerTriggers();

                editor::object::remove(setup->m_areaMoveTriggerPtr);
                editor::object::remove(setup->m_advancedFollowTriggerPtr);

                setup->deleteAllRootAndFollowObjects();
            
                //now remove the setup from the list and update the ui
                m_parallaxSetupList.removeSetupByIndex(m_selectedSetupIndex);

                //reload ui
                size_t setupCount = m_parallaxSetupList.m_setups.size();
                if(m_selectedSetupIndex>=setupCount)
                    m_selectedSetupIndex = setupCount-1;

                loadSetupLayerList(getSelectedSetup());
                updateAllUI();
            }
        },
        true,true
    );
}

void ParallaxMenuPopup::onCreateSetupButton(CCObject *)
{
    auto newSetupPos = editor::center();
    //snap to the grid
    //maybe make this optional later
    newSetupPos.x = (floor(newSetupPos.x/editor::constants::GRID_SIZE)+0.5f)*editor::constants::GRID_SIZE;
    newSetupPos.y = (floor(newSetupPos.y/editor::constants::GRID_SIZE)+0.5f)*editor::constants::GRID_SIZE;

    //first create the area move triggre
    auto newAreaMoveTrigger = static_cast<EnterEffectObject*>(m_editorLayer->createObject(
        trigger::AREA_MOVE_TRIGGER,
        newSetupPos+CCPoint{0.0f,0.0f},
        false
    ));
    auto newAdvancedFollowTrigger = static_cast<AdvancedFollowTriggerObject*>(m_editorLayer->createObject(
        trigger::ADVANCED_FOLLOW_TRIGGER,
        newSetupPos+CCPoint{editor::constants::GRID_SIZE,0.0f},
        false
    ));

    //setup the area move trigger
    newAreaMoveTrigger->m_specialTarget = -3;//target c
    newAreaMoveTrigger->m_length = 9000;
    newAreaMoveTrigger->m_moveDistance = -9000;
    newAreaMoveTrigger->m_relative = true;
    newAreaMoveTrigger->m_directionType=0;
    newAreaMoveTrigger->m_inbound=true;
    //give it a group
    int rootID = m_editorLayer->getNextFreeGroupID(noExcludeGroups);
    trigger::setTarget(newAreaMoveTrigger, rootID);
    int followID = m_editorLayer->getNextFreeGroupID(noExcludeGroups);
    trigger::setTarget(newAdvancedFollowTrigger, followID);
    trigger::setCenter(newAdvancedFollowTrigger, rootID);

    //now add the two root and follow objects
    //these can be any object so im making them look like a gd icon cuz its cute ig

    //cuz i cant write this down in mod.json
    //	constexpr int rootObjectID = 3816;//icon face particle
    //	constexpr int followObjectID = 3805;//hollow square particle
    int rootObjectID = Mod::get()->getSettingValue<int>("int-root-object-ID");
    int followObjectID = Mod::get()->getSettingValue<int>("int-follow-object-ID");

    auto rootObject = m_editorLayer->createObject(
        rootObjectID,
        newSetupPos+CCPoint{0.0f,-editor::constants::GRID_SIZE},
        false
    );
    auto followObject = m_editorLayer->createObject(
        followObjectID,
        newSetupPos+CCPoint{0.0f,-editor::constants::GRID_SIZE},
        false
    );

    rootObject->addToGroup(rootID);
    followObject->addToGroup(followID);

    editor::object::scale(rootObject,Mod::get()->getSettingValue<float>("float-root-object-scale"));
    editor::object::scale(followObject,Mod::get()->getSettingValue<float>("float-follow-object-scale"));
    
    //add the new setup to the list and select it
    m_selectedSetupIndex = m_parallaxSetupList.m_setups.size();
    m_parallaxSetupList.addSetup(newAreaMoveTrigger,newAdvancedFollowTrigger);
    loadSetupLayerList(getSelectedSetup());
    updateAllUI();
}

void ParallaxMenuPopup::onFindSetupInEditorButton(CCObject *)
{
    auto setup = getSelectedSetup();
    if(!setup) return;

    auto setupPosition = setup->m_areaMoveTriggerPtr->getPosition();
    editor::move(setupPosition);
}
ParallaxMenuPopup *ParallaxMenuPopup::create(MyEditorUI *editorUI)
{
    auto ret = new ParallaxMenuPopup();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr; 
}
ParallaxSetup * ParallaxMenuPopup::getSelectedSetup()
{
    //if m_selectedSetupIndex is changed to be an unsigned int later the less than zero check isnt needed
    if(m_selectedSetupIndex<0) return nullptr;
    if(m_selectedSetupIndex>=m_parallaxSetupList.m_setups.size()) return nullptr;
    return &m_parallaxSetupList.m_setups[m_selectedSetupIndex];
}
void ParallaxMenuPopup::scrollToLayerIndex(int index)
{
    float targetY = LAYER_NODE_HEIGHT*float(index);
    //offset it to be the middle of the node
    targetY+=LAYER_NODE_HEIGHT/2.0f;
    //offset to be in the middle of the list
    targetY-=LAYER_LIST_SCROLL_LAYER_HEIGHT/2.0f;

    m_scrollLayer->setScrollY(targetY,true);
}
ParallaxMenuPopup::~ParallaxMenuPopup()
{
    Popup::~Popup();

    if(m_upListener)
        m_upListener->destroy();
	
    if(m_downListener)
        m_downListener->destroy();
}
void ParallaxMenuPopup::updateAllUI()
{
    updateSetupActionButtons();
    updateLayerListHint();
    updateSetupSelector();
    updateSetupDurationInput();
}
void ParallaxMenuPopup::addLayerNodeToList(ParallaxSetupLayer *layer)
{
    //if we need to add a new node do that
    //this could probably be optimized later
    auto existingLayers = m_layerListMenu->getChildrenExt();

    if (m_nextLayerListNode >= existingLayers.size()){
        auto layerNode = ParallaxMenuLayerNode::create({layerListWidth,LAYER_NODE_HEIGHT},layer);
        m_layerListMenu->addChild(layerNode);
    }else{
        auto layerNode = static_cast<ParallaxMenuLayerNode*>(existingLayers[m_nextLayerListNode]);
        layerNode->setLayer(layer);
        layerNode->setVisible(true);
    }

    m_nextLayerListNode++;
}

size_t ParallaxMenuPopup::getFocusedLayer()
{
    auto setup = getSelectedSetup();
    if(!setup) return SIZE_MAX;

    auto existingLayers = m_layerListMenu->getChildrenExt();
    //first get the focused layer
    for(size_t i=0;i<setup->getLayerCount();i++){
        auto layerNode = static_cast<ParallaxMenuLayerNode*>(existingLayers[i]);
        if(layerNode->getFocused()) return i;
    }

    return SIZE_MAX;
}

void ParallaxMenuPopup::changeFocusedLayer(int indexOffset)
{
    auto setup = getSelectedSetup();
    if(!setup) return;

    size_t focusedLayer = getFocusedLayer();
    if(focusedLayer==SIZE_MAX) return;//if there is no focused layer

    size_t targetLayer = focusedLayer;
    //for now only support +1 and -1 cuz unsigned means i cant use negative numbers
    if(indexOffset<0){
        //-1
        if(focusedLayer==0){
            targetLayer = setup->getLayerCount()-1;
        }else{
            targetLayer--;
        }
    }else{
        //+1
        targetLayer++;
        if(targetLayer>=setup->getLayerCount()){
            targetLayer=0;
        }
    }

    auto existingLayers = m_layerListMenu->getChildrenExt();
    static_cast<ParallaxMenuLayerNode*>(existingLayers[targetLayer])->focus();

    scrollToLayerIndex(targetLayer);
}
