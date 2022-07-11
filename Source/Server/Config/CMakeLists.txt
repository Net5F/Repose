target_sources(Server
    PRIVATE
        Private/ProjectConfig.cpp
    PUBLIC
        Public/ProjectConfig.h
)

target_include_directories(Server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
