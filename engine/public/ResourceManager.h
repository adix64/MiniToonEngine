#pragma once
#include "../../../dependencies/rapidxml-1.13/rapidxml_utils.hpp";
#include <Component/CharacterAnimation/CharacterAnimation.h>
#include <unordered_map>

#include "../assimp-3.3.1/include/assimp/Importer.hpp"
#include "../assimp-3.3.1/include/assimp/scene.h"
#include "../assimp-3.3.1/include/assimp/postprocess.h"
struct GameObjCreationSettings
{
	std::string name = "";
	std::string fileName = "";
	std::string colliderFileName = "";
	bool isSkybox = false;
	float depthOverride = 0.f;
	bool isDynamic = false;
	bool wireframe = false;
};
class ResourceManager
{
public:
	struct CharAnimNFO
	{
		std::string fileName;
		float minMandatoryTime;
		float maxFadeInTime;
		float relevantTime;
		float contact;
		
		CharAnimNFO(std::string &pFileName)
		{
			fileName = pFileName;
			minMandatoryTime = contact = maxFadeInTime = relevantTime = 0.0f;
		}
		
		CharAnimNFO()
		{
			fileName = "";
			minMandatoryTime = contact = maxFadeInTime = relevantTime = 0.0f;
		}

		void SetMandatoryTime(char *mt)
		{
			sscanf(mt, "%f", &minMandatoryTime);
			minMandatoryTime *= (1 / 24.f);
		}
		
		void SetFadeInTime(char *mt)
		{
			sscanf(mt, "%f", &maxFadeInTime);
			maxFadeInTime *= (1 / 24.f);

		}

		void SetRelevantTime(char *mt)
		{
			sscanf(mt, "%f", &relevantTime);
			relevantTime *= (1 / 24.f);
		}

		void SetContactTime(char *mt)
		{
			sscanf(mt, "%f", &contact);
			contact *= (1 / 24.f);
		}
	};
public:
	Assimp::Importer mImporter;
	enum MapType {DIFFUSE, SPECULAR, NORMAL};
	struct MaterialPath
	{
		std::string diffuse, specular, normal;
	};
private:
	ResourceManager();
	// Stop the compiler from generating copy methods
	ResourceManager(ResourceManager const& copy);            // do NOT implement
	ResourceManager& operator=(ResourceManager const& copy); // do NOT implement
public:
	void Init();
	static ResourceManager& getInstance()
	{
		static ResourceManager instance;
		return instance;
	}
	void LoadTextures();
	CharAnimNFO GetAnimNFO(std::string &characterName, int animationTag);
	const char * GetCharacterFileName(const char *characterName);
	const char * GetCharacterLimbColShapeFname();
	const char * GetMaterialMap(const char *assetName, MapType mt);
	const char * GetCharacterColShapeFname() { return "../assets/capsuleFwd.obj"; }
	std::vector<GameObjCreationSettings> GetEnvironment() { return environment; }
	CharacterAnimation *GetCharacterAnimationTrack(std::string &name);
private:
	std::unordered_map<std::string, std::unordered_map<int,CharAnimNFO> > characterAnimationFileNames;
	std::unordered_map<std::string, std::string> characterFileNames;
	std::unordered_map<std::string, MaterialPath> materialPaths;
	typedef std::pair<std::pair<std::string, std::string>, bool> environmentObjects;
	std::vector<GameObjCreationSettings> environment;

	std::unordered_map<std::string, CharacterAnimation*> mCharacterAnimationTracks;

public:
	unsigned flatNormalMap, defaultSpecularMap, defaultEmissiveMap;
	unsigned raysSprite;
};