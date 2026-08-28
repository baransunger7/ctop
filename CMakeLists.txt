cmake_minimum_required(VERSION 3.20)
project(ctop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/lib")

include(FetchContent)

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v5.0.0
)

set(FTXUI_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(FTXUI_BUILD_DOCS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(ftxui)

add_executable(ctop src/main.cpp
        include/sysmon/core/system_reader.hpp
        src/core/system_reader.cpp
        include/sysmon/utils/time_helpers.hpp
        src/utils/time_helpers.cpp
        include/sysmon/models/cpu_stats.hpp
        include/sysmon/models/process_info.hpp
        include/sysmon/core/process_reader.hpp
        src/core/process_reader.cpp
        include/sysmon/utils/string_helpers.hpp
        src/utils/string_helpers.cpp
        include/sysmon/core/process_view.hpp
        src/core/process_view.cpp
        src/application.cpp
        include/sysmon/application.hpp
        include/sysmon/ui/main_window.hpp
        src/ui/main_window.cpp
        include/sysmon/models/system_info.hpp
        include/sysmon/commands/command.hpp
        include/sysmon/commands/kill_command.hpp
        src/commands/kill_command.cpp
        include/sysmon/commands/command_manager.hpp
        src/commands/command_manager.cpp)

target_include_directories(ctop PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ctop PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
)

target_include_directories(ctop PRIVATE ${FTXUI_SOURCE_DIR}/include)

