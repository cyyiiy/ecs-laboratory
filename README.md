# Entity Component System of the Cy-Engine

> Require C++ 17

This repository contains an isolated version of the ECS made for my own game engine, as well as some example
code to test it.


## Managers & Sublists, how are the components stored?

Each component class have a dedicated **ComponentManager**, which is responsible for every existing component
of this class. \
Managers store components in **ComponentSubLists**. Sublists are objects that contains an aligned storage of
a fixed size, and this is where the components are stored. \
Each component class can define a custom number of components contained in a sublist (the default one is 64). \
Sublists are also made so that their components are always continuous in memory (by doing swaps on delete),
so iterating on components is as fast as possible.

### What happens when a component is created and deleted?

When a component is created, the manager will look for a sublist that have free space, and will create one if needed.
It will then construct the component in aligned storage of the sublist, and will call the component's constructor.

When the delete function is called, it will simply marks the component as pending, and the real deletion will occurs
once the `DeletePendingComponents` function is called (automatically done at the end of the frame). \
When a component is deleted, the manager will get it's slot in the sublist, then call the pending component destructor.
After that, it will get the last component of the sublist and move it to the slot of the deleted component, so alive
components stay packed in memory.

**Note:** Since components can be moved, it is advised to not write logic in the constructors and the destructors of 
component classes.


## Static ECS functions and where are the Managers

There are many static functions that can be used to interact with the ECS system globally. They can be use to create,
delete and get a component, test the validity of an handle (see below), and that's how the engine will call the global
update (see below) and the global pending components deletion every frame.

There is also a key function `ECS::Manager<T>` that serves two purposes:
1. Getting the reference of a **ComponentManager**. Useful for creating a system that need to iterate on all existing
components of a class. This function is also used by the other static functions such as `ECS::CreateComponent<T>`
to internally get the manager of the desired component class.
2. Creating **ComponentManagers**. Inside the function, we can find `static ComponentManager<T, SublistSize> manager;`
which will automatically create a manager for every component class this function is called with (so every component
class used in the engine / game). \
Additionaly, the first time (for every class) this function is called, the manager will also be registered (thanks to
its interface) in a static list so it's easy to call `DeletePendingComponents` on every manager.


## Keeping the reference of components with the Handles

The `CreateComponent` function doesn't return a pointer towards the created component, it returns a **ComponentHandle**.
Handles are very simple objects that hold just enough information to be able to retrieve the component in a sublist
when passed to a manager. They are trivially copyable and movable, so they can be manipulated and stored by every
object that need to keep a reference to a component, without messing with the ownership of this component. \
Handles are also templated (they look like this: `ComponentHandle<RigidbodyComponent>`), so it is not needed to remember
the component class of a handle. There is a non-templated version of the handle (`RawComponentHandle`). It mostly
exists for internal uses, but can be useful for doing a list of handles of different classes.

The ECS functions that allow to manipulate components, such as `ECS::DeleteComponent`, all take a handle in parameter.
The preffered way of doing things with components is to call `ECS::GetComponent` with a handle and get a reference.


## Behavior Components and the update function

The base `Component` class doesn't have a virtual update function. It is made to be as simple as possible, because a
majority of components will just contain some data and won't need to have a logic. \
For logic components (useful when creating a game), there is a `BehaviorComponent` class that have virtual `init`, 
`exit` and `update` functions. Any component class derived from `BehaviorComponent` can override these, and they will
automatically be called by the ECS system whenever a component is created, deleted and updated every frame.
Behavior components can also disable their update individually.

**Note:** The `exit` function is called when `DeleteComponent` is called (so when the component is pending deletion),
not when `DeletePendingComponents` is called.


## The Entity class and the basic way to use the ECS

The `Entity` class is an object that provides an easy way to use the ECS. It has several functions seach as
`addComponentByClass<T>` and `removeComponent` that allows to manipulate components on the entity. \
Like the static ECS functions, the entity only allows to manipulate components with handles. It also has a
`getComponent` function that allows to retrieve a reference to a component on the entity from a handle. \
Note that the entity does **not** own its components. Components are only owned by the managers, and the entity simply
keeps a list of handles towards every components attached to itself. The entity also provides some utility functions
that use this list, such as `getComponentOfClass<T>` (returns a handle). \
Finally, the entity also has a function `clearAllComponents` (pretty self-explanatory). In the engine, this function
is automatically called by the game system when an entity is destroyed.

The basic pipeline to use the ECS look like this:
- Create entities
- Add engine components to them (model rendering, physics, audio...) and set their data
- Create some behavior components to have some logic and add them to entities

It works, the ECS has been made for this, and it's perfectly fine for most use cases. But you can go a step further
and create systems that don't rely on behavior components.


## The fast iteration and the advanced way to use the ECS

Components that aren't based of `BehaviorComponent` and don't have virtual functions are more efficient. Having an
external system that read data stored in these components and execute some actions is faster than using the update
system of behavior components. \
To be able to do this, you just need to retrieve a reference to the manager of the component class you want to
iterate on, and then use the foreach function with a lambda to execute action with the components.

Example:
```c++
auto& manager = ECS::Manager<SomeComponent>();
manager.ForEach([this](const SomeComponent& component)
{
    this->updateSystem(component);
});
```

**Note:** It is advised to use `auto` for the manager, since the real type returned by the static ECS function is
`ComponentManager<SomeComponent, 64>&`, with 64 being the sublist size defined for `SomeComponent`. If the sublist
size changes, this code become invalid. \
If you truly hate the auto keyword, you can use
`ComponentManager<SomeComponent, ComponentSublistSize<SomeComponent>::value>&` as it will compile into the defined
sublist size (or the default one), but `auto` is easier.


## Notes for the future

There are two major points that could be improved in a future version of this ECS.
1. Make the entity a simple ID shared by components instead of a complex class.
2. Upgrade the fast iteration system to be able to have a "view" of multiple component classes (for example iterate
on light components paired with transform components).

I believe the last point is currently the biggest bottleneck of the ECS, as it is not fully cache friendly for the moment.