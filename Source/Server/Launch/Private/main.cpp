#include "Log.h"
#include "Application.h"
#include "Ignore.h"

#include "SDL2pp/Exception.hh"

#include <exception>

int main(int argc, char** argv)
try {
    // SDL2 needs this signature for main, but we don't use the parameters.
    AM::ignore(argc);
    AM::ignore(argv);

    // Set up file logging.
    AM::Log::enableFileLogging("Server.log");

    // Start the application (assumes control of the thread).
    AM::Server::Application app;
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
