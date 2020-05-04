#pragma once
#ifndef ECSWORLD_HPP
#define ECSWORLD_HPP

#include "ecsComponent.hpp"
#include "ecsEntity.hpp"
#include "ecsHandle.hpp"
#include "ecsSystem.hpp"

/** Useful Aliases. */
using EntityMap = std::map<EntityHandle, std::shared_ptr<ecsEntity>>;

/** A set of ECS entities and components forming a single level. */
class ecsWorld {
    public:
    // Public (De)Constructors
    /** Destroy this ECS World. */
    ~ecsWorld();
    /** Construct an empty ECS World. */
    ecsWorld();
    /** Move an ECS world.
    @param	other				another ecsWorld to move to here. */
    ecsWorld(ecsWorld&& other) noexcept;

    /////////////////////////////
    /// PUBLIC MAKE FUNCTIONS ///
    /////////////////////////////
    /** Create an entity from a list of input components.
    @param	components			array of component pointers, whom will be hard
    copied.
    @param	numComponents		the number of components in the array. */
    EntityHandle makeEntity(
        const ecsBaseComponent* const* const components,
        const size_t& numComponents);
    /** Adds a component to an entity.
    @param	entityHandle		handle to the entity to add the component to.
    @param	component			the component being added. */
    ComponentHandle makeComponent(
        const EntityHandle& entityHandle,
        const ecsBaseComponent* const component);
    /** Adds a component to an entity.
    @param	entityHandle		handle to the entity to add the component to.
    @param	componentID			the runtime component class.
    @param	component			the component being added. */
    ComponentHandle makeComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID,
        const ecsBaseComponent* const component);

    ///////////////////////////////
    /// PUBLIC REMOVE FUNCTIONS ///
    ///////////////////////////////
    /** Search for and remove an entity matching the specific handle provided.
    @param	entityHandle		handle to the entity to be removed.
    @return						true on successful removal, false otherwise. */
    bool removeEntity(const EntityHandle& entityHandle);
    /** Search for and remove a component matching the specific handle provided.
    @param	componentHandle		handle to the component to be removed.
    @return						true on successful removal, false otherwise. */
    bool removeComponent(const ComponentHandle& componentHandle);
    /** Remove a specific component class from within a specific entity.
    @param	entityHandle		handle to the entity to remove the component
    from.
    @param	componentID			the runtime ID identifying the component class.
    @return						true on successful removal, false otherwise. */
    bool removeEntityComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID);

    ////////////////////////////
    /// PUBLIC GET FUNCTIONS ///
    ////////////////////////////
    /** Try to find an entity matching the UUID provided.
    @param	UUID				the target entity's UUID.
    @return						pointer to the found entity on success,
    nullptr on failure. */
    std::shared_ptr<ecsEntity> getEntity(const EntityHandle& UUID) const;
    /** Try to find a list of entities matching the UUID's provided.
    @param	uuids				list of target entity UUID's.
    @return						list of pointers to the found entities.
    Dimensions may not match input list (nullptr's omitted). */
    std::vector<std::shared_ptr<ecsEntity>>
    getEntities(const std::vector<EntityHandle>& uuids) const;
    /** Try to retrieve a component of a specific type from an entity matching
    the handle supplied.
    @tparam	T					the category of component being retrieved.
    @param	entityHandle		handle to the entity to retrieve from.
    @return						the specific component of the type requested
    on success, nullptr otherwise. */
    template <typename T>
    inline T* getComponent(const EntityHandle& entityHandle) const {
        if (auto* component = getComponent(entityHandle, T::Runtime_ID))
            return dynamic_cast<T*>(component);
        return nullptr;
    }
    /** Retrieve a component.
    @param	entityHandle		handle to the entity to retrieve from.
    @param	componentID			the runtime ID identifying the component class.
    @return						the specific component on success, nullptr
    otherwise. */
    ecsBaseComponent* getComponent(
        const EntityHandle& entityHandle, const ComponentID& componentID) const;
    /** Try to retrieve a component of a specific type matching the UUID
    provided.
    @tparam	T					the category of component being retrieved.
    @param	componentHandle		the target component's handle.
    @return						the specific component of the type requested
    on success, nullptr otherwise. */
    template <typename T>
    inline T* getComponent(const ComponentHandle& componentHandle) const {
        if (auto* component = getComponent(componentHandle))
            return dynamic_cast<T*>(component);
        return nullptr;
    }
    /** Try to find a component matching the UUID provided.
    @param	componentHandle		the target component's UUID.
    @return						pointer to the found component on success,
    nullptr on failure. */
    ecsBaseComponent*
    getComponent(const ComponentHandle& componentHandle) const;
    /** Retrieve the component from an entity matching the class specified.
    @param	entityComponents	the array of entity component IDS.
    @param	mem_array			the array of component data.
    @param	componentID			the class ID of the component.
    @return						the component pointer matching the ID
    specified. */
    static ecsBaseComponent* getComponent(
        const std::vector<std::tuple<ComponentID, int, ComponentHandle>>&
            entityComponents,
        const ComponentDataSpace& mem_array,
        const ComponentID& componentID) noexcept;

    ////////////////////////
    /// PUBLIC FUNCTIONS ///
    ////////////////////////
    /** Move the data from another ecsWorld into this.
    @param	other		another ecsWorld to move the data from.
    @return				reference to this. */
    ecsWorld& operator=(ecsWorld&& other) noexcept;
    /** Disallow ECS world copy assignment. */
    inline ecsWorld& operator=(const ecsWorld&) noexcept = delete;
    /** Clear the data out of this ecsWorld. */
    void clear();
    /** Generate a universally unique identifier for entities or components.
    @return						a new ID. */
    static ecsHandle generateUUID();
    /** Update the components of all systems provided.
    @param	systems				the systems to update.
    @param	deltaTime			the delta time. */
    void updateSystems(ecsSystemList& systems, const double& deltaTime);
    /** Update the components of a single system.
    @param	system				the system to update.
    @param	deltaTime			the delta time. */
    void updateSystem(ecsBaseSystem* system, const double& deltaTime);
    /** Update the components of a single system.
    @param	system				the system to update.
    @param	deltaTime			the delta time. */
    void updateSystem(
        const std::shared_ptr<ecsBaseSystem>& system, const double& deltaTime);
    /** Update the components of a single system.
    @param	deltaTime			the delta time.
    @param	componentTypes		list of component types to retrieve.
    @param	func				lambda function serving as a system. */
    void updateSystem(
        const double& deltaTime,
        const std::vector<
            std::pair<ComponentID, ecsBaseSystem::RequirementsFlag>>&
            componentTypes,
        const std::function<void(
            const double&, const std::vector<std::vector<ecsBaseComponent*>>&)>&
            func);

    private:
    // Private Methods
    /** Disallow copying an ECS world. */
    inline ecsWorld(const ecsWorld&) noexcept = delete;
    /** Check if a given component ID has been previously registered and deemed
    valid.
    @param	componentID			the component ID to verify.
    @return						true if valid and registered, false otherwise.
  */
    static bool isComponentIDValid(const ComponentID& componentID) noexcept;
    /** Delete a component matching the category ID supplied, at the given
    index.
    @param	componentID			the component class/category ID.
    @param	index				the component index to delete. */
    void
    deleteComponent(const ComponentID& componentID, const ComponentID& index);
    /** Retrieve the components relevant to an ECS system.
    @param	componentTypes		list of component types to retrieve. */
    [[nodiscard]] std::vector<std::vector<ecsBaseComponent*>>
    getRelevantComponents(
        const std::vector<
            std::pair<ComponentID, ecsBaseSystem::RequirementsFlag>>&
            componentTypes);
    /** Find the least common component.
    @param	componentTypes		the component types.
    @return						the byte-size of the least common component. */
    size_t findLeastCommonComponent(
        const std::vector<
            std::pair<ComponentID, ecsBaseSystem::RequirementsFlag>>&
            componentTypes);

    // Private Attributes
    ComponentMap m_components;
    EntityMap m_entities;
};

#endif // ECSWORLD_HPP