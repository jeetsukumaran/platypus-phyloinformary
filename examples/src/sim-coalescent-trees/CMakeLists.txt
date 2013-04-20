add_executable(sim-coalescent-trees sim-coalescent-trees.cpp)
install(TARGETS sim-coalescent-trees
        RUNTIME DESTINATION bin
        COMPONENT sim-coalescent-trees)
