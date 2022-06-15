target_sources(Client
    PRIVATE
        Private/main.cpp
    #PUBLIC
)

target_include_directories(Client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    #PUBLIC
        #${CMAKE_CURRENT_SOURCE_DIR}/Public
)