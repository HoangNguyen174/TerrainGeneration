#include <algorithm>
#include "SoulStoneEngine/JobSystem/MemoryPoolManager.hpp"
#include "SoulStoneEngine/Utilities/stb_image.c"
#include "Terrain.hpp"
#include "World.hpp"
#include "Chunk.hpp"

int Chunk::s_edgeTable[256]= { 0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
							   0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
							   0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
							   0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
							   0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
							   0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
							   0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
							   0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
							   0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
							   0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
							   0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
							   0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
							   0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
							   0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
							   0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
							   0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
							   0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
							   0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
							   0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
							   0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
							   0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
							   0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
							   0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
							   0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
							   0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
							   0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
							   0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
							   0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
							   0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
							   0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
							   0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
							   0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };


int Chunk::s_triTable[256][16] =
			{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
			{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
			{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
			{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
			{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
			{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
			{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
			{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
			{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
			{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
			{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
			{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
			{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
			{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
			{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
			{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
			{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
			{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
			{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
			{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
			{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
			{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
			{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
			{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
			{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
			{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
			{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
			{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
			{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
			{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
			{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
			{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
			{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
			{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
			{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
			{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
			{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
			{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
			{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
			{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
			{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
			{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
			{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
			{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
			{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
			{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
			{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
			{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
			{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
			{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
			{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
			{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
			{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
			{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
			{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
			{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
			{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
			{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
			{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
			{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
			{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
			{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
			{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
			{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
			{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
			{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
			{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
			{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
			{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
			{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
			{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
			{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
			{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
			{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
			{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
			{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
			{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
			{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
			{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
			{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
			{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
			{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
			{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
			{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
			{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
			{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
			{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
			{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
			{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
			{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
			{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
			{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
			{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
			{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
			{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
			{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
			{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

Chunk::Chunk( const ChunkCoords2D& chunkCoords, const float isoValue )
{
	m_minChunkCoords = chunkCoords;
	m_minWorldCoords.x = (float)m_minChunkCoords.x * CHUNK_WIDTH_X;
	m_minWorldCoords.y = (float)m_minChunkCoords.y * CHUNK_DEPTH_Y;
	m_vertexList.reserve(6000);
	m_isoLevel = isoValue;
	m_isVboDirty = true;
	m_numVertex = 0;
	m_vboId = 0;
	m_material = new Material( "Terrain", Terrain::s_terrainShaderProgram );
	m_grassTexture = Texture::CreateOrGetTexture( "./Data/Texture/grass.jpg" );
	m_rockTexture = Texture::CreateOrGetTexture( "./Data/Texture/rock.png" );
	m_snowTexture = Texture::CreateOrGetTexture( "./Data/Texture/snow.png" );
	m_treeBillboardTexture = Texture::CreateOrGetTexture( "./Data/Texture/tree.png" );
	m_grassBillboardTexture = Texture::CreateOrGetTexture( "./Data/Texture/grass01.png" );

	m_material->m_diffuseTexture = m_grassTexture;
	m_material->m_specularTexture = m_rockTexture;
	m_material->m_normalTexture = m_snowTexture;

	m_trees = new BillboardSet( "Tree", m_treeBillboardTexture, 1000 );
	m_trees->SetShaderProgram( Terrain::s_billboardShaderProgram );

	m_grass = new BillboardSet( "Grass", m_grassBillboardTexture, 1000 );
	m_grass->SetShaderProgram( Terrain::s_grassShaderProgram );

	GenerateChunk();

	GenerateRiverPath();

	GenerateRiverGeometry();

	ComputeVertexAttributes();

	GenerateTreeAndGrass();
}

void Chunk::GenerateTreeAndGrass()
{
	int MAX_TREE_PER_TRI = MathUtilities::GetRandomNumber( 3, 5 );
    int MAX_GRASS_PER_TRI = MathUtilities::GetRandomNumber( 70, 100 );

	for( unsigned int index = 0; index < m_vertexList.size(); index += 3 )
	{
		int iteration = 0;
		Vertex3D& vertex1 = m_vertexList[ index ];
		Vertex3D& vertex2 = m_vertexList[ index + 1 ];
		Vertex3D& vertex3 = m_vertexList[ index + 2 ];

		for( iteration = 0; iteration < MAX_TREE_PER_TRI; iteration++ )
		{
			Vector3 randPosition = GetRandomPointInsideTriangle( vertex1.m_position, vertex2.m_position, vertex3.m_position );
			Vector3 surfaceNormal = ComputeSurfaceNormalAt( vertex1.m_position, vertex2.m_position, vertex3.m_position );

			Vector3 up = Vector3( 0.f, 0.f, 1.f );
			float dotProduct = up.DotProduct( surfaceNormal );

			float rand = MathUtilities::GetRandomFloatZeroToOne();
			
			if( randPosition.z > WATER_LEVEL 
				&& randPosition.z < WATER_LEVEL + 5 
				&& rand < 0.5f
				&& dotProduct > 0.98f )
			{
				randPosition.z += 1.0f;
				m_trees->AddBillboard( randPosition, 1, 2 );
			}
		}		

		for( iteration = 0; iteration < MAX_GRASS_PER_TRI; iteration++ )
		{
			Vector3 randPosition = GetRandomPointInsideTriangle( vertex1.m_position, vertex2.m_position, vertex3.m_position );
			Vector3 surfaceNormal = ComputeSurfaceNormalAt( vertex1.m_position, vertex2.m_position, vertex3.m_position );
			Vector3 up = Vector3( 0.f, 0.f, 1.f );

			float dotProduct = up.DotProduct( surfaceNormal );

			float rand = MathUtilities::GetRandomFloatZeroToOne();

			if( randPosition.z > WATER_LEVEL 
				&& randPosition.z < WATER_LEVEL + 5
				&& rand < 0.5f
				&& dotProduct > 0.94f )
			{
				randPosition.z += 0.15f;
				m_grass->AddBillboard( randPosition, 1, 2 );
			}
		}
	}
}

void Chunk::GenerateChunk()
{
	Vector3 vertexList[12];

	for( int zIndex = 0; zIndex < CHUNK_HEIGHT_Z; zIndex++ )
	{
		for( int yIndex = 0; yIndex < CHUNK_DEPTH_Y; yIndex++ )
		{
			for( int xIndex = 0; xIndex < CHUNK_WIDTH_X; xIndex++ )
			{
				Cube cube;
				float x = static_cast<float>( xIndex ) + m_minWorldCoords.x;
				float y = static_cast<float>( yIndex ) + m_minWorldCoords.y;

				cube.position[0] = Vector3( x , y + 1.f, static_cast<float>( zIndex ) );
				cube.position[1] = Vector3( x + 1.f , y + 1.f, static_cast<float>( zIndex ) );
				cube.position[2] = Vector3( x + 1.f , y, static_cast<float>( zIndex ) );
				cube.position[3] = Vector3( x , y, static_cast<float>( zIndex ) );
											
				cube.position[4] = Vector3( x , y + 1.f, static_cast<float>( zIndex + 1.f ) );
				cube.position[5] = Vector3( x + 1.f, y + 1.f , static_cast<float>( zIndex + 1.f ) );
				cube.position[6] = Vector3( x + 1.f, y, static_cast<float>( zIndex + 1.f ) );
				cube.position[7] = Vector3( x, y, static_cast<float>( zIndex + 1.f ) );

				cube.densityValue[0] = DensityFunction2( cube.position[0] );
				cube.densityValue[1] = DensityFunction2( cube.position[1] );
				cube.densityValue[2] = DensityFunction2( cube.position[2] );
				cube.densityValue[3] = DensityFunction2( cube.position[3] );
				cube.densityValue[4] = DensityFunction2( cube.position[4] );
				cube.densityValue[5] = DensityFunction2( cube.position[5] );
				cube.densityValue[6] = DensityFunction2( cube.position[6] );
				cube.densityValue[7] = DensityFunction2( cube.position[7] );

				int cubeIndex = 0;
				if( cube.densityValue[0] < m_isoLevel )
					cubeIndex |= 1;
				if( cube.densityValue[1] < m_isoLevel )
					cubeIndex |= 2;
				if( cube.densityValue[2] < m_isoLevel )
					cubeIndex |= 4;
				if( cube.densityValue[3] < m_isoLevel )
					cubeIndex |= 8;
				if( cube.densityValue[4] < m_isoLevel )
					cubeIndex |= 16;
				if( cube.densityValue[5] < m_isoLevel )
					cubeIndex |= 32;
				if( cube.densityValue[6] < m_isoLevel )
					cubeIndex |= 64;
				if( cube.densityValue[7] < m_isoLevel )
					cubeIndex |= 128;

				// cube is entirely inside or outside surface, do nothing
				if( s_edgeTable[ cubeIndex ] == 0 )
					continue;

				if (s_edgeTable[ cubeIndex ] & 1)
					vertexList[0] = InterpolateVertex( cube.position[0],cube.position[1], cube.densityValue[0], cube.densityValue[1] );
				if (s_edgeTable[ cubeIndex ] & 2)
					vertexList[1] = InterpolateVertex( cube.position[1],cube.position[2], cube.densityValue[1], cube.densityValue[2] );
				if (s_edgeTable[ cubeIndex ] & 4)
					vertexList[2] = InterpolateVertex( cube.position[2],cube.position[3], cube.densityValue[2], cube.densityValue[3] );
				if (s_edgeTable[ cubeIndex ] & 8)
					vertexList[3] = InterpolateVertex( cube.position[3],cube.position[0], cube.densityValue[3], cube.densityValue[0] );
				if (s_edgeTable[ cubeIndex ] & 16)
					vertexList[4] = InterpolateVertex( cube.position[4],cube.position[5], cube.densityValue[4], cube.densityValue[5] );
				if (s_edgeTable[ cubeIndex ] & 32)
					vertexList[5] = InterpolateVertex( cube.position[5],cube.position[6], cube.densityValue[5], cube.densityValue[6] );
				if (s_edgeTable[ cubeIndex ] & 64)
					vertexList[6] = InterpolateVertex( cube.position[6],cube.position[7], cube.densityValue[6], cube.densityValue[7] );
				if (s_edgeTable[ cubeIndex ] & 128)
					vertexList[7] = InterpolateVertex( cube.position[7],cube.position[4], cube.densityValue[7], cube.densityValue[4] );
				if (s_edgeTable[ cubeIndex ] & 256)
					vertexList[8] = InterpolateVertex( cube.position[0],cube.position[4], cube.densityValue[0], cube.densityValue[4] );
				if (s_edgeTable[ cubeIndex ] & 512)
					vertexList[9] = InterpolateVertex( cube.position[1],cube.position[5], cube.densityValue[1], cube.densityValue[5] );
				if (s_edgeTable[ cubeIndex ] & 1024)
					vertexList[10] = InterpolateVertex( cube.position[2],cube.position[6], cube.densityValue[2], cube.densityValue[6] );
				if (s_edgeTable[ cubeIndex ] & 2048)
					vertexList[11] = InterpolateVertex( cube.position[3],cube.position[7], cube.densityValue[3], cube.densityValue[7] );

				for( int triIndex = 0; s_triTable[ cubeIndex ][triIndex] != -1; triIndex += 3 )
				{
					Vector3 v1 = vertexList[ s_triTable[ cubeIndex ][ triIndex ] ];
					Vector3 v2 = vertexList[ s_triTable[ cubeIndex ][ triIndex + 1 ] ];
					Vector3 v3 = vertexList[ s_triTable[ cubeIndex ][ triIndex + 2 ] ];
					Vertex3D vert1;
					Vertex3D vert2;
					Vertex3D vert3;
					vert1.m_position = v1;
					vert2.m_position = v2;
					vert3.m_position = v3;

					vert1.m_color = RGBColor::White();
					vert2.m_color = RGBColor::White();
					vert3.m_color = RGBColor::White();

// 					vert1.m_texCoords = ComputeTexCoordsAt( vert1.m_position );
// 					vert2.m_texCoords = ComputeTexCoordsAt( vert2.m_position );
// 					vert3.m_texCoords = ComputeTexCoordsAt( vert3.m_position );
// 
// 					vert1.m_normal = ComputeNormalByGradient( vert1.m_position );
// 					vert2.m_normal = ComputeNormalByGradient( vert2.m_position );
// 					vert3.m_normal = ComputeNormalByGradient( vert3.m_position );
// 
// 					ComputeTerrainTypeWeight( vert1.m_position, vert1.m_terrainTypeWeight );
// 					ComputeTerrainTypeWeight( vert2.m_position, vert2.m_terrainTypeWeight );
// 					ComputeTerrainTypeWeight( vert3.m_position, vert3.m_terrainTypeWeight );

					m_vertexList.push_back(vert1);
					m_vertexList.push_back(vert2);
					m_vertexList.push_back(vert3);
				}
			}
		}
	}

}

void Chunk::ComputeVertexAttributes()
{
	for( unsigned int index = 0; index < m_vertexList.size(); index++ )
	{
		Vertex3D& vertex = m_vertexList[index];

		vertex.m_texCoords = ComputeTexCoordsAt( vertex.m_position );
		vertex.m_normal = ComputeNormalByGradient( vertex.m_position );
		ComputeTerrainTypeWeight( vertex.m_position, vertex.m_terrainTypeWeight );
	}
}

Vector3 Chunk::InterpolateVertex( const Vector3& p1, const Vector3& p2, const float valueAtP1, const float valueAtP2)
{
	Vector3 returnValue;
	float ratio;

	if( abs( m_isoLevel - valueAtP1 ) < 0.0001 )
		return p1;
	if( abs( m_isoLevel - valueAtP2 ) < 0.0001 )
		return p2;
	if( abs( valueAtP2 - valueAtP1 ) < 0.0001 )
		return p1;
	ratio = ( m_isoLevel - valueAtP1 ) / ( valueAtP2 - valueAtP1 );

	returnValue = p1;
	returnValue += ratio * ( p2 - p1 ); 

	return returnValue;
}

void Chunk::Render()
{
	//RenderRiverPath();

	if( m_isVboDirty )
		CreateVBO();

	RenderVBO();

	RenderGrassAndTree();
}

void Chunk::RenderGrassAndTree()
{
	if( !Terrain::s_isRenderingReflection )
		//m_trees->m_worldToScreenMatrix = World::s_matrixStack.StackTop();
		m_trees->m_worldToScreenMatrix = World::s_camera3D->m_projectionViewMatrix;
	else
		m_trees->m_worldToScreenMatrix = World::s_ViewProjectionMatrixReflectCamera;

	m_trees->m_cameraPosition = World::s_camera3D->m_cameraPosition;
	m_trees->Render();

	if( !Terrain::s_isRenderingReflection )
		//m_grass->m_worldToScreenMatrix = World::s_matrixStack.StackTop();
		m_grass->m_worldToScreenMatrix = World::s_camera3D->m_projectionViewMatrix;
	else
		m_grass->m_worldToScreenMatrix = World::s_ViewProjectionMatrixReflectCamera;

	m_grass->m_cameraPosition = World::s_camera3D->m_cameraPosition;
	m_grass->Render();
}

void Chunk::CreateVBO()
{
	if( m_vboId == 0 )
	{
		GraphicManager::s_render->GenerateBuffer( 1, &m_vboId );
	}

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, m_vboId );
	GraphicManager::s_render->BufferData( GL_ARRAY_BUFFER, sizeof( Vertex3D ) * m_vertexList.size(), m_vertexList.data(), GL_STATIC_DRAW );

	m_numVertex = m_vertexList.size();

	m_isVboDirty = false;
}

void Chunk::RenderVBO()
{
	m_material->ActivateMaterial();

 	SetUpMaterial();

	GraphicManager::s_render->Enable( GL_DEPTH_TEST );
	GraphicManager::s_render->Enable(GL_CULL_FACE);

	glEnableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_COLORS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_TEXCOORDS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_NORMALS );
	glEnableVertexAttribArray( VERTEX_ATTRIB_TERRAIN_WEIGHT );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, m_vboId );

	glVertexAttribPointer( VERTEX_ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_position) );
	glVertexAttribPointer( VERTEX_ATTRIB_COLORS, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_color) );
	glVertexAttribPointer( VERTEX_ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_texCoords) );
	glVertexAttribPointer( VERTEX_ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D,m_normal) );
	glVertexAttribPointer( VERTEX_ATTRIB_TERRAIN_WEIGHT, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex3D ), (const GLvoid*) offsetof(Vertex3D, m_terrainTypeWeight) );

	GraphicManager::s_render->DrawArray( GL_TRIANGLES ,0, m_numVertex );

	glDisableVertexAttribArray( VERTEX_ATTRIB_POSITIONS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_COLORS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_TEXCOORDS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_NORMALS );
	glDisableVertexAttribArray( VERTEX_ATTRIB_TERRAIN_WEIGHT );

	GraphicManager::s_render->BindBuffer( GL_ARRAY_BUFFER, 0 );

	m_material->DisableMaterial();

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Chunk::SetUpMaterial()
{
	if( !Terrain::s_isRenderingReflection )
		m_material->SetMat4UniformValue( "u_WorldToScreenMatrix", World::s_matrixStack.StackTop() );
	else
		m_material->SetMat4UniformValue( "u_WorldToScreenMatrix", World::s_ViewProjectionMatrixReflectCamera );

	m_material->SetVec3UniformValue( "u_cameraWorldPosition", World::s_camera3D->m_cameraPosition );
	m_material->SetVec4UniformValue( "u_clipPlane", Terrain::s_clipPlane );
	m_material->SetIntUniformValue( "u_grassTexture", 0 );
	m_material->SetIntUniformValue( "u_snowTexture", 1 );
	m_material->SetIntUniformValue( "u_rockTexture", 2 );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, m_grassTexture->m_openglTextureID );
 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture( GL_TEXTURE_2D, m_snowTexture->m_openglTextureID );

	glActiveTexture(GL_TEXTURE2);
	glBindTexture( GL_TEXTURE_2D, m_rockTexture->m_openglTextureID );	
}

float Chunk::DensityFunction1(const Vector3& position)
{
	float x = MathUtilities::GetRandomFloatInRange( -1.f, 1.f );
	float y = MathUtilities::GetRandomFloatInRange( -1.f, 1.f );
	float z = MathUtilities::GetRandomFloatInRange( -1.f, 1.f );
	Vector3 normal( x, y ,z );
	normal = normal.Normalize();
	return 1.f - normal.DotProduct(position);
}


float Chunk::DensityFunction2( const Vector3& position )
{
	float noise = ComputePerlinNoiseValueAtPosition2D( Vector2( position.x, position.y ), 20.f, 5, 10.f, 0.4f );

 	float effectiveHeight = position.z;
	float densityWithHeightBias = effectiveHeight - noise;
	float density = densityWithHeightBias;

	return density;
}

Vector3 Chunk::ComputeSurfaceNormalAt( const Vector3& v1, const WorldCoords3D& v2,  const WorldCoords3D& v3 )
{
	Vector3 normal;
	Vector3 v1tov2 = v2 - v1;
	Vector3 v2tov3 = v3 - v2;

	normal = v1tov2.CrossProduct( v2tov3 ).Normalize();

	return normal;
}

Vector2 Chunk::ComputeTexCoordsAt(const WorldCoords3D& position)
{
	Vector2 minWorldCoords = Vector2( m_minWorldCoords.x, m_minWorldCoords.y );
	float distanceToMinX = abs( position.x - minWorldCoords.x );
	float distanceToMinY = abs( position.y - minWorldCoords.y );

	Vector2 texCoords;
	texCoords.x = distanceToMinX / CHUNK_WIDTH_X;
	texCoords.y = distanceToMinY / CHUNK_DEPTH_Y;

	return texCoords;
}

Vector3 Chunk::ComputeNormalByGradient(const WorldCoords3D& position)
{
	Vector3 normal;
	normal.x = DensityFunction2( Vector3( position.x - 1.f, position.y, position.z ) ) - DensityFunction2( Vector3( position.x + 1.f, position.y, position.z ) );
	normal.y = DensityFunction2( Vector3( position.x, position.y - 1.f, position.z ) ) - DensityFunction2( Vector3( position.x, position.y + 1.f, position.z ) );
	normal.z = DensityFunction2( Vector3( position.x, position.y, position.z - 1.f ) ) - DensityFunction2( Vector3( position.x, position.y, position.z + 1.f ) );

	normal = -normal.Normalize();

	return normal;
}

void Chunk::ComputeTerrainTypeWeight( const WorldCoords3D& position, float terrainWeightArray[4] )
{
	const Vector2 GRASS_RANGE( 0.f, 9.f );
	const Vector2 ROCK_RANGE( 8.f, 12.0f );
	const Vector2 SNOW_RANGE( 10.f, 20.f );

	for( int i = 0; i < 3; i++ )
	{
		terrainWeightArray[i] = 0.0f;
	}

	if( position.z < ROCK_RANGE.x /*8*/ )
	{
		terrainWeightArray[GRASS] = 1.f;
		return;
	}
	if( position.z >= ROCK_RANGE.x /*8*/ && position.z <= GRASS_RANGE.y /*9*/ )
	{
		float dist = GRASS_RANGE.y - ROCK_RANGE.x;
		float distToGrassMax = GRASS_RANGE.y - position.z;
		float grassRatio = distToGrassMax / dist;
		float rockRatio = 1.f - grassRatio;
		terrainWeightArray[GRASS] = grassRatio;
		terrainWeightArray[ROCK] = rockRatio;
		return;
	}
	if( position.z > GRASS_RANGE.y /*9*/ && position.z <= SNOW_RANGE.x /*10*/ )
	{
		terrainWeightArray[ROCK] = 1.f;
		return;
	}
	if( position.z > SNOW_RANGE.x && position.z <= ROCK_RANGE.y )
	{
		float dist = ROCK_RANGE.y - SNOW_RANGE.x;
		float distToRockMax = ROCK_RANGE.y - position.z;
		float rockRatio = distToRockMax / dist;
		float snowRatio = 1.f - rockRatio;
		terrainWeightArray[ROCK] = rockRatio;
		terrainWeightArray[SNOW] = snowRatio;
		return;
	}
	if( position.z > ROCK_RANGE.y )
	{
		terrainWeightArray[SNOW] = 1.f;
		return;
	}
}

Vector3 Chunk::GetRandomPointInsideTriangle( const Vector3& v1, const Vector3& v2, const Vector3& v3 )
{
	float rand1 = MathUtilities::GetRandomFloatZeroToOne();
	float rand2 = MathUtilities::GetRandomFloatZeroToOne();
	float sqrtOfRand1 = sqrt( rand1 );
	float coefficient1 = 1.f - sqrtOfRand1;
	float coefficient2 = sqrtOfRand1 * ( 1.f - rand2 );
	float coefficient3 = sqrtOfRand1 * rand2;

	Vector3 returnValue;

	returnValue = coefficient1 * v1 + coefficient2 * v2 + coefficient3 * v3;

	return returnValue;
}

void Chunk::Update(float elapsedTime)
{
	m_grass->Update( elapsedTime );
	m_trees->Update( elapsedTime );
}

void Chunk::GenerateRiverPath()
{
	std::vector<float> wormSegment;
	std::vector<float> copySegment;

	for( int x = 0; x < CHUNK_WIDTH_X; x++ )
	{
		float noise = ComputePerlinNoiseValueAtPosition2D( Vector2( x, 2.f ), 32.f, 1, 1.f, 0.4f );

		wormSegment.push_back( noise );
	}
	
	copySegment = wormSegment;
	std::sort( copySegment.begin(), copySegment.end() );

	float smallest = copySegment.front();
	float biggest = copySegment.back();
	float difference = biggest - smallest;

	Vector2 steerDirection = Vector2( 0.f, 1.f );
	float steerAngleDegree = 0;

	Vector2 riverStart;
	m_riverNode.push_back( riverStart );
	Vector2 pointOnRiver = riverStart;
	float riverSegmentLength = 1.5f;
	float totalLength = 0;

	for( unsigned int index = 0; index < wormSegment.size(); index++ )
	{
		steerDirection = Vector2( 0.f, 1.f );
		float wormNoise = wormSegment[index];
		steerAngleDegree = MathUtilities::ConvertNumInRangeToNewRange( smallest, biggest, -60.f, 60.f, wormNoise );
		steerDirection.RotateDegree( steerAngleDegree );
		steerDirection = steerDirection.Normalize();
		pointOnRiver += steerDirection * riverSegmentLength;
		m_riverNode.push_back( pointOnRiver );
	}
}

void Chunk::GenerateRiverGeometry()
{
	const float MAX_RIVER_RADIUS = 5.f;
	const float MAX_Z_DROP = 3.f;

	float effectivePullRadiusSquare = MAX_RIVER_RADIUS * MAX_RIVER_RADIUS;

	int size = m_vertexList.size();

	for( unsigned int riverIndex = 0; riverIndex < m_riverNode.size(); riverIndex++ )
	{
		Vector2 riverNode = m_riverNode[riverIndex];
		for( unsigned int vertexIndex = 0; vertexIndex < m_vertexList.size(); vertexIndex++ )
		{
			Vector3 vertexPosition = m_vertexList[vertexIndex].m_position;
			Vector2 vertexPosition2D = Vector2( vertexPosition.x, vertexPosition.y );

			float distanceSquare = ( riverNode - vertexPosition2D ).CalcLengthSquare();

			if( distanceSquare > effectivePullRadiusSquare )
				continue;

			float distance = sqrt( distanceSquare );

			float distanceFraction = distance / MAX_RIVER_RADIUS;
			float dropZ = 1 - distanceFraction;
			dropZ = SmoothStep( dropZ );

			dropZ *= MAX_Z_DROP;

			m_vertexList[vertexIndex].m_position.z -= dropZ;

// 			float scale = 3.f;
// 			if( m_vertexList[vertexIndex].m_position.z < 0.f )
// 				m_vertexList[vertexIndex].m_position.z = 0.f;
// 			else
// 				m_vertexList[vertexIndex].m_position.z -= 1.f / ( distanceSquare + 0.001f ) * scale;
		}
	}
}

void Chunk::RenderRiverPath()
{
	return;
	GraphicManager::s_render->Disable( GL_DEPTH_TEST );
	GraphicManager::s_render->Disable( GL_TEXTURE_2D );

	GraphicManager::s_render->BeginDraw( GL_LINES );
	GraphicManager::s_render->LineWidth( 3.f );
	GraphicManager::s_render->Color3f( 1.f, 0.f, 0.f );
	for( unsigned int index = 0; index < m_riverNode.size() - 1; index++ )
	{
		Vector2 p1 = m_riverNode[index];
		Vector2 p2 = m_riverNode[index + 1];

		GraphicManager::s_render->Vertex3f( p1.x, p1.y, 0.f );
		GraphicManager::s_render->Vertex3f( p2.x, p2.y, 0.f );
	}
	GraphicManager::s_render->EndDraw();

}