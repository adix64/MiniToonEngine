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
    removefiles 
    {
        "src/B3DImporter.cpp",
        "src/B3DImporter.h",
        "src/ACLoader.cpp",
        "src/ACLoader.h",
        "src/ASELoader.cpp",
        "src/ASELoader.h",
        "src/ASEParser.cpp",
        "src/ASEParser.h",
        "src/BlenderBMesh.cpp",
        "src/BlenderBMesh.h",
        "src/BlenderDNA.cpp",
        "src/BlenderDNA.h",
        "src/BlenderDNA.inl",
        "src/BlenderIntermediate.h",
        "src/BlenderLoader.cpp",
        "src/BlenderLoader.h",
        "src/BlenderModifier.cpp",
        "src/BlenderModifier.h",
        "src/BlenderScene.cpp",
        "src/BlenderScene.h",
        "src/BlenderSceneGen.h",
        "src/BlenderTessellator.cpp",
        "src/BlenderTessellator.h",
        "src/BVHLoader.cpp",
        "src/BVHLoader.h",
        "src/C4DImporter.cpp",
        "src/C4DImporter.h",
        "src/COBLoader.cpp",
        "src/COBLoader.h",
        "src/COBScene.h",
        "src/CSMLoader.cpp",
        "src/CSMLoader.h",
        "src/D3MFImporter.cpp",
        "src/D3MFImporter.h",
        "src/D3MFOpcPackage.cpp",
        "src/D3MFOpcPackage.h",
        "src/DXFHelper.h",
        "src/DXFLoader.cpp",
        "src/DXFLoader.h",
        "src/FBXAnimation.cpp",
        "src/FBXBinaryTokenizer.cpp",
        "src/FBXCompileConfig.h",
        "src/FBXConverter.cpp",
        "src/FBXConverter.h",
        "src/FBXDeformer.cpp",
        "src/FBXDocument.cpp",
        "src/FBXDocument.h",
        "src/FBXDocumentUtil.cpp",
        "src/FBXDocumentUtil.h",
        "src/FBXImporter.cpp",
        "src/FBXImporter.h",
        "src/FBXImportSettings.h",
        "src/FBXMaterial.cpp",
        "src/FBXMeshGeometry.cpp",
        "src/FBXMeshGeometry.h",
        "src/FBXModel.cpp",
        "src/FBXNodeAttribute.cpp",
        "src/FBXParser.cpp",
        "src/FBXParser.h",
        "src/FBXProperties.cpp",
        "src/FBXProperties.h",
        "src/FBXTokenizer.cpp",
        "src/FBXTokenizer.h",
        "src/FBXUtil.cpp",
        "src/FBXUtil.h",
        "src/HalfLifeFileData.h",
        "src/HMPFileData.h",
        "src/HMPLoader.cpp",
        "src/HMPLoader.h",
        "src/IFCBoolean.cpp",
        "src/IFCCurve.cpp",
        "src/IFCGeometry.cpp",
        "src/IFCLoader.cpp",
        "src/IFCLoader.h",
        "src/IFCMaterial.cpp",
        "src/IFCOpenings.cpp",
        "src/IFCProfile.cpp",
        "src/IFCReaderGen.cpp",
        "src/IFCReaderGen.h",
        "src/IFCUtil.cpp",
        "src/IFCUtil.h",
        "src/IFF.h",
        "src/IRRLoader.cpp",
        "src/IRRLoader.h",
        "src/IRRMeshLoader.cpp",
        "src/IRRMeshLoader.h",
        "src/IRRShared.cpp",
        "src/IRRShared.h",
        "src/LWOAnimation.cpp",
        "src/LWOAnimation.h",
        "src/LWOBLoader.cpp",
        "src/LWOFileData.h",
        "src/LWOLoader.cpp",
        "src/LWOLoader.h",
        "src/LWOMaterial.cpp",
        "src/LWSLoader.cpp",
        "src/LWSLoader.h",
        "src/MD2FileData.h",
        "src/MD2Loader.cpp",
        "src/MD2Loader.h",
        "src/MD2NormalTable.h",
        "src/MD3FileData.h",
        "src/MD3Loader.cpp",
        "src/MD3Loader.h",
        "src/MD4FileData.h",
        "src/MD5Loader.cpp",
        "src/MD5Loader.h",
        "src/MD5Parser.cpp",
        "src/MD5Parser.h",
        "src/MDCFileData.h",
        "src/MDCLoader.cpp",
        "src/MDCLoader.h",
        "src/MDCNormalTable.h",
        "src/MDLDefaultColorMap.h",
        "src/MDLFileData.h",
        "src/MDLLoader.cpp",
        "src/MDLLoader.h",
        "src/MDLMaterialLoader.cpp",
        "src/MS3DLoader.cpp",
        "src/MS3DLoader.h",
        "src/NDOLoader.cpp",
        "src/NDOLoader.h",
        "src/NFFLoader.cpp",
        "src/NFFLoader.h",
        "src/OFFLoader.cpp",
        "src/OFFLoader.h",
        "src/OgreBinarySerializer.cpp",
        "src/OgreBinarySerializer.h",
        "src/OgreImporter.cpp",
        "src/OgreImporter.h",
        "src/OgreMaterial.cpp",
        "src/OgreParsingUtils.h",
        "src/OgreStructs.cpp",
        "src/OgreStructs.h",
        "src/OgreXmlSerializer.cpp",
        "src/OgreXmlSerializer.h",
        "src/OpenGEXExporter.cpp",
        "src/OpenGEXExporter.h",
        "src/OpenGEXImporter.cpp",
        "src/OpenGEXImporter.h",
        "src/OpenGEXStructs.h",
        "src/Q3BSPFileData.h",
        "src/Q3BSPFileImporter.cpp",
        "src/Q3BSPFileImporter.h",
        "src/Q3BSPFileParser.cpp",
        "src/Q3BSPFileParser.h",
        "src/Q3BSPZipArchive.cpp",
        "src/Q3BSPZipArchive.h",
        "src/Q3DLoader.cpp",
        "src/Q3DLoader.h",
        "src/SIBImporter.cpp",
        "src/SIBImporter.h",
        "src/SMDLoader.cpp",
        "src/SMDLoader.h",
--        "src/STLExporter.cpp",
--        "src/STLExporter.h",
--        "src/STLLoader.cpp",
--        "src/STLLoader.h",
        "src/TargetAnimation.cpp",
        "src/TargetAnimation.h",
        "src/TerragenLoader.cpp",
        "src/TerragenLoader.h",
        "src/TinyFormatter.h",
        "src/UnrealLoader.cpp",
        "src/UnrealLoader.h"
     }

    flags { "Symbols" }
    defines { "_WINDLL", "_WINDOWS" }
    includedirs { "include" }
    
    links 
    {
        "winmm",
        "imm32",
        "version",
        "msvcrtd",
--        "MSVCPRTD"
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