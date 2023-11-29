set_arch("x64")
set_languages("c17", "c++20")

add_rules("mode.debug", "mode.release")
add_requires("imgui", {configs = {sdl2 = true, vulkan = true}})
add_requires("vulkansdk")

target("Renderer")
    set_kind("binary")
    add_files("./main.cpp")
    add_packages("vulkansdk", "imgui")