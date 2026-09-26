#include "DuplicateAndLayerPopup.hpp"

DuplicateAndLayerPopup* DuplicateAndLayerPopup::create(std::function<void(float,float,int)> onConfirmCallback) {
    auto ret = new DuplicateAndLayerPopup();
    if (ret->init(onConfirmCallback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool DuplicateAndLayerPopup::init(std::function<void(float,float,int)> onConfirmCallback)
{
    constexpr float popupWidth = 300;
    constexpr float popupHeight = 150;

    if(!Popup::init(popupWidth,popupHeight,"GJ_square02.png")) return false;
    m_closeBtn->setVisible(false);
    setTitle("Duplicate And Layer");

    //create the range input
    auto depthRangeBackground = NineSlice::create("square02b_001.png");
    depthRangeBackground->setColor({0, 0, 0});
    depthRangeBackground->setOpacity(50);
    depthRangeBackground->setAnchorPoint({0.5f,0.5f});
    depthRangeBackground->setLayout(AnchorLayout::create());

    m_mainLayer->addChild(depthRangeBackground);

    constexpr float numberInputWidth = 40.0f;
    constexpr float padAmount = 10.0f;

    auto depthRangeLabel = Label::create("Range:","bigFont.fnt");
    depthRangeBackground->addChild(depthRangeLabel);
    depthRangeLabel->setLayoutOptions(        
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::TopLeft)
        ->setOffset({padAmount,0.0f})
    );
    depthRangeLabel->setScale(0.45f);
    depthRangeLabel->setAnchorPoint({0.0f,0.5f});

    depthRangeBackground->setContentSize({
        (numberInputWidth*2.0f) + (padAmount*3.0f),
        30.0f+(padAmount*2.0f)
    });

    m_rangeInput0 = CustomNumberInput::create(numberInputWidth);
    m_rangeInput0->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Left)
        ->setOffset({((numberInputWidth/2.0f) + padAmount),0.0f})
    );
    depthRangeBackground->addChild(m_rangeInput0);

    m_rangeInput1 = CustomNumberInput::create(numberInputWidth);
    m_rangeInput1->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Right)
        ->setOffset({((numberInputWidth/2.0f) + padAmount) * -1.0f,0.0f})
    );
    depthRangeBackground->addChild(m_rangeInput1);

    m_rangeInput0->setNumber(-0.25f);
    m_rangeInput1->setNumber(0.25f);

    //create the layer count inputtt
    auto layerCountMenu = CCMenu::create();
    m_mainLayer->addChild(layerCountMenu);
    auto layerCountLabel = Label::create("Count:","bigFont.fnt");
    layerCountLabel->setScale(0.5f);
    layerCountLabel->setAnchorPoint({1.0f,0.5});
    layerCountLabel->setPosition(-padAmount/2,0.0f);
    layerCountMenu->addChild(layerCountLabel);
    
    m_layerCountInput = CustomNumberInput::create(numberInputWidth);
    m_layerCountInput->setAnchorPoint({0.0f,0.5});
    layerCountMenu->addChild(m_layerCountInput);
    

    m_layerCountInput->setNumber(5);

    depthRangeBackground->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
        ->setOffset({-popupWidth / 4.0f,0.0f})
    );
    layerCountMenu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
        ->setOffset({popupWidth / 4.0f,0.0f})
    );

    layerCountMenu->updateLayout();
    depthRangeBackground->updateLayout();

    //now create the two close buttons
    auto closeButtonMenu = CCMenu::create();
    m_mainLayer->addChild(closeButtonMenu);
    closeButtonMenu->setAnchorPoint({0.5f,0.0f});
    closeButtonMenu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Bottom)
        ->setOffset({0.0f,padAmount})
    );
    closeButtonMenu->setLayout(
        AxisLayout::create()
        ->setAxis(Axis::Row)
    );

    auto cancelButton = Button::createWithNode(
        ButtonSprite::create("Cancel"),
        [this](Button*){
            this->onClose(this);
        }
    );
    m_onConfirmCallback = onConfirmCallback;
    auto confirmButton = Button::createWithNode(
        ButtonSprite::create("Confirm"),
        [this](Button*){
            int count = this->m_layerCountInput->getNumber<int>();
            
            float rangeMin = this->m_rangeInput0->getNumber<float>();
            float rangeMax = this->m_rangeInput1->getNumber<float>();
            //make sure theyre the right order
            if(rangeMin>rangeMax)
                std::swap(rangeMin,rangeMax);

            if(this->m_onConfirmCallback)
                this->m_onConfirmCallback(rangeMin,rangeMax,count);
            
            this->onClose(this);
        }
    );
    closeButtonMenu->addChild(cancelButton);
    closeButtonMenu->addChild(confirmButton);
    closeButtonMenu->updateLayout();

    updateLayout();

    return true;
}
