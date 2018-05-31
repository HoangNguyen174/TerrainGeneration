#ifndef WORLD_H
#define WORLD_H
#include "SoulStoneEngine/Utilities/GameCommon.hpp"
#include "SoulStoneEngine/Render/GraphicManager.hpp"
#include "SoulStoneEngine/Debugger/DebugPoint.hpp"
#include "SoulStoneEngine/Utilities/XMLLoaderUtilities.hpp"

#include "Terrain.hpp"
#include "Chunk.hpp"

class World
{
	public:
		static bool						s_turnOnInfiniteGeneration;
		static Camera3D*				s_camera3D;
		static FBO*						s_fbo;
		static Matrix44					s_ViewProjectionMatrixReflectCamera;
		static MatrixStack				s_matrixStack;
		bool							m_renderWorldOriginAxes;
		float							m_zoomFactor;
		Chunk*							m_chunk;
		Terrain*						m_terrain;
		OpenGLShaderProgram*			m_fboShaderProgram;

	private:
		RGBColor						m_mouseClickColor;
		float							m_mouseClickRadius;
		float							m_mouseClickFlashTime;

	public:
		World();
		~World();
		void Initialize();
		void InitializeGraphic();
		bool ProcessKeyDownEvent(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam );
		bool ProcessMouseDownEvent(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam );
		void UpdateFromKeyboard(float elapsedTime);
		void SetupPerspectiveProjection();
		void ApplyCameraTransform( const Camera3D* camera );
		void OpenOrCloseConsole();
		Vector2 GetMouseSinceLastChecked();
		void Update(float elapsedTime);
		void ClearWorld();
		void UpdateMouseWorldPosition();
		void Render();
		void RenderWorldAxes();
		void DrawCursor();
		void RenderFlashCursor();
		void RenderFBOToScreen();
};

extern World* theWorld;
extern WorldCoords2D g_mouseWorldPosition;
extern WorldCoords2D g_mouseScreenPosition;
extern bool g_isLeftMouseDown;
extern bool g_isRightMouseDown;
extern bool g_isHoldingShift;
extern bool g_isQuitting;
extern Matrix44 g_viewMatrix;

#endif