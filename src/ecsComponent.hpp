#pragma once
#ifndef ECSCOMPONENT_HPP
#define ECSCOMPONENT_HPP

#include "ecsHandle.hpp"
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

/** Forward Declarations. */
class ecsBaseComponent;

/** Useful Aliases. */
using ComponentID = int;
using ComponentDataSpace = std::vector<uint8_t>;
using ComponentMap = std::map<ComponentID, ComponentDataSpace>;
using ComponentCreateFunction = std::function<ComponentID(
    ComponentDataSpace& memory, const ComponentHandle& componentHandle,
    const EntityHandle& entityHandle, const ecsBaseComponent*)>;
using ComponentNewFunction = std::function<std::shared_ptr<ecsBaseComponent>()>;
using ComponentFreeFunction = std::function<void(ecsBaseComponent* comp)>;

/** A base class representing components in an ECS architecture. */
class ecsBaseComponent {
    public:
    // Public (De)Constructors
    /** Destroy this base ecsComponent. */
    virtual ~ecsBaseComponent() = default;
    /** Construct a base ecsComponent.
    @param	ID			the runtime ID for this component.
    @param	size		the byte-size of this component. */
    ecsBaseComponent(const ComponentID& ID, const size_t& size) noexcept;
    /** Default Move Constructor. */
    ecsBaseComponent(ecsBaseComponent&&) noexcept = default;
    /** Default Copy Constructor. */
    ecsBaseComponent(const ecsBaseComponent&) noexcept = default;
    /** Default Move-Assignment Operator. */
    ecsBaseComponent& operator=(ecsBaseComponent&&) noexcept = default;
    /** Default Copy-Assignment Operator. */
    ecsBaseComponent& operator=(const ecsBaseComponent&) noexcept = default;

    // Public Attributes
    /** Runtime generated ID per class. */
    ComponentID m_runtimeID;
    /** Total component byte-size. */
    size_t m_size;
    /** This component's UUID. */
    ComponentHandle m_handle;
    /** This component's parents' UUID (entity). */
    EntityHandle m_entityHandle;

    protected:
    // Protected Methods
    /** Register a specific sub-class component into the component registry for
    creating and freeing them at runtime.
    @param	createFn	function for creating a specific component type within
    an input memory block.
    @param	freeFn		function for freeing a specific component type from its
    memory block.
    @param	newFn		function for creating a new component anywhere.
    @param	size		the total size of a single component.ange.
    @return				runtime component ID. */
    static ComponentID registerType(
        const ComponentCreateFunction& createFn,
        const ComponentFreeFunction& freeFn, const ComponentNewFunction& newFn,
        const size_t& size);

    // Protected Attributes
    /** Runtime container mapping indices to creation/destruction functions for
     * components. */
    static std::vector<std::tuple<
        ComponentCreateFunction, ComponentFreeFunction, ComponentNewFunction,
        size_t>>
        m_componentRegistry;
    /** Allow the ecsWorld to interact with these members. */
    friend class ecsWorld;
};

/** A specialized, specific type of component.
@param	C	the type of this component */
template <typename C> struct ecsComponent : public ecsBaseComponent {
    // (De)Constructors
    /** Construct this specific component. */
    ecsComponent() noexcept
        : ecsBaseComponent(ecsComponent::Runtime_ID, sizeof(C)) {}

    // Static Type-Specific Attributes
    /** Runtime generated ID per class, also stored in base-component. */
    static const ComponentID Runtime_ID;
};

/** Constructs a new component of type <C> into the memory space provided.
@param	memory			raw data vector representing all components of type <C>.
@param	componentHandle	handle to the component.
@param	entityHandle	handle to the component's parent entity.
@return					the index into the memory array for this component. */
template <typename C>
constexpr static int createFn(
    ComponentDataSpace& memory, const ComponentHandle& componentHandle,
    const EntityHandle& entityHandle,
    const ecsBaseComponent* component) noexcept {
    const size_t index = memory.size();
    memory.resize(index + sizeof(C));
    C* clone = new (&memory[index]) C(*(C*)component);
    clone->m_handle = componentHandle;
    clone->m_entityHandle = entityHandle;
    return static_cast<int>(index);
}

/** Construct a new component of type <C> on the heap.
@return					shared pointer to the new component. */
template <typename C> constexpr static auto newFn() noexcept {
    return std::make_shared<C>();
}

/** Destructs the supplied component, invalidating the memory range it occupied.
@param	component		the component to destruct. */
template <typename C>
constexpr static void freeFn(ecsBaseComponent* component) noexcept {
    if (auto* c = dynamic_cast<C*>(component))
        c->~C();
}

/** Generate a static ID at run time for each type of component class used. */
template <typename C>
const ComponentID ecsComponent<C>::Runtime_ID(
    registerType(createFn<C>, freeFn<C>, newFn<C>, sizeof(C)));

#endif // ECSCOMPONENT_HPP