#pragma once

#include "ProjectMessageType.h"
#include "ClientEntityInit.h"
#include "Rotation.h"
#include <SDL_stdinc.h>
#include <string>
#include <vector>

namespace AM
{

/**
 * Used to send the latest list of dynamic object templates to a client.
 */
struct DynamicObjectTemplates {
    // The MessageType enum value that this message corresponds to.
    // Declares this struct as a message that the Network can send and receive.
    static constexpr ProjectMessageType MESSAGE_TYPE{
        ProjectMessageType::DynamicObjectTemplates};

    /** Used as a "we should never hit this" cap on the number of templates that
        we send at once. Only checked in debug builds. */
    static constexpr std::size_t MAX_TEMPLATES{1000};

    /**
     * The data for a single dynamic object.
     */
    struct Data {
        std::string name{""};

        Rotation rotation{};

        Uint16 spriteSetID{0};
    };

    std::vector<Data> templates;
};

template<typename S>
void serialize(S& serializer, DynamicObjectTemplates::Data& data)
{
    serializer.text1b(data.name, ClientEntityInit::MAX_NAME_LENGTH);
    serializer.object(data.rotation);
    serializer.value2b(data.spriteSetID);
}

template<typename S>
void serialize(S& serializer, DynamicObjectTemplates& dynamicObjectTemplates)
{
    serializer.container(dynamicObjectTemplates.templates,
                         DynamicObjectTemplates::MAX_TEMPLATES);
}

} // End namespace AM
