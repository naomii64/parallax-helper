#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ParallaxSetupLayer {
public:
    int m_layerID = 0;
    
    //the depth will no longer be stored here
    //float m_layerDepth = 0.0f;
    float getDepth() const;
    gd::string getDepthString() const;

    float getDuration() const;
    void setDuration(float duration);

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

    float getDuration() const;
    gd::string getDurationString() const;
    size_t getLayerCount() const;

    void setDuration(float duration);

    EnterEffectObject* m_areaMoveTriggerPtr = nullptr;
    AdvancedFollowTriggerObject* m_advancedFollowTriggerPtr = nullptr;

    ParallaxSetupLayer* addLayer(TransformTriggerGameObject* scaleTrigger,EffectGameObject* followTrigger);
    CCPoint getPositionForNewLayerTriggers();

    //sorting functions
    std::vector<ParallaxSetupLayer*> getLayerPointerList();//basically no sort, used as a base for sort functions
    std::vector<ParallaxSetupLayer*> sortDepth();
    std::vector<ParallaxSetupLayer*> sortGroupID();
private:

};

class ParallaxSetupList {
public:
    //scans the given editor for setups and makes a list
    void scanEditorForSetups(LevelEditorLayer* editorLayer);
    std::deque<ParallaxSetup> m_setups;

    ParallaxSetup* addSetup(EnterEffectObject* areaMoveTrigger,AdvancedFollowTriggerObject* advancedFollowTrigger);
private:

};