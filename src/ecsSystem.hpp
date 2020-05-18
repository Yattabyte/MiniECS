#pragma once
#ifndef MINIECS_ECSYSTEM_HPP
#define MINIECS_ECSYSTEM_HPP

#include "ecsComponent.hpp"
#include <vector>

namespace mini {
///////////////////////////////////////////////////////////////////////////
/// \class  ecsSystem
/// \brief  An interface for an ecsSystem.
class ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Component flag types.
    enum class RequirementsFlag : unsigned int {
        FLAG_REQUIRED = 0,
        FLAG_OPTIONAL = 1
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Virtual system destructor.
    virtual ~ecsSystem() = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a base ecsSystem.
    ecsSystem() = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move a base system.
    ecsSystem(ecsSystem&&) = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Copy a base system.
    ecsSystem(const ecsSystem&) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move-assign a base system.
    ecsSystem& operator=(ecsSystem&&) = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Copy-assign a base system.
    ecsSystem& operator=(const ecsSystem&) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieves the component types supported by this system.
    /// \return	the component types supported by this system.
    std::vector<std::pair<ComponentID, RequirementsFlag>>
    getComponentTypes() const {
        return m_componentTypes;
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieves whether or not this system >= 1 mandatory component.
    /// \return true if the system is valid, false otherwise.
    bool isValid() const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    virtual void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& components) = 0;

    protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Add a component type to be used by this system.
    /// \param	componentType	the type of component to use
    /// \param	componentFlag	flag indicating required/optional.
    void addComponentType(
        const ComponentID& componentType,
        const RequirementsFlag& componentFlag =
            RequirementsFlag::FLAG_REQUIRED);

    private:
    std::vector<std::pair<ComponentID, RequirementsFlag>>
        m_componentTypes; ///< Required component types.
};

///////////////////////////////////////////////////////////////////////////
/// \class  ecsSystemList
/// \brief  An ordered list of systems to be updated.
class ecsSystemList final {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a system list.
    explicit ecsSystemList() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a specific system at a given index.
    /// \param	index	the index to fetch the system from.
    /// \return			the system found at the index specified.
    std::shared_ptr<ecsSystem> operator[](const size_t& index) const noexcept {
        return m_systems[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Generate a system with a specific type and input arguments.
    /// \tparam	T		    the system class type.
    /// \param	...args     arguments to forward to the system constructor.
    template <typename T, class... Args> void makeSystem(Args&&... args) {
        if (const auto system = std::make_shared<T>(args...); system->isValid())
            m_systems.emplace_back(system);
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Adds a system to the list.
    /// \param	system	the system to add.
    bool addSystem(const std::shared_ptr<ecsSystem>& system);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Removes a system from the list.
    /// \param	system	the system to remove.
    /// \return	true if successfully removed, false otherwise.
    bool removeSystem(const ecsSystem& system);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve the number of systems in the list.
    /// \return	the size of the list.
    size_t size() const noexcept { return m_systems.size(); }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve an iterator to the beginning of this system list.
    /// \return	an iterator to the beginning of this system list.
    std::vector<std::shared_ptr<ecsSystem>>::iterator begin() noexcept {
        return m_systems.begin();
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a const iterator to the beginning of this system list.
    /// \return	a const iterator to the beginning of this system list.
    std::vector<std::shared_ptr<ecsSystem>>::const_iterator cbegin() const
        noexcept {
        return m_systems.cbegin();
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve an iterator to the end of this system list.
    /// \return	an iterator to the end of this system list.
    std::vector<std::shared_ptr<ecsSystem>>::iterator end() noexcept {
        return m_systems.end();
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve a const iterator to the end of this system list.
    /// \return	a const iterator to the end of this system list.
    std::vector<std::shared_ptr<ecsSystem>>::const_iterator cend() const
        noexcept {
        return m_systems.cend();
    }

    private:
    std::vector<std::shared_ptr<ecsSystem>> m_systems; ///< List of systems.
};
};     // namespace mini
#endif // MINIECS_ECSYSTEM_HPP