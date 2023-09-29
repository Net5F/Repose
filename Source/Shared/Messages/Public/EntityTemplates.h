#pragma once

#include "ProjectMessageType.h"
#include "Name.h"
#include "AnimationState.h"
#include <vector>

namespace AM
{

/**
 * Used to send the latest list of entity templates to a client.
 */
struct EntityTemplates {
    // The ProjectMessageType enum value that this message corresponds to.
    // Declares this struct as a message that the Network can send and receive.
    static constexpr ProjectMessageType MESSAGE_TYPE{
        ProjectMessageType::EntityTemplates};

    /** Used as a "we should never hit this" cap on the number of templates that
        we send at once. Only checked in debug builds. */
    static constexpr std::size_t MAX_TEMPLATES{1000};

    /**
     * The data for a single entity template.
     */
    struct Data {
        Name name{};

        AnimationState animationState{};

        // TODO: This should also have init script, or we should 
        //       request it when a template is clicked.
    };

    std::vector<Data> templates;
};

template<typename S>
void serialize(S& serializer, EntityTemplates::Data& data)
{
    serializer.object(data.name);
    serializer.object(data.animationState);
}

template<typename S>
void serialize(S& serializer, EntityTemplates& entityTemplates)
{
    serializer.container(entityTemplates.templates,
                         EntityTemplates::MAX_TEMPLATES);
}

} // End namespace AM