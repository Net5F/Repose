target_sources(ServerLib
    PUBLIC
        Public/Components/Plant.h
        Public/Components/RandomWalkerAI.h
        Public/TypeLists/ProjectAITypes.h
        Public/TypeLists/ProjectObservedComponentTypes.h
        Public/TypeLists/ProjectPersistedComponentTypes.h
)

target_include_directories(ServerLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/TypeLists
)
