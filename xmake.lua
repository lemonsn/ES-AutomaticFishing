add_rules("mode.debug", "mode.release")

add_repositories(
    "liteldev-repo https://github.com/LiteLDev/xmake-repo.git",
    "groupmountain-repo https://github.com/GroupMountain/xmake-repo.git"
)

add_requires("glaciehook 1.0.1", { configs = { static = true } })

add_requires(
    "endstone 0.6.1",
    "fmt >=10.0.0 <11.0.0",
    "expected-lite 0.8.0",
    "libhat 2024.9.22",
    "detours v4.0.1-xmake.1"
)

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("AutomaticFishing")
    add_defines(
        "NOMINMAX", 
        "UNICODE", 
        "_HAS_CXX23=1"
    )
    add_packages(
        "endstone",
        "fmt",
        "expected-lite",
        "libhat",
        "detours",
        "glaciehook"
    )
    set_kind("shared")
    set_languages("cxx20")
    set_symbols("debug")
    add_includedirs("src")
    add_files("src/**.cpp")

    -- EndStone Entt
    add_defines("ENTT_SPARSE_PAGE=2048")
    add_defines("ENTT_PACKED_PAGE=128")

    add_cxxflags("/Zc:__cplusplus")
    add_cxflags(
        "/EHa",
        "/utf-8",
        -- "/W4",
        "/sdl"
    )

    if is_mode("debug") then
        add_defines("DEBUG")
    elseif is_mode("release") then
        add_defines("NDEBUG")
    end

    after_build(function(target)
        local output_dir = path.join(os.projectdir(), "bin")

        os.cp(
            target:targetfile(), 
            path.join(output_dir, target:name() .. ".dll")
        )

        local pdb_path = path.join(output_dir, target:name() .. ".pdb")
        if os.isfile(target:symbolfile()) then 
            os.cp(target:symbolfile(), pdb_path) 
        end

        cprint("${bright green}[plugin Packer]: ${reset}plugin already generated to " .. output_dir)
    end)