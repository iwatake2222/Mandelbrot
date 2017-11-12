#include "stdafx.h"
#include "WindowManager.h"

WindowManager WindowManager::s_instanceOpenGLHandler;
int WindowManager::m_drawIntervalMS;

WindowManager::WindowManager()
{
	WindowManager::m_drawIntervalMS = 0;
}


WindowManager::~WindowManager()
{
}

WindowManager* WindowManager::getInstance()
{
	return &s_instanceOpenGLHandler;
}

void WindowManager::init()
{
	int argc = 0;
	glutInit(&argc, NULL);
	glutIdleFunc(idle);
}

void WindowManager::startLoop()
{
	glutMainLoop();
}

void WindowManager::registerWindow(int windowId, IWindow* pWindow)
{
	m_windowMap[windowId] = pWindow;
	// need to register callback functions for each window(display)
	glutDisplayFunc(onUpdate);
	glutReshapeFunc(onResize);
	glutMouseFunc(onClick);
	glutMotionFunc(onDrag);
	glutMouseWheelFunc(onWheel);
	glutKeyboardFunc(onKeyboard);
}

void WindowManager::unregisterWindow(int windowId)
{
	//glutSetWindow(windowId);
	//glutDisplayFunc(NULL);
	//glutReshapeFunc(NULL);
	//glutMouseFunc(NULL);
	//glutMotionFunc(NULL);
	//glutMouseWheelFunc(NULL);
	//glutKeyboardFunc(NULL);
	m_windowMap.erase(windowId);
}

int WindowManager::getDrawIntervalMS()
{
	return WindowManager::m_drawIntervalMS;
}

void WindowManager::idle(void)
{
	for (std::map<int, IWindow *>::iterator it = getInstance()->m_windowMap.begin(); it != getInstance()->m_windowMap.end(); ++it) {
		glutSetWindow(it->first);
		glutPostRedisplay();
	}
}

void WindowManager::onUpdate()
{
	int windowId = glutGetWindow();
	getInstance()->m_windowMap[windowId]->onUpdate();

	static std::chrono::system_clock::time_point  timePrevious;
	std::chrono::system_clock::time_point  timeNow;;
	timeNow = std::chrono::system_clock::now();
	WindowManager::m_drawIntervalMS = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timePrevious).count();
	//printf("fpsDraw = %lf\n", 1000.0 / m_drawIntervalMS);
	timePrevious = timeNow;
}

void WindowManager::onResize(int w, int h)
{
	int windowId = glutGetWindow();
	getInstance()->m_windowMap[windowId]->onResize(w, h);
}

void WindowManager::onClick(int button, int state, int x, int y)
{
	int windowId = glutGetWindow();
	getInstance()->m_windowMap[windowId]->onClick(button, state, x, y);
}

void WindowManager::onDrag(int x, int y)
{
	int windowId = glutGetWindow();
	getInstance()->m_windowMap[windowId]->onDrag(x, y);
}

void WindowManager::onWheel(int wheel_number, int direction, int x, int y)
{
	int windowId = glutGetWindow();
	getInstance()->m_windowMap[windowId]->onWheel(wheel_number, direction, x, y);
}

void WindowManager::onKeyboard(unsigned char key, int x, int y)
{
	int windowId = glutGetWindow();
	getInstance()->m_windowMap[windowId]->onKeyboard(key, x, y);
}
