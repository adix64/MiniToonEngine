#include <sstream>
#include <ResourceManager.h>
#include <direct.h>
#include <Component/CharacterAnimation/ChAnimDefines.h>
#include <Rendering/Texture.h>
#define GetCurrentDir _getcwd

using namespace rapidxml;

ResourceManager::ResourceManager()
{
}

ResourceManager::CharAnimNFO ResourceManager::GetAnimNFO(std::string &characterName, int animationTag)
{
	return characterAnimationFileNames[characterName][animationTag];
}
//
//const char * ResourceManager::GetCharacterLimbColShapeFname()
//{
//	return mColLimbShapeFname.c_str();
//}
//
//const char * ResourceManager::GetCharacterColShapeFname()
//{
//	return mColShapeFname.c_str();
//}


const char* ResourceManager::GetCharacterFileName(const char *characterName)
{
	return characterFileNames[characterName].c_str();
}

const char * ResourceManager::GetMaterialMap(const char *assetName, MapType mt)
{
	if(mt == MapType::DIFFUSE)
		return materialPaths[assetName].diffuse.c_str();
	else if (mt == MapType::NORMAL)
		return materialPaths[assetName].normal.c_str();
	else if (mt == MapType::SPECULAR)
		return materialPaths[assetName].specular.c_str();
	else return "error: invalid material enum";
}

void ResourceManager::Init()
{
	//mColShapeFname = "../assets/capsule.obj";
	//mColLimbShapeFname = "../assets/collLimbSimplex.obj";
	char cCurrentPath[FILENAME_MAX];
	GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
	printf("CWD %s\n", cCurrentPath);

	xml_document<> doc;

	char *file_contents;
	long input_file_size;
	FILE *input_file = fopen("../assets/config/resourceManager.xml", "rt");
	fseek(input_file, 0, SEEK_END);
	input_file_size = ftell(input_file);
	rewind(input_file);
	file_contents = (char*)malloc(input_file_size * (sizeof(char)));
	memset(file_contents, 0, input_file_size);
	fread(file_contents, sizeof(char), input_file_size, input_file);
	fclose(input_file);
	//printf("%s\n", file_contents);

	doc.parse<0>(file_contents);



	xml_node<> *pRoot = doc.first_node();
	xml_node<> *charactersNode= pRoot->first_node("characters");
	xml_node<> *objectsNode = pRoot->first_node("objects");

	for (xml_node<> *characterNode = charactersNode->first_node("character"); characterNode; characterNode = characterNode->next_sibling())
	{

		std::string characterName(characterNode->first_attribute("name")->value());
		std::unordered_map<int, CharAnimNFO> animations;

		xml_node<> *skMeshNode = characterNode->first_node("skinnedMeshRenderer");
		characterFileNames[characterName] = std::string(skMeshNode->value());

		
		std::string diffuse = "../assets/checkerboard.png";
		std::string normal = "../assets/flatNormal.png";
		std::string specular = "../assets/white.png";

		xml_node<> *diffuseNode = characterNode->first_node("diffuse");		
		if (diffuseNode)
			diffuse = diffuseNode->value();
		xml_node<> *specularNode = characterNode->first_node("specular");
		if (specularNode)
			specular = specularNode->value();
		xml_node<> *normalNode = characterNode->first_node("normal");
		if (normalNode)
			normal = normalNode->value();

		MaterialPath mp;
		mp.diffuse = diffuse;
		mp.specular = specular;
		mp.normal = normal;

		materialPaths[characterName] = mp;

		xml_node<> *animsNode = characterNode->first_node("animations");
		for (xml_node<> *animNode = animsNode->first_node(); animNode; animNode = animNode->next_sibling())
		{
			xml_node<> *fileNode = animNode->first_node("file");
			
			int animTag;
			if (!strcmp(animNode->name(), "runFwd"))
				animTag = ChAnim_RUN_FWD_FILE;
			else if (!strcmp(animNode->name(), "runLeft"))
				animTag = ChAnim_RUN_LEFT_FILE;
			else if (!strcmp(animNode->name(), "runRight"))
				animTag = ChAnim_RUN_RIGHT_FILE;
			else if (!strcmp(animNode->name(), "walkFwd"))
				animTag = ChAnim_WALK_FWD_FILE;
			else if (!strcmp(animNode->name(), "walkLeft"))
				animTag = ChAnim_WALK_LEFT_FILE;
			else if (!strcmp(animNode->name(), "walkRight"))
				animTag = ChAnim_WALK_RIGHT_FILE;
			else if (!strcmp(animNode->name(), "fwdL"))
				animTag = ChAnim_FSTANCE_MOVE_FWDL_FILE;
			else if (!strcmp(animNode->name(), "fwdR"))
				animTag = ChAnim_FSTANCE_MOVE_FWDR_FILE;
			else if (!strcmp(animNode->name(), "leftFwd"))
				animTag = ChAnim_FSTANCE_MOVE_LEFTFWD_FILE;
			else if (!strcmp(animNode->name(), "rightFwd"))
				animTag = ChAnim_FSTANCE_MOVE_RIGHTFWD_FILE;
			else if (!strcmp(animNode->name(), "left"))
				animTag = ChAnim_FSTANCE_MOVE_LEFT_FILE;
			else if (!strcmp(animNode->name(), "right"))
				animTag = ChAnim_FSTANCE_MOVE_RIGHT_FILE;
			else if (!strcmp(animNode->name(), "leftBack"))
				animTag = ChAnim_FSTANCE_MOVE_LEFTBACK_FILE;
			else if (!strcmp(animNode->name(), "rightBack"))
				animTag = ChAnim_FSTANCE_MOVE_RIGHTBACK_FILE;
			else if (!strcmp(animNode->name(), "backL"))
				animTag = ChAnim_FSTANCE_MOVE_BACKL_FILE;
			else if (!strcmp(animNode->name(), "backR"))
				animTag = ChAnim_FSTANCE_MOVE_BACKR_FILE;
			else if (!strcmp(animNode->name(), "relaxedAttack1"))
				animTag = ChAnim_RELAXED_ATTACK_1_FILE;
			else if (!strcmp(animNode->name(), "relaxedAttack2"))
				animTag = ChAnim_RELAXED_ATTACK_2_FILE;
			else if (!strcmp(animNode->name(), "relaxedAttack3"))
				animTag = ChAnim_RELAXED_ATTACK_3_FILE;
			else if (!strcmp(animNode->name(), "relaxedAttack4"))
				animTag = ChAnim_RELAXED_ATTACK_4_FILE;
			else if (!strcmp(animNode->name(), "relaxedAttack5"))
				animTag = ChAnim_RELAXED_ATTACK_5_FILE;
			else if (!strcmp(animNode->name(), "enragedAttack1"))
				animTag = ChAnim_ENRAGED_ATTACK_1_FILE;
			else if (!strcmp(animNode->name(), "enragedAttack2"))
				animTag = ChAnim_ENRAGED_ATTACK_2_FILE;
			else if (!strcmp(animNode->name(), "enragedAttack3"))
				animTag = ChAnim_ENRAGED_ATTACK_3_FILE;
			else if (!strcmp(animNode->name(), "enragedAttack4"))
				animTag = ChAnim_ENRAGED_ATTACK_4_FILE;
			else if (!strcmp(animNode->name(), "enragedAttack5"))
				animTag = ChAnim_ENRAGED_ATTACK_5_FILE;
			else if (!strcmp(animNode->name(), "block"))
				animTag = ChAnim_BLOCK_FILE;
			else if (!strcmp(animNode->name(), "hitReactJab"))
				animTag = ChAnim_TAKE_HIT_1_FILE;
			else if (!strcmp(animNode->name(), "hitReactLeft"))
				animTag = ChAnim_TAKE_HIT_2_FILE;
			else if (!strcmp(animNode->name(), "hitReactRight"))
				animTag = ChAnim_TAKE_HIT_3_FILE;
			else if (!strcmp(animNode->name(), "fightStanceBreath"))
				animTag = ChAnim_BREATHE_FILE;
			else if (!strcmp(animNode->name(), "relaxedBreath"))
				animTag = ChAnim_RELAXED_BREATHE_FILE;
			else if (!strcmp(animNode->name(), "lookAround"))
				animTag = ChAnim_LOOK_AROUND_FILE;
			else if (!strcmp(animNode->name(), "jump"))
				animTag = ChAnim_JUMP_FILE;
			else if (!strcmp(animNode->name(), "midair"))
				animTag = ChAnim_MIDAIR_FILE;
			else if (!strcmp(animNode->name(), "land"))
				animTag = ChAnim_LAND_FILE;
			else if (!strcmp(animNode->name(), "die1"))
				animTag = ChAnim_DIE_1_FILE;
			else if (!strcmp(animNode->name(), "dead1"))
				animTag = ChAnim_DEAD_1_FILE;
			else animTag = -1;

			if (animTag != -1 && fileNode)
			{
				std::string fileName = fileNode->value();

				xml_node<> *minMandatory = animNode->first_node("mandatoryTime");
				xml_node<> *maxFadeInTime = animNode->first_node("maxFadeInTime");
				xml_node<> *relevantTime = animNode->first_node("relevantTime");
				xml_node<> *contactTime = animNode->first_node("contact");

				CharAnimNFO animNfo(fileName);
				if (minMandatory)
					animNfo.SetMandatoryTime(minMandatory->first_attribute("t")->value());
				
				if (maxFadeInTime)
					animNfo.SetFadeInTime(maxFadeInTime->first_attribute("t")->value());

				if (relevantTime)
					animNfo.SetRelevantTime(relevantTime->first_attribute("t")->value());

				if (contactTime)
					animNfo.SetContactTime(contactTime->first_attribute("t")->value());

				animations[animTag] = animNfo;
			}

		}

		characterAnimationFileNames[characterName] = animations;
	}

	for (xml_node<> *objectNode = objectsNode->first_node("object"); objectNode; objectNode = objectNode->next_sibling())
	{
		GameObjCreationSettings objSettings;
		std::string objName(objectNode->first_attribute("name")->value());
		objSettings.name = objName;
		
		xml_node<> *meshNode = objectNode->first_node("meshRenderer");
		if (meshNode)
			objSettings.fileName = meshNode->value();
			
		xml_node<> *skyNode = objectNode->first_node("skybox");
		if (skyNode)
		{
			objSettings.isSkybox = true;
			sscanf(skyNode->first_attribute("depthOverride")->value(), "%f", &(objSettings.depthOverride));
		}	

		xml_node<> *colliderNode = objectNode->first_node("collider");
		if (colliderNode)
			objSettings.colliderFileName = colliderNode->value();

		xml_node<> *wireframeNode = objectNode->first_node("wireframe");
		if (wireframeNode)
		{
			objSettings.wireframe = !strcmp(wireframeNode->value(), "true");
		}

		xml_node<> *dynamicNode = objectNode->first_node("isDynamic");
		if (dynamicNode)
		{
			objSettings.isDynamic = !strcmp(dynamicNode->value(), "true");
		}
		environment.push_back(objSettings);
	}
	free(file_contents);
}

void ResourceManager::LoadTextures()
{
	flatNormalMap = Texture::png_texture_load("../assets/flatNormal.png", NULL, NULL);
	defaultSpecularMap = Texture::png_texture_load("../assets/black.png", NULL, NULL);
	defaultEmissiveMap = Texture::png_texture_load("../assets/black.png", NULL, NULL);
	raysSprite = Texture::png_texture_load("../assets/punchMark.png", NULL, NULL);
}

CharacterAnimation * ResourceManager::GetCharacterAnimationTrack(std::string &name)
{
	CharacterAnimation *chAnim = mCharacterAnimationTracks[name];
	if (!chAnim)
		mCharacterAnimationTracks[name] = new CharacterAnimation(name);
	return mCharacterAnimationTracks[name];
}
