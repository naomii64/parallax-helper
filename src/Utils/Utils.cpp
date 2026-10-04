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

Button *ph::Utils::createButtonWithALittleIconNextToTheText(geode::Button::ButtonCallback callback, const std::string& labelText,float width,float height,const std::string& iconSpritePath,const std::string& backgroundSprite)
{

    auto btn = Button::create(std::move(callback));

    auto bg = NineSlice::create(backgroundSprite);
    btn->setLayout(CopySizeLayout::create());
    bg->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );
    btn->addChild(bg);
    
    btn->setContentSize({width,height});
    bg->setContentSize(btn->getContentSize());

    constexpr float LABEL_SCALE = 0.5f;
    auto label = Label::create(labelText,"bigFont.fnt");
    label->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );
    label->setScale(LABEL_SCALE);
    btn->addChild(label);

    if(!iconSpritePath.empty()){
        //add the icon
        auto iconSprite = CCSprite::create(iconSpritePath.c_str());
        float xOffset = -(label->getContentWidth())/2.0f;
        xOffset*=LABEL_SCALE;
        iconSprite->setScale(0.8);
        iconSprite->setAnchorPoint({1.0f,0.5f});

        //offset the text and the icon so that theyre centered
        float offsetText = iconSprite->getContentWidth()*iconSprite->getScale()*0.5f;

        iconSprite->setLayoutOptions(
            AnchorLayoutOptions::create()
            ->setAnchor(Anchor::Center)
            ->setOffset({xOffset+offsetText,0.0f})
        );


        static_cast<AnchorLayoutOptions*>(label->getLayoutOptions())->setOffset({offsetText,0.0f});

        btn->addChild(iconSprite);
    }

    //make the text more centered on the y
    auto labelLayoutOptions = static_cast<AnchorLayoutOptions*>(label->getLayoutOptions());
    labelLayoutOptions->setOffset({labelLayoutOptions->getOffset().x,1.0f});


    btn->updateLayout();

    return btn;
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