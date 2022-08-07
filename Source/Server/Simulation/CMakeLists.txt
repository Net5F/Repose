target_sources(Server
    PRIVATE
        Private/MazeGenerationSystem.cpp
        Private/SimulationExtension.cpp
    PUBLIC
        Public/MazeGenerationSystem.h
        Public/SimulationExtension.h
)

target_include_directories(Server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
