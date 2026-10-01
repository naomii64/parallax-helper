#include "CustomNumberInput.hpp"

bool CustomNumberInput::init(float width, geode::ZStringView placeholder, geode::ZStringView font)
{
    if(!TextInput::init(width,placeholder,font)) return false;
    
    setTypeFloat();
    
    auto inputNode = getInputNode();
    inputNode->m_numberInput = true;//clear non numeric
    inputNode->m_placeholderColor = ccColor3B{120,170,240};//copy the color robtob uses
    inputNode->setString("");//updates the placeholder color

    return true;
}

CustomNumberInput* CustomNumberInput::create(float width, ZStringView placeholder, ZStringView font) {
    auto ret = new CustomNumberInput();
    if (ret->init(width, placeholder, font)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void CustomNumberInput::setTypeInt(bool isSigned)
{
    m_isSigned = isSigned;
    if(isSigned)setCommonFilter(CommonFilter::Int);
    else setCommonFilter(CommonFilter::Uint);
}
void CustomNumberInput::setTypeFloat()
{
    m_isSigned = true;
    setCommonFilter(CommonFilter::Float);
}

void CustomNumberInput::enableArrows()
{
    //just use these arrows
    constexpr float arrowOffset = 15.f;

    //maybe make this change depending on the type
    auto arrowPlus = Button::createWithSpriteFrameName("edit_rightBtn_001.png",[this](Button*){
        int num = getNumber<int>();
        num++;

        //only happens for integer overflow but still idc
        if((!m_isSigned) && (num < 0)) num = 0;

        setNumber(num);
    });
    arrowPlus->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Right)
        ->setOffset({arrowOffset,0.0f})
    );

    auto arrowMinus = Button::createWithSpriteFrameName("edit_leftBtn_001.png",[this](Button*){
        int num = getNumber<int>();
        num--;

        if((!m_isSigned) && (num < 0)) num = 0;

        setNumber(num);
    });
    arrowMinus->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Left)
        ->setOffset({-arrowOffset,0.0f})
    );

    addChild(arrowPlus);
    addChild(arrowMinus);

    updateLayout();
}
