#include "PCH.h"
#include "KeyCode.h"
#include <bitset>

class InputSystem : public TSingleton<InputSystem>
{
public:
	bool KeyIsPressed(KeyCode keycode) const;

	void OnKeyPressed(KeyCode keycode);
	void OnKeyReleased(KeyCode keycode);
	void ClearState();

	std::pair<int, int> GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
	bool LeftIsPressed() const;
	bool RightIsPressed() const;
	bool IsInWindow() const;

	void OnMouseMove(int x, int y);
	void OnMouseLeave();
	void OnMouseEnter();
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);

private:
	static constexpr unsigned int s_NumKeys = 256u;
	std::bitset<s_NumKeys> m_KeyStates;

	int m_MouseX;
	int m_MouseY;
	bool m_LeftIsPressed = false;
	bool m_RightIsPressed = false;
	bool m_IsInWindow = false;
};