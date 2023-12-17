#include <tesla.hpp>    // The Tesla Header

class MainGui : public tsl::Gui {
public:
    MainGui();
    virtual ~MainGui();
    
    virtual tsl::elm::Element* createUI() override;
};
