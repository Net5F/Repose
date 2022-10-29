#include "WorldSinks.h"
#include "WorldSignals.h"

namespace AM
{
namespace Client
{
WorldSinks::WorldSinks(WorldSignals& worldSignals)
: simulationStarted{worldSignals.simulationStarted}
, serverConnectionError{worldSignals.serverConnectionError}
, tileMapExtentChanged{worldSignals.tileMapExtentChanged}
{
}

} // End namespace Client
} // End namespace AM
