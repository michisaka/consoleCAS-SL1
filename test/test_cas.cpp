#include <string.h>

#include "gtest/gtest.h"

extern "C" {
#include "cassl1.h"
}

// namespace {

// class CASTest : public ::testing::Test{};

// TEST_F(CASTest, sample)
// {
//   EXPECT_EQ(false, true);
//   EXPECT_EQ(true, true);
// }

// } // namespace

void register_mazoyer_states(void);
void register_mazoyer_rules(void);

TEST(CASTest, state_test)
{
  ASSERT_EQ(SUCCESS, allocate_states(5));

  EXPECT_FALSE(get_state_byname("G"));
  EXPECT_FALSE(get_state_byindex(GENERAL));

  state new_state;
  strcpy(new_state.name, "G");
  new_state.type = GENERAL;
  EXPECT_EQ(SUCCESS, register_state(new_state));

  EXPECT_EQ(ERR_ALREADY_REGISTERED_TYPE, register_state(new_state));

  new_state.type = EXTERNAL;
  EXPECT_EQ(ERR_ALREADY_REGISTERED_NAME, register_state(new_state));

  EXPECT_EQ(GENERAL, get_state_byname("G")->index);
  EXPECT_STREQ("G", get_state_byindex(GENERAL)->name);

  EXPECT_FALSE(has_all_special_state());

  free_states();

  ASSERT_EQ(SUCCESS, allocate_states(7));
  register_mazoyer_states();

  EXPECT_TRUE(has_all_special_state());

  strcpy(new_state.name, "X");
  new_state.type = OTHERS;
  EXPECT_EQ(ERR_TOO_MANY, register_state(new_state));

  free_states();
}

TEST(CASTest, rule_test)
{
  ASSERT_EQ(SUCCESS, allocate_states(7));
  register_mazoyer_states();

  ASSERT_EQ(SUCCESS, allocate_ruleset(7));

  EXPECT_EQ(EXTERNAL, get_next_state(SOLDIER, SOLDIER, SOLDIER));

  EXPECT_EQ(SUCCESS, register_rule("L", "L", "L", "L"));
  EXPECT_EQ(ERR_ALREADY_REGISTERED_RULE, register_rule("L", "L", "L", "G"));
  EXPECT_EQ(ERR_INVALID_RULE, register_rule("F", "L", "L", "L"));
  EXPECT_EQ(ERR_INVALID_RULE, register_rule("L", "G", "L", "WWW"));
  EXPECT_EQ(ERR_UNDEFINED_STATE, register_rule("L", "X", "L", "L"));

  EXPECT_EQ(SOLDIER, get_next_state(SOLDIER, SOLDIER, SOLDIER));

  free_ruleset();

  ASSERT_EQ(SUCCESS, allocate_ruleset(7));
  register_mazoyer_rules();

  EXPECT_EQ(SOLDIER, get_next_state(SOLDIER, SOLDIER, SOLDIER));
  EXPECT_EQ(get_state_byname("A")->index, get_next_state(EXTERNAL, GENERAL, SOLDIER));


  free_ruleset();
  free_states();
}

TEST(CASTest, cell_test)
{
  state_num_t cells[7] = {EXTERNAL, GENERAL, SOLDIER, SOLDIER, SOLDIER, SOLDIER, EXTERNAL};
  int i, j;

  ASSERT_EQ(SUCCESS, allocate_states(7));
  ASSERT_EQ(SUCCESS, allocate_ruleset(7));

  register_mazoyer_states();
  register_mazoyer_rules();

  for (i = 0; i < 9; i++) {
    switch (i) {
    case 7:
      EXPECT_EQ(SYNCHRONIZE, change_state(cells, 7));
      break;
    case 8:
      EXPECT_EQ(ERR_UNDEFINED_RULE, change_state(cells, 7));
      break;
    default:
      EXPECT_EQ(SUCCESS, change_state(cells, 7));
      break;
    }
  }

  free_ruleset();
  free_states();
}

TEST(CASTest, rulefile_test)
{
  state_num_t cells[7] = {EXTERNAL, GENERAL, SOLDIER, SOLDIER, SOLDIER, SOLDIER, EXTERNAL};
  int i, j;
  file_property file_property;


  ASSERT_EQ(SUCCESS, load_rulefile("mazoyer.rul", &file_property));

  for (i = 0; i < 9; i++) {
    switch (i) {
    case 7:
      EXPECT_EQ(SYNCHRONIZE, change_state(cells, 7));
      break;
    case 8:
      EXPECT_EQ(ERR_UNDEFINED_RULE, change_state(cells, 7));
      break;
    default:
      EXPECT_EQ(SUCCESS, change_state(cells, 7));
      break;
    }
  }

  EXPECT_EQ(7, file_property.state_num);
  EXPECT_EQ(120, file_property.rule_num);

  free_ruleset();
  free_states();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
