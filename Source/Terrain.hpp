#ifndef TERRAIN_H
#define TERRAIN_H
#include "SoulStoneEngine/Utilities/GameCommon.hpp"
#include "SoulStoneEngine/Utilities/Clock.hpp"
#include "SoulStoneEngine/ModelSystem/Scene.hpp"
#include "Chunk.hpp"
#include "BillboardSet.hpp"

const int INNER_RADIUS_IN_CHUNK = 1;
const int OUTTER_RADIUS_IN_CHUNK = 1;
const int INNER_RADIUS_IN_PIXEL = INNER_RADIUS_IN_CHUNK * CHUNK_WIDTH_X;
const int OUTTER_RADIUS_IN_PIXEL = OUTTER_RADIUS_IN_CHUNK * CHUNK_WIDTH_X;
const int DEFAULT_MAP_WIDTH_X = 0;
const int DEFAULT_MAP_DEPTH_Y = 0;
const float WATER_LEVEL = 3.0f;
const float SKY_HEIGHT = 14.0f;
const bool RENDER_ONE_CHUNK = true;

class Terrain
{
	public:
		static OpenGLShaderProgram*			s_terrainShaderProgram;
		static OpenGLShaderProgram*			s_waterShaderProgram;
		static OpenGLShaderProgram*			s_defaultShaderProgram;
		static OpenGLShaderProgram*			s_3dModelShaderProgram;
		static OpenGLShaderProgram*			s_billboardShaderProgram;
		static OpenGLShaderProgram*			s_grassShaderProgram;
		static Matrix44						s_reflectCameraTransformation;
		static Vector4						s_clipPlane;
		static bool							s_isRenderingReflection;
		std::string						    m_name;
		float							    m_isoValue;
		WorldCoords3D					    m_cameraWorldCoords;
		set< ChunkCoords2D >			    m_setOfChunksToGenerate;
		set< ChunkCoords2D >			    m_setOfChunksToDelete;
		FBO*								m_waterRefractionFBO;
		FBO*								m_waterReflectionFBO;
		unsigned int						m_waterReflectionTextureID;
		unsigned int						m_waterRefractionTextureID;
		Material*							m_waterMaterial;
		Material*							m_skyboxMaterial;
		unsigned int						m_waterVboId;
		Texture*							m_waterNormalTexture;
		Texture*							m_waterDUDVTexture;
		Texture*							m_skyBoxTexture;
		Scene*								m_testModel;
		float								m_currentTime;

	public:
		Terrain( const std::string& name, float isoValue );
		~Terrain();
		void GenerateToDoList();
		void GenerateChunkFromToDo();
		void DeleteChunkFromToDo();
		void Render();
		void Update( float elapsedTime );
		void RenderSkyBox( bool isReflrected );
		void RenderTopSkyBoxOnWaterSurface();

	private:
		ChunkCoords2D ConvertWorldCoords3Dto2D( const WorldCoords3D& pos );
		float CalcDistanceSquareFromCameraToChunk( const ChunkCoords2D& pos );
		WorldCoords3D CalcChunkCenterFromChunkCoords( const ChunkCoords2D& pos );
		void CreateWaterRefractionTexture();
		void CreateWaterReflectionTexture();
		void RenderWaterSurface();
		void SetUpWaterMaterial();
};

extern map< ChunkCoords2D, Chunk* > g_mapChunk;

#endif