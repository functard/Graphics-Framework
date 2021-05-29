#pragma once
#include "DirectXSettings.h"
#include "WindowSettings.h"
#include "InputManager.h"
#include "Circle.h"
#include "Button2D.h"
#include "BlackHole.h"
#include "Planet.h"
#include "Text2D.h"
#include "Sphere.h"

#define DXS (*(CEngine::Get()->GetDirectXSettings()))
#define WDS (*(CEngine::Get()->GetWindowSettings()))
#define IPM (*(CEngine::Get()->GetInputManager()))

class CContentManager;
class CCamera;
class Meteor;
class Cuboid;
class WaterPlane;
class Quad;

class CEngine
{
private:
	CEngine();

public:
	/**
	 * \brief	Returns the instance of the engine.
	 * \returns \c Pointer to the engine.
	 */
	static CEngine* Get();

public:
	/**
	 * \brief					Initializes the engine.
	 *							Here all necessary components will be initialized.
	 * \param[in]	_hInstance	The handle of the program [.exe] in memory.
	 * \returns					0 if successful or some error code (< 0).
	 */
	int Init(HINSTANCE _hInstance);
	/**
	 * \brief	Runs the engine.
	 *			Calls the internal update and render functions.
	 * \returns	0 if successful or some error code (< 0).
	 */
	int Run();
	/**
	 * \brief	To call when the engine execution has ended.
	 *			All used resources will be freed and destructor of internal systems will be called.
	 */
	void Finish();

	void Shutdown();
	inline SDirectXSettings* GetDirectXSettings() { return m_directXSettings; }
	inline SWindowSettings* GetWindowSettings() { return m_windowSettings; }
	inline CInputManager* GetInputManager() { return m_inputManager; }

	void Retry(Button2D* _button);
	void LoadNextLevel(Button2D* _button);


private:
	void Update(float _deltaTime);
	void Render();

	int InitApplication(HINSTANCE _hInstance);
	int InitDirectX();
	int InitDirectXToolKit();

	void LoadContent();
	
	int CreateSimpleShader();
	int CreateCelShadingShader();
	int CreateTexturedShader();
	int CreateHeightBlendingShader();
	int CreateTerrainShader();
	int CreateWaterShader();

	int CreateLightConstantBuffer();
	int CreateWaterConstantBuffer();

private:
	static CEngine* s_engine;

	bool m_isRunning = false;
	SWindowSettings* m_windowSettings;
	SDirectXSettings* m_directXSettings;
	CContentManager* m_contentManager;
	CInputManager* m_inputManager;
	CCamera* m_camera;

	Sphere* m_testSphere;

	CEntity* m_player;
	Circle* m_playerCollider;

	float m_targetFrameRate = 60.0f;
	std::chrono::system_clock::time_point m_previousTime;

	Sphere* m_planetArrived;
	BlackHole* m_blackHole;
	Planet* m_goalPlanet;
	std::vector<Meteor*> m_meteors;

	bool m_gameWon = false;
	bool m_gameLost = false;
	bool m_gameStarted = false;
	bool m_nextLevelStarted = false;

	Image2D* m_winScreen;
	Image2D* m_LoseScreen;
	Image2D* m_menuScreen;
	Button2D* m_retryButton;
	Button2D* m_nextLevelButton;
	Button2D* m_quitButton;
	Button2D* m_startButton;
	Text2D* m_warningText;
	Text2D* m_tutorialText;

	std::vector<Quad*> m_grasses;
	std::vector<Cuboid*> m_buildings;
	std::vector<WaterPlane*> m_waters;
	
};