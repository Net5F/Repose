# Note: We normally would build this as a static library target, but this 
#       library currently only has headers, thus we must make it an interface 
#       target. The lines for making it a static library are commented out.

target_sources(Shared
#    PUBLIC
    INTERFACE
        Public/BuildModeDefs.h
        Public/ProjectInteractionType.h
)

target_include_directories(Shared
#    PUBLIC
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
