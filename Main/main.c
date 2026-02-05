/* q="배경 화면 변경 프로그램 with C", (AI 개요)
#include <windows.h>
#include <stdio.h>

int main() {
    // 변경할 배경화면의 절대 경로 (파일 형식 주의: bmp, jpg 등)
    const char* imagePath = "C:\\path\\to\\your\\image.jpg";

    // SystemParametersInfoA 함수를 사용하여 배경화면 설정
    // SPI_SETDESKWALLPAPER: 배경화면 설정 옵션
    // SPIF_UPDATEINIFILE | SPIF_SENDCHANGE: 설정 반영 및 즉시 변경
    if (SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)imagePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) {
        printf("배경 화면이 성공적으로 변경되었습니다: %s\n", imagePath);
    } else {
        printf("배경 화면 변경에 실패했습니다. 경로를 확인하세요.\n");
    }

    return 0;
}
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>

#define IMAGES_FILE_PATH "images.txt"
#define MAX_IMAGE_COUNT 10
#define MAX_BUFFER_SIZE 1024

int numImage;
int index;
const char imagePathList[MAX_IMAGE_COUNT][MAX_BUFFER_SIZE];

void Init();
void Setup();
int Update();

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

        Sleep(10);
    }

	return 0;
}

void Init()
{
    // Set random seed
    srand((unsigned int)time(NULL));

    // Setup console title
    system("title The Background");

    // Hide console cursor
    CONSOLE_CURSOR_INFO consoleCursorInfo;

    consoleCursorInfo.bVisible = 0;
    consoleCursorInfo.dwSize = 1;

    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInfo);
}

void Setup()
{
    FILE* fp;

    fopen_s(&fp, IMAGES_FILE_PATH, "r");

    if (fp == NULL)
    {
        printf("%s 파일을 열 수 없습니다.\n", IMAGES_FILE_PATH);

        return;
    }

    char buffer[MAX_BUFFER_SIZE];

    for (numImage = 0; fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL;)
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        if (numImage >= MAX_IMAGE_COUNT)
        {
            printf("이미지 파일의 개수가 %d가 넘습니다.\n(상단에 위치한 %d개의 이미지만 가져옵니다...)\n", MAX_IMAGE_COUNT, MAX_IMAGE_COUNT);

            break;
        }

        strcpy_s(imagePathList[numImage++], MAX_BUFFER_SIZE, buffer);
    }

    printf("%d개의 이미지 파일을 불러왔습니다.\n", numImage);
}

int Update()
{
    for (; !(GetAsyncKeyState(VK_SPACE) & 0x8000); Sleep(10));

    if (SetWallpaper(imagePathList[index]))
    {
        printf("배경 화면이 성공적으로 변경되었습니다: %s\n", imagePathList[index]);
    }
    else
    {
        printf("배경 화면 변경에 실패했습니다. %s\n경로를 확인하세요.\n", imagePathList[index]);
    }

    index++;

    if (index >= numImage)
    {
        index = 0;
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        return 1;
    }

    return 0;
}

int SetWallpaper(const char* filePath)
{
    if (SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)filePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE))
    {
        return 1;
    }

    return 0;
}