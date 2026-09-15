#include "ParallaxMenuPopup.hpp"


#include "ParallaxMenuLayerNode.hpp"

#include "ObjectID.hpp"


constexpr float popupWidth = 380;
constexpr float popupHeight = 290;

constexpr float padAmount = 10;
constexpr float layerListWidth = 200.0f;
constexpr float layerListHeight = 240.0f;
    
constexpr float editorTileSize = 30.0f;

bool ParallaxMenuPopup::init(LevelEditorLayer* editor,MyEditorUI* editorUI) {

    if (!Popup::init({popupWidth, popupHeight})) return false;
    //disable the popup animation
    m_noElasticity = true;

    //save these pointer
    m_editorLayer = editor;
    m_editorUI = editorUI;

    //set the title of the menu
    //this->setTitle("Parallax Helper");
    
    /*
        CREATE THE LAYER LIST
    */

    //setup the layer list maybe
    auto layerList_bg = NineSlice::create("square02b_001.png");
    layerList_bg->setColor({0, 0, 0});
    layerList_bg->setOpacity(44);
    //setup the position like this for now
    layerList_bg->setAnchorPoint({1.0f,0.0f});
    //aligning stuff witht he layer list
    constexpr float layerListX = popupWidth-padAmount;
    layerList_bg->setPosition({layerListX,padAmount+1.5});/*+1.5 to account for the shadow*/
    layerList_bg->setContentSize({layerListWidth,layerListHeight});

    layerList_bg->setLayout(
        AxisLayout::create()
        ->setPadding({5.0f,5.0f,5.0f,5.0f})
    );

    m_mainLayer->addChild(layerList_bg);

    //create the list of notes
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
    //make a bar for it
    auto scrollBar = AdvancedScrollBar::create(m_scrollLayer,ScrollOrientation::VERTICAL);
    


    layerList_bg->addChild(m_scrollLayer);
    layerList_bg->addChild(scrollBar);
    
    m_scrollLayer->addChild(m_layerListNode);

    //create the parallax setup list
	m_parallaxSetupList.scanEditorForSetups(editor);
    
    auto setup = getSelectedSetup();
    if(setup){
        //sort the list
        //make a list of pointers to sort
        std::vector<ParallaxSetupLayer*> layerPointers;
        layerPointers.reserve(setup->m_layers.size());
        for(auto& layer : setup->m_layers){
            layerPointers.push_back(&layer);
        }
        //now they can be sorted or whatever
        //sort by depth
        std::sort(layerPointers.begin(), layerPointers.end(),
            [](const ParallaxSetupLayer* a, const ParallaxSetupLayer* b) {
                return a->m_layerDepth < b->m_layerDepth;
            }
        );

        //add them to the screen
        for(auto layerPtr : layerPointers){
            addLayerNodeToList(layerPtr);
        }
    }
    //add nodes for testing
    //for(size_t i=0;i<5;i++){
    //    auto layerNode = ParallaxMenuLayerNode::create({layerListWidth,40});
    //    layerListNode->addChild(layerNode);
    //}

    //update everything in the right order
    m_layerListNode->updateLayout();
    m_scrollLayer->updateLayout();
    layerList_bg->updateLayout();

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
        nullptr/*callback*/
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
    auto addLayerButton = CCMenuItemSpriteExtra::create(
        EditorButtonSprite::createWithSpriteFrameName("edit_addCBtn_001.png"), 
        this, 
        menu_selector(ParallaxMenuPopup::onAddLayerButton)
    );
    addLayerMenu->addChild(addLayerButton);
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
    actionButtonMenu->setPosition({padAmount,padAmount+1.5f});
    actionButtonMenu->setScale(0.5f);

    auto createSetupButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Create Parallax Setup"),
        this,
        menu_selector(ParallaxMenuPopup::onCreateSetupButton)
    );
    auto cleanTriggersButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Clean up Triggers"),
        this,
        menu_selector(ParallaxMenuPopup::onCleanupTriggersButton)
    );

    actionButtonMenu->addChild(createSetupButton);
    actionButtonMenu->addChild(cleanTriggersButton);
    actionButtonMenu->updateLayout();
    
    m_mainLayer->addChild(actionButtonMenu);

    updateLayout();

    return true;
}

//TODO: clean this up
//to avoid build issues on android this needs to be a variable to use with nextFreeGroupID
const gd::unordered_set<int> excludegroups = {};

void ParallaxMenuPopup::onAddLayerButton(CCObject *){

    if(!m_editorLayer) return;

    auto setup = getSelectedSetup();
    if(!setup) return;
    //TODO: make a system for finding where to place the new layer triggers
    //possibly just locate the highest (or lowest depending on direction setting maybe) existing trigger and place it above or below

    //first add the triggers
    auto newFollowTrigger = static_cast<EffectGameObject*>(m_editorLayer->createObject(objectID::FOLLOW_TRIGGER,{0.0f,0.0f},false));
    auto newScaleTrigger = static_cast<TransformTriggerGameObject*>(m_editorLayer->createObject(objectID::SCALE_TRIGGER,{editorTileSize,0.0f},false));
    //give them the correct groups
    int newLayerGroupID = m_editorLayer->getNextFreeGroupID(excludegroups);
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
    auto newLayer = &setup->m_layers.emplace_back();
    newLayer->m_layerID = newLayerGroupID;
    newLayer->m_followTriggerPtr = newFollowTrigger;
    newLayer->m_scaleTriggerPtr = newScaleTrigger;
    newLayer->setTriggerValuesByDepth(0.0f);//this also sets the layer depth
    //next add it to the ui
    addLayerNodeToList(newLayer);
    m_layerListNode->updateLayout();
    m_scrollLayer->updateLayout();

    //update the group id
    LevelEditorLayer::updateObjectLabel(newScaleTrigger);
    LevelEditorLayer::updateObjectLabel(newFollowTrigger);
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
    m_editorUI->setObjectPosition(setup->m_advancedFollowTriggerPtr,{basePosition.x+editorTileSize,basePosition.y});

    //for now just go through all the layers (maybe sort them later though)
    float y = basePosition.y;
    for(auto& layer : setup->m_layers){
        y+=editorTileSize;

        m_editorUI->setObjectPosition(layer.m_scaleTriggerPtr,{basePosition.x,y});
        m_editorUI->setObjectPosition(layer.m_followTriggerPtr,{basePosition.x+editorTileSize,y});
    }

}
void ParallaxMenuPopup::onCreateSetupButton(CCObject *)
{
    //first create the area move triggre
    auto newAreaMoveTrigger = static_cast<EnterEffectObject*>(m_editorLayer->createObject(objectID::AREA_MOVE_TRIGGER,{0.0f,0.0f},false));
    auto newAdvancedFollowTrigger = static_cast<AdvancedFollowTriggerObject*>(m_editorLayer->createObject(objectID::ADVANCED_FOLLOW_TRIGGER,{editorTileSize,0.0f},false));
    //setup the area move trigger
    newAreaMoveTrigger->m_specialTarget = -3;//target c
    newAreaMoveTrigger->m_length = 9000;
    newAreaMoveTrigger->m_moveDistance = -9000;
    newAreaMoveTrigger->m_relative = true;
    newAreaMoveTrigger->m_directionType=0;
    newAreaMoveTrigger->m_inbound=true;
    //give it a group
    int rootID = m_editorLayer->getNextFreeGroupID(excludegroups);
    newAreaMoveTrigger->m_targetGroupID=rootID;
    int followID = m_editorLayer->getNextFreeGroupID(excludegroups);
    newAdvancedFollowTrigger->m_centerGroupID = rootID;
    newAdvancedFollowTrigger->m_targetGroupID = followID;
    //update these
    LevelEditorLayer::updateObjectLabel(newAreaMoveTrigger);
    LevelEditorLayer::updateObjectLabel(newAdvancedFollowTrigger);
    //now add the two root and follow objects
    //these can be any object so im making them look like a gd icon cuz its cute ig
    constexpr int rootObjectID = 3816;//icon face particle
    constexpr int followObjectID = 3805;//hollow square particle
    auto rootObject = m_editorLayer->createObject(rootObjectID,{0.0f,-editorTileSize},false);
    auto followObject = m_editorLayer->createObject(followObjectID,{0.0f,-editorTileSize},false);
    rootObject->addToGroup(rootID);
    followObject->addToGroup(followID);
    //for some reason you need to do both of these or it wont work
    //this is just visual anyway
    //rootObject->m_scaleX=0.5;
    //rootObject->m_scaleY=0.5;
    //rootObject->setScale(0.5f);
    rootObject->updateCustomScaleX(0.5);
    rootObject->updateCustomScaleY(0.5);
    

    //probably update the ui here now
}
ParallaxMenuPopup *ParallaxMenuPopup::create(LevelEditorLayer *editor,MyEditorUI* editorUI)
{
    auto ret = new ParallaxMenuPopup();
    if (ret->init(editor,editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
ParallaxSetup * ParallaxMenuPopup::getSelectedSetup()
{
    //for now just use the 0th one
    if(m_parallaxSetupList.m_setups.empty()) return nullptr;
    else return &m_parallaxSetupList.m_setups[0];
}
void ParallaxMenuPopup::addLayerNodeToList(ParallaxSetupLayer * layer)
{
    auto layerNode = ParallaxMenuLayerNode::create({layerListWidth,40},layer);
    m_layerListNode->addChild(layerNode);
}

