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

			if(obj->m_objectID == constants::objectID::AREA_MOVE_TRIGGER){
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
		else if(obj->m_objectID == constants::objectID::ADVANCED_FOLLOW_TRIGGER){
			auto ego = static_cast<AdvancedFollowTriggerObject*>(obj);
			//add some filters here later
			//probably based on whatever the "add" checkbox means
					
			//add it to the map
			advancedFollowTriggersByTargetGID[ego->m_targetGroupID] = ego;
		}
		//the follow and scale triggers
		else if(obj->m_objectID == constants::objectID::FOLLOW_TRIGGER){
			auto ego = static_cast<EffectGameObject*>(obj);
			followTriggersByTargetGID[ego->m_targetGroupID] = ego;
		}
		else if(obj->m_objectID == constants::objectID::SCALE_TRIGGER){
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
	for(auto& kv : followTriggersByTargetGID){
		auto followTrigger = kv.second;
		auto scaleTrigger = scaleTriggersByTargetGID[followTrigger->m_targetGroupID];
		//see if it has a matching scale trigger
		if(!scaleTrigger) continue;

		int parallaxID = followTrigger->m_centerGroupID;
		int layerID = followTrigger->m_targetGroupID;

		//now get the parallax setup
		auto parallaxSetup = parallaxSetupsByAdvFollowTargetGID[parallaxID];
		if(!parallaxSetup) continue;

		//make sure they have the same layer group and parallax effect group
        if(followTrigger->m_targetGroupID!=scaleTrigger->m_targetGroupID) continue;
		//the scale trigger has to share the same target group, but its center group can either be the root or follow group
        //probably make this display or be editable in the gui later
        if(!((
            scaleTrigger->m_centerGroupID==parallaxSetup->m_followID
        )||(
            scaleTrigger->m_centerGroupID==parallaxSetup->m_rootID
        ))) continue;

		parallaxSetup->addLayer(scaleTrigger,followTrigger);
	}
}
ParallaxSetup* ParallaxSetupList::addSetup(EnterEffectObject* p_areaMoveTrigger,AdvancedFollowTriggerObject* p_advancedFollowTrigger){
	auto& newSetup = m_setups.emplace_back();

	newSetup.m_areaMoveTriggerPtr = p_areaMoveTrigger;
	newSetup.m_advancedFollowTriggerPtr = p_advancedFollowTrigger;

	newSetup.m_rootID = p_areaMoveTrigger->m_targetGroupID;
	newSetup.m_followID = p_advancedFollowTrigger->m_targetGroupID;

	return &newSetup;
}


ParallaxSetupLayer *ParallaxSetup::addLayer(TransformTriggerGameObject *p_scaleTrigger, EffectGameObject *p_followTrigger)
{
	auto& newLayer = m_layers.emplace_back();
	int layerGroupID = p_followTrigger->m_targetGroupID;

	newLayer.m_layerID = layerGroupID;
	newLayer.m_layerDepth = p_followTrigger->m_followXMod;

	newLayer.m_scaleTriggerPtr = p_scaleTrigger;
	newLayer.m_followTriggerPtr = p_followTrigger;
    return &newLayer;
}

CCPoint ParallaxSetup::getPositionForNewLayerTriggers()
{
	CCPoint ret = m_areaMoveTriggerPtr->getPosition();
	//get the point above the highest trigger
	for(auto& layer : m_layers){
		auto layerPos = layer.m_scaleTriggerPtr->getPosition();
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
            return a->m_layerDepth < b->m_layerDepth;
        }
    );

	return ret;
}
std::vector<ParallaxSetupLayer *> ParallaxSetup::sortGroupID()
{
	auto ret = getLayerPointerList();

    std::ranges::sort(ret,
        [](const ParallaxSetupLayer* a, const ParallaxSetupLayer* b) {
            return a->m_layerID < b->m_layerID;
        }
    );

	return ret;
}

float scaleFromDepth(float depth){
    return 1.0f-depth;
}
void ParallaxSetupLayer::setTriggerValuesByDepth(float depth)
{
    m_layerDepth = depth;
    //now that we got the input depth, set the triggers
    m_followTriggerPtr->m_followXMod = depth;
    m_followTriggerPtr->m_followYMod = depth;
    
    float scale = scaleFromDepth(depth);
    m_scaleTriggerPtr->m_objectScaleX = scale;
    m_scaleTriggerPtr->m_objectScaleY = scale;
}

