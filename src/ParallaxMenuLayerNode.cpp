#include "ParallaxMenuLayerNode.hpp"

#include "Utils/NumberRequestPopup.hpp"
#include "Utils/Settings.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;


void ParallaxMenuLayerNode::initDepthInput()
{

    constexpr float depthInputWidth = 110.0f;
    constexpr float padDepthLabel = 2.5f;

    auto depthInputNode = CCMenu::create();
    depthInputNode->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Right)
    );
    depthInputNode->setContentSize({depthInputWidth,30.0f});
    depthInputNode->setLayout(AnchorLayout::create());
    depthInputNode->setAnchorPoint({1.0f,0.5f});
    addChild(depthInputNode);
    
    m_depthInput = CustomNumberInput::create(depthInputWidth);
    
    m_depthInput->setCallback([this](const std::string&){
    
        this->m_layerPtr->setDepth(m_depthInput->getNumber<float>());
        
        this->updateDepthLabelColor();
    });
    
    //move it to the side middle
    m_depthInput->setAnchorPoint({1.0f,0.5f});
    m_depthInput->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Right)
    );

    depthInputNode->addChild(m_depthInput);

    //add the depth
    m_depthLabel = Label::create("Depth:","bigFont.fnt");
    m_depthLabel->setAnchorPoint({1.0f,0.5f});
    m_depthLabel->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Left)
        ->setOffset({-padDepthLabel,0.0f})
    );
    m_depthLabel->setScale(0.5);//scale the text down
    depthInputNode->addChild(m_depthLabel);

    depthInputNode->updateLayout();
}
bool ParallaxMenuLayerNode::init(const cocos2d::CCSize &size,ParallaxSetupLayer* layer)
{
    if(!CCMenu::init()) return false;

    setContentSize(size);

    setLayout(AnchorLayout::create());

    initDepthInput();

    //make the line 
    constexpr float lineThickness = 2.0f;
    auto line = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
    //the sprite is 10x10
    constexpr float baseSpriteSize = 10.0f;
    line->setContentSize({baseSpriteSize,baseSpriteSize});
    //scale it 
    line->setScaleX(size.width/baseSpriteSize);
    line->setScaleY(lineThickness/baseSpriteSize);
    //set the color
    line->setColor({0,0,0});
    line->setOpacity(50);

    line->setAnchorPoint({0.0f,0.5f});
    addChild(line);

    m_layerGroupIDButton = TriggerButton::create([this](Button* btn) {
        auto popup = NumberRequestPopup::create(
            [this](NumberRequestPopup* popup, bool didConfirm){
                if(didConfirm){
                    m_layerPtr->changeGroupID(popup->m_numberInput->getNumber<int>());
                    updateGroupIDLabel();
                }
            },
            "Change Layer Group ID",
            "Enter a value to change the group ID of this layer.\n"
            "This will also replace the previous ID in any object that already has it."
        );
        popup->m_numberInput->setNumber<int>(m_layerPtr->m_layerID);
        popup->show();
    });
    m_layerGroupIDButton->setPosition({25.0f,size.height/2});
    m_layerGroupIDButton->setScale(1.2f);

    if(layer->hasScaleTrigger())
        m_layerGroupIDButton->addTrigger(editor::trigger::SCALE_TRIGGER);
    m_layerGroupIDButton->addTrigger(editor::trigger::FOLLOW_TRIGGER);

    addChild(m_layerGroupIDButton);

    updateLayout();

    setLayer(layer);

    return true;
}

void ParallaxMenuLayerNode::setLayer(ParallaxSetupLayer *layer)
{
    int groupID = layer->m_layerID;
    m_layerPtr = layer;

    if(m_depthInput)
        m_depthInput->setString(layer->getDepthString());

    updateDepthLabelColor();
    updateGroupIDLabel();
}

void ParallaxMenuLayerNode::updateGroupIDLabel()
{
    m_layerGroupIDButton->setLabel(fmt::to_string(m_layerPtr->m_layerID));    
}


void ParallaxMenuLayerNode::defocus()
{
    m_depthInput->defocus();
}
void ParallaxMenuLayerNode::focus()
{
    m_depthInput->focus();
}

bool ParallaxMenuLayerNode::getFocused()
{
    return m_depthInput->getInputNode()->m_selected;
}

void ParallaxMenuLayerNode::updateDepthLabelColor()
{
    if(!m_depthLabel) return;
    if(!m_depthInput) return;

    float depth = m_depthInput->getNumber<float>();

    if(depth>0.0f){
        m_depthLabel->setColor(ph::settings::depthLabelColorPositive.get());
        return;
    }
    if(depth<0.0f){
        m_depthLabel->setColor(ph::settings::depthLabelColorNegative.get());
        return;
    }
    
    m_depthLabel->setColor(ph::settings::depthLabelColorDefault.get());
    return;
}


ParallaxMenuLayerNode* ParallaxMenuLayerNode::create(const cocos2d::CCSize &size,ParallaxSetupLayer* layer) {
    auto ret = new ParallaxMenuLayerNode();
    if (ret->init(size,layer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
};

