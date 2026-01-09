#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <memory>

class OpenGLContext
{
public:
	using Functions = QOpenGLFunctions_3_3_Core;
	OpenGLContext(Functions * functions = nullptr);
	~OpenGLContext() = default;

	void setFunctions(Functions * functions);
	Functions * functions() const { return functions_; }

	bool isValid() const { return functions_ != nullptr; }

	Functions * operator->() const { return functions_; }
	operator Functions *() const { return functions_; }

private:
	Functions * functions_;
};

using OpenGLContextPtr = std::shared_ptr<OpenGLContext>;
