#pragma once

#include "EntityInteractionType.h"
#include "ItemInteractionType.h"
#include "EntityGraphicType.h"
#include "Log.h"
#include <SDL_stdinc.h>
#include <string>

namespace AM
{

/**
 * Shared static functions for converting enums to display strings.
 */
class DisplayStrings
{
public:
    static std::string get(EntityInteractionType interactionType)
    {
        switch (interactionType) {
            // Engine interaction types.
            case EntityInteractionType::Talk:
                return "Talk";
            // Project interaction types.
            case EntityInteractionType::Replant:
                return "Replant";
            default:
                break;
        }

        LOG_ERROR("Tried to get display name for unknown interaction type.");
        return "?";
    }

    static std::string get(ItemInteractionType interactionType)
    {
        switch (interactionType) {
            // Engine interaction types.
            case ItemInteractionType::UseOn:
                return "Use";
            case ItemInteractionType::Destroy:
                return "Destroy";
            case ItemInteractionType::Examine:
                return "Examine";
            // Project interaction types.
            default:
                break;
        }

        LOG_ERROR("Tried to get display name for unknown interaction type.");
        return "?";
    }

    static std::string get(EntityGraphicType graphicType)
    {
        switch (graphicType) {
            // Engine graphic types.
            case EntityGraphicType::IdleSouth:
                return "Idle S";
            case EntityGraphicType::IdleSouthWest:
                return "Idle SW";
            case EntityGraphicType::IdleWest:
                return "Idle W";
            case EntityGraphicType::IdleNorthWest:
                return "Idle NW";
            case EntityGraphicType::IdleNorth:
                return "Idle N";
            case EntityGraphicType::IdleNorthEast:
                return "Idle NE";
            case EntityGraphicType::IdleEast:
                return "Idle E";
            case EntityGraphicType::IdleSouthEast:
                return "Idle SE";
            case EntityGraphicType::RunSouth:
                return "Run S";
            case EntityGraphicType::RunSouthWest:
                return "Run SW";
            case EntityGraphicType::RunWest:
                return "Run W";
            case EntityGraphicType::RunNorthWest:
                return "Run NW";
            case EntityGraphicType::RunNorth:
                return "Run N";
            case EntityGraphicType::RunNorthEast:
                return "Run NE";
            case EntityGraphicType::RunEast:
                return "Run E";
            case EntityGraphicType::RunSouthEast:
                return "Run SE";
            case EntityGraphicType::CrouchSouth:
                return "Crouch S";
            case EntityGraphicType::CrouchSouthWest:
                return "Crouch SW";
            case EntityGraphicType::CrouchWest:
                return "Crouch W";
            case EntityGraphicType::CrouchNorthWest:
                return "Crouch NW";
            case EntityGraphicType::CrouchNorth:
                return "Crouch N";
            case EntityGraphicType::CrouchNorthEast:
                return "Crouch NE";
            case EntityGraphicType::CrouchEast:
                return "Crouch E";
            case EntityGraphicType::CrouchSouthEast:
                return "Crouch SE";
            case EntityGraphicType::JumpSouth:
                return "Jump S";
            case EntityGraphicType::JumpSouthWest:
                return "Jump SW";
            case EntityGraphicType::JumpWest:
                return "Jump W";
            case EntityGraphicType::JumpNorthWest:
                return "Jump NW";
            case EntityGraphicType::JumpNorth:
                return "Jump N";
            case EntityGraphicType::JumpNorthEast:
                return "Jump NE";
            case EntityGraphicType::JumpEast:
                return "Jump E";
            case EntityGraphicType::JumpSouthEast:
                return "Jump SE";
            // Project graphic types.
            default:
                break;
        }

        LOG_ERROR("Tried to get display name for unknown graphic type.");
        return "?";
    }
};

} // namespace AM
