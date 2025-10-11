#pragma once

namespace AM
{
namespace Server
{

/**
 * Tag component for entities that should not be editable.
 *
 * Used to protect our tutors and example entities.
 */
struct NoEdit {
};

template<typename S>
void serialize(S&, NoEdit&)
{
}

} // namespace Server
} // namespace AM
