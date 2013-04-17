add_executable(sim-coalescent-trees sim-coalescent-trees.cpp)
install(TARGETS sim-coalescent-trees
        DESTINATION ${INSTALL_BIN_DIR}
        COMPONENT sim-coalescent-trees)
