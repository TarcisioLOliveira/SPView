set(SOURCES_EXEC
    client.cpp
    gmsh.cpp
    gmsh_view_handler.cpp
    main.cpp
)

set(SOURCES_LIB
    spview.cpp
    data_queue.cpp
)

add_library(${LIB_NAME} ${SOURCES_LIB})
add_executable(${PROJECT_NAME} ${SOURCES_EXEC})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GMSH_LIB} ${Boost_FILESYSTEM_LIBRARIES} ${Boost_THREAD_LIBRARIES})
target_link_libraries(${LIB_NAME} PUBLIC ${GMSH_LIB} ${Boost_FILESYSTEM_LIBRARIES} ${Boost_THREAD_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .
                                LIBRARY DESTINATION .
                                ARCHIVE DESTINATION .)

install(TARGETS ${LIB_NAME} RUNTIME DESTINATION .
                            LIBRARY DESTINATION .
                            ARCHIVE DESTINATION .)
