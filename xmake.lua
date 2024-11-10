set_xmakever("2.9.5")

-- Globals
PROJECT_NAME = "ScrabsPapyrusExtender"

-- Project
set_project(PROJECT_NAME)
set_version("1.1.0")
set_languages("cxx23")
set_license("Apache-2.0")
set_warnings("allextra", "error")

-- Options
option("copy_to_papyrus")
    set_default(true)
    set_description("Copy finished build to Papyrus SKSE folder")
option_end()

-- Dependencies & Includes
-- https://github.com/xmake-io/xmake-repo/tree/dev
add_requires("magic_enum", "nlohmann_json", "directxtk", "rapidcsv", "xbyak", "catch2", "lua", "sol2")
add_requires("spdlog", { configs = { header_only = false, wchar = true, std_format = true } })

includes("lib/commonlibsse-ng")

-- policies
set_policy("package.requires_lock", true)

-- rules
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("DEBUG")
    set_optimize("none")
elseif is_mode("release") then
    add_defines("NDEBUG")
    set_optimize("fastest")
    set_symbols("debug")
end

set_config("skse_xbyak", true)
-- set_config("skyrim_se", true)
-- set_config("skyrim_ae", true)
-- set_config("skyrim_vr", true)

-- Target
target(PROJECT_NAME)
    -- Dependencies
    add_packages("magic_enum", "nlohmann_json", "directxtk", "rapidcsv", "xbyak", "catch2", "clib-util", "detours", "lua", "sol2")
    add_defines("GLM_ENABLE_EXPERIMENTAL", "XMAKE")

    -- CommonLibSSE
    add_deps("commonlibsse-ng")
    add_rules("commonlibsse-ng.plugin", {
        name = PROJECT_NAME,
        author = "Scrab Joséline",
        description = "Backend systems for SexLab P+"
    })

    -- Source files
    set_pcxxheader("src/PCH.h")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")

    -- flags
    add_cxxflags(
        "cl::/cgthreads4",
        "cl::/diagnostics:caret",
        "cl::/external:W0",
        "cl::/fp:contract",
        "cl::/fp:except-",
        "cl::/guard:cf-",
        "cl::/Zc:enumTypes",
        "cl::/Zc:preprocessor",
        "cl::/Zc:templateScope",
        "cl::/utf-8"
    )
    -- flags (cl: warnings -> errors)
    add_cxxflags("cl::/we4715") -- `function` : not all control paths return a value
    -- flags (cl: disable warnings)
    add_cxxflags(
        "cl::/wd4068", -- unknown pragma 'clang'
        "cl::/wd4201", -- nonstandard extension used : nameless struct/union
        "cl::/wd4265" -- 'type': class has virtual functions, but its non-trivial destructor is not virtual; instances of this class may not be destructed correctly
    )
    -- Conditional flags
    if is_mode("debug") then
        add_cxxflags("cl::/bigobj")
    elseif is_mode("release") then
        add_cxxflags("cl::/Zc:inline", "cl::/JMC-", "cl::/Ob3")
    end

    -- Post Build 
    if has_config("copy_to_papyrus") then
        after_build(function (target)
            local folder = os.getenv("XSE_TES5_MODS_PATH")
            if folder then
                local SkyrimPath = path.join(folder, target:name(), "SKSE/Plugins")
                os.cp(target:targetfile(), SkyrimPath)
                os.cp(target:symbolfile(), SkyrimPath)
            else
                print("Warning: SkyrimPath not defined. Skipping post-build copy.")
            end
        end)
    end
target_end()