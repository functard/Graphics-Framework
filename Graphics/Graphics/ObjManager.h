#pragma once

#include <map>
#include <string>
#include <vector>

struct SVertexPosColor;
class CEntity;

class CObjManager
{
public:
	static void SaveToObj(std::string _filePath, CEntity* _entity);
	static CEntity* LoadObj(std::string _filePath);

private:
	static std::vector<std::string> SplitString(std::string _input, char _seperator, bool _includeEmpty = false);

	static void ParseFaceString(std::string _input,
		std::vector<XMFLOAT3>* _inPositions, std::vector<XMFLOAT3>* _inNormals, std::vector<XMFLOAT2>* _inUVs,
		std::vector<WORD>* _inOutIndices, std::vector<SVertexPosColor>* _inOutVertices,
		std::map<std::string, int>* _inOutIndexPerVertex);
};

