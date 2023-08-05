#pragma once

#include "ProjectMessageType.h"
#include "EntityInit.h"
#include <SDL_stdinc.h>
#include <string>
#include <vector>

namespace AM
{

/**
 * Used to send the latest list of entity templates to a client.
 */
struct EntityTemplates {
    // The MessageType enum value that this message corresponds to.
    // Declares this struct as a message that the Network can send and receive.
    static constexpr ProjectMessageType MESSAGE_TYPE{
        ProjectMessageType::EntityTemplates};

    /** Used as a "we should never hit this" cap on the number of templates that
        we send at once. Only checked in debug builds. */
    static constexpr std::size_t MAX_TEMPLATES{1000};

    /**
     * The data for a single entity.
     */
    struct Data {
        std::string name{""};

        // TODO: Figure out if we're sending object sprite sets or something else
        Uint16 spriteSetID{0};

        Uint8 spriteIndex{0};
    };

    std::vector<Data> templates;
};

template<typename S>
void serialize(S& serializer, EntityTemplates::Data& data)
{
    serializer.text1b(data.name, EntityInit::NAME_LENGTH);
    serializer.value2b(data.spriteSetID);
    serializer.value1b(data.spriteIndex);
}

template<typename S>
void serialize(S& serializer, EntityTemplates& entityTemplates)
{
    serializer.container(entityTemplates.templates,
                         EntityTemplates::MAX_TEMPLATES);
}

} // End namespace AM
