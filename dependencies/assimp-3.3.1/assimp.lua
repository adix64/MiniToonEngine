project "AssImp"
    kind "SharedLib"
    language "C++"
    location(baseDir .. ".prj")
    targetdir(baseDir .. ".bin/" .. _ACTION)
    objdir(baseDir .. ".obj/" .. _ACTION)
    
    files { 
         "./**.hpp",
         "./**.h",
         "./**.inl",
         "./**.c",
         "./**.cc",
         "./**.cpp"
         }

    flags { "Symbols" }
    defines { "_WINDLL", "_WINDOWS" }
    includedirs { "include" }
    
    links 
    {
        "winmm.lib",
        "imm32.lib",
        "version.lib",
        "msvcrt.lib"
    }
    
    filter "configurations:Debug"
      defines { "DEBUG" }
   
    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"


   -- local state = {includes={}, links={}, defines={}}
   -- dependsOn["libpng"](state)
   local inclPath = path.getabsolute("./include/")
   local inclPath1 = path.getabsolute("./contrib/")
   local inclPath2 = path.getabsolute("./")

--   local inclPath1 = path.getabsolute("./contrib/clipper/")
--   local inclPath2 = path.getabsolute("./contrib/ConvertUTF/")
--   local inclPath3 = path.getabsolute("./contrib/irrXML/")
--   local inclPath4 = path.getabsolute("./contrib/openddlparser/include/")
--   local inclPath5 = path.getabsolute("./contrib/poly2tri/")
--   local inclPath6 = path.getabsolute("./contrib/poly2tri/common/")
--   local inclPath7 = path.getabsolute("./contrib/poly2tri/sweep/")
--   local inclPath8 = path.getabsolute("./contrib/rapidjson/")
--   local inclPath9 = path.getabsolute("./contrib/rapidjson/error/")
--   local inclPath10 = path.getabsolute("./contrib/rapidjson/internal/")
--   local inclPath11 = path.getabsolute("./contrib/rapidjson/msintttypes/")
--   local inclPath12 = path.getabsolute("./contrib/unzip/")
--   local inclPath13 = path.getabsolute("./contrib/zlib/")

    dependsOn["assimp"] = function (state)
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath1))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath2))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath2))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath3))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath4))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath5))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath6))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath7))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath8))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath9))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath10))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath11))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath12))
--            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath13))
            table.insert(state.links, "assimp")
    end