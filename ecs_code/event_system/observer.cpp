#include "observer.h"
#include "event.h"
#include <algorithm>


Observer::~Observer()
{
    unregisterFromAll();
}

Observer::Observer(const Observer& other)
{
    copySubscriptionsFrom(other);
}

Observer& Observer::operator=(const Observer& other)
{
    if (this == &other) return *this;
    
    unregisterFromAll();
    copySubscriptionsFrom(other);
    return *this;
}

Observer::Observer(Observer&& other) noexcept
{
    stealSubscriptionsFrom(other);
}

Observer& Observer::operator=(Observer&& other) noexcept
{
    if (this == &other) return *this;
    
    unregisterFromAll();
    stealSubscriptionsFrom(other);
    return *this;
}

void Observer::trackEvent(EventBase* event)
{
    if (std::find(events.begin(), events.end(), event) != events.end()) return;
    
    events.push_back(event);
}

void Observer::untrackEvent(EventBase* event)
{
    events.erase(std::remove(events.begin(), events.end(), event), events.end());
}

void Observer::unregisterFromAll()
{
    if (events.empty()) return;
    
    // Create a safe copy cause the original events vector can be modified during the iteration
    const std::vector<EventBase*> events_safe = events;
    for (EventBase* event : events_safe)
    {
        event->unregisterObserver(this);
    }
}

void Observer::copySubscriptionsFrom(const Observer& src)
{
    if (src.events.empty()) return;

    const std::vector<EventBase*> src_events = src.events;
    for (EventBase* event : src_events)
    {
        event->cloneBindingsFrom(&src, this);
    }
}

void Observer::stealSubscriptionsFrom(Observer& src)
{
    if (src.events.empty()) return;

    const std::vector<EventBase*> src_events = src.events;
    for (EventBase* event : src_events)
    {
        event->cloneBindingsFrom(&src, this);
        event->unregisterObserver(&src);
    }
}