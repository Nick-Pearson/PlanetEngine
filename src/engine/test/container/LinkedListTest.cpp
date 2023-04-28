#include "gtest/gtest.h"

#include "Container/LinkedList.h"

TEST(LinkedListTest, list_starts_empty)
{
  LinkedList<int> list{};

  EXPECT_EQ(0, list.Length());
}

TEST(LinkedListTest, list_after_add_is_not_empty)
{
  LinkedList<int> list{};

  list.Add(1);

  EXPECT_EQ(1, list.Length());
}

TEST(LinkedListTest, non_empty_list_after_add_is_not_empty)
{
  LinkedList<int> list{};
  list.Add(1);

  list.Add(2);

  EXPECT_EQ(2, list.Length());
}

TEST(LinkedListTest, empty_list_remove_does_nothing)
{
  LinkedList<int> list{};

  bool result = list.Remove(2);

  EXPECT_FALSE(result);
  EXPECT_EQ(0, list.Length());
}

TEST(LinkedListTest, remove_non_existant_item_does_nothing)
{
  LinkedList<int> list{};
  list.Add(1);

  bool result = list.Remove(2);

  EXPECT_FALSE(result);
  EXPECT_EQ(1, list.Length());
}

TEST(LinkedListTest, remove_singleton_item_empties_list)
{
  LinkedList<int> list{};
  list.Add(1);

  bool result = list.Remove(1);

  EXPECT_TRUE(result);
  EXPECT_EQ(0, list.Length());
}

TEST(LinkedListTest, remove_item_calls_destructor_on_it)
{
  static int called;
  called = 0;
  struct Item
  {
    int val;
    Item() : val(7) {}
    ~Item()
    {
      called++;
    }

    bool operator==(const Item& other) const { return val == other.val; }
  };
  LinkedList<Item> list{};
  list.Add(Item());

  bool result = list.Remove(Item());

  EXPECT_TRUE(result);
  EXPECT_EQ(0, list.Length());
  EXPECT_EQ(3, called);
}

TEST(LinkedListTest, remove_nullptr_does_nothing)
{
  LinkedList<int> list{};
  list.Add(1);

  bool result = list.Remove(nullptr);

  EXPECT_FALSE(result);
  EXPECT_EQ(1, list.Length());
}

TEST(LinkedListTest, empty_list_remove_ptr_does_nothing)
{
  LinkedList<int> list{};
  int val = 2;

  bool result = list.Remove(&val);

  EXPECT_FALSE(result);
  EXPECT_EQ(0, list.Length());
}

TEST(LinkedListTest, remove_ptr_non_existant_item_does_nothing)
{
  LinkedList<int> list{};
  int val = 2;
  list.Add(1);

  bool result = list.Remove(&val);

  EXPECT_FALSE(result);
  EXPECT_EQ(1, list.Length());
}

TEST(LinkedListTest, remove_ptr_singleton_item_empties_list)
{
  LinkedList<int> list{};
  int val = 1;
  list.Add(1);

  bool result = list.Remove(&val);

  EXPECT_TRUE(result);
  EXPECT_EQ(0, list.Length());
}

TEST(LinkedListTest, remove_ptr_item_calls_destructor_on_it)
{
  static int called;
  called = 0;
  struct Item
  {
    int val;
    Item() : val(7) {}
    ~Item()
    {
      called++;
    }
    bool operator==(const Item& other) const { return val == other.val; }
  };
  LinkedList<Item> list{};
  Item val{};
  list.Add(val);

  bool result = list.Remove(&val);

  EXPECT_TRUE(result);
  EXPECT_EQ(0, list.Length());
  EXPECT_EQ(1, called);
}