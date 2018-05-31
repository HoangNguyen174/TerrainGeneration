#ifndef CHUNK_H
#define CHUNK_H
#include "SoulStoneEngine/Utilities/NoiseGenerator.hpp"
#include "SoulStoneEngine/Utilities/Noise.hpp"
#include "SoulStoneEngine/Utilities/GameCommon.hpp"
#include "SoulStoneEngine/Utilities/Texture.hpp"
#include "SoulStoneEngine/Render/Material.hpp"
#include "SoulStoneEngine/Render/OpenGLShaderProgram.hpp"
#include "BillboardSet.hpp"

const int CHUNK_WIDTH_X = 128;
const int CHUNK_DEPTH_Y = 128;
const int CHUNK_HEIGHT_Z = 20;

struct Cube
{
	Vector3 position[8];
	float   densityValue[8];
};

enum TERRAIN_TYPE { GRASS = 0, ROCK, SNOW };

class Chunk
{
	public:
		static int					s_edgeTable[256];
		static int					s_triTable[256][16];
		ChunkCoords2D				m_minChunkCoords;
		WorldCoords3D				m_minWorldCoords;
		float						m_isoLevel;
		std::vector<Vertex3D>		m_vertexList;
		int							m_numVertex;
		unsigned int				m_vboId;
		bool						m_isVboDirty;
		Material*					m_material;
		Texture*					m_rockTexture;
		Texture*					m_grassTexture;
		Texture*					m_sandTexture;
		Texture*					m_snowTexture;
		Texture*					m_treeBillboardTexture;
		Texture*					m_grassBillboardTexture;
		BillboardSet*				m_trees;
		BillboardSet*				m_grass;
		std::vector<Vector2>		m_riverNode;
		
	public:
		Chunk( const ChunkCoords2D& chunkCoords, const float isoValue );
		Vector3 InterpolateVertex( const Vector3& p1, const Vector3& p2, const float valueAtP1, const float valueAtP2 );
		void Update( float elapsedTime );
		void Render();

	private:
		void GenerateChunk();
		void CreateVBO();
		void RenderVBO();
		float DensityFunction1( const WorldCoords3D& position );
		float DensityFunction2( const WorldCoords3D& position );
		void SetUpMaterial();
		Vector3 ComputeSurfaceNormalAt( const WorldCoords3D& position,  const WorldCoords3D& position1,  const WorldCoords3D& position2 );
		Vector3 ComputeNormalByGradient( const WorldCoords3D& position );
		Vector2 ComputeTexCoordsAt( const WorldCoords3D& position );
		void ComputeTerrainTypeWeight( const WorldCoords3D& position, float terrainWeightArray[4] );
		void GenerateTreeAndGrass();
		void RenderGrassAndTree();
		Vector3 GetRandomPointInsideTriangle( const Vector3& v1, const Vector3& v2, const Vector3& v3 );
		void GenerateRiverPath();
		void RenderRiverPath();
		void GenerateRiverGeometry();
		void ComputeVertexAttributes();
};

#endif




