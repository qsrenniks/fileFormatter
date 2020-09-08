cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(fileFormatter)

macro(addunit FILENAME)
	set(FF_SOURCE ${FF_SOURCE} source/${FILENAME})
endmacro()

addunit(main.cpp)

add_executable(ff ${FF_SOURCE})

target_compile_options(ff PRIVATE /std:c++17)

install(TARGETS ff)

