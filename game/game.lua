baseDir = path.getabsolute("..").."/"

workspace "Game"
    configurations {"Debug", "Release"}
     location(baseDir .. ".prj")
     buildoptions{"/wd4251 /wd4273"}
    flags
    {
      --  "FatalCompileWarnings",
        "MultiProcessorCompile",
        
    }
dependsOn = {}

dofile(baseDir.."dependencies/SDL2-2.0.4/sdl.lua")
dofile(baseDir.."engine/engine.lua")

project "Game"
    kind "ConsoleApp"
    language "C++"
    
    targetdir(baseDir .. ".bin/" .. _ACTION)
    objdir(baseDir .. ".obj/" .. _ACTION)
    location(baseDir .. ".prj")
    
     files { 
         "src/**.h", 
         "src/**.cpp" 
         }
    flags { "Symbols" }
       
    filter "configurations:Debug"
      defines { "DEBUG" }
 
    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
    filter "*"
    local state = {includes={}, links={}, defines={}}
    dependsOn["engine"](state)
      
    includedirs(state.includes)
    links(state.links)
    defines(state.defines)