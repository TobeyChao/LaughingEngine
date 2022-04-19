#include "PCH.h"
#include "InputSystem.h"

bool InputSystem::KeyIsPressed(KeyCode keycode) const
{
	return m_KeyStates[(unsigned int)keycode];
}

void InputSystem::OnKeyPressed(KeyCode keycode)
{
#ifdef _DEBUG
	Utility::Printf("Key %d Is Pressed\n", keycode);
#endif // _DEBUG
	m_KeyStates[(unsigned int)keycode] = true;
}

void InputSystem::OnKeyReleased(KeyCode keycode)
{
#ifdef _DEBUG
	Utility::Printf("Key %d Is Released\n", keycode);
#endif // _DEBUG
	m_KeyStates[(unsigned int)keycode] = false;
}

void InputSystem::ClearState()
{
	m_KeyStates.reset();
}

std::pair<int, int> InputSystem::GetPos() const
{
	return { m_MouseX, m_MouseY };
}

int InputSystem::GetPosX() const
{
	return m_MouseX;
}

int InputSystem::GetPosY() const
{
	return m_MouseY;
}

bool InputSystem::LeftIsPressed() const
{
	return m_LeftIsPressed;
}

bool InputSystem::RightIsPressed() const
{
	return m_RightIsPressed;
}

bool InputSystem::IsInWindow() const
{
	return m_IsInWindow;
}

void InputSystem::OnMouseMove(int newx, int newy)
{
#ifdef _DEBUG
	Utility::Printf("Mouse Pos Is (%d£¬%d)\n", newx, newy);
#endif // _DEBUG
	m_MouseX = newx;
	m_MouseY = newy;
}

void InputSystem::OnMouseLeave()
{
	m_IsInWindow = false;
}

void InputSystem::OnMouseEnter()
{
	m_IsInWindow = true;
}

void InputSystem::OnLeftPressed(int x, int y)
{
#ifdef _DEBUG
	Utility::Printf("Mouse Left Pressed\n");
#endif // _DEBUG
	m_LeftIsPressed = true;
}

void InputSystem::OnLeftReleased(int x, int y)
{
#ifdef _DEBUG
	Utility::Printf("Mouse Left Released\n");
#endif // _DEBUG
	m_LeftIsPressed = false;
}

void InputSystem::OnRightPressed(int x, int y)
{
#ifdef _DEBUG
	Utility::Printf("Mouse Right Pressed\n");
#endif // _DEBUG
	m_RightIsPressed = true;
}

void InputSystem::OnRightReleased(int x, int y)
{
#ifdef _DEBUG
	Utility::Printf("Mouse Right Released\n");
#endif // _DEBUG
	m_RightIsPressed = false;
}