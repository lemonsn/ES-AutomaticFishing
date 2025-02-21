add_rules("mode.debug", "mode.release")

add_repositories(
    "liteldev-repo https://github.com/LiteLDev/xmake-repo.git",
    "iceblcokmc https://github.com/IceBlcokMC/xmake-repo.git"
)

add_requires(
    "endstone 0.6.0",
    "expected-lite 0.8.0",
    "entt 3.14.0",
    "microsoft-gsl 4.0.0",
    "nlohmann_json 3.11.3",
    "boost 1.85.0",
    "glm 1.0.1",
    "concurrentqueue 1.0.4",
    "magic_enum 0.9.7",
    "libhat 2024.9.22",
    "polyhook2 2024.8.1"
)

local fmt_version = "fmt >=10.0.0 <11.0.0";
if is_plat("windows") then
    add_requires(fmt_version)
elseif is_plat("linux") then
    set_toolchains("clang")
    add_requires("libelf 0.8.13")
    add_requires(fmt_version, {configs = {header_only = true}})
end

if is_plat("windows") and not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("AutomaticFishing")
    add_defines(
        "NOMINMAX", 
        "UNICODE", 
        "_HAS_CXX17",
        "_HAS_CXX20",
        "_HAS_CXX23"
    )
    add_packages(
        "fmt",
        "expected-lite",
        "entt",
        "microsoft-gsl",
        "nlohmann_json",
        "boost",
        "glm",
        "concurrentqueue",
        "endstone",
        "magic_enum",
        "libhat",
        "polyhook2"
    )
    set_kind("shared")
    set_languages("cxx20")
    set_symbols("debug")
    add_includedirs("src")
    add_files("src/**.cpp")

    -- EndStone Entt
    add_defines("ENTT_SPARSE_PAGE=2048")
    add_defines("ENTT_PACKED_PAGE=128")

    if is_plat("windows") then
        add_cxxflags("/Zc:__cplusplus")
        add_cxflags(
            "/EHa",
            "/utf-8",
            -- "/W4",
            "/sdl"
        )
    elseif is_plat("linux") then
        add_rpathdirs("$ORIGIN/../")
        add_cxflags(
            "-fPIC",
            "-stdlib=libc++",
            "-fdeclspec",
            {force = true}
        )
        add_ldflags(
            "-stdlib=libc++",
            {force = true}
        )
        add_packages("libelf")
        add_syslinks("dl", "pthread", "c++", "c++abi")
    end

    if is_mode("debug") then
        add_defines("DEBUG")
    elseif is_mode("release") then
        add_defines("NDEBUG")
    end

    after_build(function(target)
        local output_dir = path.join(os.projectdir(), "bin")

        os.cp(
            target:targetfile(), 
            path.join(
                output_dir, 
                target:name() .. (is_plat("linux") and ".so" or ".dll")
            )
        )

        local pdb_path = path.join(output_dir, target:name() .. ".pdb")
        if os.isfile(target:symbolfile()) then 
            os.cp(target:symbolfile(), pdb_path) 
        end

        cprint("${bright green}[plugin Packer]: ${reset}plugin already generated to " .. output_dir)
    end)