#pragma once
#ifndef MINIECS_ECSENTITY_HPP
#define MINIECS_ECSENTITY_HPP

#include "ecsComponent.hpp"
#include "ecsHandle.hpp"
#include <tuple>
#include <vector>

namespace mini {
///////////////////////////////////////////////////////////////////////////
/// \class      ecsEntity
/// \brief      A base class representing components in an ECS architecture.
/// \warning    Don't subclass, functionality derived from components/systems.
struct ecsEntity final {
    EntityHandle m_handle; ///< Unique handle for this entity.
    std::vector<std::tuple<ComponentID, int, ComponentHandle>>
        m_components = {}; ///< Vector of components.
};
};     // namespace mini
#endif // MINIECS_ECSENTITY_HPP