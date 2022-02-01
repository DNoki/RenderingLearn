#pragma once

enum KeyCode
{
	Space,
};

class Input
{
public:
	Input();
	~Input();

	static bool TestKeyDown(KeyCode key);

	static void OnKeyDown(WPARAM wParam);

private:

};
