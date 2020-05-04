#pragma once
#ifndef ECSENTITY_HPP
#define ECSENTITY_HPP

#include "ecsComponent.hpp"
#include "ecsHandle.hpp"
#include <tuple>
#include <vector>

/** A base class representing components in an ECS architecture. */
class ecsEntity {
    public:
    // Public Attributes
    std::vector<std::tuple<ComponentID, int, ComponentHandle>>
        m_components = {};
};
#endif // ECSENTITY_HPP