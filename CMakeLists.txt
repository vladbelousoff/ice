cmake_minimum_required(VERSION 3.0)
project(ice)
add_executable(unit unit.c munit/munit.c ice_ast.c)
