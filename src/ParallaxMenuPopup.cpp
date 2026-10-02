#include "ParallaxMenuPopup.hpp"
#include "ParallaxMenuLayerNode.hpp"

#include "Utils/constants.hpp"
#include "Utils/Settings.hpp"
#include "Utils/Utils.hpp"
#include "Utils/TriggerButton.hpp"
#include "Utils/NumberRequestPopup.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

#include "SortPopup.hpp"
#include "DuplicateAndLayerPopup.hpp"

constexpr float popupWidth = 430;
constexpr float popupHeight = 290;

constexpr float padAmount = 10;
constexpr float layerListWidth = 230.0f;
constexpr float layerListHeight = 240.0f;

constexpr float layerListX = popupWidth-padAmount;

constexpr float LAYER_NODE_HEIGHT = 40.0f;
constexpr float LAYER_LIST_SCROLL_LAYER_HEIGHT = 260.0f;

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
        auto sortedLayers = setup->sortBySortingType(Utils::getSortingType());
        for(auto layerPtr : sortedLayers){
            addLayerNodeToList(layerPtr);
        }
    }

    //update everything in the right order
    m_layerListMenu->updateLayout();
    m_scrollLayer->updateLayout();
    m_layerListBackground->updateLayout();
}

void ParallaxMenuPopup::initSetupSwitcher()
{
    //create the setup swticher (maybe make this a custom node later?)
    constexpr float setupSwitcherWidth = 90;
    constexpr float setupSwitcherHeight = 20;
    constexpr float setupSwitcherButtonScale = 0.8f;
    auto setupSwitcherMenu = CCMenu::create();
    //setupSwitcherMenu->setPosition({padAmount,popupHeight-padAmount-setupSwitcherHeight});
    setupSwitcherMenu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::TopLeft)
        ->setOffset({10.f,-30.f})
    );
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

void ParallaxMenuPopup::initInfoButtons()
{
    //this might be able to be moved where the label is created
    auto layerListInfoButton = Button::createWithSpriteFrameName("GJ_infoIcon_001.png", [this](Button*) {
        auto keybindsUp = ph::settings::keyLayerListUp.get();
        auto keybindsDown = ph::settings::keyLayerListDown.get();
        
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
void ParallaxMenuPopup::initLayerList()
{

    m_layerListBackground = NineSlice::create("square02b_001.png");
    m_layerListBackground->setColor({0, 0, 0});
    m_layerListBackground->setOpacity(44);
    m_layerListBackground->setAnchorPoint({1.0f,0.0f});

    m_layerListBackground->setContentSize({layerListWidth,layerListHeight});
    
    m_layerListBackground->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::BottomRight)
        ->setOffset({-padAmount,padAmount})
    );

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
    
    auto setup = getSelectedSetup();
    loadSetupLayerList(setup);
}
void ParallaxMenuPopup::initDevButtons()
{
    //create the menu for test stuff (give this nicer ui later)
    auto dev_actionButtonMenu = CCMenu::create();
    
    dev_actionButtonMenu->setLayout(AxisLayout::create()
        ->setAxis(Axis::Column)
        ->setAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::Start)
    );
    dev_actionButtonMenu->setAnchorPoint({0.0f,0.0f});
    dev_actionButtonMenu->setPosition({padAmount,padAmount+35.0f});
    dev_actionButtonMenu->setScale(0.45f);
    
    auto addDevBtn = [&](const char *caption,cocos2d::SEL_MenuHandler callback){
        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(caption),
            this,
            callback
        );
        dev_actionButtonMenu->addChild(btn);
        return btn;//cuz why not
    };

    addDevBtn("Duplicate and Layer Selected",menu_selector(ParallaxMenuPopup::onDuplicateAndLayerButton));
    addDevBtn("Create Quick Gradient",menu_selector(ParallaxMenuPopup::onCreateQuickGradientButton));
    

    //auto dev_deleteUnusedLayersButton = CCMenuItemSpriteExtra::create(
    //    ButtonSprite::create("Delete Unused Layers"),
    //    this,
    //    nullptr//TODO: impl
    //);

    auto dev_stupidLabel = Label::create("TEMPORARY DEV BUTTONS","goldFont.fnt");
    dev_actionButtonMenu->addChild(dev_stupidLabel);
    dev_actionButtonMenu->updateLayout();

    m_mainLayer->addChild(dev_actionButtonMenu);
}

//todo: clean up this init ui function
bool ParallaxMenuPopup::init()
{

    if (!Popup::init({popupWidth, popupHeight})) return false;
    //popup settings
    m_noElasticity = true;
    m_closeBtn->setVisible(false);

    //initialize the tootip
    auto tooltip = nwo5::ui::Tooltip::create("chatFont.fnt");
    tooltip->setScale(0.5f);//scale this down
    m_mainLayer->addChild(tooltip);
    
    m_parallaxSetupList.scanEditorForSetups(editor::layer());
    
    initLayerList();

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
    m_sortButton = Button::createWithNode(
        EditorButtonSprite::createWithSpriteFrameName("GJ_sortIcon_001.png"), 
        [this](Button* btn){
            SortPopup::create([this](SortPopup* popup){
                //save the sorting and then refresh
                Mod::get()->setSavedValue<int>(constants::keystrings::SAVED_SELECTED_SORTING_ID, popup->getSelectedSortingID());
                //re-sort the menu
                loadSetupLayerList(getSelectedSetup());
            })->show();
        }
    );
    m_sortButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Change Sorting"));

    sortMenu->addChild(m_sortButton);
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
    
    m_addLayerButton = Button::createWithNode(
        EditorButtonSprite::createWithSpriteFrameName("edit_addCBtn_001.png"), 
        [this](Button* btn){
            this->onAddLayerButton(btn);
        }
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
    
    //add a label for the layer list
    m_layerListHint = Label::create("","bigFont.fnt");
    m_mainLayer->addChild(m_layerListHint);
    m_layerListHint->setPosition(popupWidth-(padAmount*1.25f)-layerListWidth+(m_layerListScrollBar->getPositionX()/2),padAmount+(layerListHeight/2));
    m_layerListHint->setScale(0.3f);
    m_layerListHint->setColor(constants::ui::DISABLED_COLOR);
    m_layerListHint->setOpacity(constants::ui::DISABLED_ALPHA);


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
        
        setup->setDuration(m_durationInput->getNumber<float>());
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
    durationInputMenu->setPosition(padAmount,padAmount+64);
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
    setupActionMenu->setPosition(padAmount,30+64);
    setupActionMenu->setContentSize({setupOptionsWidth/actionButtonScale,setupActionButtonSize});
    setupActionMenu->setScale(actionButtonScale);
    setupOptionsBackground->addChild(setupActionMenu);

    m_findCenterButton = Button::createWithSpriteFrameName("gj_findBtn_001.png", [this](Button* btn) {
        this->onFindCenterButton(btn);
    });
    m_findCenterButton->setUserObject("nwo5.silly-api/tooltip", nwo5::ui::TooltipInfo::create("Find Center Objects In Editor"));
    setupActionMenu->addChild(m_findCenterButton);
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

    //create the root id and follow id buttons
    auto rootIDMenu = CCMenu::create();
    rootIDMenu->setLayout(AnchorLayout::create());
    rootIDMenu->setContentSize({0,0});

    auto createRootOrFollowMenu = [&](const geode::ZStringView& label,int triggerObjID,float yoffset, geode::Button::ButtonCallback btncb){

        auto rootIDMenuLabel = Label::create(label,"bigFont.fnt");
        rootIDMenuLabel->setAnchorPoint({1.0f,0.5f});
        rootIDMenuLabel->setLayoutOptions(
            AnchorLayoutOptions::create()
            ->setAnchor(Anchor::Right)
            ->setOffset({-16.0,yoffset})
        );
        rootIDMenuLabel->setScale(0.5f);
        //TODO: make an abstract way of creating a trigger button later
        auto rootIDSpriteButton = TriggerButton::create(std::move(btncb));
        rootIDSpriteButton->addTrigger(triggerObjID);

        rootIDSpriteButton->setLayoutOptions(
            AnchorLayoutOptions::create()
            ->setAnchor(Anchor::Right)
            ->setOffset({0.0,yoffset})
        );

        rootIDMenu->addChild(rootIDMenuLabel);
        rootIDMenu->addChild(rootIDSpriteButton);

        return rootIDSpriteButton;
    };    
    m_setupRootIDButton = createRootOrFollowMenu("Root ID:",nwo5::editor::trigger::AREA_MOVE_TRIGGER,16,[this](Button*){
        auto setup = getSelectedSetup();
        if(!setup) return;
        
        auto popup = NumberRequestPopup::create(
            [this,setup](NumberRequestPopup* popup, bool didConfirm){
                if(didConfirm){
                    setup->changeRootID(popup->m_numberInput->getNumber<int>());
                    updateRootIDButton();
                }
            },
            "Change Root ID",
            "Enter a value to change the root ID of this setup.\n"
            "This will also replace the previous ID in any object that already has it."
        );

        popup->m_numberInput->setNumber(setup->m_rootID);
        popup->show();
    });
    m_setupFollowIDButton = createRootOrFollowMenu("follow ID:",nwo5::editor::trigger::ADVANCED_FOLLOW_TRIGGER,-16,[this](Button*){
        auto setup = getSelectedSetup();
        if(!setup) return;
        
        auto popup = NumberRequestPopup::create(
            [this,setup](NumberRequestPopup* popup, bool didConfirm){
                if(didConfirm){
                    setup->changeFollowID(popup->m_numberInput->getNumber<int>());
                    updateFollowIDButton();
                }
            },
            "Change Follow ID",
            "Enter a value to change the follow ID of this setup.\n"
            "This will also replace the previous ID in any object that already has it."
        );

        popup->m_numberInput->setNumber(setup->m_followID);
        popup->show();
    });
    
    updateRootIDButton();
    updateFollowIDButton();
    rootIDMenu->setPosition(setupOptionsBackgroundWidth-padAmount,30);

    setupOptionsBackground->addChild(rootIDMenu);
    rootIDMenu->updateLayout();

    //resize the background
    setupOptionsBackground->setContentSize({setupOptionsBackgroundWidth,30+64+(setupActionButtonSize*actionButtonScale)+padAmount});


    //create the ok button that closes the ui
    auto okButton = Button::createWithNode(ButtonSprite::create("OK",0.8f),[this](Button*){
        this->onClose(this);
    });
    okButton->setPosition(padAmount+(setupOptionsBackgroundWidth/2),padAmount+(30/2));//buttons are 30 units tall
    m_mainLayer->addChild(okButton);


    initSetupSwitcher();
    initInfoButtons();

    //initDevButtons();

    updateAllUI();
    m_mainLayer->updateLayout();

    //the inputs
    m_upListener = listenForKeybindSettingPresses(ph::settings::keyLayerListUp.key(), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down) {
            this->changeFocusedLayer(-1);
        }
    });
	m_downListener = listenForKeybindSettingPresses(ph::settings::keyLayerListDown.key(), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down) {
            this->changeFocusedLayer(1);
        }
    });

    return true;
}

void ParallaxMenuPopup::onMakeDurationInfiniteButton(CCObject *)
{
    auto setup = getSelectedSetup();
    if(!setup) return;
    // "Inf" also works to represent infinity apparently so maybe add that as an option later
    m_durationInput->setString("-1");
    setup->setDuration(-1.0f);
}


void ParallaxMenuPopup::onDuplicateAndLayerButton(CCObject *)
{
    auto setup = getSelectedSetup();
    if(!setup) return;

    if(editor::selection::empty()){
        Notification::create("Please Select Some Objects First.")->show();
        return;
    }

    DuplicateAndLayerPopup::create([this](float rangeMin,float rangeMax, int count){
        auto setup = this->getSelectedSetup();
        if(!setup) return;


        if(editor::selection::empty()) return;//shouldnt be possible but better safe than sorry

        //now setup wich groups the objects need to be given
        //for now just create them all
        std::vector<ParallaxSetupLayer*> layers(count,nullptr);
        for(size_t i=0;i<count;i++){
            float v = float(i)/float(count-1);

            float depth = std::lerp(rangeMin,rangeMax,v);
            //later im gonna make it reuse existing layers
            layers[i] = setup->createNewLayer(depth);
        }




        //get the amount that the z order needs to be changed by
        int minZorder = 0;
        int maxZorder = 0;
        bool isFirst = false;
        for(auto& obj : editor::selection::getExt()){
            int zOrder = obj->getZOrder();

            if(!isFirst){
                minZorder = zOrder;
                maxZorder = zOrder;
                isFirst=true;
                continue;
            }

            if(zOrder>maxZorder) maxZorder = zOrder;
            if(zOrder<minZorder) minZorder = zOrder;
        }
        //now calculate it
        int zOrderOffset = (maxZorder - minZorder) + 1;

        auto objString = editor::ui()->copyObjectsDetailed(editor::selection::get());
        auto center = editor::selection::center();

        //delete the original
        editor::object::remove(editor::selection::get());

        //duplicate it!
        //TODO: add an option for this to go either backwards or forwards
        for(size_t i = 0; i<count;i++){
            auto pastedObjs = editor::ui()->pasteObjects(objString,true,false);
            editor::object::moveBy(pastedObjs,center);    

            //change the z order
            for(auto& obj : CCArrayExt<GameObject*>(pastedObjs)){
                //setZOrder doesnt work for some reason
                obj->m_zOrder = (obj->getZOrder() - (zOrderOffset * i));
            }

            editor::object::addGroup(pastedObjs,layers[i]->m_layerID);
        }

        //Notification::create(fmt::format("{} to {} | count: {}",rangeMin,rangeMax,count))->show();
        this->onClose(this);
    })->show();
}

void ParallaxMenuPopup::onCreateQuickGradientButton(CCObject *)
{
    int selObjectCount = editor::selection::count();
    if((selObjectCount < 3) || (selObjectCount > 4)){
        Notification::create("Select 3 or 4 objects.")->show();        
        return;
    }

    std::array<int,4> vertexGroupIds = {{0,0,0,0}};
    
    //sort objects to be in the order: BL BR TL TR
    auto selectedObjects = editor::selection::getExt();
    //TODO: fix thissss

    //put the pointers in an array
    std::array<GameObject*,4> objArray = {{
        selectedObjects[0],
        selectedObjects[1],
        selectedObjects[2],
        (selObjectCount >= 4) ? selectedObjects[3] : selectedObjects[2]
    }};

    //sort from top to bottom
    std::ranges::sort(objArray,
        [](GameObject* a, GameObject* b) {
            return a->getPositionY() < b->getPositionY();
        }
    );
    //then sort from left to right
    if(objArray[0]->getPositionX() > objArray[1]->getPositionX())
        std::swap(objArray[0],objArray[1]);

    if(objArray[2]->getPositionX() > objArray[3]->getPositionX())
        std::swap(objArray[2],objArray[3]);

    size_t i = 0;
    for(auto& obj : objArray){
        //skip the final object if only 3 were selected
        if((selObjectCount <= 3) && (i >= 3)) continue;

        vertexGroupIds[i] = Utils::getNextFreeGroupID();
        obj->addToGroup(vertexGroupIds[i]);
        i++;
    }
    //now create the gradient trigger
    auto gradientTrigger = static_cast<GradientTriggerObject*>(editor::object::createObject(
        editor::trigger::GRADIENT_TRIGGER,
        editor::ui()->getGridSnappedPos(editor::center())
    ));

    gradientTrigger->m_vertexMode = true;
    gradientTrigger->m_gradientID = editor::layer()->getNextFreeGradientID(constants::EMPTY_SET);
    
    gradientTrigger->m_upBottomLeftID = vertexGroupIds[0];
    gradientTrigger->m_downBottomRightID = vertexGroupIds[1];
    gradientTrigger->m_leftTopLeftID = vertexGroupIds[2];
    if(selObjectCount==4) gradientTrigger->m_rightTopRightID = vertexGroupIds[3];
    else gradientTrigger->m_rightTopRightID = vertexGroupIds[2];

    editor::layer()->updateGradientLayers();
}

void ParallaxMenuPopup::updateSetupActionButtons()
{
    auto setup = getSelectedSetup();
    //maybe just have a list so i dont have to manually add all these
    if(setup){
        Utils::enableButton(m_addLayerButton);
        Utils::enableButton(m_sortButton);
        //actions
        Utils::enableButton(m_findCenterButton);
        Utils::enableButton(m_findSetupButton);
        Utils::enableButton(m_cleanSetupButton);
        Utils::enableButton(m_deleteSetupButton);
    }else{
        Utils::disableButton(m_addLayerButton);
        Utils::disableButton(m_sortButton);
        //actions
        Utils::disableButton(m_findCenterButton);
        Utils::disableButton(m_findSetupButton);
        Utils::disableButton(m_cleanSetupButton);
        Utils::disableButton(m_deleteSetupButton);
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

    if(enablePrev) Utils::enableButton(m_setupSwitcherPrevButton);
    else Utils::disableButton(m_setupSwitcherPrevButton);

    if(enableNext) Utils::enableButton(m_setupSwitcherNextButton);
    else Utils::disableButton(m_setupSwitcherNextButton);
    

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

void ParallaxMenuPopup::updateRootIDButton()
{
    auto setup = getSelectedSetup();
    if(!setup) return;

    m_setupRootIDButton->setLabel(fmt::to_string(setup->m_rootID));
}

void ParallaxMenuPopup::updateFollowIDButton()
{
    auto setup = getSelectedSetup();
    if(!setup) return;

    m_setupFollowIDButton->setLabel(fmt::to_string(setup->m_followID));
}

void ParallaxMenuPopup::onAddLayerButton(CCObject *){

    auto setup = getSelectedSetup();
    if(!setup) return;

    auto newLayer = setup->createNewLayer();

    //next add it to the ui
    addLayerNodeToList(newLayer);
    m_layerListMenu->updateLayout();
    m_scrollLayer->updateLayout();

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

    //TODO: probably move this into parallaxSetup.cpp later

    auto basePosition = setup->m_areaMoveTriggerPtr->getPosition();
    //use the area move as the base
    //the follow trigger goes next to the area move trigger
    editor::object::move(setup->m_advancedFollowTriggerPtr,{basePosition.x+editor::constants::GRID_SIZE,basePosition.y});

    //for now just go through all the layers (maybe sort them later though)
    float y = basePosition.y;
    for(auto& layer : setup->m_layers){
        y+=editor::constants::GRID_SIZE;

        if(layer.hasScaleTrigger())
        editor::object::move(layer.m_scaleTriggerPtr,{basePosition.x,y});
        editor::object::move(layer.m_followTriggerPtr,{basePosition.x+editor::constants::GRID_SIZE,y});
    }
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

void ParallaxMenuPopup::onFindCenterButton(CCObject *)
{
    auto setup = getSelectedSetup();
    if(!setup) return;

    editor::selection::clear();
    //select the objects
    auto rootObjs = editor::objectsWithGroup(setup->m_rootID);
    auto followObjs = editor::objectsWithGroup(setup->m_followID);
    editor::selection::add(rootObjs);
    editor::selection::add(followObjs);
    //now go to the first one
    if(auto obj = rootObjs->firstObject()){
        auto gameObj = static_cast<GameObject*>(obj);
        editor::move(gameObj->getPosition());
        editor::setLayer(gameObj->m_editorLayer);
    }

    editor::update();//cuz theyre selected now
}

void ParallaxMenuPopup::onCreateSetupButton(CCObject *)
{
    auto newSetupPos = editor::ui()->getGridSnappedPos(editor::center());

    //first create the area move triggre
    auto newAreaMoveTrigger = static_cast<EnterEffectObject*>(editor::object::createObject(
        trigger::AREA_MOVE_TRIGGER,
        newSetupPos+CCPoint{0.0f,0.0f},
        false
    ));
    auto newAdvancedFollowTrigger = static_cast<AdvancedFollowTriggerObject*>(editor::object::createObject(
        trigger::ADVANCED_FOLLOW_TRIGGER,
        newSetupPos+CCPoint{editor::constants::GRID_SIZE,0.0f},
        false
    ));

    //setup the area move trigger
    //why is this making length and movedist 3000 and not 9000????
    newAreaMoveTrigger->m_specialTarget = -3;//target c
    newAreaMoveTrigger->m_length = 9000*3;//multiply by 3 because the triggers store 3 times the value they display
    newAreaMoveTrigger->m_moveDistance = -9000*3;
    newAreaMoveTrigger->m_relative = true;
    newAreaMoveTrigger->m_directionType=0;
    newAreaMoveTrigger->m_inbound=true;
    //give it a group
    int rootID = Utils::getNextFreeGroupID();
    trigger::setTarget(newAreaMoveTrigger, rootID);
    int followID = Utils::getNextFreeGroupID();
    trigger::setTarget(newAdvancedFollowTrigger, followID);
    trigger::setCenter(newAdvancedFollowTrigger, rootID);

    //now add the two root and follow objects
    //these can be any object so im making them look like a gd icon cuz its cute ig

    //cuz i cant write this down in mod.json
    //	constexpr int rootObjectID = 3816;//icon face particle
    //	constexpr int followObjectID = 3805;//hollow square particle
    int rootObjectID = ph::settings::rootObjectID.get();
    int followObjectID = ph::settings::followObjectID.get();

    auto rootObject = editor::object::createObject(
        rootObjectID,
        newSetupPos+CCPoint{0.0f,-editor::constants::GRID_SIZE},
        false
    );
    auto followObject = editor::object::createObject(
        followObjectID,
        newSetupPos+CCPoint{0.0f,-editor::constants::GRID_SIZE},
        false
    );

    rootObject->addToGroup(rootID);
    followObject->addToGroup(followID);

    editor::object::scale(rootObject,ph::settings::rootObjectScale.get());
    editor::object::scale(followObject,ph::settings::followObjectScale.get());
    
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
ParallaxMenuPopup *ParallaxMenuPopup::create()
{
    auto ret = new ParallaxMenuPopup();
    if (ret->init()) {
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

    m_layerListMenu->getChildByIndex<ParallaxMenuLayerNode>(targetLayer)->focus();

    scrollToLayerIndex(targetLayer);
}
