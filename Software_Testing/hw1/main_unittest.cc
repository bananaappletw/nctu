#include "gtest/gtest.h"
#include "main.h"

TEST(triangle_type, BoundaryValue){
	EXPECT_STREQ("Isosceles", triangle_type(100, 100, 1));
	EXPECT_STREQ("Isosceles", triangle_type(100, 100, 2));
	EXPECT_STREQ("Equilateral", triangle_type(100, 100, 100));
	EXPECT_STREQ("Isosceles", triangle_type(100, 100, 199));
	EXPECT_STREQ("Not a Triangle", triangle_type(100, 100, 200));
	EXPECT_STREQ("Isosceles", triangle_type(100, 1, 100));
	EXPECT_STREQ("Isosceles", triangle_type(100, 2, 100));
	EXPECT_STREQ("Isosceles", triangle_type(100, 199, 100));
	EXPECT_STREQ("Not a Triangle", triangle_type(100, 200, 100));
	EXPECT_STREQ("Isosceles", triangle_type(1, 100, 100));
	EXPECT_STREQ("Isosceles", triangle_type(2, 100, 100));
	EXPECT_STREQ("Isosceles", triangle_type(199, 100, 100));
	EXPECT_STREQ("Not a Triangle", triangle_type(200, 100, 100));
}

TEST(triangle_type, EquivalenceClass){
  EXPECT_STREQ("Equilateral", triangle_type(5, 5, 5));
  EXPECT_STREQ("Invalid", triangle_type(-1 , 5, 5));
  EXPECT_STREQ("Invalid", triangle_type(5 , -1, 5));
  EXPECT_STREQ("Invalid", triangle_type(5 , 5, -1));
  EXPECT_STREQ("Invalid", triangle_type(201 , 5, 5));
  EXPECT_STREQ("Invalid", triangle_type(5 , 201, 5));
  EXPECT_STREQ("Invalid", triangle_type(5 , 5, 201));
}

TEST(triangle_type, Edge){
  EXPECT_STREQ("Invalid", triangle_type(0, 5, 5));
  EXPECT_STREQ("Isosceles", triangle_type(1 , 5, 5));
  EXPECT_STREQ("Isosceles", triangle_type(2 , 5, 5));
  EXPECT_STREQ("Not a Triangle", triangle_type(199 , 5, 5));
  EXPECT_STREQ("Not a Triangle", triangle_type(200 , 5, 5));
  EXPECT_STREQ("Invalid", triangle_type(201, 5, 5));
}
