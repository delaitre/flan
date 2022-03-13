
set(SOURCES
    main.cpp
    flan.qrc
)

add_executable(flan_app ${SOURCES})
target_link_libraries(flan_app PRIVATE flan)
set_target_properties(flan_app PROPERTIES OUTPUT_NAME flan)
