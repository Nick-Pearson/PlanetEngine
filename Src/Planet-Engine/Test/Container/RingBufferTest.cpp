#include "gtest/gtest.h"

#include "Container/RingBuffer.h"

TEST(RingBufferTest, buffer_starts_empty)
{
  RingBuffer<int> buffer{10};

  EXPECT_EQ(true, buffer.IsEmpty());
  EXPECT_EQ(10, buffer.Capacity());
}

TEST(RingBufferTest, buffer_after_add_is_not_empty)
{
  RingBuffer<int> buffer{10};

  buffer.Add(1);

  EXPECT_EQ(false, buffer.IsEmpty());
  EXPECT_EQ(10, buffer.Capacity());
}

TEST(RingBufferTest, non_empty_buffer_after_add_is_not_empty)
{
  RingBuffer<int> buffer{10};
  buffer.Add(1);

  buffer.Add(2);

  EXPECT_EQ(false, buffer.IsEmpty());
  EXPECT_EQ(10, buffer.Capacity());
}

TEST(RingBufferTest, add_over_capacity_keeps_length_same)
{
  RingBuffer<int> buffer{3};
  buffer.Add(1);
  buffer.Add(2);
  buffer.Add(3);

  buffer.Add(4);
  buffer.Add(5);

  EXPECT_EQ(false, buffer.IsEmpty());
  EXPECT_EQ(3, buffer.Capacity());
}

TEST(RingBufferTest, head_returns_latest_item)
{
  RingBuffer<int> buffer{3};

  buffer.Add(1);
  EXPECT_EQ(1, buffer.Head());

  buffer.Add(2);
  EXPECT_EQ(2, buffer.Head());

  buffer.Add(3);
  EXPECT_EQ(3, buffer.Head());

  buffer.Add(4);
  EXPECT_EQ(4, buffer.Head());
}