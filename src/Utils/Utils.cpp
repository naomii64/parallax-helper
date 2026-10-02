#include "Utils.hpp"

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

Utils::LayerSortingType ph::Utils::getSortingType()
{
    int gottenfromthefile = Mod::get()->getSavedValue<int>(constants::keystrings::SAVED_SELECTED_SORTING_ID,0);
    
    return Utils::LayerSortingType(std::clamp(gottenfromthefile,0,int(Utils::LayerSortingType::_count) - 1));
}

geode::ZStringView ph::Utils::getTriggerSprite(int objectID)
{
    //for now only area move matters
    if(objectID == editor::trigger::AREA_MOVE_TRIGGER){
        return "triggerSymbolSquare.png"_spr;
    }

    return "triggerSymbolNormal.png"_spr;
}

void ph::Utils::replaceIDinObjects(int oldID, int newID)
{
    auto objs = nwo5::utils::array::copy(editor::objectsWithGroup(oldID));
	editor::object::removeGroup(objs,oldID);
	editor::object::addGroup(objs,newID);
}

int ph::Utils::getNextFreeGroupID()
{
    return editor::layer()->getNextFreeGroupID(constants::EMPTY_SET);
}

void Utils::enableButton(Button *item)
{
    item->setEnabled(true);
    item->setColor(ccWHITE);
    item->setOpacity(255);   
}
void Utils::disableButton(Button *item)
{
    item->setEnabled(false);
    item->setColor(constants::ui::DISABLED_COLOR);
    item->setOpacity(constants::ui::DISABLED_ALPHA);
}

CCMenu *Utils::createTwoButtonMenu(std::function<void(bool)> callback, const char *btn1Title, const char *btn2Title)
{
    auto menu = CCMenu::create();
    menu->setLayout(
        AxisLayout::create()
        ->setAxis(Axis::Row)
    );

    auto cancelButton = Button::createWithNode(
        ButtonSprite::create(btn1Title),
        [callback](Button*){
            callback(false);
        }
    );
    auto confirmButton = Button::createWithNode(
        ButtonSprite::create(btn2Title),
        [callback](Button*){
            callback(true);
        }
    );
    menu->addChild(cancelButton);
    menu->addChild(confirmButton);
    menu->updateLayout();
    
    //this relates to how the menu gets attatched to its parent object maybe have a dedicated function for this later    
    menu->setAnchorPoint({0.5f,0.0f});
    menu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Bottom)
        ->setOffset({0.0f,constants::ui::PADDING})
    );

    return menu;
}