#pragma once

#include "IWindow.h"

class WindowManager
{
private:
	static WindowManager s_instanceOpenGLHandler;
	static int m_drawIntervalMS;
	std::map<int, IWindow *> m_windowMap;

private:
	WindowManager();
	~WindowManager();

	static void idle(void);
	static void onUpdate();
	static void onResize(int w, int h);
	static void onClick(int button, int state, int x, int y);
	static void onDrag(int x, int y);
	static void onWheel(int wheel_number, int direction, int x, int y);
	static void onKeyboard(unsigned char key, int x, int y);

public:
	static WindowManager* getInstance();
	void init();
	void startLoop();
	void registerWindow(int windowId, IWindow* pWindow);
	void unregisterWindow(int windowId);
	int getDrawIntervalMS();
};


