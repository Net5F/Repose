#include "EntityTool.h"
#include "World.h"
#include "Network.h"
#include "GraphicData.h"
#include "WorldObjectLocator.h"
#include "EntityInitRequest.h"
#include "IsClientEntity.h"
#include "GraphicState.h"
#include "QueuedEvents.h"
#include "AMAssert.h"
#include "entt/entity/entity.hpp"

namespace AM
{
namespace Client
{

EntityTool::EntityTool(World& inWorld,
                       const WorldObjectLocator& inWorldObjectLocator,
                       Network& inNetwork, GraphicData& inGraphicData)
: BuildTool(inWorld, inNetwork)
, worldObjectLocator{inWorldObjectLocator}
, graphicData{inGraphicData}
, highlightColor{255, 255, 255, 255}
, selectedEntityID{entt::null}
, selectedTemplateName{""}
, selectedTemplateGraphicState{}
{
    // Listen for destruction events for the current selected object.
    world.registry.on_destroy<entt::entity>()
        .connect<&EntityTool::onEntityDestroyed>(this);
}

void EntityTool::setSelectedTemplate(const Name& name,
                                     const GraphicState& graphicState)
{
    // Save the name and graphic state.
    selectedTemplateName = name;
    selectedTemplateGraphicState = graphicState;
}

void EntityTool::setOnEntitySelected(
    std::function<void(entt::entity entity)> inOnEntitySelected)
{
    onEntitySelected = inOnEntitySelected;
}

void EntityTool::setOnSelectionCleared(
    std::function<void(void)> inOnSelectionCleared)
{
    onSelectionCleared = inOnSelectionCleared;
}

void EntityTool::onMouseDown(AUI::MouseButtonType buttonType,
                             const SDL_Point& cursorPosition)
{
    // Note: mouseTilePosition is set in onMouseMove().

    // If this tool is active and the user left clicked.
    if (isActive && (buttonType == AUI::MouseButtonType::Left)) {
        // If a template is selected in the content panel.
        if (selectedTemplateGraphicState.graphicSetID
            != NULL_ENTITY_GRAPHIC_SET_ID) {
            // Tell the sim to create an object based on the template.
            network.serializeAndSend(EntityInitRequest{
                entt::null, selectedTemplateName, mouseWorldPoint,
                selectedTemplateGraphicState});

            // To deter users from placing a million entities, we deselect after
            // placement. This also makes it faster if the user's next goal is
            // to select the object and modify it.
            clearCurrentSelection();
        }
        // Else we don't have a template selected. If we hit an entity, try
        // to select it.
        else {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                trySelectEntity(*entity);
            }
        }
    }
    else if (isActive && (buttonType == AUI::MouseButtonType::Right)) {
        // The user right clicked. Deselect the current selection.
        clearCurrentSelection();
    }
}

void EntityTool::onMouseDoubleClick(AUI::MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    onMouseDown(buttonType, cursorPosition);
}

void EntityTool::onMouseMove(const SDL_Point& cursorPosition)
{
    // Call the parent function to update mouse position and isActive.
    BuildTool::onMouseMove(cursorPosition);

    // Clear any old phantoms and color mods.
    phantomSprites.clear();
    spriteColorMods.clear();

    // If we have a selected entity, highlight it.
    if (selectedEntityID != entt::null) {
        spriteColorMods.emplace_back(selectedEntityID, highlightColor);
    }

    // If this tool is active.
    if (isActive) {
        // If we have a template selected, set the selected sprite as a phantom
        // at the new location.
        if (selectedTemplateGraphicState.graphicSetID
            != NULL_ENTITY_GRAPHIC_SET_ID) {
            PhantomSpriteInfo phantomInfo{};
            setSelectedTemplateSprite(phantomInfo);
            phantomSprites.push_back(phantomInfo);
        }
        // Else we don't have a selection. Check if we're hovering an entity.
        else {
            // Get the first world object under the mouse.
            WorldObjectID objectID{
                worldObjectLocator.getObjectUnderPoint(cursorPosition)};

            // If we hit a non-client entity that isn't selected, highlight it.
            if (entt::entity* entity = std::get_if<entt::entity>(&objectID)) {
                if (!(world.registry.all_of<IsClientEntity>(*entity))
                    && (*entity != selectedEntityID)) {
                    spriteColorMods.emplace_back(*entity, highlightColor);
                }
            }
        }
    }
}

void EntityTool::onMouseLeave()
{
    isActive = false;
    phantomSprites.clear();
    spriteColorMods.clear();

    // If we have a selected entity, keep it highlighted.
    if (selectedEntityID != entt::null) {
        spriteColorMods.emplace_back(selectedEntityID, highlightColor);
    }
}

void EntityTool::trySelectEntity(entt::entity entity)
{
    // If this isn't a client entity and it isn't already selected, select it.
    if (!(world.registry.all_of<IsClientEntity>(entity))
        && (entity != selectedEntityID)) {
        clearCurrentSelection();

        selectedEntityID = entity;

        if (onEntitySelected != nullptr) {
            onEntitySelected(selectedEntityID);
        }
    }
}

void EntityTool::onEntityDestroyed(entt::registry&, entt::entity entity)
{
    // If the selected entity was destroyed, clear our selection.
    if (entity == selectedEntityID) {
        clearCurrentSelection();
    }
}

void EntityTool::clearCurrentSelection()
{
    // If we have an entity or template selected, clear everything.
    if ((selectedEntityID != entt::null)
        || (selectedTemplateGraphicState.graphicSetID
            != NULL_ENTITY_GRAPHIC_SET_ID)) {
        selectedEntityID = entt::null;
        selectedTemplateName = {};
        selectedTemplateGraphicState = {};
        phantomSprites.clear();
        spriteColorMods.clear();

        if (onSelectionCleared != nullptr) {
            onSelectionCleared();
        }
    }
}

void EntityTool::setSelectedTemplateSprite(PhantomSpriteInfo& phantomSpriteInfo)
{
    if (!(selectedTemplateGraphicState.graphicSetID)) {
        phantomSpriteInfo.graphicSet = nullptr;
        phantomSpriteInfo.graphicValue = 0;
        return;
    }

    // Note: IdleSouth is guaranteed to be present in every entity graphic set.
    const EntityGraphicSet& graphicSet{graphicData.getEntityGraphicSet(
        selectedTemplateGraphicState.graphicSetID)};
    phantomSpriteInfo.graphicSet = &graphicSet;
    phantomSpriteInfo.graphicValue
        = static_cast<Uint8>(EntityGraphicType::IdleSouth);
}

} // End namespace Client
} // End namespace AM
