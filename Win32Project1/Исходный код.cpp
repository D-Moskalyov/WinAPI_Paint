#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include "resource.h"
#include <Commctrl.h>
#include <shlobj.h>
#pragma warning(disable : 4996)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//HINSTANCE hInst;
BOOL CALLBACK DlgProc(HWND,UINT,WPARAM,LPARAM);

HWND hWndStatusBar; 
RECT rect;

char path1[MAX_PATH]="style.txt\0";
char path2[MAX_PATH]="koord.txt\0";
char path1tmp[MAX_PATH];
char path2tmp[MAX_PATH];

OPENFILENAME ofn;
char FileDefaultName[MAX_PATH]="SomeFile\0";

BROWSEINFO odn;
char DirDefaultName[MAX_PATH]="SomeDir\0";
LPITEMIDLIST pidl;

CHOOSECOLOR clr;

char WinName[] = "MainFrame";

class SLine{
public:
	SLine(){
		style=0;
		size=0;
		col=RGB(0,0,0);
	}
	int style;
	int size;
	COLORREF col;
	std::vector<POINT> points;
/* 	void operator =(SLine tmp){
		this->col=tmp.col;
		this->style=tmp.style;
		this->size=tmp.size;
	} */
};

POINT buffP;
SLine buffL;
std::vector<SLine> lines;

int CursorCoordX=0;
int CursorCoordY=0;

char buff[10];
int style=0;
int thick=4;
int styleTmp;
int sizeTmp;
COLORREF colTmp;
int rTmp=0,gTmp=0,bTmp=0;
COLORREF color=RGB(255,0,0);

std::ofstream ofileKoord;
std::ifstream ifileKoord;
std::ofstream ofileStyle;
std::ifstream ifileStyle;

int X1 = 0, X2=0;
int Y1 = 0, Y2=0;
int X1Tmp = 0, X2Tmp=0;
int Y1Tmp = 0, Y2Tmp=0;
int n=0, k=1;
bool flag = 0, flag2=0, click=0;
static COLORREF acrCustClr[16];
static DWORD rgbCurrent;

std::string str;

HPEN penTmp;

HDC hDC;
INT_PTR nResult;

/* void Paint(std::vector<SLine> vect){
	for(int j=0; j<i; j++){
		HPEN pen = CreatePen(style, thick, color);
		PAINTSTRUCT ps;
		hDC=GetDC(hWnd);
		SelectObject(hDC, pen);
		MoveToEx(hDC, X1, Y1, NULL);
		LineTo(hDC, X2, Y2);
		DeleteObject(pen);
		ReleaseDC(hWnd, hDC);
		j++;
	}
	return;
} */

int APIENTRY WinMain(HINSTANCE This,		
	HINSTANCE Prev,							
	LPTSTR cmd,								
	int mode)								
{
	HWND hWnd;								
	MSG msg;								
	WNDCLASS wc;							

											
	wc.hInstance = This;		
	wc.lpszClassName = WinName;				
	wc.lpfnWndProc = WndProc;					
	wc.style = CS_HREDRAW | CS_VREDRAW;			
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);	
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);	
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.cbClsExtra = 0;		
	wc.cbWndExtra = 0;		
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	if(!RegisterClass(&wc)) return 0;	

	hWnd = CreateWindow(WinName, "Window", WS_OVERLAPPEDWINDOW , 10, 10 , 500, 500, HWND_DESKTOP, NULL, This, NULL);

	ShowWindow(hWnd, mode); 

	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hWnd;
	ofn.lpstrFile=FileDefaultName;
	ofn.nMaxFile=1024;
	ofn.lpstrFilter="All\0*.*\0Text\0*.TXT\0\0";
	ofn.nFilterIndex=1;
	ofn.Flags=OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	odn.hwndOwner=hWnd;
	odn.pszDisplayName=DirDefaultName;
	odn.ulFlags = BIF_RETURNONLYFSDIRS;  

	/*ZeroMemory(&clr, sizeof(clr));*/
	clr.lStructSize=sizeof(clr);
	clr.hwndOwner=hWnd;
	clr.lpCustColors = (LPDWORD) acrCustClr;
	clr.Flags = CC_FULLOPEN | CC_RGBINIT;
	clr.rgbResult=rgbCurrent;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}

	return 0;
}

BOOL CALLBACK SettingDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) 
	{
		case WM_INITDIALOG:{
			//break;
			return true;
		}
	case WM_PAINT:{}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) 
		{

			case IDOK:{
				rTmp=GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
				gTmp=GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
				bTmp=GetDlgItemInt(hDlg, IDC_EDIT4, NULL, FALSE);
				if(rTmp!=0||gTmp!=0||bTmp!=0)
					color=RGB(rTmp, gTmp, bTmp);
				//int h=GetDlgItemInt(hDlg, IDC_SLIDER1, NULL, FALSE);
				//thick=(h/33+1)*4;
					  }
			case IDCANCEL:
				EndDialog(hDlg, 0);
				return TRUE;
		}
		break;
	}
	case WM_CLOSE: {  
        SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);   
        break; 
				   }
	//break;
 }
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
				 WPARAM wParam, LPARAM lParam)
{	
	static HINSTANCE hInst;
	switch(message)
	{
	case WM_DESTROY : {
		ofileStyle.open(path1, std::ios::out);
		ofileKoord.open(path2, std::ios::out);
		for(int j=0; j<n; j++){
			/*MessageBox(hWnd, "destroy",0,0);*/
			buffL=lines[j];
			/*lines.pop_back();*/
			std::string numS=std::to_string((int)j);
			std::string sizeS=std::to_string((int)buffL.size);
			std::string styleS=std::to_string((int)buffL.style);
			std::string rS=std::to_string((int)GetRValue(buffL.col));
			std::string gS=std::to_string((int)GetGValue(buffL.col));
			std::string bS=std::to_string((int)GetBValue(buffL.col));
			std::string toFile=numS+"."+sizeS+"."+styleS+"."+rS+"."+gS+"."+bS+'\n';
			//itoa(j, buff, 10);
			//strcat(buff, ".");
			ofileStyle.write(toFile.c_str(), strlen(toFile.c_str()));
			for(int i=0; i<lines[j].points.size(); i++){
				buffP=lines[j].points[i];
				std::string pointXS=std::to_string((int)buffP.x);
				std::string pointYS=std::to_string((int)buffP.y);
				std::string toFile=numS+"."+pointXS+"."+pointYS+'\n';
				ofileKoord.write(toFile.c_str(), strlen(toFile.c_str()));
			}
		}
		ofileStyle.close();
		ofileKoord.close();
		PostQuitMessage(0); 
		break; 
	}
	case WM_CREATE:{
		//hWndStatusBar = CreateStatusWindow (WS_CHILD | WS_VISIBLE, "Status Bar", hWnd, 222);
		//SetWindowPos(hWndStatusBar, HWND_TOP, 0,0,100,100,SWP_SHOWWINDOW);

		hWndStatusBar = CreateWindow(STATUSCLASSNAME, "Status Message", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_BORDER,
			0,0,0,0,hWnd,(HMENU)2555,NULL,0);
		
		int ptWidth[2] = {150,450}; 

		SendMessage(hWndStatusBar, SB_SETPARTS, 2, (LPARAM)ptWidth);
		ifileStyle.open(path1, std::ios::_Nocreate);
		ifileKoord.open(path2, std::ios::_Nocreate);
		if(ifileStyle.is_open()&&ifileKoord.is_open()){
			while((ifileStyle.peek()!=EOF)){
				ifileStyle.getline(buff, 10, '.');	n=atoi(buff);
				str=std::to_string((int)n);
				/*MessageBox(hWnd, str.c_str(), "n", 0);*/
				ifileStyle.getline(buff, 10, '.');	buffL.size=atoi(buff);
				ifileStyle.getline(buff, 10, '.');	buffL.style=atoi(buff);
				ifileStyle.getline(buff, 10, '.');	rTmp=atoi(buff);
				ifileStyle.getline(buff, 10, '.');	gTmp=atoi(buff);
				ifileStyle.getline(buff, 10, '\n');	bTmp=atoi(buff);
				colTmp=RGB(rTmp, gTmp, bTmp);
				buffL.col=colTmp;
				lines.push_back(buffL);
				ifileKoord.getline(buff, 10, '.');
				/*MessageBox(hWnd, buff, "buff", 0);*/
				k=atoi(buff);
				str=std::to_string((int)k);
				/*MessageBox(hWnd, str.c_str(), "k", 0);*/
				while(k==n){
					/*MessageBox(hWnd, "k=n",0 ,0);*/
					ifileKoord.getline(buff, 10, '.');	buffP.x=atoi(buff);
					ifileKoord.getline(buff, 10, '\n');	buffP.y=atoi(buff);
					lines[n].points.push_back(buffP);
					if((ifileKoord.peek()!=EOF)){
						ifileKoord.getline(buff, 10, '.');
						k=atoi(buff);
						if(k!=n){
							int z=ifileKoord.tellg(); 
							str=std::to_string((int)z);
							/*MessageBox(hWnd, str.c_str(), "z", 0);*/
							ifileKoord.seekg(z-strlen(buff)-1);
						}
						str=std::to_string((int)k);
						/*MessageBox(hWnd, str.c_str(),"k" ,0);*/
					}
					else{
						/*MessageBox(hWnd, "noCreate",0 ,0);*/
						k+=2;
						n++;
					}
				}
			}
			ifileStyle.close();
			ifileKoord.close();
			GetClientRect(hWnd, &rect);
			InvalidateRect(hWnd, &rect, false);
			//for (int j=0; j<n; j++){
			//	styleTmp=lines[j].style;
			//	sizeTmp=lines[j].size;
			//	colTmp=lines[j].col;
			//	penTmp = CreatePen(styleTmp, sizeTmp, colTmp);
			//	hDC=GetDC(hWnd);
			//	SelectObject(hDC, penTmp);
			//	int m=lines[j].points.size()-1;
			//	/*POINT tmpP;*/
			//	for (int i=0; i<m; i++){
			//		X1Tmp=lines[j].points[m-i].x;
			//		X2Tmp=lines[j].points[m-i-1].x;
			//		Y1Tmp=lines[j].points[m-i].y;
			//		Y2Tmp=lines[j].points[m-i-1].y;
			//		MessageBox(hWnd, "point",0 ,0);
			//		MoveToEx(hDC, X1Tmp, Y1Tmp, NULL);
			//		LineTo(hDC, X2Tmp, Y2Tmp);
			//		ReleaseDC(hWnd, hDC);
			//		/*RECT rect;
			//		GetClientRect(hWnd, &rect);
			//		InvalidateRect(hWnd, &rect, true);*/
			//		/*tmpP.x=X1Tmp;
			//		tmpP.y=Y1Tmp;*/
			//	}
			//	DeleteObject(penTmp);
			//}
		}
		break;
	}
	case WM_LBUTTONDOWN:{
		flag2=1;
		X1 = LOWORD(lParam);
		Y1 = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONUP:{
		buffL.col=color;
		buffL.size=thick;
		buffL.style=style;
		lines.push_back(buffL);
		for(int i=0; i<buffL.points.size(); i++)
			buffL.points.pop_back();
		flag2=0;
		n++;
		/* MessageBox(hWnd, "fdsf",0 ,0);
		if(!click){
			X1 = LOWORD(lParam);
			Y1 = HIWORD(lParam);
			click=1;
		}
		else{	
			X2 = LOWORD(lParam);
			Y2 = HIWORD(lParam);
			//char buff[10];
			//itoa(X2, buff, 10);
			//MessageBox(hWnd, buff,0 ,0);
			HPEN pen = CreatePen(style, thick, color);
			// PAINTSTRUCT ps;
			hDC=GetDC(hWnd);
			SelectObject(hDC, pen);
			MoveToEx(hDC, X1, Y1, NULL);
			LineTo(hDC, X2, Y2);
			DeleteObject(pen);
			ReleaseDC(hWnd, hDC);
			click=0;
			buffL.col=color;
			buffL.size=thick;
			buffL.style=style;
			buffP.x=X1;
			buffP.y=Y1;
			buffL.points.push_back(buffP);
			buffP.x=X2;
			buffP.y=Y2;
			buffL.points.push_back(buffP);
			lines.push_back(buffL);
			buffL.points.pop_back(); buffL.points.pop_back();
			n++;
			UpdateWindow(hWnd);
		} */
		break;
	}
	case WM_SYSCOMMAND:{
		/*if(wParam==SC_RESTORE)*/
			/*MessageBox(hWnd, "!!!!!", 0,0);*/
		break;
	}
	case WM_ERASEBKGND:{
		/*MessageBox(hWnd, "Erase", 0, 0);*/
		if(flag) {flag=0;	/*MessageBox(hWnd, "ERASE", 0,0);*/	break;}
		if(!flag){
			/*MessageBox(hWnd, "ERASEno", 0,0);*/
			/*RECT rect;
			GetClientRect(hWnd, &rect);
			InvalidateRect(hWnd, &rect, true);*/
			 /*for (int j=0; j<n; j++){
				styleTmp=lines[j].style;
				sizeTmp=lines[j].size;
				colTmp=lines[j].col;
				HPEN penTmp = CreatePen(styleTmp, sizeTmp, colTmp);
				hDC=GetDC(hWnd);
				SelectObject(hDC, penTmp);
				int m=lines[j].points.size()-1;
				for (int i=0; i<m; i++){
					MessageBox(hWnd, "dasd", 0,0);
					X1Tmp=lines[j].points[m-i].x;
					X2Tmp=lines[j].points[m-i-1].x;
					Y1Tmp=lines[j].points[m-i].y;
					Y2Tmp=lines[j].points[m-i-1].y;
					MoveToEx(hDC, X1Tmp, Y1Tmp, NULL);
					LineTo(hDC, X2Tmp, Y2Tmp);
					ReleaseDC(hWnd, hDC);	
				}
				DeleteObject(penTmp);			
			} */
			return 1;
		}
	}

	case WM_SIZE:{
			flag=1;
			RECT rect;
			GetClientRect(hWnd, &rect);
			InvalidateRect(hWnd, &rect, false);
			/*UpdateWindow(hWnd);*/
			/*MessageBox(hWnd, "size", 0, 0);*/
			//for (int j=0; j<n; j++){
			//	styleTmp=lines[j].style;
			//	sizeTmp=lines[j].size;
			//	colTmp=lines[j].col;
			//	HPEN penTmp = CreatePen(styleTmp, sizeTmp, colTmp);
			//	hDC=GetDC(hWnd);
			//	SelectObject(hDC, penTmp);
			//	int m=lines[j].points.size()-1;
			//	/*MessageBox(hWnd, "sr", 0, 0);*/
			//	for (int i=0; i<m; i++){
			//		/*MessageBox(hWnd, "dasd", 0,0);*/
			//		X1Tmp=lines[j].points[m-i].x;
			//		X2Tmp=lines[j].points[m-i-1].x;
			//		Y1Tmp=lines[j].points[m-i].y;
			//		Y2Tmp=lines[j].points[m-i-1].y;
			//		MoveToEx(hDC, X1Tmp, Y1Tmp, NULL);
			//		LineTo(hDC, X2Tmp, Y2Tmp);
			//		ReleaseDC(hWnd, hDC);	
			//	}
			//	DeleteObject(penTmp);			
			//}
            SendMessage(hWndStatusBar, WM_SIZE, 100, 20);
			//SendMessage(hWndStatusBar, WM_SETTEXT, 0, (LPARAM) "Message");
			/*RECT rect;
			GetClientRect(hWnd, &rect);
			UpdateWindow(hWnd);*/
			break;
	}

	case WM_MOUSEMOVE:{
			CursorCoordX = LOWORD(lParam);
			CursorCoordY = HIWORD(lParam);
	
			std::string XCoord = std::to_string((long double)CursorCoordX);
			std::string YCoord = std::to_string((long double)CursorCoordY);
			std::string cursorStatusMessage = "Cursor: X = " + XCoord + "| Y = " + YCoord;

			SendMessage(hWndStatusBar, SB_SETTEXT, 1 , (LPARAM)cursorStatusMessage.c_str());

			RECT rct;
			GetWindowRect(hWnd, &rct);
			std::string str_LX = std::to_string((long double)rct.left + rct.right);
			std::string str_LY = std::to_string((long double)rct.top +  rct.bottom);
			std::string str_Left_Corner = "Left X = "  + str_LX + "Left Y " + str_LY;

			SendMessage(hWndStatusBar,SB_SETTEXT,0  , (LPARAM)"This is Status Bar");
			if(flag2){
				X2 = LOWORD(lParam);
				Y2 = HIWORD(lParam);
				HPEN pen = CreatePen(style, thick, color);
				hDC=GetDC(hWnd);
				SelectObject(hDC, pen);
				MoveToEx(hDC, X1, Y1, NULL);
				LineTo(hDC, X2, Y2);
				DeleteObject(pen);
				ReleaseDC(hWnd, hDC);
				X1=X2; Y1=Y2;
				
				buffP.x=X1;
				buffP.y=Y1;
				buffL.points.push_back(buffP);
				buffP.x=X2;
				buffP.y=Y2;
				buffL.points.push_back(buffP);
				
				UpdateWindow(hWnd);
			}
			break;
		}

		case WM_PAINT:	
			{	
			for (int j=0; j<n; j++){
				/*MessageBox(hWnd, "paint", 0, 0);*/
				styleTmp=lines[j].style;
				sizeTmp=lines[j].size;
				colTmp=lines[j].col;
				HPEN penTmp = CreatePen(styleTmp, sizeTmp, colTmp);
				hDC=GetDC(hWnd);
				SelectObject(hDC, penTmp);
				int m=lines[j].points.size()-1;
				/*MessageBox(hWnd, "sr", 0, 0);*/
				for (int i=0; i<m; i++){
					/*MessageBox(hWnd, "dasd", 0,0);*/
					X1Tmp=lines[j].points[m-i].x;
					X2Tmp=lines[j].points[m-i-1].x;
					Y1Tmp=lines[j].points[m-i].y;
					Y2Tmp=lines[j].points[m-i-1].y;
					MoveToEx(hDC, X1Tmp, Y1Tmp, NULL);
					LineTo(hDC, X2Tmp, Y2Tmp);
					/*ReleaseDC(hWnd, hDC);	*/
				}
				DeleteObject(penTmp);			
			}
			break;
		}

		case WM_COMMAND:
		{
			if(LOWORD(wParam)==ID_SETTING){
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, SettingDlgProc);
				break;
			}
			if(LOWORD(wParam)==ID_COLOR){
				ChooseColor(&clr);
				color=clr.rgbResult;
				break;
			}
			if(LOWORD(wParam)==ID_FILE_LOAD){
				GetOpenFileName(&ofn);
				MessageBox(NULL, ofn.lpstrFile, "TEST", MB_OK);
				break;
			}
			if(LOWORD(wParam)==ID_FILE_SAVE40016){
				GetSaveFileName(&ofn);
				MessageBox(NULL, ofn.lpstrFile, "TEST", MB_OK);
				break;
			}
			if(LOWORD(wParam)==ID_DIR_OPEN){
				pidl=SHBrowseForFolder(&odn);
				if(pidl)
					SHGetPathFromIDList(pidl, DirDefaultName);
				/*MessageBox(NULL, odn.pszDisplayName, "TEST", MB_OK);*/
				strcpy(path1tmp, odn.pszDisplayName);
				strcpy(path2tmp, odn.pszDisplayName);

				strcat(path1tmp, "/");		strcat(path1tmp, "style.txt");
				strcat(path2tmp, "/");		strcat(path2tmp, "koord.txt");
				ifileStyle.open(path1tmp, std::ios::in | std::ios::_Nocreate);
				ifileKoord.open(path2tmp, std::ios::in | std::ios::_Nocreate);

				if(ifileStyle.is_open()&&ifileKoord.is_open()){
					ofileStyle.open(path1, std::ios::out);
					ofileKoord.open(path2, std::ios::out);
					for(int j=0; j<n; j++){
						/*MessageBox(hWnd, "destroy",0,0);*/
						buffL=lines[j];
						/*lines.pop_back();*/
						std::string numS=std::to_string((int)j);
						std::string sizeS=std::to_string((int)buffL.size);
						std::string styleS=std::to_string((int)buffL.style);
						std::string rS=std::to_string((int)GetRValue(buffL.col));
						std::string gS=std::to_string((int)GetGValue(buffL.col));
						std::string bS=std::to_string((int)GetBValue(buffL.col));
						std::string toFile=numS+"."+sizeS+"."+styleS+"."+rS+"."+gS+"."+bS+'\n';
						//itoa(j, buff, 10);
						//strcat(buff, ".");
						ofileStyle.write(toFile.c_str(), strlen(toFile.c_str()));
						for(int i=0; i<lines[j].points.size(); i++){
							buffP=lines[j].points[i];
							std::string pointXS=std::to_string((int)buffP.x);
							std::string pointYS=std::to_string((int)buffP.y);
							std::string toFile=numS+"."+pointXS+"."+pointYS+'\n';
							ofileKoord.write(toFile.c_str(), strlen(toFile.c_str()));
						}
					}
					ofileStyle.close();
					ofileKoord.close();

					click=0;
					lines.clear();
					//for(int i=0; i<n; i++){
					//	//s=lines[n-i-1].points.size();
					//	//for(int j=0; j<s; j++)
					//	//	lines[n-i-1].points.pop_back();
					//	//MessageBox(hWnd, "lines-",0,0);
					//	lines.pop_back();
					//}
					n=0;
					flag=1;
					buffL.points.clear();
					/*for(int i=0; i<t; i++)
						buffL.points.pop_back();*/
					GetClientRect(hWnd, &rect);
					InvalidateRect(hWnd, &rect, true);

					while((ifileStyle.peek()!=EOF)){
						ifileStyle.getline(buff, 10, '.');	n=atoi(buff);
						str=std::to_string((int)n);
						/*MessageBox(hWnd, str.c_str(), "n", 0);*/
						ifileStyle.getline(buff, 10, '.');	buffL.size=atoi(buff);
						ifileStyle.getline(buff, 10, '.');	buffL.style=atoi(buff);
						ifileStyle.getline(buff, 10, '.');	rTmp=atoi(buff);
						ifileStyle.getline(buff, 10, '.');	gTmp=atoi(buff);
						ifileStyle.getline(buff, 10, '\n');	bTmp=atoi(buff);
						colTmp=RGB(rTmp, gTmp, bTmp);
						buffL.col=colTmp;
						lines.push_back(buffL);
						ifileKoord.getline(buff, 10, '.');
						/*MessageBox(hWnd, buff, "buff", 0);*/
						k=atoi(buff);
						str=std::to_string((int)k);
						/*MessageBox(hWnd, str.c_str(), "k", 0);*/
						while(k==n){
							/*MessageBox(hWnd, "k=n",0 ,0);*/
							ifileKoord.getline(buff, 10, '.');	buffP.x=atoi(buff);
							ifileKoord.getline(buff, 10, '\n');	buffP.y=atoi(buff);
							lines[n].points.push_back(buffP);
							if((ifileKoord.peek()!=EOF)){
								ifileKoord.getline(buff, 10, '.');
								k=atoi(buff);
								if(k!=n){
									int z=ifileKoord.tellg(); 
									str=std::to_string((int)z);
									/*MessageBox(hWnd, str.c_str(), "z", 0);*/
									ifileKoord.seekg(z-strlen(buff)-1);
								}
								/*str=std::to_string((int)k);*/
								/*MessageBox(hWnd, str.c_str(),"k" ,0);*/
							}
							else{
								/*MessageBox(hWnd, "noCreate",0 ,0);*/
								k+=2;
								n++;
							}
						}
					}
					ifileStyle.close();
					ifileKoord.close();
					GetClientRect(hWnd, &rect);
					InvalidateRect(hWnd, &rect, false);
					//for (int j=0; j<n; j++){
					//	styleTmp=lines[j].style;
					//	sizeTmp=lines[j].size;
					//	colTmp=lines[j].col;
					//	int m=lines[j].points.size()-1;
					//	/*POINT tmpP;*/
					//	for (int i=0; i<m; i++){
					//		X1Tmp=lines[j].points[m-i].x;
					//		X2Tmp=lines[j].points[m-i-1].x;
					//		Y1Tmp=lines[j].points[m-i].y;
					//		Y2Tmp=lines[j].points[m-i-1].y;
					//		penTmp = CreatePen(styleTmp, sizeTmp, colTmp);
					//		hDC=GetDC(hWnd);
					//		SelectObject(hDC, penTmp);
					//		MoveToEx(hDC, X1Tmp, Y1Tmp, NULL);
					//		LineTo(hDC, X2Tmp, Y2Tmp);
					//		DeleteObject(penTmp);
					//		ReleaseDC(hWnd, hDC);
					//		/*tmpP.x=X1Tmp;
					//		tmpP.y=Y1Tmp;*/
					//	}
					//}	
					strcpy(path1, path1tmp);
					strcpy(path2, path2tmp);
				}
				else
					MessageBox(hWnd, "noFiles",0 ,0);
				break;
			}
			if(LOWORD(wParam)==ID_DIR_SAVE){
				pidl=SHBrowseForFolder(&odn);
				if(pidl)
					SHGetPathFromIDList(pidl, DirDefaultName); 	
				/*MessageBox(NULL, odn.pszDisplayName, "TEST", MB_OK);*/
				strcpy(path1, odn.pszDisplayName);
				strcpy(path2, odn.pszDisplayName);
				strcat(path1, "/");		strcat(path1, "style.txt");
				strcat(path2, "/");		strcat(path2, "koord.txt");
				ofileStyle.open(path1, std::ios::out);
				ofileKoord.open(path2, std::ios::out);
				for(int j=0; j<n; j++){
					/*MessageBox(hWnd, "destroy",0,0);*/
					buffL=lines[j];
					/*lines.pop_back();*/
					std::string numS=std::to_string((int)j);
					std::string sizeS=std::to_string((int)buffL.size);
					std::string styleS=std::to_string((int)buffL.style);
					std::string rS=std::to_string((int)GetRValue(buffL.col));
					std::string gS=std::to_string((int)GetGValue(buffL.col));
					std::string bS=std::to_string((int)GetBValue(buffL.col));
					std::string toFile=numS+"."+sizeS+"."+styleS+"."+rS+"."+gS+"."+bS+'\n';
					//itoa(j, buff, 10);
					//strcat(buff, ".");
					ofileStyle.write(toFile.c_str(), strlen(toFile.c_str()));
					for(int i=0; i<lines[j].points.size(); i++){
						buffP=lines[j].points[i];
						std::string pointXS=std::to_string((int)buffP.x);
						std::string pointYS=std::to_string((int)buffP.y);
						std::string toFile=numS+"."+pointXS+"."+pointYS+'\n';
						ofileKoord.write(toFile.c_str(), strlen(toFile.c_str()));
					}
				}
				ofileStyle.close();
				ofileKoord.close();
				break;
			}
			if(LOWORD(wParam)==ID_CLEAN)
			{
				flag=1;
				RECT rect;
				GetClientRect(hWnd, &rect);
				InvalidateRect(hWnd, &rect, true);
				click=0;
				for(int i=0; i<n; i++)
					lines.pop_back();
				n=0;
				break;
			}
			if(LOWORD(wParam)==ID_COLOR_RED){color=RGB(255,0,0); break;}
			if(LOWORD(wParam)==ID_COLOR_GREEN){color=RGB(0,255,0); break;}
			if(LOWORD(wParam)==ID_COLOR_BLUE){color=RGB(0,0,255); break;}
			if(LOWORD(wParam)==ID_THICK_4){thick=4; break;}
			if(LOWORD(wParam)==ID_THICK_8){thick=8; break;}
			if(LOWORD(wParam)==ID_THICK_12){thick=12; break;}
			if(LOWORD(wParam)==ID_TIPE_DOTTED){style=1; break;}
			if(LOWORD(wParam)==ID_TIPE_NORMAL){style=0; break;}
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//editы ползунок - для толщины