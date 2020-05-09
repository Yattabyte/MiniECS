#pragma once
#ifndef MINI_ECSENTITY_HPP
#define MINI_ECSENTITY_HPP

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
    std::vector<std::tuple<ComponentID, int, ComponentHandle>>
        m_components = {}; ///< Vector of components.
};
};     // namespace mini
#endif // MINI_ECSENTITY_HPP