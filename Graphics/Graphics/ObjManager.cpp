#include "PCH.h"
#include "ObjManager.h"
#include "Entity.h"
#include "VertexPosColor.h"
#include "ObjEntity.h"

#include <iostream>
#include <fstream>

void CObjManager::SaveToObj(std::string _filePath, CEntity* _entity)
{
    std::ofstream file;
    file.open(_filePath);

    auto vertices = _entity->GetVertices();
    auto indices = *_entity->GetIndices();
    // g
    file << "g" << std::endl;
    file << std::endl;

    // v x y z
    for (SVertexPosColor _vertex : *vertices)
    {
        file << "v " << _vertex.m_Position.x << " " << _vertex.m_Position.y << " " << _vertex.m_Position.z << std::endl;
    }
    file << "# vertices " << vertices->size() << std::endl;
    file << std::endl;
    
    // vn x y z
    for (SVertexPosColor _vertex : *vertices)
    {
        file << "vn " << _vertex.m_Normal.x << " " << _vertex.m_Normal.y << " " << _vertex.m_Normal.z << std::endl;
    }
    file << "# normal " << vertices->size() << std::endl;
    file << std::endl;

    // vt u v
    for (SVertexPosColor _vertex : *vertices)
    {
        file << "vt " << _vertex.m_UV.x << " " << _vertex.m_UV.y << std::endl;
    }
    file << "# uvs " << vertices->size() << std::endl;
    file << std::endl;

    // f 1 2 3
    for (int i = 0; i < indices.size(); i+=3)
    {
        file << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << std::endl;
    }
    file << "# faces " << indices.size() / 3 << std::endl;
}

CEntity* CObjManager::LoadObj(std::string _filePath)
{
    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT3> normals;
    std::vector<XMFLOAT2> uvs;
    std::map<std::string, int> indicesPerVertex;

    std::vector<WORD> indices;
    std::vector<SVertexPosColor> vertices;

    std::ifstream fileStream(_filePath);

    std::string line;

    while (std::getline(fileStream, line))
    {
        std::vector<std::string> split = SplitString(line, ' ');

        if (split.size() == 0)
            continue;
        if (split[0] == "#")
            continue;
        if (split[0] == "g")
            continue;

        if (split[0] == "v")
        {
            if (split.size() == 4)
            {
                positions.push_back(XMFLOAT3( std::stof(split[1]), std::stof(split[2]), std::stof(split[3])));
            }
        }
        if (split[0] == "vn")
        {
            if (split.size() == 4)
            {
                normals.push_back(XMFLOAT3(std::stof(split[1]), std::stof(split[2]), std::stof(split[3])));
            }
        }
        if (split[0] == "vt")
        {
            if (split.size() > 2)
            {
                uvs.push_back(XMFLOAT2(std::stof(split[1]), std::stof(split[2])));
            }
        }
        if (split[0] == "f")
        {
            for (int i = 1; i < split.size() - 2; i++)
            {
                ParseFaceString(split[1], &positions, &normals, &uvs, &indices, &vertices, &indicesPerVertex);
                ParseFaceString(split[i + 1], &positions, &normals, &uvs, &indices, &vertices, &indicesPerVertex);
                ParseFaceString(split[i + 2], &positions, &normals, &uvs, &indices, &vertices, &indicesPerVertex);
            }
        }
    }

    return new ObjEntity(&vertices, &indices, L"Textures/T_Walrus_A.jpg", EResourceType::SQUIRREL);
}

void CObjManager::ParseFaceString(std::string _input, 
    std::vector<XMFLOAT3>* _inPositions, std::vector<XMFLOAT3>* _inNormals,  std::vector<XMFLOAT2>* _inUVs,
    std::vector<WORD>* _inOutIndices, std::vector<SVertexPosColor>* _inOutVertices,
    std::map<std::string, int>* _inOutIndexPerVertex)
{
    if (_inOutIndexPerVertex->find(_input) != _inOutIndexPerVertex->end())
    {
        _inOutIndices->push_back(_inOutIndexPerVertex->at(_input));
        return;
    }

    SVertexPosColor vertex = {};
    std::vector<std::string> split = SplitString(_input, '/', true);

    if (split.size() == 1)
    {
        int value = std::stoi(split[0]);
        if (value < 0)
        {
            value = _inPositions->size() + value;
        }
        else
        {
            value -= 1;
        }

        vertex.m_Position = (*_inPositions)[value];
        if (_inNormals->size() > value)
        {
            vertex.m_Normal = (*_inNormals)[value];
        }
        if (_inUVs->size() > value)
        {
            vertex.m_UV = (*_inUVs)[value];
        }
    }
    else
    {
        // 1/2/3 = 1 2 3
        // 1//3 =  1   3
        // 1/2  =  1 2  
        // 1//
        int value = std::stoi(split[0]);
        if (value < 0)
        {
            value = _inPositions->size() + value;
        }
        else
        {
            value -= 1;
        }
        vertex.m_Position = (*_inPositions)[value];

        if (!split[1].empty())
        {
            value = std::stoi(split[1]);
            if (value < 0)
            {
                value = _inUVs->size() + value;
            }
            else
            {
                value -= 1;
            }
            vertex.m_UV = (*_inUVs)[value];
        }

        if (split.size() == 3)
        {
            value = std::stoi(split[2]);
            if (value < 0)
            {
                value = _inNormals->size() + value;
            }
            else
            {
                value -= 1;
            }
            vertex.m_Normal = (*_inNormals)[value];
        }
    }

    int index = _inOutVertices->size();
    _inOutVertices->push_back(vertex);
    _inOutIndices->push_back(index);
    _inOutIndexPerVertex->emplace(_input, index);
}

std::vector<std::string> CObjManager::SplitString(std::string _input, char _seperator, bool _includeEmpty)
{
    std::vector<std::string> returnValue;
    std::string word;

    for (char c : _input)
    {
        if (c == _seperator)
        {
            if (!word.empty() || _includeEmpty)
            {
                returnValue.push_back(word);
                word = "";
            }
        }
        else
        {
            word += c;
        }
    }
    if (!word.empty())
    {
        returnValue.push_back(word);
    }

    return returnValue;
}
