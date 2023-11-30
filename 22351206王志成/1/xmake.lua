set_arch("x64")
set_languages("c17", "c++20")

add_rules("mode.debug", "mode.release")
add_requires("imgui", {configs = {sdl2 = true, vulkan = true}})
add_requires("vulkansdk")

target("Renderer")
    set_kind("binary")
    add_rules("utils.glsl2spv", {bin2c = true, outputdir = "shaders/compiled"})
    add_includedirs("./shaders/compiled")
    add_files("./main.cpp")
    add_files("./shaders/*.vert", "./shaders/*.frag")
    add_packages("vulkansdk", "imgui")