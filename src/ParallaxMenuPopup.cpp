#include "ParallaxMenuPopup.hpp"


#include "ParallaxMenuLayerNode.hpp"

#include "constants.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

constexpr float popupWidth = 380;
constexpr float popupHeight = 290;

constexpr float padAmount = 10;
constexpr float layerListWidth = 200.0f;
constexpr float layerListHeight = 240.0f;

constexpr float layerListX = popupWidth-padAmount;

void ParallaxMenuPopup::loadSetupLayerList(ParallaxSetup *setup)
{
    m_layerListNode->removeAllChildrenWithCleanup(true);

    if(setup){
        auto sortedLayers = setup->sortDepth();
        for(auto layerPtr : sortedLayers){
            addLayerNodeToList(layerPtr);
        }
    }

    //update everything in the right order
    m_layerListNode->updateLayout();
    m_scrollLayer->updateLayout();
    m_layerListBackground->updateLayout();
}

void ParallaxMenuPopup::init_createSetupSwitcher()
{
    //create the setup swticher (maybe make this a custom node later?)
    const float setupSwitcherWidth = 90;
    const float setupSwitcherHeight = 20;
    const float setupSwitcherButtonScale = 0.8f;
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

    m_mainLayer->addChild(setupSwitcherMenu);
}

//todo: clean up this init ui function
bool ParallaxMenuPopup::init(MyEditorUI *editorUI)
{

    if (!Popup::init({popupWidth, popupHeight})) return false;
    //disable the popup animation
    m_noElasticity = true;

    //save these pointers
    m_editorUI = editorUI;
    m_editorLayer = editorUI->m_editorLayer;
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

    m_layerListNode = CCNode::create();
    m_layerListNode->setLayout(
        AxisLayout::create(Axis::Column)
        ->setAutoGrowAxis({0.0f})
        ->setGap(0.0f)
    );
    //setup a scroll layer to make it scrollable
    //this is where the size of the entire scrolling box thing is adjusted
    m_scrollLayer = AdvancedScrollLayer::create({layerListWidth,260});
    m_scrollLayer->setAnchorPoint({0.0f,0.0f});
    m_scrollLayer->setLayout(AxisLayout::create());

    
    m_layerListScrollBar = AdvancedScrollBar::create(m_scrollLayer,ScrollOrientation::VERTICAL);

    m_layerListBackground->addChild(m_scrollLayer);
    m_layerListBackground->addChild(m_layerListScrollBar);
    
    m_scrollLayer->addChild(m_layerListNode);

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

    addLayerMenu->addChild(m_addLayerButton);
    addLayerMenu->updateLayout();
    m_mainLayer->addChild(addLayerMenu);

    /*
        CREATE THE LABEL FOR THE LAYER LIST
    */
    auto layerListLabel = Label::create("Layers","goldFont.fnt");
    //place it right above the layer list
    //try to line the y up with the sort button
    layerListLabel->setPosition({popupWidth-padAmount-(layerListWidth/2),layerListHeight+padAmount+3.0f});
    layerListLabel->setAnchorPoint({0.5f,0.0f});

    m_mainLayer->addChild(layerListLabel);
    

    //create the menu for test stuff (give this nicer ui later)
    auto actionButtonMenu = CCMenu::create();

    actionButtonMenu->setLayout(AxisLayout::create()
        ->setAxis(Axis::Column)
        ->setAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::Start)
    );
    actionButtonMenu->setAnchorPoint({0.0f,0.0f});
    actionButtonMenu->setPosition({padAmount,padAmount});
    actionButtonMenu->setScale(0.5f);

    //this one just works whenever
    auto createSetupButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Create Parallax Setup"),
        this,
        menu_selector(ParallaxMenuPopup::onCreateSetupButton)
    );
    //these two work on the current setup
    auto cleanTriggersButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Clean up Triggers"),
        this,
        menu_selector(ParallaxMenuPopup::onCleanupTriggersButton)
    );
    auto findSetupInEditorButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Find in Editor"),
        this,
        menu_selector(ParallaxMenuPopup::onFindSetupInEditorButton)
    );

    auto stupidLabel = Label::create("TEMPORARY DEV BUTTONS","goldFont.fnt");

    
    actionButtonMenu->addChild(createSetupButton);
    
    actionButtonMenu->addChild(cleanTriggersButton);
    actionButtonMenu->addChild(findSetupInEditorButton);


    actionButtonMenu->addChild(stupidLabel);

    actionButtonMenu->updateLayout();
    
    m_mainLayer->addChild(actionButtonMenu);

    //add a label for the layer list
    m_layerListHint = Label::create("","bigFont.fnt");
    m_mainLayer->addChild(m_layerListHint);
    m_layerListHint->setPosition(popupWidth-(padAmount*1.25f)-layerListWidth+(m_layerListScrollBar->getPositionX()/2),padAmount+(layerListHeight/2));
    m_layerListHint->setScale(0.3f);
    m_layerListHint->setColor(constants::ui::disabledColor);
    m_layerListHint->setOpacity(constants::ui::disabledAlpha);


    init_createSetupSwitcher();

    updateAllUI();
    updateLayout();

    return true;
}

//idk where else to put this function
template <typename T>
void enableNode(T* item){
    item->setEnabled(true);
    item->setColor(ccColor3B{255,255,255});
    item->setOpacity(255);
}
template <typename T>
void disableNode(T* item){
    item->setEnabled(false);
    item->setColor(constants::ui::disabledColor);
    item->setOpacity(constants::ui::disabledAlpha);
}

void ParallaxMenuPopup::updateAddLayerButton()
{
    auto setup = getSelectedSetup();
    if(setup){
        enableNode(m_addLayerButton);
    }else{
        disableNode(m_addLayerButton);
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
    
    m_setupSelectorLabel->setText(fmt::format("{}/{}",m_selectedSetupIndex+1,m_parallaxSetupList.m_setups.size()));
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
    auto newScaleTrigger = static_cast<TransformTriggerGameObject*>(m_editorLayer->createObject(constants::objectID::SCALE_TRIGGER,newTriggersPosition,false));
    auto newFollowTrigger = static_cast<EffectGameObject*>(m_editorLayer->createObject(constants::objectID::FOLLOW_TRIGGER,newTriggersPosition+CCPoint{editor::constants::GRID_SIZE,0.0f},false));
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
    float defaultFollowDuration = -1.0f;//should be infinite (idk why you cant set it to this in game)
    newScaleTrigger->m_duration = 0.0f;
    newFollowTrigger->m_duration = defaultFollowDuration;
    
    //create a new layer object
    auto newLayer = setup->addLayer(newScaleTrigger,newFollowTrigger);
    newLayer->setTriggerValuesByDepth(0.0f);

    //next add it to the ui
    addLayerNodeToList(newLayer);
    m_layerListNode->updateLayout();
    m_scrollLayer->updateLayout();

    //update the group id
    LevelEditorLayer::updateObjectLabel(newScaleTrigger);
    LevelEditorLayer::updateObjectLabel(newFollowTrigger);

    updateAllUI();
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
        constants::objectID::AREA_MOVE_TRIGGER,
        newSetupPos+CCPoint{0.0f,0.0f},
        false
    ));
    auto newAdvancedFollowTrigger = static_cast<AdvancedFollowTriggerObject*>(m_editorLayer->createObject(
        constants::objectID::ADVANCED_FOLLOW_TRIGGER,
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
    newAreaMoveTrigger->m_targetGroupID=rootID;
    int followID = m_editorLayer->getNextFreeGroupID(noExcludeGroups);
    newAdvancedFollowTrigger->m_centerGroupID = rootID;
    newAdvancedFollowTrigger->m_targetGroupID = followID;
    //update these
    LevelEditorLayer::updateObjectLabel(newAreaMoveTrigger);
    LevelEditorLayer::updateObjectLabel(newAdvancedFollowTrigger);
    //now add the two root and follow objects
    //these can be any object so im making them look like a gd icon cuz its cute ig
    constexpr int rootObjectID = 3816;//icon face particle
    constexpr int followObjectID = 3805;//hollow square particle
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

    editor::object::scale(rootObject,0.5);
    //add the new setup to the list
    m_parallaxSetupList.addSetup(newAreaMoveTrigger,newAdvancedFollowTrigger);
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
void ParallaxMenuPopup::updateAllUI()
{
    updateAddLayerButton();
    updateLayerListHint();
    updateSetupSelector();
}
void ParallaxMenuPopup::addLayerNodeToList(ParallaxSetupLayer *layer)
{
    auto layerNode = ParallaxMenuLayerNode::create({layerListWidth,40},layer);
    m_layerListNode->addChild(layerNode);
}

