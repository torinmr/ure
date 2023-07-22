#include <vector>

#include <gtest/gtest.h>

#include "instruction.h"

using namespace ure;
using namespace std;

TEST(InstructionTest, TestToPartial) {
  vector<Instruction> re = {
    Instruction::Literal('a'),
    Instruction::Literal('b'),
    Instruction::Match()
  };
  vector<Instruction> expected = {
    Instruction::Split(3),
    Instruction::Wildcard(),
    Instruction::Jump(-2),
    Instruction::Literal('a'),
    Instruction::Literal('b'),
    Instruction::Split(3),
    Instruction::Wildcard(),
    Instruction::Jump(-2),
    Instruction::Match()
  };
  ASSERT_EQ(expected, Instruction::to_partial(re));
}