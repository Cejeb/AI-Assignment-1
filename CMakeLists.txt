cmake_minimum_required(VERSION 3.10)

# raylib-tileson fails at runtime on Linux with a debug build due to an
# allocation bug spotted by ASan. To avoid the message, set the environment via:
# export ASAN_OPTIONS=new_delete_type_mismatch=0

set(DEPS "${CMAKE_CURRENT_SOURCE_DIR}/deps")

# Commented out - we are using a local vcpkg export provided with the lab
#if (DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_CMAKE "${DEPS}/vcpkg-export/scripts/buildsystems/vcpkg.cmake")
  set(CMAKE_TOOLCHAIN_FILE ${VCPKG_CMAKE})
#endif ()

set(RLCPP "${DEPS}/raylib-cpp")
set(LIBOAI "${DEPS}/liboai/install")
set(LIB_RAYLIB_TILESON "${DEPS}/raylib-tileson/install")

project(aipfg-ass1)

find_package(raylib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

foreach(n raylib-openai-npcs)

  if ("raylib-openai-npcs" STREQUAL n)
    add_executable(${n} ${n}.cpp openai-helper.cpp openai-helper.hpp sprite.hpp)
  else()
    add_executable(${n} ${n}.cpp)
  endif()

  target_compile_features(${n} PRIVATE cxx_std_17)

  target_include_directories(${n} PRIVATE ${raylib_INCLUDE_DIRS}) # needed?
  target_include_directories(${n} PRIVATE ${RLCPP}/include) # ${RLEXTRAS})
  target_include_directories(${n} PRIVATE ${LIBOAI})
  target_include_directories(${n} PRIVATE ${LIB_RAYLIB_TILESON}/include)


  target_link_directories(${n} PRIVATE ${LIBOAI}/lib)
  target_link_directories(${n} PRIVATE ${LIB_RAYLIB_TILESON}/lib)

  target_link_libraries(${n} PRIVATE oai)
  target_link_libraries(${n} PRIVATE raylib-tileson)
  target_link_libraries(${n} PRIVATE raylib)
  target_link_libraries(${n} PRIVATE cpr::cpr)
  target_link_libraries(${n} PRIVATE nlohmann_json::nlohmann_json)

  if (UNIX)
    # asan and ubsan now seem required, even with a non-Debug build.
    target_link_libraries(${n} PRIVATE asan ubsan m dl pthread)
  endif()

endforeach(n)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT raylib-openai-npcs)
