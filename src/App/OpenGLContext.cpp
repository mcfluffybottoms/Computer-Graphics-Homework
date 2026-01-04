#include "OpenGLContext.h"

OpenGLContext::OpenGLContext(QOpenGLFunctions_3_3_Core * functions)
	: functions_(functions)
{
}

void OpenGLContext::setFunctions(QOpenGLFunctions_3_3_Core * functions)
{
	functions_ = functions;
}
