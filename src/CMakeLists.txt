target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)