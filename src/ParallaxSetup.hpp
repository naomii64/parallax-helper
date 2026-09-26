#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ParallaxSetupLayer {
public:
    int m_layerID = 0;
    
    //the depth will no longer be stored here
    //float m_layerDepth = 0.0f;
    float getDepth() const;
    std::string getDepthString() const;

    float getDuration() const;
    void setDuration(float duration);

    TransformTriggerGameObject* m_scaleTriggerPtr = nullptr;
    EffectGameObject* m_followTriggerPtr = nullptr;

    void setTriggerValuesByDepth(float depth);

    void deleteTriggerObjects();//deletes the two trigger objects in the editor

    cocos2d::CCPoint getEditorPosition();

    bool hasScaleTrigger() const;
};

class ParallaxSetup {
public:
    int m_rootID = 0;//the group the area move trigger targets
    int m_followID = 0;//the group the layers of the parallax follow
    std::deque<ParallaxSetupLayer> m_layers;

    float getDuration() const;
    std::string getDurationString() const;
    size_t getLayerCount() const;
    int getEditorLayer() const;

    void setDuration(float duration);

    void deleteAllLayersAndLayerTriggers();
    void deleteAllRootAndFollowObjects();


    EnterEffectObject* m_areaMoveTriggerPtr = nullptr;
    AdvancedFollowTriggerObject* m_advancedFollowTriggerPtr = nullptr;

    ParallaxSetupLayer* addLayer(TransformTriggerGameObject* scaleTrigger,EffectGameObject* followTrigger);//scaleTrigger can be nullptr to create a follow only layer
    ParallaxSetupLayer* createNewLayer(float depth = 0.0f);//creates a new layer along with its triggers in the editor
    CCPoint getPositionForNewLayerTriggers();

    //sorting functions
    std::vector<ParallaxSetupLayer*> getLayerPointerList();//basically no sort, used as a base for sort functions
    std::vector<ParallaxSetupLayer*> sortDepth();
    std::vector<ParallaxSetupLayer*> sortGroupID();
private:

};

class ParallaxSetupList {
public:
    
    void scanEditorForSetups(LevelEditorLayer* editorLayer);//scans the given editor for setups and makes a list
    std::deque<ParallaxSetup> m_setups;

    ParallaxSetup* addSetup(EnterEffectObject* areaMoveTrigger,AdvancedFollowTriggerObject* advancedFollowTrigger);
    void removeSetupByIndex(size_t index);
private:

};