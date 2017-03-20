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