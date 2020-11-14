#pragma once
#ifndef MINIECS_ECSHANDLE_HPP
#define MINIECS_ECSHANDLE_HPP

#include <algorithm>
#include <cstring>

namespace mini {
///////////////////////////////////////////////////////////////////////////
/// \class  ecsHandle
/// \brief  Uniquely identifies elements in the ECS architecture.
class ecsHandle {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Destroy this handle.
    virtual ~ecsHandle() = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Construct an empty handle.
    ecsHandle() noexcept = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a specific handle.
    /// \param	id			specific handle name as char array of size 32.
    explicit ecsHandle(const char id[32]) { std::copy(&id[0], &id[32], &m_uuid[0]); }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Copy Constructor.
    /// \param	other		an other handle to copy from.
    ecsHandle(const ecsHandle& other) noexcept = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Move Constructor.
    /// \param	other		an other handle to move from.
    ecsHandle(ecsHandle&& other) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Move from another handle.
    /// \param	other		an other handle to move from.
    /// \return	reference to this.
    ecsHandle& operator=(ecsHandle&& other) = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Copy from another handle.
    /// \param	other		an other handle to copy from.
    /// \return	reference to this.
    ecsHandle& operator=(const ecsHandle& other) = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Compare against another handle.
    /// \param	other		an other handle to compare against.
    /// \return	true if this handle is the same as the other handle.
    [[nodiscard]] bool operator==(const ecsHandle& other) const noexcept {
        return static_cast<bool>(std::strncmp(m_uuid, other.m_uuid, 32ULL) == 0);
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Compare if this should be ordered before another handle.
    /// \param	other		an other handle to compare against.
    /// \return	true if this handle is the less than the other handle.
    [[nodiscard]] bool operator<(const ecsHandle& other) const noexcept {
        return static_cast<bool>(std::strncmp(m_uuid, other.m_uuid, 32ULL) < 0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Conversion to bool operator.
    /// \return	true if this handle is valid, false otherwise.
    [[nodiscard]] operator bool() const noexcept;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Retrieve if this handle is valid.
    /// \return	true if this handle is valid, false otherwise.
    [[nodiscard]] bool isValid() const noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// Public Attributes
    char m_uuid[32] = { '\0' }; ///< The UUID container.
};

///////////////////////////////////////////////////////////////////////////
/// \class  EntityHandle
/// \brief  Specialized handle for labeling Entities.
struct EntityHandle final : ecsHandle {
    ~EntityHandle() = default;
    EntityHandle() noexcept = default;
    EntityHandle(EntityHandle&&) noexcept = default;
    EntityHandle(const EntityHandle&) noexcept = default;
    explicit EntityHandle(const ecsHandle& handle) noexcept : ecsHandle(handle) {}
    EntityHandle& operator=(EntityHandle&& other) noexcept = default;
    EntityHandle& operator=(const EntityHandle& other) noexcept = default;
};

///////////////////////////////////////////////////////////////////////////
/// \class  ComponentHandle
/// \brief  Specialized handle for labeling Components.
struct ComponentHandle final : ecsHandle {
    ~ComponentHandle() = default;
    ComponentHandle() noexcept = default;
    ComponentHandle(ComponentHandle&&) noexcept = default;
    ComponentHandle(const ComponentHandle&) noexcept = default;
    explicit ComponentHandle(const ecsHandle& handle) noexcept : ecsHandle(handle) {}
    ComponentHandle& operator=(ComponentHandle&& other) noexcept = default;
    ComponentHandle& operator=(const ComponentHandle& other) noexcept = default;
};
};     // namespace mini
#endif // MINIECS_ECSHANDLE_HPP