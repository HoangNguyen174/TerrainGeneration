#ifndef BILLBOARD_H
#define BILLBOARD_H
#include "SoulStoneEngine/Utilities/Vector3.h"

class Billboard
{
	public:
		float		m_width;
		float		m_height;
		Vector3		m_center;

	public:
		Billboard();
		Billboard( const Vector3& center, float width, float height );
};


#endif