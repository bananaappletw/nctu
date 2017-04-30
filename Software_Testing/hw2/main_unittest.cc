#include "gtest/gtest.h"
#include "main.h"

TEST(triangle_type, C0) {
    EXPECT_STREQ("Invalid", triangle_type(-1 , 5, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , -1, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , 5, -1));
    EXPECT_STREQ("Not a Triangle", triangle_type(200, 100, 50));
}

TEST(triangle_type, C1) {
    EXPECT_STREQ("Invalid", triangle_type(-1 , 5, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , -1, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , 5, -1));
    EXPECT_STREQ("Not a Triangle", triangle_type(200, 100, 50));
    EXPECT_STREQ("Equilateral", triangle_type(100, 100, 100));
    EXPECT_STREQ("Isosceles", triangle_type(100, 199, 100));
    EXPECT_STREQ("Isosceles", triangle_type(100, 2, 100));
    EXPECT_STREQ("Scalene", triangle_type(3 , 4, 5));
}

TEST(triangle_type, C2) {
    EXPECT_STREQ("Invalid", triangle_type(-1 , 5, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , -1, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , 5, -1));
    EXPECT_STREQ("Not a Triangle", triangle_type(200, 100, 50));
    EXPECT_STREQ("Equilateral", triangle_type(100, 100, 100));
    EXPECT_STREQ("Isosceles", triangle_type(100, 199, 100));
    EXPECT_STREQ("Isosceles", triangle_type(100, 2, 100));
    EXPECT_STREQ("Scalene", triangle_type(3 , 4, 5));
}

TEST(triangle_type, MCDC) {
    EXPECT_STREQ("Invalid", triangle_type(-1 , 5, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , -1, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , 5, -1));
    EXPECT_STREQ("Invalid", triangle_type(201 , 5, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , 201, 5));
    EXPECT_STREQ("Invalid", triangle_type(5 , 5, 201));
    EXPECT_STREQ("Equilateral", triangle_type(100, 100, 100));
    EXPECT_STREQ("Not a Triangle", triangle_type(200, 100, 50));
    EXPECT_STREQ("Isosceles", triangle_type(100, 199, 100));
    EXPECT_STREQ("Isosceles", triangle_type(100, 2, 100));
    EXPECT_STREQ("Scalene", triangle_type(3 , 4, 5));
}
