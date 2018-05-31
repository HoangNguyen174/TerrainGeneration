#include "Billboard.hpp"

Billboard::Billboard()
	: m_width(0.f), m_height(0.f)
{
}

Billboard::Billboard( const Vector3& center, float width, float height )
	: m_center(center), m_width(width), m_height(height)
{
}