#include "../FreeRTOSTimer.hpp"

FreeRTOSTimer::FreeRTOSTimer(tmrTIMER_CALLBACK pFunction, TIME_MS t, TimerType type)
{
	// xTimerCreate() copies the pointer for the name, so the tName variable
	// cannot simply be on the stack, it should be global or static
	static signed char tName[] = "Tmr";

	mTimerHandle = xTimerCreate(tName, OS_MS(t),
								type == TimerOneShot ? pdFALSE : pdTRUE,
								0,
								pFunction);
}
FreeRTOSTimer::~FreeRTOSTimer()
{
	xTimerDelete(mTimerHandle, 0);
}

void FreeRTOSTimer::start()
{
	xTimerStart(mTimerHandle, 0);
}
void FreeRTOSTimer::stop()
{
	xTimerStop(mTimerHandle, 0);
}
void FreeRTOSTimer::reset()
{
	xTimerReset(mTimerHandle, 0);
}
void FreeRTOSTimer::changePeriod(TIME_MS t)
{
	xTimerChangePeriod(mTimerHandle, t, 0);
}
bool FreeRTOSTimer::isRunning()
{
	return (pdFALSE != xTimerIsTimerActive(mTimerHandle) ? true : false);
}

#define callTimerFunctionFromIsr(f) \
		portBASE_TYPE higherPrTaskWoken = 0;	\
		f(mTimerHandle, &higherPrTaskWoken);	\
		if(higherPrTaskWoken)					\
			vPortYieldFromISR()

void FreeRTOSTimer::startFromISR()
{
	callTimerFunctionFromIsr(xTimerStartFromISR);
}
void FreeRTOSTimer::stopFromISR()
{
	callTimerFunctionFromIsr(xTimerStopFromISR);
}
void FreeRTOSTimer::resetFromISR()
{
	callTimerFunctionFromIsr(xTimerResetFromISR);
}
void FreeRTOSTimer::changePeriodFromISR(TIME_MS t)
{
	portBASE_TYPE higherPrTaskWoken = 0;
	xTimerChangePeriodFromISR(mTimerHandle, t, &higherPrTaskWoken);
	if( higherPrTaskWoken ) {
		vPortYieldFromISR();
	}
}
