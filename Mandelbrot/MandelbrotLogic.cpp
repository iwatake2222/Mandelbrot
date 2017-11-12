#include "stdafx.h"
#include "MandelbrotLogic.h"


MandelbrotLogic::MandelbrotLogic()
{
	m_status = IDLE;
}

MandelbrotLogic::~MandelbrotLogic()
{
}

void MandelbrotLogic::init()
{
	/* start main thread now */
	std::thread t(&MandelbrotLogic::threadFunc, this);
	m_thread.swap(t);
}

/*
* calculation area in Complex Plane (x0, y0) - (x1, y1)  
* result is stored in mat. mat must be allocated by caller and size is [width * height]
*/
bool MandelbrotLogic::startCalculation(double x0, double x1, double y0, double y1, int *matOut, int matWidth, int matHeight)
{
	if (m_status != IDLE && m_status != CALCULATION_DONE) return false;

	m_x0 = x0;
	m_x1 = x1;
	m_y0 = y0;
	m_y1 = y1;
	m_matWidth = matWidth;
	m_matHeight = matHeight;
	m_matOut = matOut;

	m_status = START_REQUEST;
	return true;
}

bool MandelbrotLogic::isResultReady()
{
	if (m_status == CALCULATION_DONE) return true;
	return false;
}

bool MandelbrotLogic::isCalculating()
{
	if (m_status == CALCULATING || m_status == START_REQUEST) return true;
	return false;
}

int* MandelbrotLogic::getResult()
{
	if (m_status == CALCULATION_DONE) {
		m_status = IDLE;
		return m_matOut;
	} else {
		return NULL;
	}
}

void MandelbrotLogic::threadFunc()
{
	bool isExit = false;
	while (!isExit) {
		switch (m_status) {
		case IDLE:
		case CALCULATION_DONE:
			break;
		case START_REQUEST:
			m_status = CALCULATING;
			createMandelbrotMat();
			m_status = CALCULATION_DONE;
			break;
		case CALCULATING:
			printf("something is wrong. thread loop should not come here during CALCULATING status\n");
			break;
		default:
			printf("coding error\n");
			break;
		}
		/* workaround. without this, this thread occupies CPU */
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		//for (int i = 0; i < 1000; i++) std::this_thread::yield();
	}
}

void MandelbrotLogic::createMandelbrotMat()
{
	double stepX = (m_x1 - m_x0) / m_matWidth;
	double stepY = (m_y1 - m_y0) / m_matHeight;

#pragma omp parallel for
	for (int cntY = 0; cntY < m_matHeight; cntY++) {
		for (int cntX = 0; cntX < m_matWidth; cntX++) {
			double x = m_x0 + cntX * stepX;
			double y = m_y0 + cntY * stepY;
			//int result = LOOP_NUM - checkMandelbrot(x, y);
			int result = checkMandelbrot(x, y);
			m_matOut[cntY * m_matWidth + cntX] = result;
		}
	}
}

int MandelbrotLogic::checkMandelbrot(double x, double y) {
	double zx = 0;
	double zy = 0;
	int loop = 0;
	for (loop = 0; loop <= LOOP_NUM; loop++) {
		double orgZx = zx;
		zx = orgZx * orgZx - zy * zy + x;
		zy = 2 * orgZx * zy + y;
		if (zx * zx + zy * zy > THRESHOLD_DIVERGENCE) break;
	}
	return loop;
}