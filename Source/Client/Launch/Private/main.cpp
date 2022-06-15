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
    // TODO: This currently will do weird stuff if you have 2 clients open.
    //       If we need a temporary solution we can use PIDs, but the real
    //       solution will be to eventually use account IDs in the file name.
    AM::Log::enableFileLogging("Client.log");

    // Start the application (assumes control of the thread).
    AM::Client::Application app;
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
