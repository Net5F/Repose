#include "Log.h"
#include "Application.h"
#include "ProjectUserConfig.h"
#include "MessageProcessorExtension.h"
#include "RendererExtension.h"
#include "SimulationExtension.h"
#include "UserInterfaceExtension.h"

#include "SDL2pp/Exception.hh"

#include <exception>

using namespace AM;
using namespace AM::Client;

// Note: SDL2 needs this signature for main, but we don't use the parameters.
int main(int, char**)
try {
    // Set up file logging.
    // TODO: This currently will do weird stuff if you have 2 clients open.
    //       If we need a temporary solution we can use PIDs, but the real
    //       solution will be to eventually use account IDs in the file name.
    Log::enableFileLogging("Client.log");

    // Construct the app.
    Application app;

    // Init the project user config before it's used in the extension classes.
    ProjectUserConfig::get();

    // Register our extension classes.
    app.registerMessageProcessorExtension<MessageProcessorExtension>();
    app.registerRendererExtension<RendererExtension>();
    app.registerSimulationExtension<SimulationExtension>();
    app.registerUserInterfaceExtension<UserInterfaceExtension>();

    // Start the application (assumes control of the thread).
    app.start();

    return 0;
} catch (SDL2pp::Exception& e) {
    LOG_INFO("Error in: %s  Reason:  %s", e.GetSDLFunction().c_str(),
             e.GetSDLError().c_str());
    return 1;
} catch (std::exception& e) {
    LOG_INFO("%s", e.what());
    return 1;
}
