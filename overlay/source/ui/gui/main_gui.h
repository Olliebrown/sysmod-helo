#pragma once

#include <tesla.hpp>    // The Tesla Header

#include "base_gui.h"

class MainGui : public BaseGui {
public:
    MainGui();
    ~MainGui();

    tsl::elm::Element* baseUI() override;
};
