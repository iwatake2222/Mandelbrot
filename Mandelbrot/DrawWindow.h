#pragma once
#include "IWindow.h"
#include "MandelbrotLogic.h"
class DrawWindow : public IWindow
{
private:
	/* initial draw area (these will be adjusted according to window aspect) */
	const double INIT_X0 = -1.0;
	const double INIT_X1 =  1.0;
	const double INIT_Y0 = -1.0;
	const double INIT_Y1 =  1.0;

private:
	/* calculation area in Complex Plane (x0, y0) - (x1, y1)  
	* changed when mouse drag/wheel, resize
	* the aspect must be the same as that of window
	*/
	double m_x0;
	double m_x1;
	double m_y0;
	double m_y1;

	/* In this window, window size is the same as visible OpenGL area
	* Window Area (Device Area) is in [pixel]  top left is(0, 0), bottom right is (m_windowWidth - 1, m_windowHeight - 1)
	* OpenGL Area bottom left is(0, 0), top right is (m_windowWidth - 1, m_windowHeight - 1)
	*/
	/* Memo: I originaly used calculation area as OpenGL area, however when the area becomes too tiny, OpenGL library cannot draw well. So, I decided to use integer for OpenGL area */
	int m_width;
	int m_height;
	int m_newWidth;		// during calculation after resize
	int m_newHeight;	// during calculation after resize

	/* valid on right button dragging. value is in window area[px] */
	int m_startRDragX;
	int m_startRDragY;
	int m_previousRDragX;
	int m_previousRDragY;

	int m_windowId;

	MandelbrotLogic m_logic;
	bool m_isAreaChanged;

	int *m_matDisplaying;

private:
	void initWindow();
	void draw();

public:
	DrawWindow();
	virtual ~DrawWindow() override;
	virtual void onUpdate(void) override;
	virtual void onResize(int w, int h) override;
	virtual void onClick(int button, int state, int x, int y) override;
	virtual void onDrag(int x, int y) override;
	virtual void onWheel(int wheel_number, int direction, int x, int y) override;
	virtual void onKeyboard(unsigned char key, int x, int y) override;
};

