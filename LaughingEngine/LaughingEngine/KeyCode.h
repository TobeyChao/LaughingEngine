#pragma once

enum class KeyCode
{
	KEYCODE_LBUTTON = 0x01,		// 鼠标左键
	KEYCODE_RBUTTON = 0x02,		// 鼠标右键
	KEYCODE_CANCEL = 0x03,		// Ctrl-Break
	KEYCODE_MBUTTON = 0x04,		// 鼠标中键

	KEYCODE_XBUTTON1 = 0x05,	// NOT contiguous with L & RBUTTON
	KEYCODE_XBUTTON2 = 0x06,	// NOT contiguous with L & RBUTTON

	// 0x07 Undefined

	KEYCODE_BACK = 0x08,		// BACKSPACE key
	KEYCODE_TAB = 0x09,			// TAB key

	// 0x0A - 0x0B Undefined

	KEYCODE_CLEAR = 0x0C,		// NumLock关闭时的数字键盘5
	KEYCODE_RETURN = 0x0D,		// Enter

	// 0x0E - 0x0F Undefined

	KEYCODE_SHIFT = 0x10,		// SHIFT key
	KEYCODE_CONTROL = 0x11,		// CTRL key
	KEYCODE_MENU = 0x12,		// ALT key
	KEYCODE_PAUSE = 0x13,		// PAUSE key
	KEYCODE_CAPITAL = 0x14,		// CAPS LOCK key

	// 0x15 - 0x19 : Reserved for Kanji systems

	// 1A Undefined

	KEYCODE_ESCAPE = 0x1B,		// ESC key

	// 0x1C - 0x1F : Reserved for Kanji systems

	KEYCODE_SPACE = 0x20,		// SPACEBAR
	KEYCODE_PRIOR = 0x21,		// PAGE UP key
	KEYCODE_NEXT = 0x22,		// PAGE DOWN key
	KEYCODE_END = 0x23,			// END key
	KEYCODE_HOME = 0x24,		// HOME key
	KEYCODE_LEFT = 0x25,		// LEFT ARROW key
	KEYCODE_UP = 0x26,			// UP ARROW key
	KEYCODE_RIGHT = 0x27,		// RIGHT ARROW key
	KEYCODE_DOWN = 0x28,		// DOWN ARROW key
	KEYCODE_SELECT = 0x29,		// SELECT key
	KEYCODE_PRINT = 0x2A,		// 
	KEYCODE_EXECUTE = 0x2B,		// EXECUTE key
	KEYCODE_SNAPSHOT = 0x2C,	// PRINT SCREEN key
	KEYCODE_INSERT = 0x2D,		// INS key
	KEYCODE_DELETE = 0x2E,		// DEL key
	KEYCODE_HELP = 0x2F,		// HELP key

	KEYCODE_0 = 0x30,			// 0
	KEYCODE_1 = 0x31,			// 1
	KEYCODE_2 = 0x32,			// 2
	KEYCODE_3 = 0x33,			// 3
	KEYCODE_4 = 0x34,			// 4
	KEYCODE_5 = 0x35,			// 5
	KEYCODE_6 = 0x36,			// 6
	KEYCODE_7 = 0x37,			// 7
	KEYCODE_8 = 0x38,			// 8
	KEYCODE_9 = 0x39,			// 9

	// 0x3A - 0x40 : unassigned

	KEYCODE_A = 0x41,			// A
	KEYCODE_B = 0x42,			// B
	KEYCODE_C = 0x43,			// C
	KEYCODE_D = 0x44,			// D
	KEYCODE_E = 0x45,			// E
	KEYCODE_F = 0x46,			// F
	KEYCODE_G = 0x47,			// G
	KEYCODE_H = 0x48,			// H
	KEYCODE_I = 0x49,			// I
	KEYCODE_J = 0x4A,			// J
	KEYCODE_K = 0x4B,			// K
	KEYCODE_L = 0x4C,			// L
	KEYCODE_M = 0x4D,			// M
	KEYCODE_N = 0x4E,			// N
	KEYCODE_O = 0x4F,			// O
	KEYCODE_P = 0x50,			// P
	KEYCODE_Q = 0x51,			// Q
	KEYCODE_R = 0x52,			// R
	KEYCODE_S = 0x53,			// S
	KEYCODE_T = 0x54,			// T
	KEYCODE_U = 0x55,			// U
	KEYCODE_V = 0x56,			// V
	KEYCODE_W = 0x57,			// W
	KEYCODE_X = 0x58,			// X
	KEYCODE_Y = 0x59,			// Y
	KEYCODE_Z = 0x5A,			// Z

	KEYCODE_LWIN = 0x5B,		// Left Windows key on a Microsoft Natural Keyboard
	KEYCODE_RWIN = 0x5C,		// Right Windows key on a Microsoft Natural Keyboard
	KEYCODE_APPS = 0x5D,		// Applications key on a Microsoft Natural Keyboard

	// 0x5E - 0x5F : Undefined

	KEYCODE_NUMPAD0 = 0x60,		// Numeric keypad 0 key
	KEYCODE_NUMPAD1 = 0x61,		// Numeric keypad 1 key
	KEYCODE_NUMPAD2 = 0x62,		// Numeric keypad 2 key
	KEYCODE_NUMPAD3 = 0x63,		// Numeric keypad 3 key
	KEYCODE_NUMPAD4 = 0x64,		// Numeric keypad 4 key
	KEYCODE_NUMPAD5 = 0x65,		// Numeric keypad 5 key
	KEYCODE_NUMPAD6 = 0x66,		// Numeric keypad 6 key
	KEYCODE_NUMPAD7 = 0x67,		// Numeric keypad 7 key
	KEYCODE_NUMPAD8 = 0x68,		// Numeric keypad 8 key
	KEYCODE_NUMPAD9 = 0x69,		// Numeric keypad 9 key
	KEYCODE_MULTIPLY = 0x6A,	// Multiply key
	KEYCODE_ADD = 0x6B,			// Add key
	KEYCODE_SEPARATOR = 0x6C,	// Separator key
	KEYCODE_SUBTRACT = 0x6D,	// Subtract key
	KEYCODE_DECIMAL = 0x6E,		// Decimal key
	KEYCODE_DIVIDE = 0x6F,		// Divide key
	KEYCODE_F1 = 0x70,			// F1 Key
	KEYCODE_F2 = 0x71,			// F2 Key
	KEYCODE_F3 = 0x72,			// F3 Key
	KEYCODE_F4 = 0x73,			// F4 Key
	KEYCODE_F5 = 0x74,			// F5 Key
	KEYCODE_F6 = 0x75,			// F6 Key
	KEYCODE_F7 = 0x76,			// F7 Key
	KEYCODE_F8 = 0x77,			// F8 Key
	KEYCODE_F9 = 0x78,			// F9 Key
	KEYCODE_F10 = 0x79,			// F10 Key
	KEYCODE_F11 = 0x7A,			// F11 Key
	KEYCODE_F12 = 0x7B,			// F12 Key
	KEYCODE_F13 = 0x7C,			// F13 Key
	KEYCODE_F14 = 0x7D,			// F14 Key
	KEYCODE_F15 = 0x7E,			// F15 Key
	KEYCODE_F16 = 0x7F,			// F16 Key
	KEYCODE_F17 = 0x80,			// F17 Key
	KEYCODE_F18 = 0x81,			// F18 Key
	KEYCODE_F19 = 0x82,			// F19 Key
	KEYCODE_F20 = 0x83,			// F20 Key
	KEYCODE_F21 = 0x84,			// F21 Key
	KEYCODE_F22 = 0x85,			// F22 Key
	KEYCODE_F23 = 0x86,			// F23 Key
	KEYCODE_F24 = 0x87,			// F24 Key

	// 0x88 - 0x8F : Unassigned

	KEYCODE_NUMLOCK = 0x90,		// NUM LOCK key
	KEYCODE_SCROLL = 0x91,		// SCROLL LOCK key

	// 0x92 - 0x9F : Unassigned

	KEYCODE_LSHIFT = 0xA0,		// Left SHIFT
	KEYCODE_RSHIFT = 0xA1,		// Right SHIFT
	KEYCODE_LCONTROL = 0xA2,	// Left CTRL
	KEYCODE_RCONTROL = 0xA3,	// Right CTRL
	KEYCODE_LMENU = 0xA4,		// Left ALT
	KEYCODE_RMENU = 0xA5,		// Right ALT

	// 0xA6 - 0xFF : Unassigned
};