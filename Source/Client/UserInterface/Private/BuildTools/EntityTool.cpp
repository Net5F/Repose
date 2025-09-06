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
, validTemplateGraphicIndices{}
, selectedTemplateGraphicIndex{0}
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

    // Iterate the set and track which indices contain a graphic.
    // Note: We don't save the facing direction to the template, we just 
    //       default to South and let the user set it.
    const EntityGraphicSet& graphicSet{
        graphicData.getEntityGraphicSet(graphicState.graphicSetID)};
    validTemplateGraphicIndices.clear();

    // Note: Idle is guaranteed to be present in every entity graphic set
    //       (though it may only contain null graphics).
    auto& idleGraphicArr{graphicSet.graphics.at(EntityGraphicType::Idle)};
    for (std::size_t i = 0; i < idleGraphicArr.size(); ++i) {
        if (idleGraphicArr.at(i).getGraphicID() != NULL_GRAPHIC_ID) {
            validTemplateGraphicIndices.emplace_back(i);
        }
    }
    if (validTemplateGraphicIndices.empty()) {
        // There were no non-null graphics. Point at the first null graphic, so
        // we at least have something to reference.
        validTemplateGraphicIndices.emplace_back(0);
    }

    // Select the first graphic within validGraphicIndices.
    selectedTemplateGraphicIndex = 0;
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
            Rotation rotation{static_cast<Rotation::Direction>(
                validTemplateGraphicIndices.at(selectedTemplateGraphicIndex))};
            network.serializeAndSend(EntityInitRequest{
                entt::null, selectedTemplateName, mouseWorldPoint, rotation,
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

void EntityTool::onMouseWheel(int amountScrolled)
{
    // If this tool isn't active or we don't have a template selected, do 
    // nothing.
    if (!isActive
        || (selectedTemplateGraphicState.graphicSetID
            == NULL_ENTITY_GRAPHIC_SET_ID)) {
        return;
    }

    // Select the next valid graphic within the set's Idle type, accounting
    // for negative values.
    selectedTemplateGraphicIndex
        = (selectedTemplateGraphicIndex + amountScrolled
           + validTemplateGraphicIndices.size())
          % validTemplateGraphicIndices.size();

    // Set the newly selected graphic as a phantom at the current location.
    phantomSprites.clear();
    phantomSprites.push_back(getSelectedTemplatePhantomInfo());
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
            phantomSprites.push_back(getSelectedTemplatePhantomInfo());
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

PhantomSpriteInfo EntityTool::getSelectedTemplatePhantomInfo()
{
    // If we don't have a selected graphic set, return early.
    if (!(selectedTemplateGraphicState.graphicSetID)) {
        return {};
    }

    // Set the world position to display the sprite phantom at.
    PhantomSpriteInfo selectedPhantomInfo{};
    selectedPhantomInfo.position = mouseWorldPoint;

    // Note: We already checked that the selected graphic set has a graphic for
    //       selectedTemplateDirection when we set it.
    const EntityGraphicSet& graphicSet{graphicData.getEntityGraphicSet(
        selectedTemplateGraphicState.graphicSetID)};
    selectedPhantomInfo.graphicSet = &graphicSet;
    selectedPhantomInfo.graphicValue
        = static_cast<Uint8>(EntityGraphicType::Idle);
    selectedPhantomInfo.graphicDirection = static_cast<Rotation::Direction>(
        validTemplateGraphicIndices.at(selectedTemplateGraphicIndex));

    return selectedPhantomInfo;
}

} // End namespace Client
} // End namespace AM
