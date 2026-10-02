#include "NumberRequestPopup.hpp"

#include "constants.hpp"

#include "Utils.hpp"

bool NumberRequestPopup::init(geode::Function<void(NumberRequestPopup*,bool)> callback,geode::ZStringView title, geode::ZStringView desc)
{
    //make this reusable later
    float popupWidth = 300;
    float popupHeight = 200;

    if (!Popup::init(popupWidth,popupHeight,"GJ_square02.png")) return false;
    m_closeBtn->setVisible(false);
    setTitle(title);

    m_callback = std::move(callback);


    auto descriptionBackground = NineSlice::create("square02b_001.png");
    descriptionBackground->setColor({0, 0, 0});
    descriptionBackground->setOpacity(44);
    descriptionBackground->setAnchorPoint({0.5f,1.0f});

    auto descriptionTextArea = RichTextArea::create(
        desc,
        "chatFont.fnt",
        0.75f,
        popupWidth-(constants::ui::PADDING*4)
    ); 
    descriptionTextArea->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);

    descriptionBackground->setPosition({popupWidth/2,popupHeight-40.0f});
    descriptionBackground->setContentSize({popupWidth-(constants::ui::PADDING*2),descriptionTextArea->getContentHeight()+(constants::ui::PADDING*2)});

    descriptionTextArea->setAnchorPoint({0.5f,0.0f});
    descriptionTextArea->setPosition(descriptionBackground->getContentWidth()/2,constants::ui::PADDING);

    //descriptionTextArea->setPosition({popupWidth/2.0f,popupHeight-45.0f});
    m_mainLayer->addChild(descriptionBackground);
    descriptionBackground->addChild(descriptionTextArea);

    //create the actual input for the number
    //add arrows later
    m_numberInput = CustomNumberInput::create(70.0f);
    m_numberInput->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Bottom)
        ->setOffset({0.0f,70.0f})
    );
    m_numberInput->setTypeUnsignedInt();
    m_numberInput->enableArrows();
    //create the new id button (maybe make this optional later)
    auto newGroupIDButton = Button::createWithSpriteFrameName("GJ_plus2Btn_001.png",
        [this](Button*){
            m_numberInput->setNumber<int>(Utils::getNextFreeGroupID());
        }
    );
    newGroupIDButton->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Right)
        ->setOffset({40.0f,0.0f})
    );
    m_numberInput->addChild(newGroupIDButton);
    m_numberInput->updateLayout();

    m_mainLayer->addChild(m_numberInput);

    auto closeButtonMenu = Utils::createTwoButtonMenu(
        [this](bool isBtn2){
            if(m_callback)
                m_callback(this,isBtn2);
            
            this->onClose(this);
        }
    );
    m_mainLayer->addChild(closeButtonMenu);

    m_mainLayer->updateLayout();

    return true;
}

NumberRequestPopup* NumberRequestPopup::create(geode::Function<void(NumberRequestPopup*,bool)> callback, geode::ZStringView title, geode::ZStringView desc) {
    auto ret = new NumberRequestPopup();
    if (ret->init(std::move(callback),title,desc)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
