target_sources(Client
    PRIVATE
        Private/AnimationSystem.cpp
        Private/SimulationExtension.cpp
    PUBLIC
        Public/AnimationSystem.h
        Public/SimulationExtension.h
)

target_include_directories(Client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
