#include <gtest/gtest.h>

#include <Common/PODArray.h>

using namespace DB;

TEST(Common, PODArrayInsert)
{
    std::string str = "test_string_abacaba";
    PODArray<char> chars;
    chars.insert(chars.end(), str.begin(), str.end());
    EXPECT_EQ(str, std::string(chars.data(), chars.size()));

    std::string insert_in_the_middle = "insert_in_the_middle";
    auto pos = str.size() / 2;
    str.insert(str.begin() + pos, insert_in_the_middle.begin(), insert_in_the_middle.end());
    chars.insert(chars.begin() + pos, insert_in_the_middle.begin(), insert_in_the_middle.end());
    EXPECT_EQ(str, std::string(chars.data(), chars.size()));

    std::string insert_with_resize;
    insert_with_resize.reserve(chars.capacity() * 2);
    char cur_char = 'a';
    while (insert_with_resize.size() < insert_with_resize.capacity())
    {
        insert_with_resize += cur_char;
        if (cur_char == 'z')
            cur_char = 'a';
        else
            ++cur_char;
    }
    str.insert(str.begin(), insert_with_resize.begin(), insert_with_resize.end());
    chars.insert(chars.begin(), insert_with_resize.begin(), insert_with_resize.end());
    EXPECT_EQ(str, std::string(chars.data(), chars.size()));
}

TEST(Common, PODInsertIteratorRange)
{
    size_t size = 1 << 20;
    char value = 123;

    PODArray<size_t> big;
    PODArray<char> small(size, value);

    EXPECT_EQ(big.size(), 0);
    EXPECT_EQ(small.size(), size);

    big.insert(small.begin(), small.end());

    EXPECT_EQ(big.size(), size);
    EXPECT_EQ(big.back(), value);

    big.assign(small.begin(), small.end());

    EXPECT_EQ(big.size(), size);
    EXPECT_EQ(big.back(), value);

    big.insert(big.begin(), small.begin(), small.end());

    EXPECT_EQ(big.size(), size * 2);
    EXPECT_EQ(big.front(), value);
    EXPECT_EQ(big.back(), value);

    PODArray<uint16_t> arr1{1, 2, 3, 4, 5};
    PODArray<uint32_t> arr2{11, 12};

    arr1.insert(arr1.begin() + 3, arr2.begin(), arr2.end());

    EXPECT_EQ(arr1, (PODArray<uint16_t>{1, 2, 3, 11, 12, 4, 5}));

    arr2.insert(arr2.begin() + 1, arr1.begin(), arr1.end());

    EXPECT_EQ(arr2, (PODArray<uint32_t>{11, 1, 2, 3, 11, 12, 4, 5, 12}));
}

TEST(Common, PODPushBackRawMany)
{
    PODArray<char> chars;
    chars.push_back_raw_many(5, "first");
    EXPECT_EQ(std::string("first"), std::string(chars.data(), chars.size()));
    EXPECT_EQ(5, chars.size());
    EXPECT_LE(chars.capacity() - chars.size(), 10);
    chars.push_back_raw_many(10, "0123456789");
    EXPECT_EQ(15, chars.size());
    EXPECT_EQ(std::string("first0123456789"), std::string(chars.data(), chars.size()));
}

TEST(Common, PODNoOverallocation)
{
    /// Check that PaddedPODArray allocates for smaller number of elements than the power of two due to padding.
    /// NOTE: It's Ok to change these numbers if you will modify initial size or padding.

    PaddedPODArray<char> chars;
    std::vector<size_t> capacities;

    size_t prev_capacity = 0;
    for (size_t i = 0; i < 1000000; ++i)
    {
        chars.emplace_back();
        if (chars.capacity() != prev_capacity)
        {
            prev_capacity = chars.capacity();
            capacities.emplace_back(prev_capacity);
        }
    }

    EXPECT_EQ(capacities, (std::vector<size_t>{4065, 8161, 16353, 32737, 65505, 131041, 262113, 524257, 1048545}));
}
