idf_component_register(INCLUDE_DIRS "." PRIV_REQUIRES idf::mbedtls LDFRAGMENTS "linker.lf")


add_definitions(-DBELL_USE_MBEDTLS)
add_subdirectory(../../../../core bindir)
target_link_libraries(${COMPONENT_LIB} INTERFACE euphonium)
