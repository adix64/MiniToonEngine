project "SDL"
    kind "SharedLib"
    language "C++"
    location(baseDir .. ".prj")
    targetdir(baseDir .. ".bin/" .. _ACTION)
    objdir(baseDir .. ".obj/" .. _ACTION)
    
     files { 
         "src/**.h", 
         "src/**.c"
         }
     removefiles 
     {
         "src/test/**",
          "src/main/**",
          
          "src/audio/alsa/**",
          "src/audio/android/**",
          "src/audio/arts/**",
          "src/audio/bsd/**",
          "src/audio/coreaudio/**",
          "src/audio/dsp/**",
          "src/audio/emscripten/**",
          "src/audio/esd/**",
          "src/audio/fusionsound/**",
          "src/audio/haiku/**",
          "src/audio/nacl/**",
          "src/audio/nas/**",
          "src/audio/paudio/**",
          "src/audio/psp/**",
          "src/audio/pulseaudio/**",
          "src/audio/qsa/**",
          "src/audio/sndio/**",
          "src/audio/sun/**",
          
          "src/core/android/**",
          "src/core/linux/**",
          "src/core/winrt/**",
          
          "src/file/cocoa/**",
          
          "src/filesystem/android/**",
          "src/filesystem/cocoa/**",
          "src/filesystem/dummy/**",
          "src/filesystem/emscripten/**",
          "src/filesystem/haiku/**",
          "src/filesystem/nacl/**",
          "src/filesystem/unix/**",
          "src/filesystem/winrt/**",
          
          "src/haptic/darwin/**",
          "src/haptic/dummy/**",
          "src/haptic/linux/**",
          
          "src/joystick/android/**",
          "src/joystick/bsd/**",
          "src/joystick/darwin/**",
          "src/joystick/dummy/**",
          "src/joystick/emscripten/**",
          "src/joystick/haiku/**",
          "src/joystick/iphoneos/**",
          "src/joystick/linux/**",
          "src/joystick/psp/**",
          
          "src/loadso/dlopen/**",
          "src/loadso/dummy/**",
          "src/loadso/haiku/**",
          
          "src/power/android/**",
          "src/power/emscripten/**",
          "src/power/haiku/**",
          "src/power/linux/**",
          "src/power/macosx/**",
          "src/power/psp/**",
          "src/power/uikit/**",
          "src/power/winrt/**",
          
          "src/render/psp/**",
          
          "src/thread/generic/SDL_sysmutex*",
          "src/thread/generic/SDL_systhread*",
          "src/thread/generic/SDL_systls*",
          "src/thread/generic/SDL_syssem*",
          "src/thread/psp/**",
          "src/thread/pthread/**",
          "src/thread/stdcpp/**",
          
          "src/timer/dummy/**",
          "src/timer/haiku/**",
          "src/timer/psp/**",
          "src/timer/unix/**",
          
          "src/video/android/**",
          "src/video/cocoa/**",
          "src/video/directfb/**",
          "src/video/emscripten/**",
          "src/video/haiku/**",
          "src/video/mir/**",
          "src/video/nacl/**",
          "src/video/pandora/**",
          "src/video/psp/**",
          "src/video/raspberry/**",
          "src/video/uikit/**",
          "src/video/vivante/**",
          "src/video/wayland/**",
          "src/video/winrt/**",
          "src/video/x11/**",
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
        "MSVCPRT",
        "vcruntime.lib",
        "ucrt.lib"
    }
    
    filter "configurations:Debug"
      defines { "DEBUG" }
   
    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

    local inclPath = path.getabsolute("./include")
    dependsOn["sdl"] = function (state)
            table.insert(state.includes, path.getrelative(path.getabsolute("."), inclPath))
            table.insert(state.links, "sdl")
            table.insert(state.links, "opengl32")
    end