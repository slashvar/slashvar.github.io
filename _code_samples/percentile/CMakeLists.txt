project(compute_percentile)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast -march=native")

add_executable(compute_percentile compute_percentile.cc)
