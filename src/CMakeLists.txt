set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include)

add_library(join_server_lib STATIC
    server.cpp
    session.cpp
    parser.cpp
    dbmanager.cpp
)

set_target_properties(join_server_lib PROPERTIES 
    OUTPUT_NAME "join_server_lib"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(join_server_lib
    PUBLIC 
        ${HEADER_DIR}
        ${Boost_INCLUDE_DIRS}
    PRIVATE
        ${SQLITE_DIR}
)

target_link_libraries(join_server_lib
    PUBLIC
        ${Boost_LIBRARIES}
        sqlite
    PRIVATE
        pthread
        dl
)

add_executable(join_server main.cpp)

target_link_libraries(join_server PRIVATE
    join_server_lib
    ${Boost_LIBRARIES}
    sqlite
    pthread
    dl
)

target_include_directories(join_server
    PRIVATE 
        ${HEADER_DIR}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
        ${SQLITE_DIR}
)

set_target_properties(join_server PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)