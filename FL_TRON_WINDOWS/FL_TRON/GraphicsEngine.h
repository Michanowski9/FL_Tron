#ifndef _GRAPHICS_ENGINE_H_
#define _GRAPHICS_ENGINE_H_

#include <memory>

class GraphicsEngine {
public:
	virtual void DrawCell(int x, int y, int fill) = 0;
};

using GraphicsEnginePtr = std::shared_ptr<GraphicsEngine>;

#endif // !_GRAPHICS_ENGINE_H_