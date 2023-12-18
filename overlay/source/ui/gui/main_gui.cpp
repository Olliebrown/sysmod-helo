#include "main_gui.h"

MainGui::MainGui() {}
MainGui::~MainGui() {}

tsl::elm::Element* MainGui::baseUI()
{
    // A list that can contain sub elements and handles scrolling
    auto list = new tsl::elm::List();

    // List Items
    list->addItem(new tsl::elm::CategoryHeader("Tick Interval"));

    auto tickInterval = new tsl::elm::TrackBar("\u23F1");
    tickInterval->setValueChangedListener([](u8 newValue) {
        // FileUtils::LogLine("[ovl] Tick Interval changed to %d", newValue);
        return true;
    });

    return list;
}
