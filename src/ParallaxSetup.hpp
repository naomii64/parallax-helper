#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ParallaxSetupLayer {
public:
    int m_layerID = 0;
    float m_layerDepth = 0.0f;
    
    TransformTriggerGameObject* m_scaleTriggerPtr = nullptr;
    EffectGameObject* m_followTriggerPtr = nullptr;

    void setTriggerValuesByDepth(float depth);
private:

};

class ParallaxSetup {
public:
    int m_rootID = 0;//the group the area move trigger targets
    int m_followID = 0;//the group the layers of the parallax follow
    std::deque<ParallaxSetupLayer> m_layers;

    EnterEffectObject* m_areaMoveTriggerPtr = nullptr;
    AdvancedFollowTriggerObject* m_advancedFollowTriggerPtr = nullptr;
private:

};

class ParallaxSetupList {
public:
    //scans the given editor for setups and makes a list
    void scanEditorForSetups(LevelEditorLayer* editorLayer);
    std::deque<ParallaxSetup> m_setups;
private:

};