project(Infantry ASM C CXX)

irm_add_arm_executable(Infantry ST
    TARGET DJI_Board_TypeA
    SOURCES rm_rtos.cc
    DEPENDS rm_shared)
