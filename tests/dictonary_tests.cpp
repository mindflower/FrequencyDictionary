#include <gtest/gtest.h>
#include <stringmapper.h>
#include <dictionary.h>

TEST(DictonaryTests, EmptyTest) { 
    auto mapper = std::make_unique<dictionary::StringMapper>("");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(0, result.size());
}

TEST(DictonaryTests, SpaceTest) { 
    auto mapper = std::make_unique<dictionary::StringMapper>("    ");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(0, result.size());
}

TEST(DictonaryTests, SpecialSymbolsTest) { 
    auto mapper = std::make_unique<dictionary::StringMapper>("@#$%^&*()   1234  567890 \t");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(0, result.size());
}

TEST(DictonaryTests, OneWordTest) { 
    auto mapper = std::make_unique<dictionary::StringMapper>("john");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(1, result.size());

    const auto& node = result.back();
    ASSERT_EQ("john", node.word);
    ASSERT_EQ(1, node.frequency);
}

TEST(DictonaryTests, TwoWordTest) { 
    const std::vector<dictionary::Node> expect = {
        {"doe", 1},
        {"john", 1},
    };
    auto mapper = std::make_unique<dictionary::StringMapper>("john doe");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(2, result.size());

    const auto& firstNode = result[0];
    const auto& secondNode = result[1];
    ASSERT_EQ("doe", firstNode.word);
    ASSERT_EQ(1, firstNode.frequency);
    ASSERT_EQ("john", secondNode.word);
    ASSERT_EQ(1, secondNode.frequency);
}

TEST(DictonaryTests, SameWordsTest) { 
    auto mapper = std::make_unique<dictionary::StringMapper>("john john john john john");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(1, result.size());

    const auto& node = result.back();
    ASSERT_EQ("john", node.word);
    ASSERT_EQ(5, node.frequency);
}

TEST(DictonaryTests, DifferentCaseTest) { 
    auto mapper = std::make_unique<dictionary::StringMapper>(" john John jOhn JoHn johN");
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();
    ASSERT_EQ(1, result.size());

    const auto& node = result.back();
    ASSERT_EQ("john", node.word);
    ASSERT_EQ(5, node.frequency);
}

TEST(DictonaryTests, SimpleTextTest) {
    const std::string input{"A person who thinks all the time"};
    const std::vector<dictionary::Node> expect = {
        {"a", 1},
        {"all", 1},
        {"person", 1},
        {"the", 1},
        {"thinks", 1},
        {"time", 1},
        {"who", 1},
    };

    auto mapper = std::make_unique<dictionary::StringMapper>(input);
    const auto result = dictionary::Dictionary{std::move(mapper)}.parse();

    const auto [first, second] = std::mismatch(expect.cbegin(), expect.cend(), result.cbegin(), [](const auto& rhd, const auto& lhd) {
        return (rhd.word == lhd.word) && (rhd.frequency == lhd.frequency);
    });
    EXPECT_EQ(expect.cend(), first);
}
