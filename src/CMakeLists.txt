cmake_minimum_required(VERSION 3.2)
project(sivenkov-cw)

set(CMAKE_CXX_STANDARD 17)

include_directories(./dep)
include_directories(./)

find_package(SFML COMPONENTS system graphics audio network CONFIG REQUIRED)
find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
       set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile)
       set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_IN})

       add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
       )

endif()

if (UNIX)
    find_package(SQLite3 REQUIRED)
endif ()
if (WIN32)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
endif()

set(IMGUI
        ./dep/imgui-SFML_export.h
        ./dep/imgui-SFML.cpp
        ./dep/imgui-SFML.h
        ./dep/imconfig-SFML.h
        ./dep/imconfig.h
        ./dep/imgui.cpp
        ./dep/imgui.h
        ./dep/imgui_demo.cpp
        ./dep/imgui_draw.cpp
        ./dep/imgui_internal.h
        ./dep/imgui_tables.cpp
        ./dep/imgui_widgets.cpp
        ./dep/imstb_rectpack.h
        ./dep/imstb_textedit.h
        ./dep/imstb_truetype.h
)

set(CC
        CustomComponent.h
        CustomComponent.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${IMGUI} ${CC})

if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-graphics sfml-audio sfml-network GL sqlite3)
endif()
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-graphics sfml-audio sfml-network opengl32 unofficial::sqlite3::sqlite3)
endif()

if(DOXYGEN_FOUND)
       install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen_docs/latex
               DESTINATION docs)
endif()