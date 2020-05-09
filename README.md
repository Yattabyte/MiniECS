[![Linux](https://img.shields.io/travis/yattabyte/MiniECS?label=Linux%20Build&logo=Travis)](https://travis-ci.com/Yattabyte/MiniECS)
[![Windows](https://img.shields.io/appveyor/ci/yattabyte/MiniECS?label=Windows%20Build&logo=Appveyor)](https://ci.appveyor.com/project/Yattabyte/MiniECS)
[![CodeFactor](https://img.shields.io/codefactor/grade/github/yattabyte/MiniECS?label=Code%20Factor&logo=CodeFactor)](https://www.codefactor.io/repository/github/yattabyte/MiniECS)
[![Codacy](https://img.shields.io/codacy/grade/d0765b60df194784986b2937b84ab526?label=Code%20Quality&logo=Codacy)](https://www.codacy.com/manual/Yattabyte/MiniECS)
[![LGTM](https://img.shields.io/lgtm/grade/cpp/github/Yattabyte/MiniECS?label=Code%20Quality&logo=LGTM)](https://lgtm.com/projects/g/Yattabyte/MiniECS)
[![Readme Score](http://readme-score-api.herokuapp.com/score.svg?url=https://github.com/yattabyte/miniecs)](http://clayallsopp.github.io/readme-score?url=https://github.com/yattabyte/miniecs)
[![license](https://img.shields.io/github/license/Yattabyte/MiniECS?label=License&logo=github)](https://github.com/Yattabyte/MiniECS/blob/master/LICENSE)


# MiniECS
This library aims to provide a reasonable basic level of Entity, Component, and Systems ***(ECS)*** functionality.

ECS refers to an architecture where specific *Systems* operate on a collection of *Entities*, filtered to match only the *Component types* the *System* cares about.  
In a high-level sense, this transforms a potentially narly-looking inheritance structure down into a *Database-like* structure. 

In this library, entities and components are stored in a collection referred to as an ***ecsWorld***, allowing run-time manipulation and access.


## Overview
This library exposes the following structures to the user:
  - ecsComponent
  - ecsEntity
  - ecsHandle
  - ecsSystem
  - ecsWorld
  
  
### Section 1: ecsComponent
An ***ecsComponent*** is what users should sub-class when creating their own components.  
Using a database analogy, these would be presented as columns.  
  - Components **can** contain **ONLY** data
  - Components **cannot** contain **ANY** functions

Example:  
```cpp
struct TransformComponent final : public ecsComponent<TransformComponent> {
    mat4 transformMatrix;
    quat orientation
    vec3 position;
    vec3 scale;
    vec3 rotation;
};
```


### Section 2: ecsEntity
An ***ecsEntity*** represents a single entity, but requires no further subclassing or modification from the user under an ECS architecture.  
All behaviour is expressed in systems responding to the presence or absence of component combinations within an entity.  
Using a database analogy, these would be presented as rows.  
  - An entity **can** contain no components
  - An entity **cannot** contain two components of the same class type
  
Example:  
An entity can be made with a set of components representing a player, such as:
  - Player component
  - Character component
  - Health component
  - Armor component
In a game, this entity could be modified and can add useful gameplay components like:
  - Invulnerable component
  or 
  - Poisoned component
  
  
### Section 3: ecsHandle
An ***ecsHandle*** is a structure used to uniquely identify entities or components using a 32 character UUID.  
The ecsWorld takes care to provide sensible values on behalf othe user when generating entities and components.
However, this class on its own should never have to be used by a user.  
Instead, when a entity or component is created, its EntityHandle or ComponentHandle will be returned - subclasses of ecsHandle.

Example:  
```cpp
// Make an entity
EntityHandle entHandle = world.makeEntity(...);

// Delete an entity
bool removedSuccesfully = world.removeEntity(entHandle);
```

  
### Section 4: ecsSystem
An ***ecsSystem*** provides the logic appropriate to entity-component combinations.  
These are set to filter certain combinations of components per-entity, and can also **optionally** accept components.  
In a database analogy, these would be represented as queries.

Example:  
```cpp
class GravitySystem final : public ecsBaseSystem {
public:
    GravitySystem() {
        addComponentType(
            ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            MassComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    }
    
    // Interface Implementation
    // Receives a vector of required components per-entity, nested in another vector
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) 
        final {
        for (auto& components : entityComponents) {
            auto& particleComponent =
                *static_cast<ParticleComponent*>(components[0]);
            auto& massComponent = *static_cast<MassComponent*>(components[1]);

            const float force = massComponent.mass * -9.81F;
            particleComponent.particle.m_velocity.y() +=
                force * static_cast<float>(deltaTime);

            particleComponent.particle.m_pos +=
                particleComponent.particle.m_velocity *
                vec2(static_cast<float>(deltaTime));
        }
    }
};
```

### Section 4: ecsWorld
An ***ecsWorld*** stores, organizes, and provides means of accessing and manipulating entities and components.  
In addition, it facilitates creation of entities and components. It encapsulates the state of a the game-world.  
It also provides means for systems to interact with the data within.  
In a database analogy, this would be a DBMS (database management system).

Example:  
```cpp
int main()
{
    ecsWorld world;
    
    // Create Empty Entity
    EntityHandle entity = world.makeEntity(nullptr, 0);
    
    // Create components
    PositionComponent pos{ vec3(0.0F) };
    ScaleComponent scl{ vec3(1.0F) };
    QuatComponent quat{ quat(0.0F) };
    ModelComponent mdl{ Model("//Models//Penguin.mdl") };
    world.makeComponent(entity, &pos);
    world.makeComponent(entity, &scl);
    ComponentHandle quatHandle = world.makeComponent(entity, &quat);
    ComponentHandle modelHandle = world.makeComponent(entity, &mdl);

    // Delete component using handles
    world.removeEntityComponent(entity, modelHandle);
    
    // Delete component using 1 handle (slow)
    world.removeComponent(quatHandle);
    
    // Delete component using type
    world.removeEntityComponent(entity, ScaleComponent::Runtime_ID);
    
    // Delete entity and remaining components
    world.removeEntity(entity);
    
    PhysicsSystem system;
    
    while (shouldRun())
    {
        world.updateSystem(&system, 0.01);
    }
}
};
```