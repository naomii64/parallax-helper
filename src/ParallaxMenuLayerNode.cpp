#include "ParallaxMenuLayerNode.hpp"


bool ParallaxMenuLayerNode::init(const cocos2d::CCSize &size,ParallaxSetupLayer* layer)
{
    if(!CCMenu::init()) return false;

    //placeholder test values
    int groupID = layer->m_layerID;
    float depth = layer->m_layerDepth;

    setContentSize(size);

    constexpr float depthInputWidth = 70.0f;
    constexpr float padDepthLabel = 2.0f;

    m_depthInput = TextInput::create(depthInputWidth,"Num");
    m_depthInput->setCommonFilter(CommonFilter::Float);

    //store a pointer to the layer
    m_layerPtr = layer;

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
    
    //depthInput->setString("0.00");
    m_depthInput->setString(fmt::to_string(depth));

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
    auto layerGroupIDLabel = Label::create(fmt::to_string(groupID),"bigFont.fnt");
    auto layerGroupBGSize = layerGroupIDBackground->getContentSize();
    layerGroupIDLabel->setPosition(layerGroupBGSize/2.0f); //put it in the middle   
    layerGroupIDLabel->setScale(0.45f);//scale the label

    layerGroupIDBackground->addChild(layerGroupIDLabel);    
    layerGroupIDBackground->updateLayout();
    updateLayout();

    //update the labels color
    updateDepthLabelColor();

    return true;
}

void ParallaxMenuLayerNode::updateDepthLabelColor()
{
    if(!m_depthLabel) return;
    if(!m_depthInput) return;
    //maybe make these customizable in settings later
    const ccColor3B tintDefault = {255,255,255};
    const ccColor3B tintPositive = {255,170,170};
    const ccColor3B tintNegative = {170,215,255};

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