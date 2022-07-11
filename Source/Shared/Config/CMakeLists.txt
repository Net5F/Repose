target_sources(Shared
    PRIVATE
        Private/ProjectSharedConfig.cpp
    PUBLIC
        Public/ProjectSharedConfig.h
)

target_include_directories(Shared
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
