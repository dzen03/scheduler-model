file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        sources/*.cpp
        includes/*.h
)

add_library(model-sources SHARED ${sources})

target_include_directories(model-sources PUBLIC sources includes)
target_link_libraries(model-sources PRIVATE ${OPENTELEMETRY_CPP_LIBRARIES})
