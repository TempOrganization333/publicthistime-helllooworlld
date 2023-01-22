#include <gtest/gtest.h>
#include "../two.cpp"

namespace {
std::string g_command_line_arg;
}

class MyTestEnvironment : public testing::Environment {
 public:
  explicit MyTestEnvironment(const std::string &command_line_arg) {
    g_command_line_arg = command_line_arg;
  }
};

TEST(MyTest, command_line_arg_test) {
  ASSERT_FALSE(g_command_line_arg.empty());
}

TEST(STR, strtest){
  EXPECT_EQ(g_command_line_arg+"1", "hi1");
}

int main(int argc, char **argv) {
  std::string command_line_arg(argc == 2 ? argv[1] : "");
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(new MyTestEnvironment(command_line_arg));
  return RUN_ALL_TESTS(); // if all correct: 0, else 1
}

