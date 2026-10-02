#include "TriggerButton.hpp"
#include "Utils.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;


TriggerButton* TriggerButton::create(geode::Button::ButtonCallback callback) {
    auto ret = new TriggerButton();
    if (ret->init(std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void TriggerButton::setLabel(const geode::ZStringView &string)
{
    m_label->setText(string);
}

void TriggerButton::addTrigger(int objectID)
{
    auto spr = CCSprite::create(Utils::getTriggerSprite(objectID).c_str());
    //spr->setContentSize({SPR_SIZE,SPR_SIZE});
    //spr->setScale(1.25f);
    spr->setColor(nwo5::editor::trigger::color(objectID));
    spr->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );

    m_triggerSpriteNode->addChild(spr);

    //rearrange all the children
    constexpr float distBetweenTriggers = 10.0f;

    int i = 0;
    for(auto& node : m_triggerSpriteNode->getChildrenExt()){
        float triggerX = float(i)*distBetweenTriggers;
        triggerX-=float(m_triggerSpriteNode->getChildrenCount()-1)*distBetweenTriggers*0.5f;
        static_cast<AnchorLayoutOptions*>(node->getLayoutOptions())->setOffset({triggerX,0.0f});
        
        i++;
    }

    m_triggerSpriteNode->updateLayout();
    updateLayout();
}

bool TriggerButton::init(geode::Button::ButtonCallback callback)
{
    //make a callback passable later
    if(!Button::init(std::move(callback))) return false;

    setContentSize({SPR_SIZE,SPR_SIZE});
    setLayout(AnchorLayout::create());

    m_triggerSpriteNode = CCNode::create();
    m_triggerSpriteNode->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );
    m_triggerSpriteNode->setContentSize({SPR_SIZE,SPR_SIZE});
    m_triggerSpriteNode->setLayout(AnchorLayout::create());
    m_triggerSpriteNode->setAnchorPoint({0.5f,0.5f});
    addChild(m_triggerSpriteNode);

    m_label = Label::create("bigFont.fnt");
    m_label->setScale(0.6f);
    m_label->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
        ->setOffset({0.0f,1.0f})
    );
    addChild(m_label);

    updateLayout();

    return true;
}
