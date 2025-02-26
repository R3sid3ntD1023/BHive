cmake_minimum_required(VERSION 3.8)


add_subdirectory(Editor)
add_subdirectory(Project)
add_subdirectory(ProjectLauncher)
add_subdirectory(SolarSystem)
add_subdirectory(Shadows)

set(VS_STARTUP_PROJECT, SolorSystem)


