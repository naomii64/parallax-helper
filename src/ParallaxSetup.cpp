#include "ParallaxSetup.hpp"
#include "ObjectID.hpp"

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

					if(obj->m_objectID == objectID::AREA_MOVE_TRIGGER){
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
					else if(obj->m_objectID == objectID::ADVANCED_FOLLOW_TRIGGER){
						auto ego = static_cast<AdvancedFollowTriggerObject*>(obj);
						//add some filters here later
						//probably based on whatever the "add" checkbox means
					
						//add it to the map
						advancedFollowTriggersByTargetGID[ego->m_targetGroupID] = ego;
					}
					//the follow and scale triggers
					else if(obj->m_objectID == objectID::FOLLOW_TRIGGER){
						auto ego = static_cast<EffectGameObject*>(obj);
						followTriggersByTargetGID[ego->m_targetGroupID] = ego;
					}
					else if(obj->m_objectID == objectID::SCALE_TRIGGER){
						auto ego = static_cast<TransformTriggerGameObject*>(obj);
						scaleTriggersByTargetGID[ego->m_targetGroupID] = ego;
					}
				}
				//now all the maps are set up
				//FLAlertLayer::create("scan results!",fmt::format("found {} possible area effect triggers\nfound {} possible advanced follow triggers\nfound {} possible follow triggers\nfound {} possible scale triggers\n",
				//	areaMoveTriggersByTargetGID.size(),
				//	advancedFollowTriggersByTargetGID.size(),
				//	followTriggersByTargetGID.size(),
				//	scaleTriggersByTargetGID.size()
				//),"ok?")->show();;


				//map for accessing the setups
				//the advfollowtargetgid is the group that the follow and scale triggers set their center around
				std::map<int,ParallaxSetup*> parallaxSetupsByAdvFollowTargetGID;
				//first find and create the parallax setups
				for(auto& kv : advancedFollowTriggersByTargetGID){
					//find an area move trigger thats target GID matches this triggers follow GID
					AdvancedFollowTriggerObject* advancedFollowTrigger = kv.second;
					if(auto matchingAreaMove = areaMoveTriggersByTargetGID[advancedFollowTrigger->m_centerGroupID]){


						//add the setup to the list
						//maybe make a constructor for this later
						auto& newSetup = m_setups.emplace_back();
						newSetup.m_rootID = matchingAreaMove->m_targetGroupID;
						//the id the layers of the gradient follow
						newSetup.m_followID = advancedFollowTrigger->m_targetGroupID;
						//store the pointers to the objects
						newSetup.m_advancedFollowTriggerPtr = advancedFollowTrigger;
						newSetup.m_areaMoveTriggerPtr = matchingAreaMove;

						parallaxSetupsByAdvFollowTargetGID[newSetup.m_followID] = &newSetup;

						//FLAlertLayer::create("scan results!",fmt::format("parallax setup detected!\nroot:{}\nfollow:{}",newSetup.m_rootID,newSetup.m_followID),"yipee!")->show();
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

					//TODO: check if the twovalues of the layer line up properly
					auto& newLayer = parallaxSetup->m_layers.emplace_back();
					newLayer.m_layerID = layerID;
					//the depth is the same as the x and y mod value in the follow trigger im pr sure
					//TODO: add checks to make sure the scale and follow trigger have the same depth
					newLayer.m_layerDepth = followTrigger->m_followXMod;
                    
                    //save the pointers to the objects
                    newLayer.m_scaleTriggerPtr = scaleTrigger;
                    newLayer.m_followTriggerPtr = followTrigger;


					//if(auto matchingParallaxSetup = parallaxSetupsByAdvFollowTargetGID[layerGroup]){
					//	//we can add the layer if theres a matching scale trigger (maybe make it possible to not have a matching scale trigger later)
					//	auto scaleTrigger = scaleTriggersByTargetGID[layerGroup];
					//	//their centers need to match too (the center should be the same as the parallax setups group)
					//	if(scaleTrigger->m_centerGroupID!=followTrigger->m_centerGroupID) continue;
					//}
				}


				//now we have the parallax setups setup!
				//for(auto& setup : m_setups){
				//	
				//	gd::string str = "";
				//	str+=fmt::format("rootID:{}\nfollowID:{}\nlayers:\n",setup.m_rootID,setup.m_followID);
				//	for(auto& layer : setup.m_layers){
				//		str+=fmt::format("group:{},depth:{}\n",layer.m_layerID,layer.m_layerDepth);
				//	}
				//	FLAlertLayer::create("parallax setup",str,"yup.")->show();
				//}
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
