#include "ParallaxLayerSwitcherMenu.hpp"

#include "Utils.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;


#include "../MyGameObject.hpp"

bool ParallaxLayerSwitcherMenu::init(const cocos2d::CCSize &size,ParallaxSetupList* setupList)
{
    if(!CCMenu::init()) return false;

    setContentSize(size);
    m_parallaxSetupList = setupList;

    //add the stuff
    constexpr float ARROW_BUTTON_SCALE = .5f;
    constexpr float DIST_BETWEEN_ARROWS = 20.f;

    constexpr float OUTER_ARROW_OFFSET = DIST_BETWEEN_ARROWS * 0.5f;
    constexpr float INNER_ARROW_OFFSET = DIST_BETWEEN_ARROWS * 1.5f;

    auto createArrowButton = [&](Button::ButtonCallback callback,const geode::ZStringView& spriteFrameName,const geode::ZStringView& nodeID,geode::Anchor anchor,float xOffset){
        auto btn = Button::createWithSpriteFrameName(spriteFrameName,std::move(callback));
        btn->setLayoutOptions(
            AnchorLayoutOptions::create()
            ->setAnchor(anchor)
            ->setOffset({xOffset,0.f})
        );
        btn->setScale(ARROW_BUTTON_SCALE);
        btn->setID(nodeID);
        addChild(btn);

        return btn;
    };

    setLayout(AnchorLayout::create());

    m_prevLayerButton = createArrowButton([this](Button*){changeSelectedLayer(-1);},"GJ_arrow_03_001.png","prev-layer-button"_spr,Anchor::Left,INNER_ARROW_OFFSET);
    m_nextLayerButton = createArrowButton([this](Button*){changeSelectedLayer(1);},"GJ_arrow_03_001.png","next-layer-button"_spr,Anchor::Right,-INNER_ARROW_OFFSET);
    Utils::flipButtonSprite(m_nextLayerButton);

    m_allLayersButton = createArrowButton([this](Button*){selectAllLayer();},"GJ_arrow_02_001.png","all-layers-button"_spr,Anchor::Left,OUTER_ARROW_OFFSET);
    m_lastLayerButton = createArrowButton([this](Button*){selectLastLayer();},"GJ_arrow_02_001.png","last-layer-button"_spr,Anchor::Right,-OUTER_ARROW_OFFSET);
    Utils::flipButtonSprite(m_lastLayerButton);

    m_mainBG = Utils::createBlackBackgroundSquare({size.width-(DIST_BETWEEN_ARROWS*4.f),size.height},90);
    m_mainBG->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );
    m_mainBG->setID("parallax-layer-selector-bg"_spr);
    m_mainBG->setScaleMultiplier(0.5f);
    addChild(m_mainBG);

    constexpr float TRIGGER_BUTTON_X = 16.f;

    m_triggerButton = TriggerButton::create();
    m_triggerButton->setScale(0.75f);
    m_triggerButton->setPosition(TRIGGER_BUTTON_X,size.height*0.5f);
    m_triggerButton->setID("parallax-layer-selector-groupID"_spr);
    m_mainBG->addChild(m_triggerButton);

    //just add these for now to test :3
    m_triggerButton->addTrigger(trigger::SCALE_TRIGGER);
    m_triggerButton->addTrigger(trigger::FOLLOW_TRIGGER);

    //calculating how big to make the depth input...
    const float triggerButtonPad = TRIGGER_BUTTON_X - (m_triggerButton->getScaledContentWidth()*0.5f);

    const float depthInputMinX = TRIGGER_BUTTON_X * 2.f;
    const float depthInputMaxX = m_mainBG->getContentWidth() - (triggerButtonPad * .5f);
    
    const float depthInputYScale = (m_mainBG->getContentHeight() - triggerButtonPad)/30.f;//30 is the default number input size

    m_depthInput = CustomNumberInput::create((depthInputMaxX - depthInputMinX) / depthInputYScale);
    m_depthInput->setPosition((depthInputMinX+depthInputMaxX) * .5f,size.height * .5f);
    m_depthInput->setScale(depthInputYScale);
    m_depthInput->setID("parallax-layer-selector-depth"_spr);
    m_mainBG->addChild(m_depthInput);

    m_allLabel = Label::create("All","bigFont.fnt");
    m_allLabel->setPosition(size * .5f);
    m_allLabel->setScale(.5f);
    m_allLabel->setID("parallax-layer-selector-all-label"_spr);
    addChild(m_allLabel);

    updateLayout();

    updateUI();
    updateObjectLayerVisibility();

    return true;
}
void ParallaxLayerSwitcherMenu::selectAllLayer()
{
    m_selectedLayerIndex=-1;
    updateUI();
    updateObjectLayerVisibility();
}
void ParallaxLayerSwitcherMenu::selectLastLayer()
{
    auto setup = m_parallaxSetupList->getSelectedSetup();
    if(!setup) return;

    m_selectedLayerIndex = setup->m_layers.size()-1;

    updateUI();
    updateObjectLayerVisibility();
}
void ParallaxLayerSwitcherMenu::changeSelectedLayer(int amount)
{
    m_selectedLayerIndex+=amount;
    updateUI();
    updateObjectLayerVisibility();
}

void ParallaxLayerSwitcherMenu::updateUI()
{
    auto setup = m_parallaxSetupList->getSelectedSetup();
    if(!setup){
        setVisible(false);
        return;
    }
    setVisible(true);

    //for now just uhm for the uhm the testing heh...
    m_triggerButton->setLabel(fmt::to_string(m_selectedLayerIndex));

    //avoid invalid layers like this for now
    if(m_selectedLayerIndex<0 || m_selectedLayerIndex>=setup->m_layers.size()){
       m_selectedLayerIndex = LAYER_ALL; 
    }

    //enable and disable the arrows
    if(m_selectedLayerIndex == LAYER_ALL){
        Utils::disableButton(m_allLayersButton);
        Utils::disableButton(m_prevLayerButton);
    } else {
        Utils::enableButton(m_allLayersButton);
        Utils::enableButton(m_prevLayerButton);
    }

    if(m_selectedLayerIndex>=int(setup->m_layers.size()-1)){
        Utils::disableButton(m_lastLayerButton);
        Utils::disableButton(m_nextLayerButton);
    } else {
        Utils::enableButton(m_lastLayerButton);
        Utils::enableButton(m_nextLayerButton);
    }

    //update visibility and stuff
    if(m_selectedLayerIndex == LAYER_ALL){
        m_allLabel->setVisible(true);
        
        m_depthInput->setVisible(false);
        m_triggerButton->setVisible(false);

        return;
    }else{
        //visibility
        m_allLabel->setVisible(false);

        m_depthInput->setVisible(true);
        m_triggerButton->setVisible(true);

        ParallaxSetupLayer* selectedLayer = &setup->m_layers[m_selectedLayerIndex];
        //load the layer details
        m_depthInput->setNumber(selectedLayer->getDepth());
        m_triggerButton->setLabel(fmt::to_string(selectedLayer->m_layerID));
    }
}
void ParallaxLayerSwitcherMenu::updateObjectLayerVisibility() {

    auto selectedLayer = getSelectedLayer();

    if(!selectedLayer){
        for(auto& obj : static_cast<CCArrayExt<MyGameObject>>(editor::objectArray())){
            obj->m_fields->m_isOnSelectedParallaxLayer = true;
        }
    }else{
        for(auto& obj : static_cast<CCArrayExt<MyGameObject>>(editor::objectArray())){
            obj->m_fields->m_isOnSelectedParallaxLayer = editor::object::hasGroup(obj,selectedLayer->m_layerID);
        }
    }
}
ParallaxSetupLayer *ParallaxLayerSwitcherMenu::getSelectedLayer()
{
    auto setup = m_parallaxSetupList->getSelectedSetup();
    if(!setup) return nullptr;

    if(m_selectedLayerIndex<0 || (m_selectedLayerIndex>=int(setup->m_layers.size())))
        return nullptr;

    return &setup->m_layers[m_selectedLayerIndex];
    
};

ParallaxLayerSwitcherMenu *ParallaxLayerSwitcherMenu::create(const cocos2d::CCSize &size,ParallaxSetupList* setupList)
{
    auto ret = new ParallaxLayerSwitcherMenu();
    if (ret->init(size,setupList)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

