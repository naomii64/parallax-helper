#include "RadioMenu.hpp"
#include "constants.hpp"

constexpr float RADIO_MENU_ENTRY_HEIGHT = 22.0f;

bool RadioMenu::init(const cocos2d::CCSize &size)
{
    if(!CCMenu::init()) return false;

    setContentSize(size);
    setLayout(AnchorLayout::create());

    //create the background
    auto background = NineSlice::create("square02b_001.png");
    background->setColor({0, 0, 0});
    background->setOpacity(44);
    background->setContentSize(size);
    background->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Center)
    );
    addChild(background);
    //create the place where the actual content is held
    m_menu = CCMenu::create();
    //m_menu->setContentSize({
    //    size.width - (constants::ui::PADDING*2),
    //    size.height - (constants::ui::PADDING*2)
    //});
    m_menu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Left)
        ->setOffset({constants::ui::PADDING,0.f})
    );
    m_menu->setLayout(
        AxisLayout::create()
        ->setAxis(Axis::Column)
        ->setAxisReverse(true)
    );
    addChild(m_menu);

    updateLayout();

    return true;
}
RadioMenu* RadioMenu::create(const cocos2d::CCSize &size) {
    auto ret = new RadioMenu();
    if (ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
int RadioMenu::addEntry(const char* labelText)
{
    //:333
    auto newEntry = RadioMenuEntry::create({0,RADIO_MENU_ENTRY_HEIGHT},labelText);
    newEntry->m_parent = this;
    newEntry->m_index = m_menu->getChildrenCount();
    m_menu->addChild(newEntry);

    //prolly doenst need to be ran with EVERY entry but
    //TODO: update the height automatically
    m_menu->updateLayout();

    return newEntry->m_index;
}
void RadioMenu::untoggleAll() {
    for(auto& child : m_menu->getChildrenExt()){
        auto entry = static_cast<RadioMenuEntry*>(child);
        entry->m_checkbox->toggle(false);
    }
}
void RadioMenu::selectEntry(int entryID) {
    if(entryID<0) return;
    if(entryID>=m_menu->getChildrenCount()) return;

    untoggleAll();
    auto entry = m_menu->getChildByIndex<RadioMenuEntry*>(entryID);
    entry->m_checkbox->toggle(true);
    m_selectedIndex = entryID;
};

bool RadioMenuEntry::init(const cocos2d::CCSize &size, const char* labelText)
{
    if(!CCMenu::init()) return false;
    setContentSize(size);
    setLayout(AnchorLayout::create());

    m_checkbox = CCMenuItemExt::createTogglerWithStandardSprites(
        0.6f,
        [this](CCMenuItemToggler* toggler){
            //untoggle all the others
            //this happsn before actually toggling so that will set this one to true
            if(m_parent){
                m_parent->untoggleAll();
                m_parent->m_selectedIndex = this->m_index;
            }
        }
    );
    m_checkbox->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Left)
        ->setOffset({m_checkbox->getContentWidth() * 0.5f,0.f})
    );
    
    addChild(m_checkbox);

    auto label = Label::create(labelText,"bigFont.fnt");
    label->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Left)
        ->setOffset({25.f,0.f})
    );
    label->setAnchorPoint({0.f,0.5f});
    label->setScale(0.5f);
    addChild(label);

    
    updateLayout();
    return true;
}

RadioMenuEntry *RadioMenuEntry::create(const cocos2d::CCSize &size,const char* labelText)
{
    auto ret = new RadioMenuEntry();
    if (ret->init(size,labelText)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
};
