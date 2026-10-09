#include "ParallaxLayerSwitcherMenu.hpp"

#include "Utils.hpp"
#include "TriggerButton.hpp"
#include "CustomNumberInput.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

bool ParallaxLayerSwitcherMenu::init(const cocos2d::CCSize &size)
{
    if(!CCMenu::init()) return false;

    setContentSize(size);

    //add the stuff
    constexpr float ARROW_BUTTON_SCALE = .5f;
    constexpr float DIST_BETWEEN_ARROWS = 20.f;

    constexpr float OUTER_ARROW_OFFSET = DIST_BETWEEN_ARROWS * 0.5f;
    constexpr float INNER_ARROW_OFFSET = DIST_BETWEEN_ARROWS * 1.5f;

    auto createArrowButton = [&](const geode::ZStringView& spriteFrameName,const geode::ZStringView& nodeID,geode::Anchor anchor,float xOffset){
        auto btn = Button::createWithSpriteFrameName(spriteFrameName);
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

    auto prevLayerButton = createArrowButton("GJ_arrow_03_001.png","prev-layer-button"_spr,Anchor::Left,INNER_ARROW_OFFSET);
    auto nextLayerButton = createArrowButton("GJ_arrow_03_001.png","next-layer-button"_spr,Anchor::Right,-INNER_ARROW_OFFSET);
    Utils::flipButtonSprite(nextLayerButton);

    auto allLayersButton = createArrowButton("GJ_arrow_02_001.png","all-layers-button"_spr,Anchor::Left,OUTER_ARROW_OFFSET);
    auto lastLayerButton = createArrowButton("GJ_arrow_02_001.png","last-layer-button"_spr,Anchor::Right,-OUTER_ARROW_OFFSET);
    Utils::flipButtonSprite(lastLayerButton);

    auto mainBG = Utils::createBlackBackgroundSquare({size.width-(DIST_BETWEEN_ARROWS*4.f),size.height},90);
    mainBG->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );
    mainBG->setID("parallax-layer-selector-bg"_spr);
    mainBG->setScaleMultiplier(0.5f);
    addChild(mainBG);

    constexpr float TRIGGER_BUTTON_X = 16.f;

    auto triggerButton = TriggerButton::create();
    triggerButton->setScale(0.75f);
    triggerButton->setPosition(TRIGGER_BUTTON_X,size.height*0.5f);
    triggerButton->setID("parallax-layer-selector-groupID"_spr);
    mainBG->addChild(triggerButton);

    //just add these for now to test :3
    triggerButton->addTrigger(trigger::SCALE_TRIGGER);
    triggerButton->addTrigger(trigger::FOLLOW_TRIGGER);
    triggerButton->setLabel("21");

    //calculating how big to make the depth input...
    const float triggerButtonPad = TRIGGER_BUTTON_X - (triggerButton->getScaledContentWidth()*0.5f);

    const float depthInputMinX = TRIGGER_BUTTON_X * 2.f;
    const float depthInputMaxX = mainBG->getContentWidth() - (triggerButtonPad * .5f);
    
    const float depthInputYScale = (mainBG->getContentHeight() - triggerButtonPad)/30.f;//30 is the default number input size

    auto depthInput = CustomNumberInput::create((depthInputMaxX - depthInputMinX) / depthInputYScale);
    depthInput->setPosition((depthInputMinX+depthInputMaxX) * .5f,size.height * .5f);
    depthInput->setScale(depthInputYScale);
    depthInput->setID("parallax-layer-selector-depth"_spr);
    mainBG->addChild(depthInput);

    auto allLabel = Label::create("All","bigFont.fnt");
    allLabel->setPosition(size * .5f);
    allLabel->setScale(.5f);
    allLabel->setID("parallax-layer-selector-all-label"_spr);
    addChild(allLabel);

    updateLayout();

    return true;
};

ParallaxLayerSwitcherMenu *ParallaxLayerSwitcherMenu::create(const cocos2d::CCSize &size)
{
    auto ret = new ParallaxLayerSwitcherMenu();
    if (ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

