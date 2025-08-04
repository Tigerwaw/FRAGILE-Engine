#include "Enginepch.h"
#include "Application.h"
#include "Application/Window.h"
#include "Application/WindowsEventHandler.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run()
{
	HRESULT result = SetThreadDescription(GetCurrentThread(), L"MainThread");
	if (FAILED(result)) return;

	Engine::Initialize();
	Engine& engine = Engine::Get();

	InitializeApplication();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

    while (myIsRunning)
    {
		PIXScopedEvent(PIX_COLOR_INDEX(0), "Main Update Loop");

#ifndef _RETAIL
		if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam))
		{
			return;
		}
#endif

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
			}

			if (msg.message == WM_ACTIVATE)
			{
				if (LOWORD(msg.wParam) == WA_INACTIVE)
				{
					myIsPaused = true;
				}
				else
				{
					myIsPaused = false;
				}
			}

			engine.GetWindowsEventHandler().HandleMessage(&msg);
			engine.GetInputHandler().UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}

		if (!myIsRunning)
			break;

		engine.Prepare();
		UpdateApplication();

#ifndef _RETAIL
		UpdateDebug();
#endif
		engine.Update();
		engine.Render();
    }

	engine.Shutdown();
}

void Application::QuitApplication()
{
	myIsRunning = false;
}

void Application::SetPaused(bool aToggle)
{
	myIsPaused = aToggle;
}