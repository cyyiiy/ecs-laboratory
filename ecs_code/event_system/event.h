#pragma once
#include "observer.h"
#include <cstddef>
#include <functional>


/** 
 * Non-templated interface for 'Event' so observers can track every event they're subscribed to. 
 */
class EventBase
{
public:
    virtual ~EventBase() = default;
    
    /** Unsubscribe an observer from an event. */
    virtual void unregisterObserver(Observer* observer) = 0;
    
    /** Clone the subscription to an event from an observer to another one. */
    virtual void cloneBindingsFrom(const Observer* src, Observer* dst) = 0;
};


/**
 * Event to which observers can subscribe a callback function.
 * 
 * @tparam Args Event parameters.
 */
template <typename... Args>
class Event : public EventBase
{
    /** Data struct that allows event to properly store a subscription. */
    struct EventBinding
    {
        /** The owner of the subscription. */
        Observer* bindingOwner = nullptr;
        
        /** The byte offset from the memory address of the Observer to the memory address of the real type of the object.
         * Used to call the callback function on the subscribed object without having to store its real type. */
        std::ptrdiff_t byte_offset = 0;
        
        /** The callback function that should be called when the event is broadcasted. */
        std::function<void(void*, Args...)> callbackFunction;
    };
    
public:
    ~Event() override
    {
        removeAllSubscriptions();
    }
    
    /** Subscribe an observer to an event.
     * 
     * @tparam T The real type of the observer object subscribing to the event.
     * @param observer The observer object subscribing to the event.
     * @param callback The callback function that will be called when the event will is broadcasted.
     */
    template <typename T>
    void subscribe(T* observer, void (T::*callback)(Args...))
    {
        static_assert(std::is_base_of_v<Observer, T>, "T must be derived from Observer");
        
        EventBinding binding;
        binding.bindingOwner = observer;
        
        // Compute the byte offset between the memory address of the Observer type and the memory address of the real type
        binding.byte_offset = 
            reinterpret_cast<const char*>(static_cast<const Observer*>(observer)) - 
            reinterpret_cast<const char*>(observer);
        
        // Store the callback function so it can be called with a void pointer by the broadcast function
        // This allow the broadcast function to not have to know the real type of the observer
        binding.callbackFunction = [callback](void* concrete, Args... args)
        {
            (static_cast<T*>(concrete)->*callback)(args...);
        };
        
        bindings.push_back(std::move(binding));
        observer->trackEvent(this);
    }

    /** Unsubscribe an observer from an event.
     * 
     * @param observer The observer object unsubscribing from the event.
     */
    void unsubscribe(Observer* observer)
    {
        unregisterObserver(observer);
    }

    /** Broadcast all subscribed callback functions.
     * 
     * @param args Events parameters.
     */
    void broadcast(Args... args) const
    {
        // Create a stable vector of bindings so if the original bindings is modified during the broadcast, we don't fall in UB
        std::vector<const EventBinding*> bindings_safe;
        bindings_safe.reserve(bindings.size());
        for (const auto& binding : bindings) bindings_safe.push_back(&binding);
        
        for (const EventBinding* binding : bindings_safe)
        {
            // Construct a void pointer with the byte offset to have the memory address of the real type of the observer
            void* concrete = reinterpret_cast<char*>(binding->bindingOwner) - binding->byte_offset;
            binding->callbackFunction(concrete, args...);
        }
    }
    
    /** Unsubscribe every observer from this event. */
    void removeAllSubscriptions()
    {
        for (const auto& binding : bindings)
        {
            binding.bindingOwner->untrackEvent(this);
        }
        bindings.clear();
    }
    
    
    void unregisterObserver(Observer* observer) override
    {
        auto erase_rule = std::remove_if(bindings.begin(), bindings.end(), 
            [observer](const EventBinding& binding) { return binding.bindingOwner == observer; });
        bindings.erase(erase_rule, bindings.end());
        observer->untrackEvent(this);
    }
    
    void cloneBindingsFrom(const Observer* src, Observer* dst) override
    {
        // Create a temp vector so we don't edit bindings while iterating on it
        std::vector<EventBinding> bindings_to_add;
        for (const auto& binding : bindings)
        {
            if (binding.bindingOwner != src) continue;
            
            EventBinding clone = binding;
            clone.bindingOwner = dst;
            bindings_to_add.push_back(std::move(clone));
        }
        for (auto& clone : bindings_to_add)
        {
            bindings.push_back(std::move(clone));
        }
        dst->trackEvent(this);
    }
    
    
private:
    std::vector<EventBinding> bindings;
};