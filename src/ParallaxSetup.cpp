#include "ParallaxSetup.hpp"
#include "constants.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;


void ParallaxSetupList::scanEditorForSetups(LevelEditorLayer* editorLayer)
{				
	//the plan:
	//1: find possible parallax roots in the form of area move triggers
	//2: find the corresponding advanced follow triggers
	//3: the roots that dont have a corresponding advanced follow arent parallax effects
	//4: find the layers of the effects 

	//keep maps for easy accessing of triggers
	//note: this doesnt account for the fact multiple triggers can target the same group
	std::map<int,EnterEffectObject*> areaMoveTriggersByTargetGID;
	std::map<int,AdvancedFollowTriggerObject*> advancedFollowTriggersByTargetGID;
	//the follow/scale triggers
	std::map<int,EffectGameObject*> followTriggersByTargetGID;
	std::map<int,TransformTriggerGameObject*> scaleTriggersByTargetGID;

	auto allObjects = editorLayer->getAllObjects();
	for (auto& obj : CCArrayExt<GameObject*>(allObjects)){
		if(!obj->m_isTrigger) continue;//this loop only cares about triggers anyway

			if(obj->m_objectID == trigger::AREA_MOVE_TRIGGER){
				auto ego = static_cast<EnterEffectObject*>(obj);
						
				//it might be faster to move these checks somewhere else later

				//for testing if an area move meets the requirements
				bool targetC = (ego->m_specialTarget == -3);
				//this might be made to be more flexible later but idk
                //removing these two cuz theyre too specific i guess
				bool hasCorrectLength = true;//ego->m_length >= 9000;
				bool hasCorrectMoveDist = true;//ego->m_moveDistance<=-9000;
						
                bool isRelative = ego->m_relative;
				bool isCorrectDirectionType = (ego->m_directionType==0)&&ego->m_inbound;
				//now we combine them all into one 
				bool isPossibleParallaxRoot = targetC&&hasCorrectLength&&hasCorrectMoveDist&&isRelative&&isCorrectDirectionType;

				if(!isPossibleParallaxRoot) continue;//if this isnt a possible root just move on
				areaMoveTriggersByTargetGID[ego->m_targetGroupID] = ego;//save the root to the map
					
		}
		else if(obj->m_objectID == trigger::ADVANCED_FOLLOW_TRIGGER){
			auto ego = static_cast<AdvancedFollowTriggerObject*>(obj);
			//add some filters here later
			//probably based on whatever the "add" checkbox means
					
			//add it to the map
			advancedFollowTriggersByTargetGID[ego->m_targetGroupID] = ego;
		}
		//the follow and scale triggers
		else if(obj->m_objectID == trigger::FOLLOW_TRIGGER){
			auto ego = static_cast<EffectGameObject*>(obj);
			followTriggersByTargetGID[ego->m_targetGroupID] = ego;
		}
		else if(obj->m_objectID == trigger::SCALE_TRIGGER){
			auto ego = static_cast<TransformTriggerGameObject*>(obj);
			scaleTriggersByTargetGID[ego->m_targetGroupID] = ego;
		}
	}


	//map for accessing the setups
	//the advfollowtargetgid is the group that the follow and scale triggers set their center around
	std::map<int,ParallaxSetup*> parallaxSetupsByAdvFollowTargetGID;
	//first find and create the parallax setups
	for(auto& kv : advancedFollowTriggersByTargetGID){
		//find an area move trigger thats target GID matches this triggers follow GID
		AdvancedFollowTriggerObject* advancedFollowTrigger = kv.second;
		if(auto matchingAreaMove = areaMoveTriggersByTargetGID[advancedFollowTrigger->m_centerGroupID]){

			auto newSetup = addSetup(matchingAreaMove,advancedFollowTrigger);
			parallaxSetupsByAdvFollowTargetGID[newSetup->m_followID] = newSetup;

		}
	}
	//finally add the layers to their setups
	for(auto& [_,followTrigger] : followTriggersByTargetGID){
		
		int parallaxID = followTrigger->m_centerGroupID;
		int layerID = followTrigger->m_targetGroupID;

		//now get the parallax setup
		auto parallaxSetup = parallaxSetupsByAdvFollowTargetGID[parallaxID];
		if(!parallaxSetup) continue;



		//see if it has a matching scale trigger
		auto scaleTrigger = scaleTriggersByTargetGID[followTrigger->m_targetGroupID];
		if(scaleTrigger) {

			bool matchesTargetID = followTrigger->m_targetGroupID == scaleTrigger->m_targetGroupID;
        	//this can center the root OR the follow object, probably make this display or be editable in the gui later
			//scaling around the root causes bugs sometimes so uhm do something to fix that
			bool matchesFollowID = scaleTrigger->m_centerGroupID == parallaxSetup->m_followID;
			bool matchesRootID = scaleTrigger->m_centerGroupID == parallaxSetup->m_rootID;

			bool isMatchingScaleTrigger = (matchesFollowID || matchesRootID) && matchesTargetID;

        	if(!isMatchingScaleTrigger) scaleTrigger = nullptr;
		}

		parallaxSetup->addLayer(scaleTrigger,followTrigger);
	}
}
ParallaxSetup* ParallaxSetupList::addSetup(EnterEffectObject* areaMoveTrigger,AdvancedFollowTriggerObject* advancedFollowTrigger){
	auto& newSetup = m_setups.emplace_back();

	newSetup.m_areaMoveTriggerPtr = areaMoveTrigger;
	newSetup.m_advancedFollowTriggerPtr = advancedFollowTrigger;

	newSetup.m_rootID = areaMoveTrigger->m_targetGroupID;
	newSetup.m_followID = advancedFollowTrigger->m_targetGroupID;

	return &newSetup;
}

void ParallaxSetupList::removeSetupByIndex(size_t index)
{
	m_setups.erase(m_setups.begin() + index);
}

ParallaxSetupLayer *ParallaxSetup::addLayer(TransformTriggerGameObject *scaleTrigger, EffectGameObject *followTrigger)
{
	auto& newLayer = m_layers.emplace_back();
	int layerGroupID = followTrigger->m_targetGroupID;

	newLayer.m_layerID = layerGroupID;

	newLayer.m_scaleTriggerPtr = scaleTrigger;
	newLayer.m_followTriggerPtr = followTrigger;
    return &newLayer;
}

CCPoint ParallaxSetup::getPositionForNewLayerTriggers()
{
	CCPoint ret = m_areaMoveTriggerPtr->getPosition();
	//get the point above the highest trigger
	for(auto& layer : m_layers){
		auto layerPos = layer.getEditorPosition();
		if(layerPos.y<ret.y) continue;
		ret=layerPos;
	}

	ret.y+=editor::constants::GRID_SIZE;

    return ret;
}

std::vector<ParallaxSetupLayer *> ParallaxSetup::getLayerPointerList()
{
    std::vector<ParallaxSetupLayer*> layerPointers;
    layerPointers.reserve(m_layers.size());
    for(auto& layer : m_layers){
        layerPointers.push_back(&layer);
    }
    
	return layerPointers;
}

std::vector<ParallaxSetupLayer *> ParallaxSetup::sortDepth()
{
	auto ret = getLayerPointerList();

    std::ranges::sort(ret,
        [](const ParallaxSetupLayer* a, const ParallaxSetupLayer* b) {
            return a->getDepth() > b->getDepth();
        }
    );

	return ret;
}
std::vector<ParallaxSetupLayer *> ParallaxSetup::sortGroupID()
{
	auto ret = getLayerPointerList();

    std::ranges::sort(ret,
        [](const ParallaxSetupLayer* a, const ParallaxSetupLayer* b) {
            return a->m_layerID > b->m_layerID;
        }
    );

	return ret;
}

float scaleFromDepth(float depth){
    return 1.0f-depth;
}

//function that returns "mixed" if a float is nan but just the number if otherwise
std::string nanIsMixed(float input){
    return std::string(std::isnan(input) ? "Mixed" : fmt::to_string(input));
}
float ParallaxSetupLayer::getDepth() const
{
	float xmod = m_followTriggerPtr->m_followXMod;
	float ymod = m_followTriggerPtr->m_followYMod;

	if(xmod!=ymod) return NAN;

	return xmod;
}
std::string ParallaxSetupLayer::getDepthString() const
{
	return nanIsMixed(getDepth());
}
float ParallaxSetup::getDuration() const
{
	//return -1 (or infinite time) by default
	if(m_layers.empty()) return -1.0f;

	float duration = m_layers.front().getDuration();

	//make sure remaining layers have the same duration
	//this does go over the first layer twice but im pr sure thats fine cuz this is a deque so doing them in order is faster
	for(auto& layer : m_layers){
		if(duration != layer.getDuration()) return NAN;
	}

	return duration;
}
std::string ParallaxSetup::getDurationString() const
{
	return nanIsMixed(getDuration());
}

size_t ParallaxSetup::getLayerCount() const
{
    return m_layers.size();
}

int ParallaxSetup::getEditorLayer() const
{
    return m_areaMoveTriggerPtr->m_editorLayer;
}

void ParallaxSetup::setDuration(float duration)
{
	for(auto& layer : m_layers)
		layer.setDuration(duration);
}

void ParallaxSetup::deleteAllLayersAndLayerTriggers()
{
	for(auto& layer : m_layers)
		layer.deleteTriggerObjects();

	m_layers.clear();
}

void ParallaxSetup::deleteAllRootAndFollowObjects()
{
	auto rootObjs = editor::objectsWithGroup(m_rootID);
    auto followObjs = editor::objectsWithGroup(m_followID);

	for(auto obj : CCArrayExt<GameObject*>(rootObjs)) editor::object::remove(obj);
    for(auto obj : CCArrayExt<GameObject*>(followObjs)) editor::object::remove(obj);
}

float ParallaxSetupLayer::getDuration() const
{
    return m_followTriggerPtr->m_duration;
}

void ParallaxSetupLayer::setDuration(float duration)
{
	trigger::setDuration(m_followTriggerPtr,duration);
}
void ParallaxSetupLayer::setTriggerValuesByDepth(float depth)
{
	//m_layerDepth = depth;
    
	//now that we got the input depth, set the triggers
    m_followTriggerPtr->m_followXMod = depth;
    m_followTriggerPtr->m_followYMod = depth;
    
	if(!hasScaleTrigger()) return;

    float scale = scaleFromDepth(depth);
    m_scaleTriggerPtr->m_objectScaleX = scale;
    m_scaleTriggerPtr->m_objectScaleY = scale;
}

void ParallaxSetupLayer::deleteTriggerObjects()
{
	if(m_scaleTriggerPtr) editor::object::remove(m_scaleTriggerPtr);
	editor::object::remove(m_followTriggerPtr);
	
	m_scaleTriggerPtr = nullptr;
	m_followTriggerPtr = nullptr;
}

cocos2d::CCPoint ParallaxSetupLayer::getEditorPosition()
{
	if(hasScaleTrigger()){
		return m_scaleTriggerPtr->getPosition();
	}
	
	return m_followTriggerPtr->getPosition();
}

bool ParallaxSetupLayer::hasScaleTrigger() const
{
	return m_scaleTriggerPtr != nullptr;
}
