#pragma once
class MandelbrotLogic
{
public:
	const static int LOOP_NUM = 200;

private:
	const static int THRESHOLD_DIVERGENCE = 200;

	typedef enum {
		IDLE = 0,
		START_REQUEST,
		CALCULATING,
		CALCULATION_DONE,
	} STATUS;

private:
	std::thread m_thread;
	STATUS m_status;

	/* setting for calculation */
	double m_x0, m_x1, m_y0, m_y1;
	int m_matWidth, m_matHeight;
	int* m_matOut;

private:
	void threadFunc();
	int checkMandelbrot(double x, double y);
	void createMandelbrotMat();

public:
	MandelbrotLogic();
	~MandelbrotLogic();
	void init();
	bool startCalculation(double x0, double x1, double y0, double y1, int *matOut, int matWidth, int matHeight);
	bool isResultReady();
	bool isCalculating();
	int* getResult();
};

