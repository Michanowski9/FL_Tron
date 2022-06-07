#ifndef _SYSTEM_KEYBOARD_H_
#define _SYSTEM_KEYBOARD_H_

#include <memory>

class SystemKeyboard {
public:
	virtual const bool GetKeyState(int key) = 0;
};

using SystemKeyboardPtr = std::shared_ptr<SystemKeyboard>;

#endif // !_SYSTEM_KEYBOARD_H_