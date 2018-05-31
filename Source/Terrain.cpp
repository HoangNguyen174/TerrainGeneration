#include "Terrain.hpp"
#include "World.hpp"
#include "SoulStoneEngine/JobSystem/MemoryPoolManager.hpp"

map< ChunkCoords2D, Chunk* >	g_mapChunk;
OpenGLShaderProgram*		    Terrain::s_terrainShaderProgram = nullptr;
OpenGLShaderProgram*		    Terrain::s_waterShaderProgram = nullptr;
OpenGLShaderProgram*			Terrain::s_defaultShaderProgram = nullptr;
OpenGLShaderProgram*			Terrain::s_3dModelShaderProgram = nullptr;
OpenGLShaderProgram*			Terrain::s_billboardShaderProgram = nullptr;
OpenGLShaderProgram*			Terrain::s_grassShaderProgram = nullptr;

Matrix44						Terrain::s_reflectCameraTransformation;
bool							Terrain::s_isRenderingReflection = false;
Vector4							Terrain::s_clipPlane;

Terrain::Terrain( const std::string& name, float isoValue )
{
	m_name = name;
	m_isoValue = isoValue;
	m_waterReflectionTextureID = 0;
	m_waterRefractionTextureID = 0;
	m_waterVboId = 0;
	Chunk* chunk = nullptr;

	s_terrainShaderProgram =  new OpenGLShaderProgram( "Terrain Shader", "./Data/Shader/TerrainVertexShader.glsl", "./Data/Shader/TerrainFragmentShader.glsl" );
	s_waterShaderProgram = new OpenGLShaderProgram( "Water Shader", "./Data/Shader/WaterVertexShader.glsl", "./Data/Shader/WaterFragmentShader.glsl" );
	s_defaultShaderProgram = new OpenGLShaderProgram( "Default Shader", "./Data/Shader/ModernVertexShader.glsl", "./Data/Shader/ModernFragmentShader.glsl" );
	s_3dModelShaderProgram =  new OpenGLShaderProgram( "3D Model Shader", "./Data/Shader/3DModelVertexShader.glsl", "./Data/Shader/ModernFragmentShader.glsl" );
	s_billboardShaderProgram = new OpenGLShaderProgram( "Billboard Shader", "./Data/Shader/BillboardVertexShader.glsl", "./Data/Shader/BillboardFragmentShader.glsl", "./Data/Shader/BillboardGeometryShader.glsl" );
	s_grassShaderProgram = new OpenGLShaderProgram( "Grass Shader", "./Data/Shader/BillboardVertexShader.glsl", "./Data/Shader/BillboardFragmentShader.glsl", "./Data/Shader/GrassGeometryShader.glsl" );

	m_waterNormalTexture= Texture::CreateOrGetTexture( "./Data/Texture/normalmap.bmp" );
	m_skyBoxTexture = Texture::CreateOrGetTexture( "./Data/Texture/skybox.png" );

	for(int yCoord2D = 0; yCoord2D < DEFAULT_MAP_DEPTH_Y; yCoord2D++)
	{
		for(int xCoord2D = 0; xCoord2D < DEFAULT_MAP_WIDTH_X; xCoord2D++)
		{
			ChunkCoords2D chunkCoords( xCoord2D, yCoord2D );
			chunk = new Chunk( chunkCoords, isoValue );

			g_mapChunk[ chunkCoords ] = chunk;
		}
	}

	m_waterRefractionFBO =  FBO::CreateOrGetFBOByName( "Water Refraction FBO" );
	m_waterReflectionFBO = FBO::CreateOrGetFBOByName( "Water Reflection FBO" );
	m_waterMaterial = new Material( "WaterSurface", s_waterShaderProgram );
	m_skyboxMaterial = new Material( "Skybox", s_defaultShaderProgram );
	m_testModel = new Scene( "./Data/Scene/s11.MY3D", s_3dModelShaderProgram );
	m_currentTime = 0.f;
}

Terrain::~Terrain()
{
	g_mapChunk.clear();
}

void Terrain::GenerateToDoList()
{
	ChunkCoords2D cameraChunkCoords = ConvertWorldCoords3Dto2D( m_cameraWorldCoords );

	ChunkCoords2D minChunkCoordsOfCheckArea;
	ChunkCoords2D temp;
	float distSquare;

	if( g_mapChunk.find( temp ) == g_mapChunk.end() )
	{
		m_setOfChunksToGenerate.insert( temp );
	}
	return;

	minChunkCoordsOfCheckArea.x = cameraChunkCoords.x - INNER_RADIUS_IN_CHUNK;
	minChunkCoordsOfCheckArea.y = cameraChunkCoords.y - INNER_RADIUS_IN_CHUNK;

	for( int yOffset = 0; yOffset < ( 2 * INNER_RADIUS_IN_CHUNK + 1 ); yOffset++ )
	{
		for( int xOffset = 0; xOffset < ( 2 * INNER_RADIUS_IN_CHUNK + 1 ); xOffset++ )
		{
			temp.x = minChunkCoordsOfCheckArea.x + xOffset;
			temp.y = minChunkCoordsOfCheckArea.y + yOffset;

			distSquare = CalcDistanceSquareFromCameraToChunk( temp ) ;

			if( distSquare <= INNER_RADIUS_IN_PIXEL * INNER_RADIUS_IN_PIXEL )
			{
				if( g_mapChunk.find( temp ) == g_mapChunk.end() )
				{
					m_setOfChunksToGenerate.insert( temp );
				}
			}
		}
	}

	for( map< ChunkCoords2D, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it )
	{
		distSquare = CalcDistanceSquareFromCameraToChunk(it->first);

		if( distSquare >= OUTTER_RADIUS_IN_PIXEL * OUTTER_RADIUS_IN_PIXEL )
		{
			m_setOfChunksToDelete.insert(it->first);
		}
	}
}

void Terrain::DeleteChunkFromToDo()
{
	float furthestChunkDistanceSquared = -VERY_FAR;
	ChunkCoords2D furthestChunkCoords;
	set< ChunkCoords2D >::iterator iter;

	for( iter = m_setOfChunksToDelete.begin(); iter != m_setOfChunksToDelete.end(); ++iter )
	{
		const ChunkCoords2D& chunkCoords = *iter;
		float distSquaredToChunk = CalcDistanceSquareFromCameraToChunk(chunkCoords);
		if(distSquaredToChunk > furthestChunkDistanceSquared)
		{
			furthestChunkDistanceSquared = distSquaredToChunk;
			furthestChunkCoords = chunkCoords;
		}
	}

	Chunk* chunkToDelete = g_mapChunk[furthestChunkCoords];
	delete chunkToDelete;

	g_mapChunk.erase(furthestChunkCoords);

	m_setOfChunksToDelete.erase(furthestChunkCoords);
}

void Terrain::GenerateChunkFromToDo()
{
	float closestChunkDistanceSquared = VERY_FAR;
	ChunkCoords2D closetChunkCoords;
	set< ChunkCoords2D >::iterator iter;

	for( iter = m_setOfChunksToGenerate.begin(); iter != m_setOfChunksToGenerate.end(); ++iter )
	{
		const ChunkCoords2D& chunkCoords = *iter;
		float distSquaredToChunk = CalcDistanceSquareFromCameraToChunk(chunkCoords);
		if(distSquaredToChunk < closestChunkDistanceSquared)
		{
			closestChunkDistanceSquared = distSquaredToChunk;
			closetChunkCoords = chunkCoords;
		}
	}

	Chunk* newChunk = new Chunk( closetChunkCoords, m_isoValue );
	g_mapChunk[closetChunkCoords] = newChunk;

	m_setOfChunksToGenerate.erase( closetChunkCoords );
}

void Terrain::Update( float elapsedTime )
{
	GenerateToDoList();

	if( m_setOfChunksToGenerate.size() != 0 )
		GenerateChunkFromToDo();

	if( m_setOfChunksToDelete.size() != 0 )
		DeleteChunkFromToDo();

	std::map<TileCoords2D, Chunk*>::iterator mapIter;

	for( mapIter = g_mapChunk.begin(); mapIter != g_mapChunk.end(); ++mapIter )
	{
		Chunk* chunk = mapIter->second;

		chunk->Update( elapsedTime );
	}

	m_currentTime += elapsedTime;
}

void Terrain::Render()
{
 	if( m_cameraWorldCoords.z >= WATER_LEVEL )
 	{
 		glEnable(GL_CLIP_DISTANCE0);
		s_clipPlane = Vector4( 0.f,0.f, 1.f, -WATER_LEVEL );
		for(map< Vector2i, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it)
		{
			Chunk* chunk = it->second;
			chunk->Render();
		}
		glDisable(GL_CLIP_DISTANCE0);
 	}
	else
	{
		glEnable(GL_CLIP_DISTANCE0);
		s_clipPlane = Vector4( 0.f,0.f, -1.f, WATER_LEVEL );
		for(map< Vector2i, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it)
		{
			Chunk* chunk = it->second;
			chunk->Render();
		}
		glDisable(GL_CLIP_DISTANCE0);
	}

	RenderWaterSurface();
}

ChunkCoords2D Terrain::ConvertWorldCoords3Dto2D( const WorldCoords3D& pos )
{
	ChunkCoords2D chunkCoords;
	if( pos.x > 0 )
		chunkCoords.x = static_cast<int>( pos.x / CHUNK_WIDTH_X );
	else								  
		chunkCoords.x = static_cast<int>( pos.x / CHUNK_WIDTH_X ) - 1;
										  
	if( pos.y > 0)						  
		chunkCoords.y = static_cast<int>( pos.y / CHUNK_DEPTH_Y );
	else								  
		chunkCoords.y = static_cast<int>( pos.y / CHUNK_DEPTH_Y ) - 1;

	return chunkCoords;
}

float Terrain::CalcDistanceSquareFromCameraToChunk( const ChunkCoords2D& pos )
{
	Vector3 temp;

	temp = CalcChunkCenterFromChunkCoords( pos );

	return ( ( m_cameraWorldCoords.x - temp.x ) * ( m_cameraWorldCoords.x - temp.x )
		   + ( m_cameraWorldCoords.y - temp.y ) * ( m_cameraWorldCoords.y - temp.y)
			);
}

WorldCoords3D Terrain::CalcChunkCenterFromChunkCoords( const ChunkCoords2D& pos )
{
	WorldCoords3D chunkCenter; 

	chunkCenter.x = pos.x * CHUNK_WIDTH_X + (float)CHUNK_WIDTH_X * .5f;
	chunkCenter.y = pos.y * CHUNK_DEPTH_Y + (float)CHUNK_DEPTH_Y * .5f;
	chunkCenter.z = (float)CHUNK_HEIGHT_Z * .5f;

	return chunkCenter;
}

void Terrain::CreateWaterRefractionTexture()
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_waterRefractionFBO->m_fboID );
	GraphicManager::s_render->Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if( m_cameraWorldCoords.z > WATER_LEVEL )
	{
		glEnable(GL_CLIP_DISTANCE0);
		s_clipPlane = Vector4( 0.f,0.f, -1.f, WATER_LEVEL );
		for(map< Vector2i, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it)
		{
			Chunk* chunk = it->second;
			chunk->Render();
		}
		glDisable(GL_CLIP_DISTANCE0);
	}
	else
	{
		glEnable(GL_CLIP_DISTANCE0);
		s_clipPlane = Vector4( 0.f,0.f, 1.f, -WATER_LEVEL );
		for(map< Vector2i, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it)
		{
			Chunk* chunk = it->second;
			chunk->Render();
		}
		glDisable(GL_CLIP_DISTANCE0);
 	}


	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Terrain::CreateWaterReflectionTexture()
{
	s_isRenderingReflection = true;
	Matrix44 identityMatrix;

	glBindFramebuffer( GL_FRAMEBUFFER, m_waterReflectionFBO->m_fboID );
	GraphicManager::s_render->Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderTopSkyBoxOnWaterSurface();

	if( m_cameraWorldCoords.z > WATER_LEVEL )
	{
		glEnable(GL_CLIP_DISTANCE0);
		s_clipPlane = Vector4( 0.f,0.f, 1.f, -WATER_LEVEL );
		for(map< Vector2i, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it)
		{
			Chunk* chunk = it->second;
			chunk->Render();
		}
		glDisable(GL_CLIP_DISTANCE0);
	}
	else
	{
		glEnable(GL_CLIP_DISTANCE0);
		s_clipPlane = Vector4( 0.f, 0.f, -1.f, WATER_LEVEL );
		for(map< Vector2i, Chunk* >::iterator it = g_mapChunk.begin(); it != g_mapChunk.end();++it)
		{
			Chunk* chunk = it->second;
			chunk->Render();
		}
		glDisable(GL_CLIP_DISTANCE0);
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	s_isRenderingReflection = false;
}

void Terrain::RenderWaterSurface()
{
	CreateWaterRefractionTexture();
	CreateWaterReflectionTexture();
		
	glBindFramebuffer( GL_FRAMEBUFFER, World::s_fbo->m_fboID );

	std::vector<Vertex3D> vertexList;

	Vertex3D vertex;
	Vector3 center = World::s_camera3D->m_cameraPosition;

	vertex.m_color = RGBColor( 1.f,1.f,1.f, 1.f );

	vertex.m_position = Vector3( center.x - INNER_RADIUS_IN_PIXEL * 2, center.y - INNER_RADIUS_IN_PIXEL * 2, WATER_LEVEL );
	vertex.m_texCoords = Vector2( 0.f, 1.f );						
	vertexList.push_back( vertex );									

	vertex.m_position = Vector3( center.x + INNER_RADIUS_IN_PIXEL * 2, center.y - INNER_RADIUS_IN_PIXEL * 2, WATER_LEVEL );
	vertex.m_texCoords = Vector2( 1.f, 1.f );						
	vertexList.push_back( vertex );									
																	
	vertex.m_position = Vector3( center.x + INNER_RADIUS_IN_PIXEL * 2, center.y + INNER_RADIUS_IN_PIXEL * 2, WATER_LEVEL );
	vertex.m_texCoords = Vector2( 1.f, 0.f );						
	vertexList.push_back( vertex );									
																	
	vertex.m_position = Vector3( center.x - INNER_RADIUS_IN_PIXEL * 2, center.y + INNER_RADIUS_IN_PIXEL * 2, WATER_LEVEL );
	vertex.m_texCoords = Vector2( 0.f, 0.f );
	vertexList.push_back( vertex );

	if( m_waterVboId == 0 )
	{
		GraphicManager::s_render->GenerateBuffer( 1, &m_waterVboId );
	}

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, m_waterVboId );
	GraphicManager::s_render->BufferData( GL_ARRAY_BUFFER, sizeof( Vertex3D ) * vertexList.size(), vertexList.data(), GL_STATIC_DRAW );

	m_waterMaterial->ActivateMaterial();

	SetUpWaterMaterial();

	GraphicManager::s_render->Enable( GL_DEPTH_TEST );
	GraphicManager::s_render->Enable(GL_CULL_FACE);

 	if( World::s_camera3D->m_cameraPosition.z > WATER_LEVEL )
 		glFrontFace( GL_CCW );
 	else
 		glFrontFace( GL_CW );

	glEnableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_COLORS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_TEXCOORDS );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, m_waterVboId );

	glVertexAttribPointer( VERTEX_ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_position) );
	glVertexAttribPointer( VERTEX_ATTRIB_COLORS, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_color) );
	glVertexAttribPointer( VERTEX_ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_texCoords) );

	GraphicManager::s_render->DrawArray( GL_POLYGON ,0, 4 );

	glDisableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_COLORS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_TEXCOORDS );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, 0 );

	m_waterMaterial->DisableMaterial();
	glFrontFace( GL_CCW );

	glActiveTexture(GL_TEXTURE2);
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Terrain::SetUpWaterMaterial()
{
  	m_waterMaterial->SetMat4UniformValue( "u_WorldToScreenMatrix", World::s_matrixStack.StackTop() );
  	m_waterMaterial->SetVec3UniformValue( "u_cameraWorldPosition", World::s_camera3D->m_cameraPosition );
  	m_waterMaterial->SetFloatUniformValue( "u_time", m_currentTime );
	m_waterMaterial->SetIntUniformValue( "u_refractionDiffuseTexture", 0 );
	m_waterMaterial->SetIntUniformValue( "u_normalTexture", 1 );
	m_waterMaterial->SetIntUniformValue( "u_reflectionDiffuseTexture", 2 );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, m_waterRefractionFBO->m_fboColorTextureID );

	glActiveTexture(GL_TEXTURE1);
	glBindTexture( GL_TEXTURE_2D, m_waterNormalTexture->m_openglTextureID );

	glActiveTexture(GL_TEXTURE2);
	glBindTexture( GL_TEXTURE_2D, m_waterReflectionFBO->m_fboColorTextureID );
}

void Terrain::RenderSkyBox( bool /*isReflected */ )
{
	float skyBoxTextWidth = 1.f / 4.f;
	float skyBoxTextHeight = 1.f / 3.f;

	glUseProgram(0);

	GraphicManager::s_render->Disable( GL_DEPTH_TEST );
	GraphicManager::s_render->Enable( GL_TEXTURE_2D );
	GraphicManager::s_render->Disable( GL_CULL_FACE );

	glActiveTexture( GL_TEXTURE0 );
	GraphicManager::s_render->BindTexture( GL_TEXTURE_2D, m_skyBoxTexture->m_openglTextureID );

	GraphicManager::s_render->PushMatrix();
	GraphicManager::s_render->Translatef( World::s_camera3D->m_cameraPosition.x, World::s_camera3D->m_cameraPosition.y, 0.f );
	GraphicManager::s_render->Scalef(1.f,1.f,3.f);

	GraphicManager::s_render->Color4f( 1.f, 1.f, 1.f, 1.f );
	GraphicManager::s_render->BeginDraw( GL_QUADS );
	{
		//south
		GraphicManager::s_render->TexCoord2d(  4 * skyBoxTextWidth ,  skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT );

		GraphicManager::s_render->TexCoord2d( 3 * skyBoxTextWidth ,  skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL, -INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT );

		GraphicManager::s_render->TexCoord2d( 3 * skyBoxTextWidth ,  2 * skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL, 0 );

		GraphicManager::s_render->TexCoord2d( 4 * skyBoxTextWidth ,  2 * skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL,INNER_RADIUS_IN_PIXEL, 0 );

		////north
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth ,  skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL,(int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth , 2 * skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL,0 );
		GraphicManager::s_render->TexCoord2d( 2 * skyBoxTextWidth , 2 * skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL,0 );
		GraphicManager::s_render->TexCoord2d( 2 * skyBoxTextWidth ,  skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i(INNER_RADIUS_IN_PIXEL, -INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT); 

		////west
		GraphicManager::s_render->TexCoord2d(  0.f , 2 * skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i(-INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL,0 );
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth , 2 * skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL,0 );
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth , skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL,(int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( 0 , skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL,(int)SKY_HEIGHT);

		////east
		GraphicManager::s_render->TexCoord2d( 3 * skyBoxTextWidth , skyBoxTextHeight );
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL,(int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( 2 * skyBoxTextWidth , 1 * skyBoxTextHeight);
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL,(int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( 2 * skyBoxTextWidth , 2 * skyBoxTextHeight);
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL,0 );
		GraphicManager::s_render->TexCoord2d( 3 * skyBoxTextWidth , 2 * skyBoxTextHeight);
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL,0 );

		////top
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth , 0.f);
		GraphicManager::s_render->Vertex3i( -INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth , skyBoxTextHeight);
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL, INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth + skyBoxTextWidth , skyBoxTextHeight);
		GraphicManager::s_render->Vertex3i( INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT);
		GraphicManager::s_render->TexCoord2d( skyBoxTextWidth + skyBoxTextWidth , 0.f);
		GraphicManager::s_render->Vertex3i(-INNER_RADIUS_IN_PIXEL,-INNER_RADIUS_IN_PIXEL, (int)SKY_HEIGHT);

	}
	GraphicManager::s_render->EndDraw();
	GraphicManager::s_render->PopMatrix();
	GraphicManager::s_render->Enable(GL_DEPTH_TEST);
	GraphicManager::s_render->Enable( GL_BLEND );
	GraphicManager::s_render->Disable( GL_TEXTURE_2D );

}

void Terrain::RenderTopSkyBoxOnWaterSurface()
{
	return;

	float skyBoxTextWidth = 1.f / 4.f;
	float skyBoxTextHeight = 1.f / 3.f;

	std::vector<Vertex3D> vertexList;
	Vertex3D skyBoxVertex;

	skyBoxVertex.m_color = RGBColor( 1.f, 1.f, 1.f, 1.f );

	//top
	skyBoxVertex.m_position = Vector3( -INNER_RADIUS_IN_PIXEL * 2.f, INNER_RADIUS_IN_PIXEL * 2.f, SKY_HEIGHT );
	skyBoxVertex.m_texCoords = Vector2( skyBoxTextHeight ,  0.f );
	vertexList.push_back( skyBoxVertex );
	skyBoxVertex.m_position = Vector3( INNER_RADIUS_IN_PIXEL * 2.f, INNER_RADIUS_IN_PIXEL * 2.f, SKY_HEIGHT );
	skyBoxVertex.m_texCoords = Vector2( 1.f * skyBoxTextWidth ,  1.f * skyBoxTextHeight );
	vertexList.push_back( skyBoxVertex );
	skyBoxVertex.m_position = Vector3( INNER_RADIUS_IN_PIXEL * 2.f, -INNER_RADIUS_IN_PIXEL * 2.f, SKY_HEIGHT );
	skyBoxVertex.m_texCoords = Vector2( 2.f * skyBoxTextWidth , 1.f * skyBoxTextHeight );
	vertexList.push_back( skyBoxVertex );
	skyBoxVertex.m_position = Vector3( -INNER_RADIUS_IN_PIXEL * 2.f, -INNER_RADIUS_IN_PIXEL * 2.f, SKY_HEIGHT );
	skyBoxVertex.m_texCoords = Vector2( 2.f * skyBoxTextWidth ,  0.f );
	vertexList.push_back( skyBoxVertex );

	static unsigned int vboId = 0;
		
	if( vboId == 0 )
		GraphicManager::s_render->GenerateBuffer( 1, &vboId );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, vboId );
	GraphicManager::s_render->BufferData( GL_ARRAY_BUFFER, sizeof( Vertex3D ) * vertexList.size(), vertexList.data(), GL_STATIC_DRAW );

 	m_skyboxMaterial->ActivateMaterial();

	m_skyboxMaterial->SetIntUniformValue( "u_diffuseTexture", 0 );
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, m_skyBoxTexture->m_openglTextureID );

	GraphicManager::s_render->Enable( GL_TEXTURE_2D );

	glEnableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_COLORS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_TEXCOORDS );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, vboId );

	glVertexAttribPointer( VERTEX_ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_position) );
	glVertexAttribPointer( VERTEX_ATTRIB_COLORS, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_color) );
	glVertexAttribPointer( VERTEX_ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_texCoords) );

	GraphicManager::s_render->DrawArray( GL_POLYGON , 0, vertexList.size() );

	glDisableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_COLORS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_TEXCOORDS );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, 0 );

	m_skyboxMaterial->DisableMaterial();
}
