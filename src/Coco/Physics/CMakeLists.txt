option(COCO_PHYSICS_INCLUDE_JOLT "Add Jolt physics support" ON)

add_library(Physics STATIC
        PhysicsService.h
        PhysicsService.cpp
)

if(COCO_PHYSICS_INCLUDE_JOLT)
    add_subdirectory(Providers/Jolt)
endif()