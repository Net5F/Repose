target_sources(SharedLib
    PUBLIC
        Public/EntityGraphicDisplayData.h
        Public/EntityGraphicType.h
        Public/EntityInteractionDisplayData.h
        Public/EntityInteractionType.h
        Public/ItemInteractionType.h
        Public/ItemInteractionDisplayData.h
        Public/TypeLists/ProjectItemPropertyTypes.h
        Public/TypeLists/ProjectReplicatedComponentTypes.h
)

target_include_directories(SharedLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/TypeLists
)
