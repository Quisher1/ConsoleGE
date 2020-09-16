#pragma once
//#define _WIN32_WINNT 0x0500 | for GetConsoleWindow work 
#include <Windows.h>
#include <string>
#include <ctime>
#include "Matrix.h"



enum Pixel {
	SOLID = 0x2588,
	THREEQUARTERS = 0x2593,
	HALF = 0x2592,
	QUARTER = 0x2591
};
enum COLOR {
	BLACK = 0x0000,
	WHITE = 0x000F,
	BLUE = 0x0009,
	GREEN = 0x000A,
	RED = 0x000C
};


class GameEngine {
	float zNear = 0.001f, zFar = 10000;

	float UpX = 0, UpY = 1, UpZ = 0, RightX = 1, RightY = 0, RightZ = 0, ForwardX = 0, ForwardY = 0, ForwardZ = 1, PositionX = 0, PositionY = 0, PositionZ = 10;

	float ErrorCount = -20010101.024862f;

	HWND hwndConsole;
	HANDLE hConsole;
	HANDLE hConsoleInput;
	SMALL_RECT RectWindow;
	CHAR_INFO* Screen;

	float PI = 3.1415926f;

	void drawLine(int x1, int y1, int x2, int y2, Pixel pixel = Pixel::SOLID, COLOR color = COLOR::WHITE) {
		const int deltaX = abs(x2 - x1);
		const int deltaY = abs(y2 - y1);
		const int signX = x1 < x2 ? 1 : -1;
		const int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;

		DrawPoint2D(x2, y2, pixel, color);
		while (x1 != x2 || y1 != y2)
		{
			DrawPoint2D(x1, y1, pixel, color);
			const int error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
		}
	}

	std::pair<float, float> PointA, PointB;

public:
	SHORT mousePosX, mousePosY;
	short ScreenHeight, ScreenWidth;
	int PixelSize;
	Matrix Projection;
	Matrix View;
	Matrix Model;


	GameEngine(short ScreenHeight, short ScheenWidth, int PixelSize) {
		
		this->PixelSize = PixelSize;
		this->ScreenHeight = ScreenHeight;
		this->ScreenWidth = ScheenWidth;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

		hwndConsole = GetConsoleWindow();


		SetConsoleTitle(L"GameEngine v1.0");

		if (hConsole == INVALID_HANDLE_VALUE)
			return; //return Error(L"Bad Handle");


		RectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(hConsole, TRUE, &RectWindow);

		// Set the size of the screen buffer
		COORD coord = { (short)ScreenWidth, (short)ScreenHeight };
		if (!SetConsoleScreenBufferSize(hConsole, coord))
			return;//Error(L"SetConsoleScreenBufferSize");

					 // Assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(hConsole))
			return;//return Error(L"SetConsoleActiveScreenBuffer");

					 // Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = PixelSize;
		cfi.dwFontSize.Y = PixelSize;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;


		wcscpy_s(cfi.FaceName, L"Console");
		if (!SetCurrentConsoleFontEx(hConsole, false, &cfi))
			return;//return Error(L"SetCurrentConsoleFontEx");

					 // Get screen buffer info and check the maximum allowed window size. Return
					 // error if exceeded, so user knows their dimensions/fontsize are too large
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
			return; //return Error(L"GetConsoleScreenBufferInfo");
		if (ScreenHeight > csbi.dwMaximumWindowSize.Y)
			return; //return Error(L"Screen Height / Font Height Too Big");
		if (ScreenWidth > csbi.dwMaximumWindowSize.X)
			return; //return Error(L"Screen Width / Font Width Too Big");

					  // Set Physical Console Window Size
		RectWindow = { 0, 0, (short)ScreenWidth - 1, (short)ScreenHeight - 1 };
		if (!SetConsoleWindowInfo(hConsole, TRUE, &RectWindow))
			return; //return Error(L"SetConsoleWindowInfo");

		if (!SetConsoleMode(hConsoleInput, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT));
					  // Allocate memory for screen buffer
		Screen = new CHAR_INFO[ScreenWidth*ScreenHeight];
		memset(Screen, 0, sizeof(CHAR_INFO) * ScreenWidth * ScreenHeight);

		//SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);

		MatrixInit();
	}

	void MatrixInit(int Size = 4) {
		Projection = Matrix(Size, Size);
		View = Matrix(Size, Size);
		Model = Matrix(Size, Size);


		SetPerspectiveProjection(90);
		SetViewMatrix();
		SetModelMatrix();
	}
	
	HWND GetConsoleHWND() {
		return hwndConsole;
	}

	void MouseEvent() {

		//RECT rectClient, rectWindow;
		//GetClientRect(hwndConsole, &rectClient);
		//GetWindowRect(hwndConsole, &rectWindow);
		////int posx, posy;
		////posx = GetSystemMetrics(SM_CXSCREEN) / 2 - (rectWindow.right - rectWindow.left) / 2;
		////posy = GetSystemMetrics(SM_CYSCREEN) / 2 - (rectWindow.bottom - rectWindow.top) / 2;


		//DrawString(-ScreenWidth / 2, ScreenHeight / 2 - 18, std::to_string(rectWindow.left) + " " + std::to_string(rectWindow.top) + " " + std::to_string(rectWindow.right) + " " + std::to_string(rectWindow.bottom));
		//DrawString(-ScreenWidth / 2, ScreenHeight / 2 - 24, std::to_string(rectClient.left) + " " + std::to_string(rectClient.top) + " " + std::to_string(rectClient.right) + " " + std::to_string(rectClient.bottom));
		//if(GetAsyncKeyState(int(18)) & 0x8000){
	
		//}
		//else {
		//
		//	SetCursorPos((rectWindow.left + rectWindow.right) / 2, (rectWindow.top + rectWindow.bottom) / 2);
		//}
		


		INPUT_RECORD inBuf[32];
		DWORD events = 0;
		GetNumberOfConsoleInputEvents(hConsoleInput, &events);
		if (events > 0)
			ReadConsoleInput(hConsoleInput, inBuf, events, &events);

		// Handle events - we only care about mouse clicks and movement
		// for now
		for (DWORD i = 0; i < events; i++)
		{
			switch (inBuf[i].EventType)
			{
			/*case FOCUS_EVENT:
			{
				m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
			}
			break;*/

			case MOUSE_EVENT:
			{
				switch (inBuf[i].Event.MouseEvent.dwEventFlags)
				{
				case MOUSE_MOVED:
				{
					mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
					mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
				}
				break;

				case 0:
				{
					//for (int m = 0; m < 5; m++)
					//	mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

				}
				break;

				default:
					break;
				}
			}
			break;

			default:
				break;
				// We don't care just at the moment
			}
		}
	}

	void SetPerspectiveProjection(float FOV) {
		Projection.Data[0] = (float(ScreenHeight) / float(ScreenWidth)) * (1.0f / (tanf((FOV / 2.0f)*(PI / 180.0f))));
		Projection.Data[1 * Projection.columns + 1] = 1.0f / (tanf((FOV / 2.0f)*(PI / 180.0f)));

		/*Projection.Data[2 * Projection.columns + 2] = (-1) * (zFar + zNear) / (zFar - zNear);
		Projection.Data[2 * Projection.columns + 3] = ((-2) * zFar * zNear) / (zFar - zNear);
		Projection.Data[3 * Projection.columns + 2] = -1;*/

		Projection.Data[2 * Projection.columns + 2] = zFar / (zFar - zNear);
		Projection.Data[2 * Projection.columns + 3] = 1;
		Projection.Data[3 * Projection.columns + 2] = zFar * zNear / (zNear - zFar);

	}
	void SetViewMatrix() {
		View.Data[0 * View.columns + 0] = RightX;
		View.Data[1 * View.columns + 0] = RightY;
		View.Data[2 * View.columns + 0] = RightZ;
	
		View.Data[0 * View.columns + 1] = UpX;
		View.Data[1 * View.columns + 1] = UpY;
		View.Data[2 * View.columns + 1] = UpZ;
	
		View.Data[0 * View.columns + 2] = ForwardX;
		View.Data[1 * View.columns + 2] = ForwardY;
		View.Data[2 * View.columns + 2] = ForwardZ;

		View.Data[0 * View.columns + 3] = PositionX;
		View.Data[1 * View.columns + 3] = PositionY;
		View.Data[2 * View.columns + 3] = PositionZ;

		View.Data[3 * View.columns + 3] = 1;
	}
	void SetModelMatrix() {
		Model.SetUnionMatrix();
	}

	inline void DrawPoint2D(int x, int y, Pixel pixel = Pixel::SOLID, COLOR color = COLOR::WHITE) {
		x += ScreenWidth / 2;
		y += ScreenHeight / 2;
		if (x < ScreenWidth && y < ScreenHeight && x >= 0 && y >= 0) {
			Screen[(ScreenHeight - y - 1)*ScreenWidth + x].Char.UnicodeChar = pixel;
			Screen[(ScreenHeight - y - 1)*ScreenWidth + x].Attributes = color;
		}
	}

	std::pair<float, float> DrawPoint3D(Matrix Point, Pixel pixel = Pixel::SOLID, COLOR color = COLOR::WHITE) {
		Matrix *tempPoint1 = new Matrix(4, 4);
		Matrix *tempPoint2 = new Matrix(4, 4);
		Matrix *tempPoint3 = new Matrix(4, 4);

		Matrix *NewPoint = new Matrix(4, 1);
		
		NewPoint->Data[0] = Point.Data[0];
		NewPoint->Data[1] = Point.Data[1];
		NewPoint->Data[2] = Point.Data[2];
		NewPoint->Data[3] = 1;
		
		Mult(4, 4, 4, Projection.Data, View.Data, tempPoint1->Data);
		Mult(4, 4, 4, tempPoint1->Data, Model.Data, tempPoint2->Data);
		Mult(4, 1, 4, tempPoint2->Data, NewPoint->Data, tempPoint3->Data);
		delete NewPoint;


		if (tempPoint3->Data[2] != 0) {
			float Xf = (ScreenWidth * tempPoint3->Data[0]) / tempPoint3->Data[2];
			float Yf = (ScreenHeight * tempPoint3->Data[1]) / tempPoint3->Data[2];

			delete tempPoint1;
			delete tempPoint2;
			delete tempPoint3;

			DrawPoint2D(Xf, Yf, pixel, color);

			return std::make_pair(Xf, Yf);
		}
		delete tempPoint1;
		delete tempPoint2;
		delete tempPoint3;

		return std::make_pair(ErrorCount, ErrorCount);
	}

	void DrawLine(Matrix PointA, Matrix PointB, Pixel pixel = Pixel::SOLID, COLOR color = COLOR::WHITE) {

		this->PointA = DrawPoint3D(PointA);
		this->PointB = DrawPoint3D(PointB);

		//if(this->PointA.first != ErrorCount && this->PointA.second != ErrorCount && this->PointB.first != ErrorCount && this->PointB.second != ErrorCount)
			drawLine(this->PointA.first, this->PointA.second, this->PointB.first, this->PointB.second, pixel, color);
	}

	void DrawTriangle(Matrix PointA, Matrix PointB, Matrix PointC, Pixel pixel = Pixel::SOLID, COLOR color = COLOR::WHITE) {
		DrawLine(PointA, PointB, pixel, color);
		DrawLine(PointB, PointC, pixel, color);
		DrawLine(PointC, PointA, pixel, color);
	}

	inline void DrawSymbol(int posX, int posY, char symbol) { // Left Down Position
		switch (symbol) {
		case '0':
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX, posY + 1);
			DrawPoint2D(posX, posY + 2);
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX + 2, posY + 2);
			DrawPoint2D(posX + 2, posY + 3);
			break;
		case '1':
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX + 1, posY + 1);
			DrawPoint2D(posX + 1, posY + 2);
			DrawPoint2D(posX + 1, posY + 3);
			DrawPoint2D(posX + 1, posY + 4);
			break;
		case '2':
			DrawPoint2D(posX, posY);
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX + 2, posY);
			DrawPoint2D(posX, posY + 1);
			DrawPoint2D(posX + 1, posY + 2);
			DrawPoint2D(posX + 2, posY + 3);
			DrawPoint2D(posX + 2, posY + 4);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX, posY + 4);
			break;
		case '3':
			DrawPoint2D(posX, posY);
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX + 2, posY);
			DrawPoint2D(posX, posY + 4);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX + 2, posY + 4);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX + 2, posY + 3);
			DrawPoint2D(posX + 1, posY + 2);
			break;
		case '4':
			DrawPoint2D(posX + 2, posY);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX + 2, posY + 2);
			DrawPoint2D(posX + 2, posY + 3);
			DrawPoint2D(posX + 2, posY + 4);
			DrawPoint2D(posX + 1, posY + 2);
			DrawPoint2D(posX, posY + 2);
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX, posY + 4);
			break;
		case '5':
			DrawPoint2D(posX, posY);
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX + 1, posY + 2);
			DrawPoint2D(posX, posY + 2);
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX, posY + 4);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX + 2, posY + 4);
			break;
		case '6':
			DrawPoint2D(posX, posY);
			DrawPoint2D(posX, posY + 1);
			DrawPoint2D(posX, posY + 2);
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX, posY + 4);
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX + 1, posY + 2);
			break;
		case '7':
			DrawPoint2D(posX, posY);
			DrawPoint2D(posX, posY + 1);
			DrawPoint2D(posX + 1, posY + 2);
			DrawPoint2D(posX + 2, posY + 3);
			DrawPoint2D(posX + 2, posY + 4);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX, posY + 4);
			break;
		case '8':
			DrawPoint2D(posX, posY);
			DrawPoint2D(posX + 1, posY);
			DrawPoint2D(posX + 2, posY);
			DrawPoint2D(posX + 1, posY + 2);
			DrawPoint2D(posX, posY + 1);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX, posY + 4);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX + 2, posY + 4);
			DrawPoint2D(posX + 2, posY + 3);
			break;
		case '9':
			DrawPoint2D(posX + 2, posY);
			DrawPoint2D(posX + 2, posY + 1);
			DrawPoint2D(posX + 2, posY + 2);
			DrawPoint2D(posX + 2, posY + 3);
			DrawPoint2D(posX + 2, posY + 4);
			DrawPoint2D(posX + 1, posY + 4);
			DrawPoint2D(posX, posY + 4);
			DrawPoint2D(posX, posY + 3);
			DrawPoint2D(posX + 1, posY + 2);
			break;
		case '.':
			DrawPoint2D(posX + 1, posY);
			break;
		case '-':
			DrawPoint2D(posX, posY + 2);
			DrawPoint2D(posX+1, posY + 2);
			DrawPoint2D(posX+2, posY + 2);
			break;
		default:
			break;
		}
	}


	void DrawSquare2D(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, COLOR color = COLOR::WHITE) {
		drawLine(x1, y1, x2, y2, SOLID, color);
		drawLine(x2, y2, x3, y3, SOLID, color);
		drawLine(x3, y3, x4, y4, SOLID, color);
		drawLine(x4, y4, x1, y1, SOLID, color);
	}

	void DrawString(int posX, int posY, std::string line) {
		for (int i = 0; i < line.size(); ++i)
			DrawSymbol(posX + i * 4, posY, line[i]);
	}

	void PrintScreen() {
		WriteConsoleOutput(hConsole, Screen, {ScreenWidth, ScreenHeight}, {0,0}, &RectWindow);
	}
	void ClearScreen() {
		for (int i = 0; i < ScreenHeight*ScreenWidth; ++i) {
			Screen[i].Char.UnicodeChar = L' ';
			Screen[i].Attributes = COLOR::BLACK;
		}
	}

	void triangle(int t0x, int t0y, int t1x, int t1y, int t2x, int t2y, TGAImage &image, TGAColor color) {
		if (t0y == t1y && t0y == t2y) return; // i dont care about degenerate triangles
		if (t0y>t1y) std::swap(t0, t1);
		if (t0y>t2y) std::swap(t0, t2);
		if (t1y>t2y) std::swap(t1, t2);
		int total_height = t2.y - t0.y;
		for (int i = 0; i<total_height; i++) {
			bool second_half = i>t1.y - t0.y || t1.y == t0.y;
			int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
			Vec2i A = t0 + (t2 - t0)*alpha;
			Vec2i B = second_half ? t1 + (t2 - t1)*beta : t0 + (t1 - t0)*beta;
			if (A.x>B.x) std::swap(A, B);
			for (int j = A.x; j <= B.x; j++) {
				image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
			}
		}
	}

	~GameEngine() {
		CloseHandle(hConsole);
		CloseHandle(hConsoleInput);
	}
};