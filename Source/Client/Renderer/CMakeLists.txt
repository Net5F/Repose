target_sources(Client
    PRIVATE
        Private/RendererExtension.cpp
    PUBLIC
        Public/RendererExtension.h
)

target_include_directories(Client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
