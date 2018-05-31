#ifndef BILLBOARD_SET_H
#define BILLBOARD_SET_H
#include <vector>
#include "SoulStoneEngine/Utilities/Texture.hpp"
#include "SoulStoneEngine/Render/OpenGLShaderProgram.hpp"
#include "SoulStoneEngine/Utilities/Matrix44.hpp"
#include "SoulStoneEngine/Render/GLRender.hpp"
#include "Billboard.hpp"

class BillboardSet
{
	public:
		std::string					m_name;
		Texture*					m_texture;
		std::vector<Billboard>		m_billboards;
		OpenGLShaderProgram*		m_shaderProgram;
		Matrix44					m_worldToScreenMatrix;
		Vector3						m_cameraPosition;
		bool						m_isVBOdirty;
		unsigned int				m_vboID;
		std::vector<Vertex3D>		m_vertexList;
		float						m_currentTime;

	public:
		BillboardSet( const std::string& name, Texture* texture, int expectedCount );
		void SetShaderProgram( OpenGLShaderProgram* shaderProgram );
		void AddBillboard( const Vector3& center, float width, float height );
		void Update( float eplapsedTime );
		void Render();
};


#endif