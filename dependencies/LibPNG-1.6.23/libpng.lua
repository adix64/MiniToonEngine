project "LibPNG"
    kind "SharedLib"
    language "C++"
    location(baseDir .. ".prj")
    targetdir(baseDir .. ".bin/" .. _ACTION)
    objdir(baseDir .. ".obj/" .. _ACTION)
    
     files { 
         "include/**.h", 
         "src/**.c"
         }

    flags { "Symbols" }
    defines { "_WINDLL", "_WINDOWS" }
    includedirs { "include" }
    
    links 
    {
        "winmm",
        "imm32",
        "version",
        "msvcrt",
        "MSVCPRT"
    }
    
    filter "configurations:Debug"
      defines { "DEBUG" }
   
    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   -- local state = {includes={}, links={}, defines={}}
   -- dependsOn["libpng"](state)
   local inclPath = path.getabsolute("./include")
    dependsOn["libpng"] = function (state)
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath))
            table.insert(state.links, "libpng")
    end