#include "BillboardSet.hpp"
#include "SoulStoneEngine/Render/GLRender.hpp"
#include "SoulStoneEngine/Render/GraphicManager.hpp"

BillboardSet::BillboardSet( const std::string& name, Texture* texture, int expectedCount )
	: m_texture(texture), m_shaderProgram(nullptr)
{
	m_isVBOdirty = true;
	m_vboID = 0;
	m_billboards.reserve(expectedCount);
	m_currentTime = 0.f;
	m_vertexList.reserve(5000);
}

void BillboardSet::AddBillboard( const Vector3& center, float width, float height )
{
	Billboard billboard( center, width, height );
	m_billboards.push_back( billboard );
	Vertex3D temp;
	temp.m_position = billboard.m_center;
	m_vertexList.push_back(temp);
}

void BillboardSet::SetShaderProgram(OpenGLShaderProgram* shaderProgram)
{
	m_shaderProgram = shaderProgram;
}

void BillboardSet::Render()
{
	if( m_shaderProgram == nullptr )
		return;

	 if( m_isVBOdirty )
	 {
		 if( m_vboID == 0 )
		 {
			 GraphicManager::s_render->GenerateBuffer( 1, &m_vboID );
		 }

		 GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, m_vboID );
		 GraphicManager::s_render->BufferData( GL_ARRAY_BUFFER, sizeof( Vertex3D ) * m_vertexList.size(), m_vertexList.data(), GL_STATIC_DRAW );
		 m_isVBOdirty = false;
	 }

	m_shaderProgram->UseShaderProgram();

	m_shaderProgram->SetMat4UniformValue( "u_worldToScreenMatrix", m_worldToScreenMatrix );
	m_shaderProgram->SetVec3UniformValue( "u_cameraWorldPos", m_cameraPosition );
	m_shaderProgram->SetIntUniformValue( "u_diffuseTexture", 0 );
	m_shaderProgram->SetFloatUniformValue( "u_time", m_currentTime );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, m_texture->m_openglTextureID );

	GraphicManager::s_render->Enable( GL_TEXTURE_2D );
	GraphicManager::s_render->Enable( GL_DEPTH_TEST );
	GraphicManager::s_render->Disable( GL_CULL_FACE );

	glEnableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, m_vboID );

	glVertexAttribPointer( VERTEX_ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_position) );

	GraphicManager::s_render->DrawArray( GL_POINTS ,0, m_vertexList.size() );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, 0 );

	m_shaderProgram->DisableShaderProgram();

	GraphicManager::s_render->Disable( GL_DEPTH_TEST );
}

void BillboardSet::Update(float eplapsedTime)
{
	m_currentTime += eplapsedTime;
}


