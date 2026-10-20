#pragma once

#include <Geode/modify/GameObject.hpp>
class $modify(MyGameObject, GameObject) {
	struct Fields {
		//litterally just store this here so this doesnt need to be figured out every frame
        bool m_isOnSelectedParallaxLayer = true;
	};
};