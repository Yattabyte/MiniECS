#pragma once
#ifndef ECSWORLD_HPP
#define ECSWORLD_HPP

#include "ecsComponent.hpp"
#include "ecsEntity.hpp"
#include "ecsHandle.hpp"
#include "ecsSystem.hpp"

///////////////////////////////////////////////////////////////////////////
/// Useful Aliases
using EntityMap = std::map<EntityHandle, std::shared_ptr<ecsEntity>>;

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
    ecsWorld() = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move an ECS world.
    /// \param	other				another ecsWorld to move to here.
    ecsWorld(ecsWorld&& other) noexcept
        : m_components(std::move(other.m_components)),
          m_entities(std::move(other.m_entities)) {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Create an entity from a list of input components.
    /// \param	components			array of component pointers to hard copy.
    /// \param	numComponents		the number of components in the array.
    EntityHandle makeEntity(
        const ecsBaseComponent* const* const components,
        const size_t& numComponents);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a component to an entity.
    /// \param	entityHandle		handle to the component's parent entity.
    /// \param	component			the component being added.
    ComponentHandle makeComponent(
        const EntityHandle& entityHandle,
        const ecsBaseComponent* const component);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a component to an entity.
    /// \param	entityHandle		handle to the component's parent entity.
    /// \param	componentID			the runtime component class.
    /// \param  component			the component being added.
    ComponentHandle makeComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID,
        const ecsBaseComponent* const component);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search for and remove an entity matching the specific handle.
    /// \param	entityHandle		handle to the entity to be removed.
    /// \return	true on successful removal, false otherwise.
    bool removeEntity(const EntityHandle& entityHandle);
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
    bool removeEntityComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find an entity matching the UUID provided.
    /// \param	UUID			the target entity's UUID.
    /// \return	pointer to the found entity on success, nullptr on failure.
    std::shared_ptr<ecsEntity> getEntity(const EntityHandle& UUID) const;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find a list of entities matching the UUID's provided.
    /// \param	uuids			list of target entity UUID's.
    /// \return	list of pointers to the found entities (nullptr's omitted).
    std::vector<std::shared_ptr<ecsEntity>>
    getEntities(const std::vector<EntityHandle>& uuids) const;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search for a component type in an entity.
    /// \tparam	T               the category of component being retrieved.
    /// \param	entityHandle	handle to the entity to retrieve from.
    /// \return	a component of type requested on success, nullptr otherwise.
    template <typename T>
    inline T* getComponent(const EntityHandle& entityHandle) const {
        if (auto* component = getComponent(entityHandle, T::Runtime_ID))
            return dynamic_cast<T*>(component);
        return nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a component.
    /// \param	entityHandle		handle to the entity to retrieve from.
    /// \param	componentID			the runtime class ID of the component.
    /// \return the specific component on success, nullptr otherwise.
    ecsBaseComponent* getComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID) const;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to retrieve a component matching the UUID provided.
    /// \tparam	T					the class type of component.
    /// \param	componentHandle		the target component's handle.
    /// \return	the component of type T on success, nullptr otherwise.
    template <typename T>
    inline T* getComponent(const ComponentHandle& componentHandle) const {
        if (auto* component = getComponent(componentHandle))
            return dynamic_cast<T*>(component);
        return nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find a component matching the UUID provided.
    /// \param	componentHandle		the target component's UUID.
    /// \return pointer to the found component on success, nullptr on failure.
    ecsBaseComponent*
    getComponent(const ComponentHandle& componentHandle) const;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Try to find a component matching the runtime ID provided.
    /// \param	entityComponents	the array of entity component IDS.
    /// \param	mem_array			the array of component data.
    /// \param  componentID			the class ID of the component.
    /// \return	the component pointer matching the ID specified.
    static ecsBaseComponent* getComponent(
        const std::vector<std::tuple<ComponentID, int, ComponentHandle>>&
            entityComponents,
        const ComponentDataSpace& mem_array,
        const ComponentID& componentID) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move the data from another ecsWorld into this.
    /// \param	other		another ecsWorld to move the data from.
    /// \return				reference to this.
    ecsWorld& operator=(ecsWorld&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Clear the data out of this ecsWorld.
    void clear();
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generate a universally unique identifier for entities or
    /// components. \return						a new ID.
    static ecsHandle generateUUID();
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Update the components of all systems provided.
    /// \param	systems				the systems to update.
    /// \param	deltaTime			the delta time.
    void updateSystems(ecsSystemList& systems, const double& deltaTime);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Update the components of a single system.
    /// \param	system				the system to update.
    /// \param	deltaTime			the delta time.
    void updateSystem(ecsSystem* system, const double& deltaTime);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Update the components of a single system.
    /// \param	system				the system to update.
    /// \param	deltaTime			the delta time.
    void updateSystem(
        const std::shared_ptr<ecsSystem>& system, const double& deltaTime);
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
    static bool isComponentIDValid(const ComponentID& componentID) noexcept {
        return (componentID < ecsBaseComponent::m_componentRegistry.size());
    }
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
    size_t findLeastCommonComponent(
        const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
            componentTypes);

    ComponentMap m_components; ///< Map of all components in this world.
    EntityMap m_entities;      ///< Map of all entities in this world.
};

#endif // ECSWORLD_HPP