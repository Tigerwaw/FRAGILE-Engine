#include "Enginepch.h"
#include "Window.h"

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

Window::~Window()
{
	DestroyWindow(myMainWindowHandle);
	myMainWindowHandle = nullptr;
}

bool Window::InitializeWindow(const std::string& aWindowTitle, Math::Vector2f aWindowSize, Math::Vector2f aWindowPos, bool aIsFullscreen, bool aIsBorderless, bool aAllowDropFiles)
{
    LOG(LogGameEngine, Log, "Initializing window...");
    
	SIZE windowSize = { static_cast<long>(aWindowSize.x), static_cast<long>(aWindowSize.y) };
	std::wstring tempString = std::wstring(aWindowTitle.begin(), aWindowTitle.end());
	LPCWSTR windowTitle = tempString.c_str();
	constexpr LPCWSTR windowClassName = L"MainWindow";

	float posX = aWindowPos.x;
	float posY = aWindowPos.y;

	if (aIsFullscreen)
	{
		windowSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

		posX = static_cast<float>((GetSystemMetrics(SM_CXSCREEN) - windowSize.cx) / 2);
		if (posX < 0)
			posX = 0;

		posY = static_cast<float>((GetSystemMetrics(SM_CYSCREEN) - windowSize.cy) / 2);
		if (posY < 0)
			posY = 0;
	}

	HCURSOR hCursor = nullptr;
	HICON hIcon = nullptr;

	hIcon = (HICON)LoadImageA(NULL, "OhWorm_Icon.ico",
		IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

	//hCursor = LoadCursorFromFileA(("EngineAssets/" + aCursorName).c_str());

	if (hCursor == nullptr)
		hCursor = LoadCursor(nullptr, IDC_ARROW);

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hCursor = hCursor;
	windowClass.hIcon = hIcon;
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	long flags;

	if (aIsBorderless)
	{
		flags = WS_POPUP;
	}
	else
	{
		flags = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_POPUP;
	}

	RECT wr{}; // set the size, but not the position
	wr.left = static_cast<LONG>(posX);
	wr.top = static_cast<LONG>(posY);
	wr.right = wr.left + static_cast<LONG>(aWindowSize.x);
	wr.bottom = wr.top + static_cast<LONG>(aWindowSize.y);
	BOOL result = AdjustWindowRect(&wr, flags, FALSE);

	if (!result)
	{
		return false;
	}

	SetProcessDPIAware();

	// Then we use the class to create our window
	myMainWindowHandle = CreateWindow(
		windowClassName,		// Classname
		windowTitle,			// Window Title
		flags,					// Flags
		static_cast<LONG>(posX),
		static_cast<LONG>(posY),
		windowSize.cx,
		windowSize.cy,
		nullptr, nullptr, nullptr,
		nullptr
	);

	DragAcceptFiles(myMainWindowHandle, static_cast<BOOL>(aAllowDropFiles));

	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);

	return true;
}

const Math::Vector2f Window::GetCenter() const
{
	Math::Vector2f pos = GetSize();
	pos /= 2.0f;

	return pos;
}

const Math::Vector2f Window::GetTopLeft() const
{
	Math::Vector2f pos;

	RECT rect;
	if (GetWindowRect(myMainWindowHandle, &rect))
	{
		pos.x = static_cast<float>(rect.left);
		pos.y = static_cast<float>(rect.top);
	}

	return pos;
}

const Math::Vector2f Window::GetTopRight() const
{
	Math::Vector2f pos;

	RECT rect;
	if (GetWindowRect(myMainWindowHandle, &rect))
	{
		pos.x = static_cast<float>(rect.right);
		pos.y = static_cast<float>(rect.top);
	}

	return pos;
}

const Math::Vector2f Window::GetBottomLeft() const
{
	Math::Vector2f pos;

	RECT rect;
	if (GetWindowRect(myMainWindowHandle, &rect))
	{
		pos.x = static_cast<float>(rect.left);
		pos.y = static_cast<float>(rect.bottom);
	}

	return pos;
}

const Math::Vector2f Window::GetBottomRight() const
{
	Math::Vector2f pos;

	RECT rect;
	if (GetWindowRect(myMainWindowHandle, &rect))
	{
		pos.x = static_cast<float>(rect.right);
		pos.y = static_cast<float>(rect.bottom);
	}

	return pos;
}

const Math::Vector2f Window::GetSize() const
{
	Math::Vector2f size;

	RECT rect;
	if (GetWindowRect(myMainWindowHandle, &rect))
	{
		size.x = static_cast<float>(rect.right - rect.left);
		size.y = static_cast<float>(rect.top - rect.bottom);
	}

	return size;
}

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}