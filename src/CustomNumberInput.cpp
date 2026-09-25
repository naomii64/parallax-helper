#include "CustomNumberInput.hpp"

bool CustomNumberInput::init(float width, geode::ZStringView placeholder, geode::ZStringView font)
{
    if(!TextInput::init(width,placeholder,font)) return false;
    
    setCommonFilter(CommonFilter::Float);
    
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
