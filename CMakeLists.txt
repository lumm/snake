cmake_minimum_required(VERSION 2.6)
project(Snake)

set(CMAKE_CXX_FLAGS "-O2 -g -Wall")

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(
		${SDL_INCLUDE_DIR}
		${SDLIMAGE_INCLUDE_DIR}
		${SDLTTF_INCLUDE_DIR}
)

set(MAIN_SRC
    main.cpp
)

add_executable(snake ${MAIN_SRC})
target_link_libraries(snake ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})

configure_file(cell.png cell.png COPYONLY)
configure_file(food.png food.png COPYONLY)
configure_file(arial.ttf arial.ttf COPYONLY)

