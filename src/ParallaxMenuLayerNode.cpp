#include "ParallaxMenuLayerNode.hpp"
#include "CustomNumberInput.hpp"

bool ParallaxMenuLayerNode::init(const cocos2d::CCSize &size,ParallaxSetupLayer* layer)
{
    if(!CCMenu::init()) return false;

    setContentSize(size);

    constexpr float depthInputWidth = 70.0f;
    constexpr float padDepthLabel = 2.0f;

    m_depthInput = CustomNumberInput::create(depthInputWidth);

    m_depthInput->setCallback([this](const std::string&){
        //calls whenever the text is changed
        //get the input value as a float
        std::string depthInputString = m_depthInput->getString();
        auto depthResult = geode::utils::numFromString<float>(depthInputString);
        float inputDepth = 0.0f;
        if (depthResult) {
            inputDepth = depthResult.unwrap();
        }

        this->m_layerPtr->setTriggerValuesByDepth(inputDepth);
        
        //update the depth label color
        this->updateDepthLabelColor();
    });

    //move it to the side middle
    m_depthInput->setAnchorPoint({1.0f,0.5f});
    m_depthInput->setPosition({size.width,size.height/2});

    addChild(m_depthInput);

    //add the depth
    m_depthLabel = Label::create("Depth:","bigFont.fnt");
    m_depthLabel->setAnchorPoint({1.0f,0.5f});
    m_depthLabel->setPosition({size.width-depthInputWidth-padDepthLabel,size.height/2});
    m_depthLabel->setScale(0.75*0.75);//scale the text down
    addChild(m_depthLabel);    

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

    //add the trigger icon where the group is displayed (maybe this can be different for different setups later)
    auto layerGroupIDBackground = CCSprite::create("layerGroupIDTriggers.png"_spr);
    layerGroupIDBackground->setAnchorPoint({0.0f,0.5f});
    layerGroupIDBackground->setPosition({0.0f,size.height/2});
    layerGroupIDBackground->setScale(1.5f);
    addChild(layerGroupIDBackground);

    //now add the text on top of it
    m_layerGroupIDLabel = Label::create("","bigFont.fnt");
    auto layerGroupBGSize = layerGroupIDBackground->getContentSize();
    m_layerGroupIDLabel->setPosition(layerGroupBGSize/2.0f); //put it in the middle   
    m_layerGroupIDLabel->setScale(0.45f);//scale the label

    layerGroupIDBackground->addChild(m_layerGroupIDLabel);    
    layerGroupIDBackground->updateLayout();
    updateLayout();

    setLayer(layer);

    return true;
}

void ParallaxMenuLayerNode::setLayer(ParallaxSetupLayer *layer)
{
    int groupID = layer->m_layerID;
    m_layerGroupIDLabel->setText(fmt::to_string(groupID));

    m_layerPtr = layer;
    m_depthInput->setString(layer->getDepthString());

    updateDepthLabelColor();
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
    //this probably doesnt need to be gotten from the mod for EVERY update
    const ccColor3B tintDefault = Mod::get()->getSettingValue<cocos2d::ccColor3B>("depth-label-color-default");
    const ccColor3B tintPositive = Mod::get()->getSettingValue<cocos2d::ccColor3B>("depth-label-color-positive");
    const ccColor3B tintNegative = Mod::get()->getSettingValue<cocos2d::ccColor3B>("depth-label-color-negative");

    std::string depthInputString = m_depthInput->getString();
    auto depthResult = geode::utils::numFromString<float>(depthInputString);
    if (depthResult) {
        float depthValue = depthResult.unwrap();
        // make sure its not nan or infinity        
        if(std::isfinite(depthValue)){
            if(depthValue>0.0f){
                m_depthLabel->setColor(tintPositive);
                return;
            }
            if(depthValue<0.0f){
                m_depthLabel->setColor(tintNegative);
                return;
            }
            //0 will just use the default color
        }
    }
    //use the default
    m_depthLabel->setColor(tintDefault);
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

