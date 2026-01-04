// RenderdocCSVToFBX.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include <set>

#include <fbxsdk.h>

#include "CSVFile.h"
#include "CommonMath.h"

using namespace std;

struct MeshVertex
{
	float x, y, z;
	float nx, ny, nz;
	float tx, ty, tz, tw;
	float u, v;
	float u2, v2;
	float u3, v3;
	float r,g,b,a;
};

void ConvertCSV2FBX(const char* sourceCSVFile, 
	bool export_normal, bool export_tangent, 
	bool export_uv, bool export_uv2, bool export_uv3,bool export_color)
{
	FbxManager* sdkManager;
	FbxIOSettings* ioSettings;
	FbxScene* scene;

	sdkManager = FbxManager::Create();

	ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);
	ioSettings->SetBoolProp(EXP_FBX_MATERIAL, true);
	ioSettings->SetBoolProp(EXP_FBX_TEXTURE, true);
	ioSettings->SetBoolProp(EXP_FBX_EMBEDDED, true);
	ioSettings->SetBoolProp(EXP_FBX_ANIMATION, true);
	ioSettings->SetBoolProp(EXP_FBX_SHAPE, true);
	ioSettings->SetIntProp(
		EXP_FBX_EXPORT_FILE_VERSION, FBX_FILE_VERSION_7400);

	sdkManager->SetIOSettings(ioSettings);
	scene = FbxScene::Create(sdkManager, "");

	CCSVFile* pSrcFile = new CCSVFile(sourceCSVFile);

	std::map<int, MeshVertex> verticsMap;
	int iVertexID = 0;
	for (int iRow = 0; iRow < pSrcFile->GetRowNum(); iRow++)
	{
		pSrcFile->GetCellValue("IDX", iRow, iVertexID);

		if (verticsMap.find(iVertexID) == verticsMap.end())
		{
			auto& v = verticsMap[iVertexID];
			pSrcFile->GetSemanticValue("POSITION", 0, 'x', iRow, v.x);
			pSrcFile->GetSemanticValue("POSITION", 0, 'y', iRow, v.y);
			pSrcFile->GetSemanticValue("POSITION", 0, 'z', iRow, v.z);

			if(export_normal)
			{
				pSrcFile->GetSemanticValue("NORMAL", 0, 'x', iRow, v.nx);
				pSrcFile->GetSemanticValue("NORMAL", 0, 'y', iRow, v.ny);
				pSrcFile->GetSemanticValue("NORMAL", 0, 'z', iRow, v.nz);
			}
			if(export_tangent)
			{
				pSrcFile->GetSemanticValue("TANGENT", 0, 'x', iRow, v.tx);
				pSrcFile->GetSemanticValue("TANGENT", 0, 'y', iRow, v.ty);
				pSrcFile->GetSemanticValue("TANGENT", 0, 'z', iRow, v.tz);
				pSrcFile->GetSemanticValue("TANGENT", 0, 'w', iRow, v.tw);
			}

			if(export_uv)
			{
				pSrcFile->GetSemanticValue("TEXCOORD", 0, 'x', iRow, v.u);
				pSrcFile->GetSemanticValue("TEXCOORD", 0, 'y', iRow, v.v);
			}
			if(export_uv2)
			{
				pSrcFile->GetSemanticValue("TEXCOORD", 1, 'x', iRow, v.u2);
				pSrcFile->GetSemanticValue("TEXCOORD", 1, 'y', iRow, v.v2);
			}
			if(export_uv3)
			{
				pSrcFile->GetSemanticValue("TEXCOORD", 2, 'x', iRow, v.u3);
				pSrcFile->GetSemanticValue("TEXCOORD", 2, 'y', iRow, v.v3);
			}
			if (export_color)
			{
				pSrcFile->GetSemanticValue("COLOR", 0, 'x', iRow, v.r);
				pSrcFile->GetSemanticValue("COLOR", 0, 'y', iRow, v.g);
				pSrcFile->GetSemanticValue("COLOR", 0, 'z', iRow, v.b);
				pSrcFile->GetSemanticValue("COLOR", 0, 'w', iRow, v.a);
			}
		}
	}

	int iTotalVerticsCount = verticsMap.size();

	// 用csv的文件名作为mesh的名字
	std::string meshName = sourceCSVFile;
	replace(meshName.begin(), meshName.end(), '/', '\\');
	meshName = meshName.substr(0, meshName.size() - 4);
	int pos = meshName.find_last_of('\\');
	if (pos != std::string::npos)
	{
		meshName = meshName.substr(pos + 1, meshName.size() - pos - 1);
	}

	FbxNode* rootNode = scene->GetRootNode();
	FbxNode* subshapeNode = FbxNode::Create(rootNode, meshName.c_str());

	// convert fbx mesh file
	FbxMesh* meshFbx = FbxMesh::Create(subshapeNode, subshapeNode->GetName());

	FbxGeometryElementNormal* meshNormal = NULL;
	FbxGeometryElementTangent* meshTangent = NULL;
	FbxGeometryElementUV* meshUV = NULL;
	FbxGeometryElementUV* meshUV2 = NULL;
	FbxGeometryElementUV* meshUV3 = NULL;
	FbxGeometryElementVertexColor* vertexColor = NULL;

	FbxLayer* layer = meshFbx->GetLayer(0);
	if (!layer)
	{
		meshFbx->CreateLayer();
		layer = meshFbx->GetLayer(0);
	}

	if(export_normal)
	{
		meshNormal = meshFbx->CreateElementNormal();
	}
	if(export_tangent)
	{
		meshTangent = meshFbx->CreateElementTangent();
	}

	if(export_uv)
	{
		meshUV = meshFbx->CreateElementUV("UV");
	}
	if(export_uv2)
	{
		meshUV2 = meshFbx->CreateElementUV("UV1");
	}
	if(export_uv3)
	{
		meshUV3 = meshFbx->CreateElementUV("UV2");
	}

	if (export_color)
	{
		vertexColor = meshFbx->CreateElementVertexColor();
	}

	meshFbx->InitControlPoints(iTotalVerticsCount);

	if(export_normal)
	{
		meshNormal->SetMappingMode(FbxGeometryElementNormal::eByControlPoint);
		meshNormal->SetReferenceMode(FbxGeometryElementNormal::eDirect);
		layer->SetNormals(meshNormal);
	}
	if(export_tangent)
	{
		meshTangent->SetMappingMode(FbxGeometryElementTangent::eByControlPoint);
		meshTangent->SetReferenceMode(FbxGeometryElementTangent::eDirect);
		layer->SetTangents(meshTangent);
	}

	if(export_uv)
	{
		meshUV->SetMappingMode(FbxGeometryElementUV::eByControlPoint);
		meshUV->SetReferenceMode(FbxGeometryElementUV::eDirect);
		layer->SetUVs(meshUV);
	}
	if(export_uv2)
	{
		meshUV2->SetMappingMode(FbxGeometryElementUV::eByControlPoint);
		meshUV2->SetReferenceMode(FbxGeometryElementUV::eDirect);
	}
	if(export_uv3)
	{
		meshUV3->SetMappingMode(FbxGeometryElementUV::eByControlPoint);
		meshUV3->SetReferenceMode(FbxGeometryElementUV::eDirect);
	}

	if (export_color)
	{
		vertexColor->SetMappingMode(FbxGeometryElementVertexColor::eByControlPoint);
		vertexColor->SetReferenceMode(FbxGeometryElementVertexColor::eDirect);
		layer->SetVertexColors(vertexColor);
	}

	FbxVector4* meshVectors = meshFbx->GetControlPoints();
	Matrix44 matRot;
	MatrixRotationZ(&matRot, -FLT_DTOR(0));

	for (int index = 0; index < verticsMap.size(); index++)
	{
		Vector3 Vertex(
			verticsMap[index].x * 100, verticsMap[index].y * 100, verticsMap[index].z * 100);
		Vec3TransformCoord(&Vertex, &Vertex, &matRot);
		meshVectors[index].Set(Vertex.x, Vertex.y, Vertex.z);

		if(export_normal)
		{
			Vector3 VertexNormal(
				verticsMap[index].nx, verticsMap[index].ny, verticsMap[index].nz);
			Vec3TransformNormal(&VertexNormal, &VertexNormal, &matRot);

			meshNormal->GetDirectArray().Add(
				FbxVector4(VertexNormal.x, VertexNormal.y, VertexNormal.z, 0));
		}
		if(export_tangent)
		{
			Vector4 VertexTangent(
				verticsMap[index].tx, verticsMap[index].ty, verticsMap[index].tz, verticsMap[index].tw);
			VertexTangent = VertexTangent * matRot;

			meshTangent->GetDirectArray().Add(
				FbxVector4(VertexTangent.x, VertexTangent.y, VertexTangent.z, VertexTangent.w));
		}
		
		if(export_uv)
		{
			meshUV->GetDirectArray().Add(
				FbxVector2(verticsMap[index].u, verticsMap[index].v));
		}
		if(export_uv2)
		{
			meshUV2->GetDirectArray().Add(
				FbxVector2(verticsMap[index].u2, verticsMap[index].v2));
		}
		if(export_uv3)
		{
			meshUV3->GetDirectArray().Add(
				FbxVector2(verticsMap[index].u3, verticsMap[index].v3));
		}
		if (export_color)
		{
			vertexColor->GetDirectArray().Add(
				FbxVector4(verticsMap[index].r, verticsMap[index].g, verticsMap[index].b, verticsMap[index].a)
			);
		}
	}

	int iFaceID = 0;
	for (int iRow = 0; iRow < pSrcFile->GetRowNum(); iRow += 3)
	{
		meshFbx->BeginPolygon(0);

		pSrcFile->GetCellValue("IDX", iRow, iFaceID);
		meshFbx->AddPolygon(iFaceID);
		pSrcFile->GetCellValue("IDX", iRow + 1, iFaceID);
		meshFbx->AddPolygon(iFaceID);
		pSrcFile->GetCellValue("IDX", iRow + 2, iFaceID);
		meshFbx->AddPolygon(iFaceID);

		meshFbx->EndPolygon();
	}

	subshapeNode->SetNodeAttribute(meshFbx);
	subshapeNode->SetShadingMode(FbxNode::eTextureShading);

	// save fbx
	std::string fbxFilePath = sourceCSVFile;
	size_t dotPos = fbxFilePath.find_last_of('.');
	if (dotPos != std::string::npos)
	{
		fbxFilePath = fbxFilePath.substr(0, dotPos);
	}
	fbxFilePath += ".fbx";

	size_t start_pos = fbxFilePath.find_last_of('\\');
	std::string fileName = fbxFilePath.substr(start_pos + 1);

	FbxExporter* exporter = FbxExporter::Create(sdkManager, fileName.c_str());
	if (!exporter->Initialize(fbxFilePath.c_str(), -1, ioSettings))
	{
		fprintf(stderr, "Failed to initialize FBX exporter\n");
		exporter->Destroy();
		return;
	}
	exporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

	if (!exporter->Export(scene))
	{
		fprintf(stderr, "Failed to produce FBX file\n");
		exporter->Destroy();
		return;
	}

	exporter->Destroy();
}

/////////////////////////////////////////////////////////////// Main /////////////////////////////////////////////

// ----------------------------------
// C++14: 判断文件是否存在
// ----------------------------------
bool FileExists(const char* path)
{
	FILE* f = nullptr;
	if (fopen_s(&f, path, "rb") == 0 && f)
	{
		fclose(f);
		return true;
	}
	return false;
}

// ----------------------------------
// 判断是否是 .csv 文件
// ----------------------------------
bool HasCsvExtension(const char* path)
{
	const char* dot = strrchr(path, '.');
	if (!dot)
		return false;

	return strcmp(dot, ".csv") == 0;
}

int main(int argc, char* argv[])
{
	printf("argc = %d\n", argc-1);

	if (argc < 2)
	{
		printf("Error: must input cvs path.\n");
		return 1;
	}

	const char* inputPath = argv[1];

	if (!FileExists(inputPath))
	{
		printf("Error: input file not found: %s\n", inputPath);
		return 1;
	}

	if (!HasCsvExtension(inputPath))
	{
		printf("Error: input file must be a .csv file: %s\n", inputPath);
		return 1;
	}

	bool export_normal = false;
	bool export_tangent = false;
	bool export_uv = false;
	bool export_uv2 = false;
	bool export_uv3 = false;
	bool export_color = false;

	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];

		if (strcmp(arg, "-normal") == 0)
			export_normal = true;
		else if (strcmp(arg, "-tangent") == 0)
			export_tangent = true;
		else if (strcmp(arg, "-uv") == 0)
			export_uv = true;
		else if (strcmp(arg, "-uv2") == 0)
			export_uv2 = true;
		else if (strcmp(arg, "-uv3") == 0)
			export_uv3 = true;
		else if (strcmp(arg, "-color") == 0)
			export_color = true;
	}

	printf("export_normal  = %d\n", export_normal);
	printf("export_tangent = %d\n", export_tangent);
	printf("export_uv      = %d\n", export_uv);
	printf("export_uv2     = %d\n", export_uv2);
	printf("export_uv3     = %d\n", export_uv3);
	printf("export_color   = %d\n", export_color);

	ConvertCSV2FBX(inputPath,
		export_normal, export_tangent, 
		export_uv, export_uv2, export_uv3, export_color);
}
