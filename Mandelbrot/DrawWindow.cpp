#include "stdafx.h"
#include "DrawWindow.h"
#include "WindowManager.h"

DrawWindow::DrawWindow()
{
	m_width = 800;
	m_height = 800;
	m_isAreaChanged = true;
	m_matDisplaying = NULL;

	initWindow();

	m_logic.init();
}


DrawWindow::~DrawWindow()
{
	glutDestroyWindow(m_windowId);
	WindowManager::getInstance()->unregisterWindow(m_windowId);
}

void DrawWindow::initWindow()
{
	glutInitWindowSize(m_width, m_height);
	glutInitDisplayMode(GLUT_RGBA);
	m_windowId = glutCreateWindow("Mandelbrot");
	WindowManager::getInstance()->registerWindow(m_windowId, this);

	glClearColor(0.1f, 0.0f, 0.3f, 1.0f);

	/* adjust calculation area */
	/* the aspect of calculation area must be the same as that of window */
	if (m_width > m_height) {
		m_x0 = INIT_X0;
		m_x1 = INIT_X1;
		m_y0 = INIT_Y0 * ((double)m_height / m_width);
		m_y1 = INIT_Y1 * ((double)m_height / m_width);
	} else {
		m_x0 = INIT_X0 * ((double)m_width / m_height);
		m_x1 = INIT_X1 * ((double)m_width / m_height);
		m_y0 = INIT_Y0;
		m_y1 = INIT_Y1;
	}

	glLoadIdentity();
	glOrtho(0, m_width, 0, m_height, -1.0, 1.0);	// The size of OpenGL Area is the same as window size
	glFlush();
}


void DrawWindow::draw(void)
{
	if (m_isAreaChanged) {
		printf("scale = %lld\n", (long long int)(1.0 / (m_x1 - m_x0)));
		printf("x0 = %lf\n", m_x0);
		printf("x1 = %lf\n", m_x1);
		printf("y0 = %lf\n", m_y0);
		printf("y1 = %lf\n", m_y1);
		int *matCalculating = new int[m_width * m_height];
		if (m_logic.startCalculation(m_x0, m_x1, m_y0, m_y1, matCalculating, m_width, m_height)) {
			// successfully started to calculate next frame
			m_isAreaChanged = false;
		} else {
			// failed to start to calculate next frame
			delete matCalculating;
		}
	}

	if (m_logic.isResultReady()) {
		/* update matrix to be displayed */
		delete m_matDisplaying;
		m_matDisplaying = m_logic.getResult();
	}

	if (m_matDisplaying != NULL) {
		int *mat = m_matDisplaying;
		glBegin(GL_QUADS);
		int index = 0;
		for (int cntY = 0; cntY < m_height; cntY++) {
			for (int cntX = 0; cntX < m_width; cntX++) {
				double l = mat[index];
				glColor3d(
					l > MandelbrotLogic::LOOP_NUM * 0.95 ? 0: l / MandelbrotLogic::LOOP_NUM,
					(l > MandelbrotLogic::LOOP_NUM * 0.95 || l < MandelbrotLogic::LOOP_NUM * 0.05) ? 0 : (MandelbrotLogic::LOOP_NUM - l) / MandelbrotLogic::LOOP_NUM,
					(!(int)l & 1) ? l / MandelbrotLogic::LOOP_NUM : (MandelbrotLogic::LOOP_NUM - l) / MandelbrotLogic::LOOP_NUM);
				glVertex2d(cntX + 0, cntY + 0);
				glVertex2d(cntX + 1, cntY + 0);
				glVertex2d(cntX + 1, cntY + 1);
				glVertex2d(cntX + 0, cntY + 1);
				index++;
			}
		}
		glEnd();
	}
}

void DrawWindow::onUpdate(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(1.0, 0.0, 0.0);

	glLoadIdentity();
	glOrtho(0, m_width, 0, m_height, -1.0, 1.0);

	draw();

	glFlush();

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void DrawWindow::onResize(int w, int h)
{
	glViewport(0, 0, w, h);

	double resizeRatioWidth = (double)w / m_width;
	double resizeRatioHeight = (double)h / m_height;

	/* the aspect of calculation must be the same as that of window */
	/* stretch calculation area in the same portion as window */
	double calcWidth = m_x1 - m_x0;
	double calcHeight = m_y1 - m_y0;
	calcWidth *= resizeRatioWidth;
	calcHeight *= resizeRatioHeight;
	m_x1 = m_x0 + calcWidth;
	m_y1 = m_y0 + calcHeight;

	// wait until current calculation is done, and set m_matDisplayingas invalid so that matrix made with old m_width/height is not used 
	// todo: can be more beautiful
	if (m_matDisplaying != NULL) {
		while (m_logic.isCalculating()) std::this_thread::sleep_for(std::chrono::microseconds(1));
		delete m_matDisplaying;
		m_matDisplaying = m_logic.getResult();
		delete m_matDisplaying;
		m_matDisplaying = NULL;
	}

	m_width = w;	
	m_height = h;

	m_isAreaChanged = true;
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glutPostRedisplay();

}

void DrawWindow::onClick(int button, int state, int x, int y)
{
	if ((GLUT_RIGHT_BUTTON == button) && (GLUT_DOWN == state)) {
		/* the beginnig of drag */
		m_startRDragX = m_previousRDragX = x;
		m_startRDragY = m_previousRDragY = y;
	} else 	if ((GLUT_RIGHT_BUTTON == button) && (GLUT_UP == state)) {
		/* the end of drag */
		m_startRDragX = m_previousRDragX = INVALID_NUM;
		m_startRDragY = m_previousRDragY = INVALID_NUM;
	}

	if ((GLUT_MIDDLE_BUTTON == button) && (GLUT_DOWN == state)) {
		/* set the current cursol position as the center */
		double calcWidth = (m_x1 - m_x0);
		double calcHeight = (m_y1 - m_y0);
		double centerX = m_x0 + calcWidth * (double)x / m_width;
		double centerY = m_y0 + calcHeight * (double)(m_height - y) / m_height;
		m_x0 = centerX - calcWidth / 2;
		m_x1 = centerX + calcWidth / 2;
		m_y0 = centerY - calcHeight / 2;
		m_y1 = centerY + calcHeight / 2;
		m_isAreaChanged = true;
		glutPostRedisplay();
	}
}

void DrawWindow::onDrag(int x, int y)
{
	if (m_previousRDragX != INVALID_NUM) {
		/* right button drag to change view position */
		double currentCalcWidth = m_x1 - m_x0;
		double currentCalcHeight = m_y1 - m_y0;
		double worldDeltaX = -(x - m_previousRDragX) * currentCalcWidth / m_width;
		double worldDeltaY = (y - m_previousRDragY) * currentCalcHeight / m_height;

		m_x0 += worldDeltaX;
		m_x1 += worldDeltaX;
		m_y0 += worldDeltaY;
		m_y1 += worldDeltaY;

		m_previousRDragX = x;
		m_previousRDragY = y;

		m_isAreaChanged = true;
		glutPostRedisplay();
	}
}

void DrawWindow::onWheel(int wheel_number, int direction, int x, int y)
{
	/* calculate the best zoom speed according to current zoom position */
	/*  - 1. zoom for X axis */
	/*  - 2. adjust Y axis to keep the original aspect */
	/* ToDo: should use bigger or smaller one */
	double currentCalcWidth = m_x1 - m_x0;
	double currentCalcHeight = m_y1 - m_y0;
	double zoomSpeed = 0.1;

	double x0Candidate;
	double x1Candidate;
	double y0Candidate;
	double y1Candidate;

	if (direction == -1) {
		/* zoom out */
		x0Candidate = m_x0 - currentCalcWidth * zoomSpeed;
		x1Candidate = m_x1 + currentCalcWidth * zoomSpeed;
	} else {
		/* zoom in */
		x0Candidate = m_x0 + currentCalcWidth * zoomSpeed;
		x1Candidate = m_x1 - currentCalcWidth * zoomSpeed;
	}
	double newVisibleHeight = (x1Candidate - x0Candidate) * currentCalcHeight / currentCalcWidth;

	y0Candidate = (m_y1 + m_y0) / 2 - newVisibleHeight / 2;
	y1Candidate = (m_y1 + m_y0) / 2 + newVisibleHeight / 2;

	if (direction == -1) {
		/* zoom out */
		m_x0 = x0Candidate;
		m_x1 = x1Candidate;
		m_y0 = y0Candidate;
		m_y1 = y1Candidate;
		m_isAreaChanged = true;
		glutPostRedisplay();
	} else {
		/* zoom in */
		const static int MINIMUM_ZOOM = 0;
		if ((x1Candidate - x0Candidate > MINIMUM_ZOOM) &&
			(newVisibleHeight > MINIMUM_ZOOM)) {
			m_x0 = x0Candidate;
			m_x1 = x1Candidate;
			m_y0 = y0Candidate;
			m_y1 = y1Candidate;
			m_isAreaChanged = true;
			glutPostRedisplay();
		} else {
			//printf("too zoom out\n");
		}
	}

	
}

void DrawWindow::onKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'i':
		break;
	default:
		break;
	}
}
