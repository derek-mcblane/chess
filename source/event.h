#pragma once

#include <functional>
#include <map>
#include <memory>
#include <variant>

namespace event {

template <typename... EventTs>
class Registry
{
    template <typename EventT>
    using Callback = std::function<void(const EventT&)>;

    template <typename EventT>
    using HandlerList = std::vector<Callback<EventT>>;

  public:
    Registry(HandlerList<EventTs>&&... subscriptions)
        : subscriptions_{subscriptions...}
    {}

    template <typename EventT>
    void add(Callback<EventT>&& subscription)
    {
        event_subscriptions<EventT>().push_back(std::move(subscription));
    }

    template <typename EventT>
    const HandlerList<EventT>& event_subscriptions() const
    {
        return std::get<HandlerList<EventT>>(subscriptions_);
    }

  private:
    std::tuple<HandlerList<EventTs>...> subscriptions_;

    template <typename EventT>
    HandlerList<EventT>& event_subscriptions()
    {
        return std::get<HandlerList<EventT>>(subscriptions_);
    }
};

template <typename... EventTs>
class Dispatcher
{
  public:
    using RegistryT = Registry<EventTs...>;

    Dispatcher(std::unique_ptr<RegistryT> registry) : registry_{std::move(registry)} {}

    template <typename EventT>
    void dispatch(const EventT& event)
    {
        // TODO
    }

  private:
    std::unique_ptr<RegistryT> registry_;
};

} // namespace event
