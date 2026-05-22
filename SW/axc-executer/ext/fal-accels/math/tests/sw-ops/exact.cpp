/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <gtest/gtest.h>

#include "operators.hpp"

TEST(Multiplication, TestExecution) {
  auto mult_ = axc::arithmetic::exact::Multiply<int>{};
  EXPECT_EQ(mult_(3, 4), 3 * 4);
  EXPECT_EQ(mult_(15, -4), 15 * -4);
  EXPECT_EQ(mult_(-11, -7), -11 * -7);
  EXPECT_EQ(mult_(-1, 3), -1 * 3);
}

TEST(Addition, TestExecution) {
  auto add_ = axc::arithmetic::exact::Add<int>{};
  EXPECT_EQ(add_(3, 4), 3 + 4);
  EXPECT_EQ(add_(15, -4), 15 + -4);
  EXPECT_EQ(add_(-11, -7), -11 + -7);
  EXPECT_EQ(add_(-1, 3), -1 + 3);
}

TEST(Division, TestExecution) {
  auto div_ = axc::arithmetic::exact::Divide<float>{};
  EXPECT_FLOAT_EQ(div_(3, 4), 3.f / 4.f);
  EXPECT_FLOAT_EQ(div_(15, -4), 15.f / -4.f);
  EXPECT_FLOAT_EQ(div_(-11, -7), -11.f / -7.f);
  EXPECT_FLOAT_EQ(div_(-1, 3), -1.f / 3.f);
}

TEST(Substraction, TestExecution) {
  auto sub_ = axc::arithmetic::exact::Substract<int>{};
  EXPECT_EQ(sub_(3, 4), 3 - 4);
  EXPECT_EQ(sub_(15, -4), 15 - -4);
  EXPECT_EQ(sub_(-11, -7), -11 - -7);
  EXPECT_EQ(sub_(-1, 3), -1 - 3);
}

TEST(FMA, TestExecution) {
  auto fma_ = axc::arithmetic::exact::FMA<int>{};
  EXPECT_EQ(fma_(3, 3, 4), 3 * 3 + 4);
  EXPECT_EQ(fma_(-5, 7, 8), (-5) * 7 + 8);
  EXPECT_EQ(fma_(3, -6, 7), 3 * (-6) + 7);
  EXPECT_EQ(fma_(2, 3, -6), 2 * 3 - 6);
}

TEST(PassThru, TestExecution) {
  auto passthru_ = axc::nonlinear::exact::PassThru<int>{};
  EXPECT_EQ(passthru_(3), 3);
  EXPECT_EQ(passthru_(-4), -4);
}

TEST(ReLU, TestExecution) {
  auto relu_ = axc::nonlinear::exact::ReLU<int>{};
  EXPECT_EQ(relu_(3), 3);
  EXPECT_EQ(relu_(-4), 0);
  EXPECT_EQ(relu_(0), 0);
}
