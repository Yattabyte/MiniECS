#pragma once
#ifndef MINIECS_ECSWORLD_HPP
#define MINIECS_ECSWORLD_HPP

#include "ecsComponent.hpp"
#include "ecsEntity.hpp"
#include "ecsHandle.hpp"
#include "ecsSystem.hpp"
#include <array>
#include <tuple>

namespace mini {
///////////////////////////////////////////////////////////////////////////
/// Useful Aliases
using EntityMap = std::map<EntityHandle, std::shared_ptr<ecsEntity>>;
using ComponentMap = std::map<ComponentID, ComponentDataSpace>;

///////////////////////////////////////////////////////////////////////////
/// \class  ecsWorld
/// \brief  A set of ECS entities and components forming a single level.
class ecsWorld {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Destroy this ECS World.
    ~ecsWorld() { clear(); }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct an empty ECS World.
    ecsWorld() {}
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move an ECS world.
    /// \param	other				another ecsWorld to move to here.
    ecsWorld(ecsWorld&& other) noexcept
        : m_components(std::move(other.m_components)),
          m_entities(std::move(other.m_entities)) {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move the data from another ecsWorld into this.
    /// \param	other		another ecsWorld to move the data from.
    /// \return				reference to this.
    ecsWorld& operator=(ecsWorld&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Create an entity from an optional list of components.
    /// \param	components			array of component pointers to hard copy.
    /// \param	numComponents		the number of components in the array.
    EntityHandle makeEntity(
        ecsBaseComponent* const* const components = nullptr,
        const size_t numComponents = 0ULL);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a component to an entity.
    /// \param	entityHandle		handle to the component's parent entity.
    /// \param	component			the component being added.
    ComponentHandle makeComponent(
        const EntityHandle& entityHandle,
        const ecsBaseComponent* const component);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a component to an entity.
    /// \param	entity      		the entity to add the component to.
    /// \param	component			the component being added.
    ComponentHandle
    makeComponent(ecsEntity& entity, const ecsBaseComponent* const component);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generates a component matching the class ID specified.
    /// \param	entityHandle		handle to the component's parent entity.
    /// \param	componentID			the runtime component class.
    ComponentHandle makeComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generates a component matching the class ID specified.
    /// \param	entity      		the entity to add the component to.
    /// \param	componentID			the runtime component class.
    ComponentHandle
    makeComponent(ecsEntity& entity, const ComponentID& componentID);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generates a component from the input template parameter.
    /// \param	entityHandle		handle to the component's parent entity.
    /// \param	componentID			the runtime component class.
    template <typename Component>
    ComponentHandle makeComponent(const EntityHandle& entityHandle) {
        return makeComponent(entityHandle, Component::Runtime_ID);
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generates a component from the input template parameter.
    /// \param	entity      		the entity to add the component to.
    /// \param	componentID			the runtime component class.
    template <typename Component>
    ComponentHandle makeComponent(ecsEntity& entity) {
        return makeComponent(entity, Component::Runtime_ID);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search for and remove an entity matching the specific handle.
    /// \param	entityHandle		handle to the entity to be removed.
    /// \return	true on successful removal, false otherwise.
    bool removeEntity(const EntityHandle& entityHandle);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Remove the entity specified.
    /// \param	entity		        the entity to be removed.
    /// \return	true on successful removal, false otherwise.
    bool removeEntity(ecsEntity& entity);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search for and remove a component matching the specific handle.
    /// \param	componentHandle		handle to the component to be removed.
    /// \return	true on successful removal, false otherwise.
    bool removeComponent(const ComponentHandle& componentHandle);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Remove a specific component class from within a specific entity.
    /// \param	entityHandle		handle component's parent entity.
    /// \param	componentID			the runtime ID component class ID.
    /// \return	true on successful removal, false otherwise.
    bool removeComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Remove a specific component class from within a specific entity.
    /// \param	entity      		the entity to remove the component from.
    /// \param	componentID			the runtime ID component class ID.
    /// \return	true on successful removal, false otherwise.
    bool removeComponent(ecsEntity& entity, const ComponentID& componentID);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Remove a specific component class from within a specific entity.
    /// \param	entity      		the entity to remove the component from.
    /// \return	true on successful removal, false otherwise.
    template <typename Component>
    bool removeComponent(const EntityHandle& entityHandle) {
        return removeComponent(entityHandle, Component::Runtime_ID);
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Remove a specific component class from within a specific entity.
    /// \param	entity      		the entity to remove the component from.
    /// \return	true on successful removal, false otherwise.
    template <typename Component> bool removeComponent(ecsEntity& entity) {
        return removeComponent(entity, Component::Runtime_ID);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find an entity matching the UUID provided.
    /// \param	UUID			the target entity's UUID.
    /// \return	pointer to the found entity on success, nullptr on failure.
    [[nodiscard]] std::shared_ptr<ecsEntity>
    getEntity(const EntityHandle& UUID) const;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find a list of entities matching the UUID's provided.
    /// \param	uuids			list of target entity UUID's.
    /// \return	list of pointers to the found entities (nullptr's omitted).
    [[nodiscard]] std::vector<std::shared_ptr<ecsEntity>>
    getEntities(const std::vector<EntityHandle>& uuids) const;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search for a component type in an entity.
    /// \tparam	T               the category of component being retrieved.
    /// \param	entityHandle	handle to the entity to retrieve from.
    /// \return	a component of type requested on success, nullptr otherwise.
    template <typename Component>
    [[nodiscard]] Component* getComponent(const EntityHandle& entityHandle) {
        return dynamic_cast<Component*>(
            getComponent(entityHandle, Component::Runtime_ID));
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search for a component type in an entity.
    /// \tparam	Component       the category of component being retrieved.
    /// \param	ecsEntity   	the entity to get the component from.
    /// \return	a component of type requested on success, nullptr otherwise.
    template <typename Component>
    [[nodiscard]] Component* getComponent(ecsEntity& entity) {
        return dynamic_cast<Component*>(
            getComponent(entity, Component::Runtime_ID));
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a component.
    /// \param	entityHandle		handle to the entity to retrieve from.
    /// \param	componentID			the runtime class ID of the component.
    /// \return the specific component on success, nullptr otherwise.
    [[nodiscard]] ecsBaseComponent* getComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a component.
    /// \param	ecsEntity   		the entity to get the component from.
    /// \param	componentID			the runtime class ID of the component.
    /// \return the specific component on success, nullptr otherwise.
    [[nodiscard]] ecsBaseComponent*
    getComponent(ecsEntity& entity, const ComponentID& componentID);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to retrieve a component matching the UUID provided.
    /// \tparam	Component   		the class type of component.
    /// \param	componentHandle		the target component's handle.
    /// \return	the component of type T on success, nullptr otherwise.
    template <typename Component>
    [[nodiscard]] Component*
    getComponent(const ComponentHandle& componentHandle) {
        return dynamic_cast<Component*>(getComponent(componentHandle));
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find a component matching the UUID provided.
    /// \param	componentHandle		the target component's UUID.
    /// \return pointer to the found component on success, nullptr on failure.
    [[nodiscard]] ecsBaseComponent*
    getComponent(const ComponentHandle& componentHandle);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find a component matching the runtime ID provided.
    /// \param	entityComponents	the array of entity component IDS.
    /// \param	mem_array			the array of component data.
    /// \param  componentID			the class ID of the component.
    /// \return	the component pointer matching the ID specified.
    [[nodiscard]] static ecsBaseComponent* getComponent(
        const std::vector<std::tuple<ComponentID, int, ComponentHandle>>&
            entityComponents,
        ComponentDataSpace& mem_array, const ComponentID& componentID) noexcept;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a list of entity components corresponding to the input.
    /// \param	componentTypes		list of component types to retrieve.
    template <typename... T_types>
    [[nodiscard]] std::vector<std::tuple<T_types...>> getComponents(
        const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
            componentTypes) {
        std::vector<std::tuple<T_types...>> entityComponents;

        // Cast each component set to the types requested
        for (const auto& groupedComponents :
             getRelevantComponents(componentTypes)) {
            // Convert the component set to a standard array
            std::array<ecsBaseComponent*, sizeof...(T_types)> arr;
            std::copy_n(
                groupedComponents.cbegin(), sizeof...(T_types), arr.begin());

            // Cast the array to our types and emplace it back in our vector
            std::apply(
                [&entityComponents](auto&... args) {
                    entityComponents.emplace_back(
                        dynamic_cast<T_types>(args)...);
                },
                arr);
        }
        return entityComponents;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Clear the data out of this ecsWorld.
    void clear();
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generate a universally unique identifier for entities or
    /// components. \return						a new ID.
    [[nodiscard]] static ecsHandle generateUUID();
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Update the components of all systems provided.
    /// \param	systems				the systems to update.
    /// \param	deltaTime			the delta time.
    void updateSystems(ecsSystemList& systems, const double& deltaTime);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Update the components of a single system.
    /// \param	system				the system to update.
    /// \param	deltaTime			the delta time.
    void updateSystem(ecsSystem& system, const double& deltaTime);
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Update the components of a single system.
    /// \param	deltaTime			the delta time.
    /// \param	componentTypes		list of component types to retrieve.
    /// \param	func				lambda function serving as a system.
    void updateSystem(
        const double& deltaTime,
        const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
            componentTypes,
        const std::function<void(
            const double&, const std::vector<std::vector<ecsBaseComponent*>>&)>&
            func);

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Disallow copying an ECS world.
    ecsWorld(const ecsWorld&) noexcept = delete;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Disallow ECS world copy assignment.
    ecsWorld& operator=(const ecsWorld&) noexcept = delete;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Check if a component ID is valid and registered.
    /// \param	componentID			the component ID to verify.
    /// \return	true if valid and registered, false otherwise.
    [[nodiscard]] static bool
    isComponentIDValid(const ComponentID& componentID) noexcept;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a component to an entity.
    /// \param	entityHandle		handle to the component's parent entity.
    /// \param	componentID			the runtime component class.
    /// \param  component			the component being added.
    ComponentHandle makeComponentInternal(
        const EntityHandle& entityHandle, const ComponentID& componentID,
        const ecsBaseComponent* const component);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a component to an entity.
    /// \param	entity      		the entity to add a component to.
    /// \param	componentID			the runtime component class.
    /// \param  component			the component being added.
    ComponentHandle makeComponentInternal(
        ecsEntity& entity, const ComponentID& componentID,
        const ecsBaseComponent* const component);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Delete a component matching an index and runtime ID.
    /// \param	componentID			the component class/category ID.
    /// \param	index				the component index to delete.
    void
    deleteComponent(const ComponentID& componentID, const ComponentID& index);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve the components relevant to an ECS system.
    /// \param	componentTypes		list of component types to retrieve.
    [[nodiscard]] std::vector<std::vector<ecsBaseComponent*>>
    getRelevantComponents(
        const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
            componentTypes);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Find the least common component.
    /// \param	componentTypes		the component types.
    /// \return	the byte-size of the least common component.
    [[nodiscard]] size_t findLeastCommonComponent(
        const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
            componentTypes);

    ComponentMap m_components = {}; ///< Map of all components in this world.
    EntityMap m_entities = {};      ///< Map of all entities in this world.
};
};     // namespace mini
#endif // MINIECS_ECSWORLD_HPP