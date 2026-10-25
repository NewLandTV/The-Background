#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <Windows.h>
#include <gdiplus.h>

// Link GDI+ library.
#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "gdi32.lib")

#define IMAGES_FILE_PATH "images.txt"
#define MAX_IMAGE_COUNT 10
#define MAX_BUFFER_SIZE 1024
#define SLEEP_MILLISECONDS 5

using namespace std;
using namespace Gdiplus;

ULONG_PTR token;
GdiplusStartupInput input;

HWND hwndDesktop;

int numImage;
int index;
char imagePathList[MAX_IMAGE_COUNT][MAX_BUFFER_SIZE];

void Init();
void Setup();
int Update();
void Exit();

void DrawFrameToDesktop(Image* image, UINT frameIndex);
int SetWallpaper(const char* filePath);

int main(void)
{
    Init();
    Setup();

    while (1)
    {
        if (Update())
        {
            break;
        }

        Sleep(SLEEP_MILLISECONDS);
    }

	return 0;
}

void Init()
{
    // Initialize GDI+
    GdiplusStartup(&token, &input, NULL);

    // Get desktop handle
    HWND hwndDesktop = GetDesktopWindow();

    // Set random seed
    srand((unsigned int)time(NULL));

    // Setup console title
    system("title The Background");

    // Hide console cursor
    CONSOLE_CURSOR_INFO consoleCursorInfo;

    consoleCursorInfo.bVisible = 0;
    consoleCursorInfo.dwSize = 1;

    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInfo);

    // Setup Fast IO
    ios::sync_with_stdio(false);

    cin.tie(nullptr);
    cout.tie(nullptr);
}

void Setup()
{
    ifstream imageFile(IMAGES_FILE_PATH);

    if (!imageFile.is_open())
    {
        cout << IMAGES_FILE_PATH << "파일을 열 수 없습니다.\n";

        return;
    }

    string line;

    for (numImage = 0; getline(imageFile, line); numImage++)
    {
        if (numImage >= MAX_IMAGE_COUNT)
        {
            cout << "이미지 파일의 개수가 " << MAX_IMAGE_COUNT << "을(를) 넘습니다.\n(상단에 위치한 " << MAX_IMAGE_COUNT << "개의 이미지만 가져옵니다...)\n";

            break;
        }

        strcpy_s(imagePathList[numImage], line.c_str());
    }

    cout << numImage << "개의 이미지 파일을 불러왔습니다.\n";
}

int Update()
{
    // Wait before pressing the Space key or Esc.
    while (!(GetAsyncKeyState(VK_SPACE) & 0x8000))
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            return 1;
        }

        Sleep(SLEEP_MILLISECONDS);
    }

    int len = strlen(imagePathList[index]);

    if (len >= 4 && imagePathList[index][len - 4] == '.' && imagePathList[index][len - 3] == 'g' && imagePathList[index][len - 2] == 'i' && imagePathList[index][strlen(imagePathList[index]) - 1] == 'f')
    {
        // Convert path to wide string
        wchar_t path[MAX_BUFFER_SIZE];
        size_t convertedChars = 0;
        
        mbstowcs_s(&convertedChars, path, MAX_BUFFER_SIZE, imagePathList[index], MAX_BUFFER_SIZE);

        // Load GIF image
        Image* gifImage = new Image(path);

        // Get frame information
        GUID pageGuid = FrameDimensionTime;
        UINT frameCount = gifImage->GetFrameCount(&pageGuid);

        // Animation loop
        while (1)
        {
            for (UINT i = 0; i < frameCount; i++)
            {
                DrawFrameToDesktop(gifImage, i);

                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                {
                    delete gifImage;

                    return 1;
                }
                if (GetAsyncKeyState(VK_SPACE) & 0x8000)
                {
                    break;
                }

                Sleep(SLEEP_MILLISECONDS);
            }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                break;
            }
        }

        delete gifImage;
    }
    else
    {
        if (SetWallpaper(imagePathList[index]))
        {
            cout << "배경 화면이 성공적으로 변경되었습니다: " << imagePathList[index]  << "\n";
        }
        else
        {
            cout << "배경 화면 변경에 실패했습니다. " << imagePathList[index] << "\n경로를 확인하세요.\n";
        }
    }

    if (++index >= numImage)
    {
        index = 0;
    }

    return 0;
}

void Exit()
{
    GdiplusShutdown(token);
}

void DrawFrameToDesktop(Image* image, UINT frameIndex)
{
    // 1. Get desktop DC
    HDC hdc = GetDC(NULL);
    Graphics graphics(hdc);

    // 2. Select frame
    GUID pageGuid = FrameDimensionTime;

    image->SelectActiveFrame(&pageGuid, frameIndex);

    // 3. Draw on background. (Full screen size)
    graphics.DrawImage(image, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    ReleaseDC(NULL, hdc);
}

int SetWallpaper(const char* filePath)
{
    if (SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)filePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE))
    {
        return 1;
    }

    return 0;
}