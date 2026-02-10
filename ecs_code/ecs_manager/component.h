#pragma once

class Entity; // Forward declaration


template <class T>
struct ComponentSublistSize
{
    static constexpr size_t value = 64; // Default sublist size
};


class Component
{
public:
    Component() = default;
    virtual ~Component() = default;
    Component(const Component& other) = default;
    
    Component& operator=(const Component&) = delete;
    
    void setPendingDelete(bool value);
    bool getPendingDelete() const;
    
    void setOwner(Entity* ownerEntity);
    Entity* getOwner() const;
    
private:
    bool pendingDelete{ false };
    Entity* owner{ nullptr };
};