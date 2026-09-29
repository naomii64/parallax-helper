#include "PopupUtils.hpp"

#include "constants.hpp"

CCMenu *PopupUtils::createTwoButtonMenu(std::function<void(bool)> callback,const char* btn1Title, const char* btn2Title)
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