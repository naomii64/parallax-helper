#include "NumberRequestPopup.hpp"

bool NumberRequestPopup::init()
{
    //make this reusable later
    float popupWidth = 300;
    float popupHeight = 200;

    if (!Popup::init(popupWidth,popupHeight,"GJ_square02.png")) return false;
    m_closeBtn->setVisible(false);
    setTitle("Change Layer Group ID");



    auto descriptionBackground = NineSlice::create("square02b_001.png");
    descriptionBackground->setColor({0, 0, 0});
    descriptionBackground->setOpacity(50);
    descriptionBackground->setAnchorPoint({0.5f,1.0f});

    auto descriptionTextArea = RichTextArea::create("hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hi hi hi hi",
        "chatFont.fnt",
        1.0f,
        300.0f
    ); 
    descriptionTextArea->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    //TextArea::create("hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hi hi hi hi","chatFont.fnt",1.0f,popupWidth,{0.5f,0.5f},30.0f,false);

    constexpr float pad = 10.0f;

    descriptionBackground->setPosition({popupWidth/2,popupHeight-30.0f});
    descriptionBackground->setContentSize({popupWidth-(pad*2),descriptionTextArea->getContentHeight()+(pad*2)});

    descriptionTextArea->setAnchorPoint({0.5f,0.0f});
    descriptionTextArea->setPosition(descriptionBackground->getContentWidth()/2,pad);

    
    //descriptionTextArea->setPosition({popupWidth/2.0f,popupHeight-45.0f});
    m_mainLayer->addChild(descriptionBackground);
    descriptionBackground->addChild(descriptionTextArea);

    //FLAlertLayer::create(":3","found possible parallax root","ok?")->show();

    return true;
}

NumberRequestPopup* NumberRequestPopup::create() {
    auto ret = new NumberRequestPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
