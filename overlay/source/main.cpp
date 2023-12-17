#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header

// Common include for sys-mod communication
#include <syshelo.h>
#include <syshelo/client/ipc.h>

class OverlaySysHelo : public tsl::Overlay {
    virtual void exitServices() override {
        sysheloIpcExit();
    }

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        uint32_t apiVersion;
        smInitialize();

        tsl::hlp::ScopeGuard smGuard([] { smExit(); });

        if(!sysheloIpcRunning())
        {
            return initially<FatalGui>(
                "sys-helo is not running.\n\n"
                "\n"
                "Please make sure it is correctly\n\n"
                "installed and enabled.",
                ""
            );
        }

        if(R_FAILED(sysheloIpcInitialize()) || R_FAILED(sysheloIpcGetAPIVersion(&apiVersion)))
        {
            return initially<FatalGui>(
                "Could not connect to sys-helo.\n\n"
                "\n"
                "Please make sure it is correctly\n\n"
                "installed and enabled.",
                ""
            );
        }

        if(SYSHELO_IPC_API_VERSION != apiVersion)
        {
            return initially<FatalGui>(
                "Overlay not compatible with\n\n"
                "the running sys-helo version.\n\n"
                "\n"
                "Please make sure everything is\n\n"
                "installed and up to date.",
                ""
            );
        }

        return initially<MainGui>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<OverlaySysHelo>(argc, argv);
}
