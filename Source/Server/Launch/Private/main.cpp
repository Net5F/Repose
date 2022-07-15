#include "Log.h"
#include "Application.h"
#include "ProjectUserConfig.h"
#include "MessageProcessorExtension.h"
#include "SimulationExtension.h"
#include "Ignore.h"

#include "SDL2pp/Exception.hh"

#include <exception>

using namespace AM;
using namespace AM::Server;

int main(int argc, char** argv)
try {
    // SDL2 needs this signature for main, but we don't use the parameters.
    ignore(argc);
    ignore(argv);

    // Set up file logging.
    Log::enableFileLogging("Server.log");

    // Construct the app.
    Application app;

    // Init the project user config before it's used in the extension classes.
    ProjectUserConfig::get();

    // Register our extension classes.
    app.registerMessageProcessorExtension<MessageProcessorExtension>();
    app.registerSimulationExtension<SimulationExtension>();

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
