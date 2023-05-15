#pragma once

#include <functional>
#include <map>

template <typename Event>
class EventHandlers
{
  public:
    using HandlerID = size_t;
    HandlerID add_handler(std::function<void(const Event&)>&& handler)
    {
        const size_t handler_id = next_id();
        handlers_[handler_id] = std::move(handler);
        return handler_id;
    }

    void remove_handler(HandlerID handler_id)
    {
        handlers_.erase(handler_id);
    }

    void call_all(const Event& event)
    {
        for (const auto& [_, handler] : handlers_) {
            handler(event);
        }
    }

  private:
    std::map<HandlerID, std::function<void(const Event&)>> handlers_;

    [[nodiscard]] HandlerID next_id() const
    {
        return handlers_.size();
    }
};
