#pragma once
#ifndef MINIECS_ECSCOMPONENT_HPP
#define MINIECS_ECSCOMPONENT_HPP

#include "ecsHandle.hpp"
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

namespace mini {
///////////////////////////////////////////////////////////////////////////
/// Forward Declarations
class ecsBaseComponent;

///////////////////////////////////////////////////////////////////////////
/// Useful Aliases.
using ComponentID = int;
using ComponentDataSpace = std::vector<uint8_t>;
using ComponentCreateFunction = std::function<ComponentID(
    ComponentDataSpace& memory, const ComponentHandle& componentHandle, const EntityHandle& entityHandle,
    const ecsBaseComponent*)>;
using ComponentFreeFunction = std::function<void(ecsBaseComponent* comp)>;

///////////////////////////////////////////////////////////////////////////
/// \class  ecsBaseComponent
/// \brief  A base class representing a component within an ECS architecture.
class ecsBaseComponent {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Destroy this base ecsComponent.
    virtual ~ecsBaseComponent() = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default construct this base ecsComponent.
    ecsBaseComponent() noexcept {}
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a base ecsComponent.
    /// \param	ID      the runtime ID for this component.
    /// \param	size    the byte-size of this component.
    ecsBaseComponent(const ComponentID& ID, const size_t& size) noexcept : m_runtimeID(ID), m_size(size) {}
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Move Constructor.
    ecsBaseComponent(ecsBaseComponent&&) noexcept = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Copy Constructor.
    ecsBaseComponent(const ecsBaseComponent&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Move-Assignment Operator.
    ecsBaseComponent& operator=(ecsBaseComponent&&) noexcept = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Copy-Assignment Operator.
    ecsBaseComponent& operator=(const ecsBaseComponent&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// Public Attributes
    ComponentID m_runtimeID = 0; ///< Runtime generated ID per class.
    size_t m_size = 0;           ///< Total component byte-size.
    ComponentHandle m_handle;    ///< This component's UUID.
    EntityHandle m_entityHandle; ///< This component's parents' UUID (entity).

    protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Register a component into the creation/destruction registry.
    /// \param	createFn    function for creating a specific component type.
    /// \param	freeFn		function for freeing a specific component type.
    /// \param	size		the total size of a single component.
    /// \return				runtime component ID.
    static ComponentID
    registerType(const ComponentCreateFunction& createFn, const ComponentFreeFunction& freeFn, const size_t& size);

    ///////////////////////////////////////////////////////////////////////////
    /// Protected Attributes
    inline static std::vector<std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t>>
        m_componentRegistry = {}; ///< Container for component functions.
    friend class ecsWorld;        ///< Allows the ecsWorld to access.
};

///////////////////////////////////////////////////////////////////////////
/// \class  ecsComponent
/// \brief  A specialized, specific type of component.
/// \tparam	C   the type of this component.
template <typename C> class ecsComponent : public ecsBaseComponent {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct this specific component.
    ecsComponent() noexcept : ecsBaseComponent(ecsComponent::Runtime_ID, sizeof(C)) {}

    ///////////////////////////////////////////////////////////////////////////
    // Public Static Type-Specific Attributes
    static const ComponentID Runtime_ID; ///< Runtime generated ID per class.
};

///////////////////////////////////////////////////////////////////////////
/// \brief Constructs a new component of type <C> into the memory space.
/// \param	memory			raw data vector representing all <C> components.
/// \param	componentHandle handle to the component.
/// \param	entityHandle    handle to the component's parent entity.
/// \return					the index into the memory array for this component.
template <typename ComponentTypeC>
constexpr static int createFn(
    ComponentDataSpace& memory, const ComponentHandle& componentHandle, const EntityHandle& entityHandle,
    const ecsBaseComponent* component) noexcept {
    size_t index = memory.size();
    memory.resize(index + sizeof(ComponentTypeC));
    ComponentTypeC* clone = nullptr;
    if (component == nullptr)
        clone = new (&memory[index]) ComponentTypeC();
    else
        clone = new (&memory[index]) ComponentTypeC(*static_cast<const ComponentTypeC*>(component));
    clone->m_handle = componentHandle;
    clone->m_entityHandle = entityHandle;
    return static_cast<int>(index);
}

///////////////////////////////////////////////////////////////////////////
/// \brief  Destructs the supplied component/
/// \param	component   the component to destruct.
template <typename ComponentType> constexpr static void freeFn(ecsBaseComponent* component) noexcept {
    auto* castComponent = static_cast<ComponentType*>(component);
    castComponent->~ComponentType();
}

///////////////////////////////////////////////////////////////////////////
/// \brief  Generate a runtime static ID for each component class used.
template <typename C> const ComponentID ecsComponent<C>::Runtime_ID(registerType(createFn<C>, freeFn<C>, sizeof(C)));
};     // namespace mini
#endif // MINIECS_ECSCOMPONENT_HPP