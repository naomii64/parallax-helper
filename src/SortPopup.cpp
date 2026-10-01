#include "SortPopup.hpp"

#include "Utils/constants.hpp"
#include "Utils/Utils.hpp"

SortPopup* SortPopup::create(geode::Function<void(SortPopup*)> callback) {
    auto ret = new SortPopup();
    if (ret->init(std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

SortPopup::~SortPopup()
{
    m_callback(this);
    Popup::~Popup();
}

int SortPopup::getSelectedSortingID() const
{
    return m_radioMenu->m_selectedIndex;
}

bool SortPopup::init(geode::Function<void(SortPopup *)> callback)
{
    float popupWidth = 250;
    float popupHeight = 130;

    if (!Popup::init(popupWidth,popupHeight,"GJ_square02.png")) return false;
    // m_closeBtn->setVisible(false);
    
    setTitle("Sort Layers");
    //make a radio menu here
    m_radioMenu = RadioMenu::create({170,70});
    m_radioMenu->setAnchorPoint({0.5f,0.0});
    m_radioMenu->setLayoutOptions(
        AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Bottom)
        ->setOffset({0.0f,20.f})
    );
    m_mainLayer->addChild(m_radioMenu);
    
    //this needs to be in the same order as the enum
    int depthEntry = m_radioMenu->addEntry("By Depth");
    int groupIDEntry = m_radioMenu->addEntry("By GroupID");
    
    m_radioMenu->selectEntry(int(Utils::getSortingType()));

    m_callback = std::move(callback);

    m_mainLayer->updateLayout();
    
    
    return true;
}
