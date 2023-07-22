#include <gtest/gtest.h>

#include "event.h"

using namespace event;

class EventTest : public ::testing::Test
{
  protected:
    struct DummyEvent
    {
        int a;
    };

    using DispatcherT = Dispatcher<DummyEvent>;
    using RegistryT = DispatcherT::RegistryT;

  private:
};

TEST_F(EventTest, Hello) {}
