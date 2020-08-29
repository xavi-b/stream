file(GLOB SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_sources(${MODULE_NAME}
    PRIVATE
        "${SRC}"
)