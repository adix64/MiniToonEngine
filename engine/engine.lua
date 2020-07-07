dofile(baseDir.."dependencies/LibPNG-1.6.23/libpng.lua")
dofile(baseDir.."dependencies/assimp-3.3.1/assimp.lua")

project "Engine"
    kind "SharedLib"
    language "C++"
    location(baseDir .. ".prj")
    targetdir(baseDir .. ".bin/" .. _ACTION)
    objdir(baseDir .. ".obj/" .. _ACTION)

     files { 
         "public/**.h",
         "private/**.h",
         "public/**.hpp",
         "private/**.hpp",
         "private/**.cpp",


         "bulletPhysics/**.h",
         "bulletPhysics/**.hpp",
         "bulletPhysics/**.c",
         "bulletPhysics/**.cpp",
         "bulletPhysics/**.cc",
         "bulletPhysics/**.inl",

         "../dependencies/imgui-master/**.h",
         "../dependencies/imgui-master/**.hpp",
         "../dependencies/imgui-master/**.c",
         "../dependencies/imgui-master/**.cpp",
         "../dependencies/imgui-master/**.cc",
         "../dependencies/imgui-master/**.inl",
         
     }
     removefiles
     {
        "../dependencies/imgui-master/examples/**"
     }
    flags { "Symbols" }
    includedirs({"./public", "./private", "./BulletPhysics",
                 "./BulletPhysics/Bullet3Collision", "./BulletPhysics/Bullet3Common", 
                 "./BulletPhysics/Bullet3Dynamics", "./BulletPhysics/Bullet3Geometry",
                 "./BulletPhysics/Bullet3Serialize", "./BulletPhysics/BulletCollision",
                 "./BulletPhysics/BulletDynamics", "./BulletPhysics/LinearMath",
                 "../dependencies/SDL2_mixer-2.0.1/include",
                 "../dependencies/imgui-master"
                 })






    defines({"ENGINE_DLL", "ENGINE_EXPORT"})

    links 
    {
        "LibPNG",
        "AssImp",
        "SDL2_mixer"
    }

    filter "configurations:Debug"
      defines { "DEBUG" }
 
    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      
    filter "*"
    local inclPath = path.getabsolute("./public")
    local inclPath1 = path.getabsolute("./BulletPhysics")
    local inclPath2 = path.getabsolute("./BulletPhysics/Bullet3Collision")
    local inclPath3 = path.getabsolute("./BulletPhysics/Bullet3Common")
    local inclPath4 = path.getabsolute("./BulletPhysics/Bullet3Dynamics")
    local inclPath5 = path.getabsolute("./BulletPhysics/Bullet3Geometry")
    local inclPath6 = path.getabsolute("./BulletPhysics/Bullet3Serialize")
    local inclPath7 = path.getabsolute("./BulletPhysics/BulletCollision")
    local inclPath8 = path.getabsolute("./BulletPhysics/BulletDynamics")
    local inclPath9 = path.getabsolute("./BulletPhysics/LinearMath")
    local inclPath10 = path.getabsolute("../dependencies/SDL2_mixer-2.0.1/include")
    dependsOn["engine"] = function (state)
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath1))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath2))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath3))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath4))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath5))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath6))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath7))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath8))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath9))
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath10))
            table.insert(state.defines, "ENGINE_DLL")
            table.insert(state.links, "Engine")
    end
    
   local state = {includes={}, links={}, defines={}}
    dependsOn["sdl"](state)
    defines(state.defines)
    includedirs(state.includes)
    links(state.links)

    local state2 = {includes = {}, links = {}, defines = {}}
    dependsOn["libpng"](state2)
    defines(state2.defines)
    includedirs(state2.includes)
    links(state2.links)

    local state3 = {includes = {}, links = {}, defines = {}}
    dependsOn["assimp"](state3)
    defines(state3.defines)
    includedirs(state3.includes)
    links(state3.links)

    -- local state4 = {includes = {}, links = {}, defines = {}}
    -- dependsOn["bulletphysicsapi"](state4)
    -- defines(state4.defines)
    -- includedirs(state4.includes)
    -- links(state4.links)

--    local state4 = {includes = {}, links = {}, defines = {}}
--    dependsOn["glm"](state4)
--    defines(state4.defines)
--    includedirs(state4.includes)
--    links(state4.links) 

    