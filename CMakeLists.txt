cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.12...3.16)
project(calvision-celeritas)

find_package(ROOT REQUIRED)
find_package(Geant4 REQUIRED gdml)
find_package(nlohmann_json REQUIRED)
find_package(Celeritas REQUIRED)
include(CudaRdcUtils)

# Sources

set(_src
    src/DetectorMapping.cc
    src/EnergyDepositData.cc
    src/OpticalHitRecorder.cc
    src/celer/CelerRunner.cc
    src/celer/EnergyDepositReader.cc
    src/g4/ActionInitialization.cc
    src/g4/DetectorConstruction.cc
    src/g4/EnergyDepositWriter.cc
    src/g4/EventAction.cc
    src/g4/GeantRunner.cc
    src/g4/GeneratorOffload.cc
    src/g4/OpticalSensitiveDetector.cc
    src/g4/PhysicsList.cc
    src/g4/PrimaryGeneratorAction.cc
)

set(_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Geant4_INCLUDE_DIR}
    ${NLOHMANN_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIR}
    ${Celeritas_ROOT}/include
)

set(_libs
    ${Geant4_LIBRARIES}
    nlohmann_json::nlohmann_json
    ROOT::Core
    ${ROOT_LIBRARIES}
    Celeritas::celeritas
    Celeritas::accel
)

add_executable(dual-readout src/main.cc ${_src})
target_include_directories(dual-readout PRIVATE ${_includes})
cuda_rdc_target_link_libraries(dual-readout ${_libs})

# configure_file(${PROJECT_SOURCE_DIR}/benchmark_analysis.C ${PROJECT_BINARY_DIR} COPYONLY)

set(_src_app
    app/celer_offload/ActionInitialization.cc
    app/celer_offload/DetectorConstruction.cc
    app/celer_offload/OpticalHit.cc
    app/celer_offload/OpticalSensitiveDetector.cc
    app/celer_offload/PhysicsList.cc
    app/celer_offload/PrimaryGeneratorAction.cc
    app/celer_offload/Runner.cc
    app/celer_offload/SignalRunAction.cc
    app/celer_offload/RootSignalWriter.cc
)

add_executable(g4_mt_app app/celer_offload/main.cc ${_src_app})
target_include_directories(g4_mt_app PRIVATE ${_includes})
cuda_rdc_target_link_libraries(g4_mt_app ${_libs})
