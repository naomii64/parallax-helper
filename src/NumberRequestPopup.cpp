#include "NumberRequestPopup.hpp"

bool NumberRequestPopup::init(geode::Function<void(NumberRequestPopup*,bool)> callback)
{
    //make this reusable later
    float popupWidth = 300;
    float popupHeight = 200;

    constexpr float pad = 10.0f;

    if (!Popup::init(popupWidth,popupHeight,"GJ_square02.png")) return false;
    m_closeBtn->setVisible(false);
    setTitle("Change Layer Group ID");

    m_callback = std::move(callback);


    auto descriptionBackground = NineSlice::create("square02b_001.png");
    descriptionBackground->setColor({0, 0, 0});
    descriptionBackground->setOpacity(50);
    descriptionBackground->setAnchorPoint({0.5f,1.0f});

    auto descriptionTextArea = RichTextArea::create(
        "Put in a value to change the group ID of this layer.\n"
        "This will also replace the previous ID in any object that already has it.",
        "chatFont.fnt",
        0.75f,
        popupWidth-(pad*4)
    ); 
    descriptionTextArea->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);

    descriptionBackground->setPosition({popupWidth/2,popupHeight-40.0f});
    descriptionBackground->setContentSize({popupWidth-(pad*2),descriptionTextArea->getContentHeight()+(pad*2)});

    descriptionTextArea->setAnchorPoint({0.5f,0.0f});
    descriptionTextArea->setPosition(descriptionBackground->getContentWidth()/2,pad);

    
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
    m_numberInput->setTypeInt(false);
    m_numberInput->enableArrows();
    m_mainLayer->addChild(m_numberInput);

    
    //now create the two close buttons (this is copied from duplicateandlayerpopup.cpp probably make setting this up more abstract later)
    auto closeButtonMenu = CCMenu::create();
    m_mainLayer->addChild(closeButtonMenu);
    closeButtonMenu->setAnchorPoint({0.5f,0.0f});
    closeButtonMenu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Bottom)
        ->setOffset({0.0f,pad})
    );
    closeButtonMenu->setLayout(
        AxisLayout::create()
        ->setAxis(Axis::Row)
    );

    auto cancelButton = Button::createWithNode(
        ButtonSprite::create("Cancel"),
        [this](Button*){
            if (m_callback) {
                m_callback(this, false);
            }
            this->onClose(this);
        }
    );
    auto confirmButton = Button::createWithNode(
        ButtonSprite::create("Confirm"),
        [this](Button*){
            if (m_callback) {
                m_callback(this, true);
            }
            this->onClose(this);
        }
    );
    closeButtonMenu->addChild(cancelButton);
    closeButtonMenu->addChild(confirmButton);
    closeButtonMenu->updateLayout();

    m_mainLayer->updateLayout();

    return true;
}

NumberRequestPopup* NumberRequestPopup::create(geode::Function<void(NumberRequestPopup*,bool)> callback) {
    auto ret = new NumberRequestPopup();
    if (ret->init(std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
