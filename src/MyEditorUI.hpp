#pragma once

#include <Geode/modify/EditorUI.hpp>
class $modify(MyEditorUI, EditorUI) {
	bool init(LevelEditorLayer* p0);
public:
    void setObjectPosition(GameObject* object,const CCPoint& position);
};