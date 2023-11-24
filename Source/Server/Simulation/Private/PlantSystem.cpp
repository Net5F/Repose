#include "PlantSystem.h"
#include "Simulation.h"
#include "SpriteData.h"
#include "EntityInteractionType.h"
#include "Name.h"
#include "AnimationState.h"
#include "InitScript.h"
#include "Interaction.h"
#include "Plant.h"
#include "NullSpriteID.h"
#include "SharedConfig.h"
#include "Log.h"
#include "AMAssert.h"

namespace AM
{
namespace Server
{

PlantSystem::PlantSystem(Simulation& inSimulation, SpriteData& inSpriteData)
: simulation{inSimulation}
, world{inSimulation.getWorld()}
, spriteData{inSpriteData}
, updateTimer{}
, plantExtent{5, 36, 9, 11}
, sunflowerSpriteSetID{spriteData.getObjectSpriteSet("sunflower").numericID}
{
    // Delete any existing plants.
    auto& entitiesInPlantExtent{world.entityLocator.getEntities(plantExtent)};
    for (entt::entity entity : entitiesInPlantExtent) {
        if (world.registry.all_of<Plant>(entity)) {
            world.registry.destroy(entity);
        }
    }

    // Create all the sunflower entities.
    for (int x{plantExtent.x}; x <= plantExtent.xMax(); ++x) {
        for (int y{plantExtent.y}; y <= plantExtent.yMax(); y += 2) {
            Position tileCenter{TilePosition{x, y}.getCenterPosition()};
            createSapling(tileCenter);
        }
    }
}

void PlantSystem::updatePlants()
{
    // Process any waiting interactions.
    Simulation::EntityInteractionData replantRequest{};
    while (simulation.popEntityInteractionRequest(
        EntityInteractionType::Replant, replantRequest)) {
        replantPlant(replantRequest.targetEntity);
    }

    // Update all the plants.
    if (updateTimer.getTime() >= UPDATE_TIMESTEP_S) {
        auto view{world.registry.view<Plant>()};
        for (entt::entity entity : view) {
            updatePlant(entity);
        }

        updateTimer.reset();
    }
}

void PlantSystem::updatePlant(entt::entity plantEntity)
{
    auto [position, plant] = world.registry.get<Position, Plant>(plantEntity);

    // Keep the plant in the fully grown stage longer than the other stages.
    double updateTimeS{Plant::UPDATE_TIME_S};
    if (plant.lifeStage == Plant::LifeStage::FullyGrown) {
        updateTimeS *= 3;
    }

    // If the plant needs to be updated.
    if (plant.timer.getTime() > updateTimeS) {
        // If the plant is aleady dead, do nothing.
        if (plant.lifeStage == Plant::LifeStage::Dead) {
            return;
        }

        // Update the plant to the next stage.
        Uint8 oldStage{static_cast<Uint8>(plant.lifeStage)};
        Uint8 newStage{static_cast<Uint8>(oldStage + 1)};
        plant.lifeStage = static_cast<Plant::LifeStage>(newStage);

        // If the plant is still alive, update its sprite.
        if (plant.lifeStage != Plant::LifeStage::Dead) {
            world.registry.patch<AnimationState>(
                plantEntity, [&](auto& animationState) {
                    animationState.spriteIndex = newStage;
                });
            plant.timer.reset();
        }
        else {
            // The plant is dead. Replace it with a new entity so we can add 
            // the "Replant" interaction.
            createDeadPlant(position);
            world.registry.destroy(plantEntity);
            return;
        }
    }
}

void PlantSystem::createSapling(const Position& position)
{
    AnimationState animationState{SpriteSet::Type::Object, sunflowerSpriteSetID,
                                  0};
    entt::entity newEntity{world.createEntity(position)};
    world.registry.emplace<Name>(newEntity, "Sunflower");
    world.addGraphicsComponents(newEntity, animationState);

    // Tag it as a Sapling.
    Plant plant{Plant::LifeStage::Sapling};
    world.registry.emplace<Plant>(newEntity, plant);
}

void PlantSystem::createDeadPlant(const Position& position)
{
    AnimationState animationState{SpriteSet::Type::Object, sunflowerSpriteSetID,
                                  3};
    entt::entity newEntity{world.createEntity(position)};
    world.registry.emplace<Name>(newEntity, "Dead Sunflower");
    world.addGraphicsComponents(newEntity, animationState);

    // Tag it as Dead.
    Plant plant{Plant::LifeStage::Dead};
    world.registry.emplace<Plant>(newEntity, plant);

    // Give it a Replant interaction.
    Interaction& interaction{
        world.registry.get_or_emplace<Interaction>(newEntity)};
    interaction.add(EntityInteractionType::Replant);
}

void PlantSystem::replantPlant(entt::entity oldPlant)
{
    // Verify that the given entity is a dead plant.
    if (!(world.entityIDIsInUse(oldPlant))
        || !(world.registry.all_of<Plant>(oldPlant))) {
        LOG_ERROR("Tried to replant entity that isn't a plant.");
        return;
    }
    const Plant& plant{world.registry.get<Plant>(oldPlant)};
    if (plant.lifeStage != Plant::LifeStage::Dead) {
        LOG_ERROR("Tried to replant plant that isn't dead.");
        return;
    }

    // Create the new sapling and delete the old dead plant.
    createSapling(world.registry.get<Position>(oldPlant));
    world.registry.destroy(oldPlant);
}

} // End namespace Server
} // End namespace AM
