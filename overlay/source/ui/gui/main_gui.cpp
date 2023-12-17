#include "main_gui.h"

MainGui::MainGui() {}
MainGui::~MainGui() {}

tsl::elm::Element* MainGui::createUI() {
    // A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
    // If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
    auto frame = new tsl::elm::OverlayFrame("Sys-helo", "v0.0.1");

    // A list that can contain sub elements and handles scrolling
    auto list = new tsl::elm::List();

    // List Items
    list->addItem(new tsl::elm::CategoryHeader("Tick Interval"));

    auto tickInterval = new tsl::elm::TrackBar("\u23F1");
    tickInterval->setValueChangedListener([](u8 newValue) {
        FileUtils::LogLine("[ovl] Tick Interval changed to %d", newValue);
        return true;
    });

    // Add the list to the frame for it to be drawn
    frame->setContent(list);

    // Return the frame to have it become the top level element of this Gui
    return frame;
}
