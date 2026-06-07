#pragma once
#include <vector>

class EventBase;


/**
 * Object that can subscribe a callback function to an event.
 * 
 * Note: observers have automatic subscriptions management when copied and destroyed.
 */
class Observer
{
public:
    Observer() = default;
    virtual ~Observer();
    
    Observer(const Observer& other);
    Observer& operator=(const Observer& other);
    
    Observer(Observer&& other) noexcept;
    Observer& operator=(Observer&& other) noexcept;
    
private:
    template<typename... Args> friend class Event;

    /** Track an event this observer is subscribed to.
     * Automatically called when subscribing this observer to an event.
     * 
     * @param event The event to track.
     */
    void trackEvent(EventBase* event);

    /** Untrack an event this observer is no longer subscribed to.
     * Automatically called when unsubscribing this observer from an event.
     * 
     * @param event The event to untrack.
     */
    void untrackEvent(EventBase* event);

    /** Unsubscribed this event from all event it has tracked. */
    void unregisterFromAll();

    /** Copy every event subscribed by another observer.
     * 
     * @param src The observer to copy subscriptions from.
     */
    void copySubscriptionsFrom(const Observer& src);

    /** Copy every event subscribed by another observer, then remove the subscriptions of the other observer.
     * 
     * @param src The observer to steal subscriptions from.
     */
    void stealSubscriptionsFrom(Observer& src);
    
    
    std::vector<EventBase*> events;
};