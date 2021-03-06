
// MainDlg.cpp: 구현 파일
//


#include "string.h"
#include <string>
#include "stdafx.h"
#include "Main.h"
#include <afx.h>
#include <TlHelp32.h>
#include <iostream>
#include "MainDlg.h"
#include "afxdialogex.h"
#include "dbt.h"
#include "des.h"
#include <tlhelp32.h>
#include <windows.h>
using namespace std;
#ifdef _DEBUG
#define TIMER_PC 1000
#define new DEBUG_NEW
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
int count = 0;
bool bSystemActive = true;
bool bLockState = true;
HHOOK g_hook = NULL;
char  *sArr[10] = { NULL, };
CString strVoca;
bool usbCheck = FALSE;
int x = 0;
int y = 150;
SIZE s;
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	//   virtual BOOL PreTranslateMessage(MSG* pMsg);
	//   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//   ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMainDlg 대화 상자



CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cListBox);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()

END_MESSAGE_MAP()

char* GetDriveInfo(HKEY Key, LPCSTR lpSubKey, LPCSTR lpValueName)
{
#include "resource.h"
	HKEY   hKey;
	char    data_buffer[256];
	DWORD   data_type = REG_BINARY;
	DWORD   data_size = 256;
	string usbData;

	if (RegOpenKey(Key, lpSubKey, &hKey) == ERROR_SUCCESS) {
		//접근하고자 하는 레지스트리 경로를 hKey에 담는다
		if (RegQueryValueEx(hKey, lpValueName, 0, &data_type, (BYTE*)data_buffer, (DWORD *)&data_size) == ERROR_SUCCESS) {   //해당 경로의 lpValueName 속성 


																															 //값을 data_buffer에 저장
			int j = 0;
			RegCloseKey(hKey);
			for (int i = 0; i<256; i += 2) {      //필요한 정보는 1byte 간격으로 있기 
												  //때문에 i의 값을 2씩 증가(사이사이에 NULL값이 포함되어 있다)
				data_buffer[j] = data_buffer[i];      //원하는 정보로 data_buffer 초기화
				j++;
				if (strchr(data_buffer, '}'))      //USB정보를 읽던 중 정보의 끝부분('}')에 
												   //다다르면 루프 종료
					break;
			}
			data_buffer[j] = NULL;      //문자열의 마지막에 NULL을 넣고

			int i = 0;                     // 문자열 포인터 배열의 인덱스로 사용할 변수

			char *ptr = strtok(data_buffer, "#");   // 공백 문자열을 기준으로 문자열을 자름

			while (ptr != NULL)            // 자른 문자열이 나오지 않을 때까지 반복
			{
				sArr[i] = ptr;             // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
				i++;                       // 인덱스 증가

				ptr = strtok(NULL, "#");   // 다음 문자열을 잘라서 포인터를 반환
			}
			//MessageBox(0, sArr[2], "hello2", MB_OK);          // 자른 문자열 출력
			usbData += sArr[2];

			for (int j = 0; j < 16; j++) {
				plainTextIn[j] = usbData.at(j);//스트링을 한글자씩 끊어 배열에 저장


			}
			
			

			return sArr[2];      //포인터를 리턴

		}
	}
}



bool SearchDrive(HINSTANCE m_hInst, HWND m_hWnd)
{

	DWORD myDrivesBitMask;
	char driveName = 'A';   //드라이브 경로
	char * message;
	myDrivesBitMask = GetLogicalDrives();      //모든 드라이브 정보를가져온다.
	if (myDrivesBitMask == 0) {
		message = "GetLogicalDrives() failed with error code:  ";
		strcat_s(message, 10, (char*)GetLastError());
		MessageBox(NULL, (LPCSTR)message, (LPCSTR)"Error", MB_OK);
	}
	else {
		while (myDrivesBitMask) {
			if (myDrivesBitMask) {
				char sDriveName[20] = { "\0" };
				char path[40] = "\\DosDevices\\";
				char cRecordFile[512];

				sDriveName[0] = driveName;      //현재 탐색중인 드라이브 경로를    
												//저장(A,B,C,D,...)
				strcat_s(sDriveName, 4, ":");
				strcat_s(path, 15, sDriveName);
				if (GetDriveTypeA(sDriveName) == DRIVE_REMOVABLE) {
					strcat_s(sDriveName, "\\USB.data");      // 해당 경로에서 설정 파일 찾는다.
					memset(cRecordFile, 0, sizeof(cRecordFile));

					//(USB.data) 찾는다.
					GetDriveInfo(HKEY_LOCAL_MACHINE,
						"SYSTEM\\MountedDevices", path);   //USB 정보를 레지스트리에서  얻어온다.

					return TRUE;
				}
			}
			driveName++;
			myDrivesBitMask >>= 1;
		}
	}
}
// CtestDlg 메시지 처리기

BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	s.cx = (LONG)::GetSystemMetrics(SM_CXFULLSCREEN);
	s.cy = (LONG)::GetSystemMetrics(SM_CYFULLSCREEN);
	//랜덤 배경
	srand((unsigned int)time(NULL));

	int random = rand() % 13;
	
	if (s.cx < 2000) 
	{
		if(random==0)
			m_bitMain.LoadBitmapA(IDB_BITMAP1);
		else if (random ==1)
			m_bitMain.LoadBitmapA(IDB_BITMAP2);
		else if (random == 2)
			m_bitMain.LoadBitmapA(IDB_BITMAP3);
		else if (random == 3)
			m_bitMain.LoadBitmapA(IDB_BITMAP4);
		else if (random == 4)
			m_bitMain.LoadBitmapA(IDB_BITMAP5);
		else if (random == 5)
			m_bitMain.LoadBitmapA(IDB_BITMAP6);
		else if (random == 6)
			m_bitMain.LoadBitmapA(IDB_BITMAP7);
		else if (random == 7)
			m_bitMain.LoadBitmapA(IDB_BITMAP8);
		else if (random == 8)
			m_bitMain.LoadBitmapA(IDB_BITMAP9);
		else if (random == 9)
			m_bitMain.LoadBitmapA(IDB_BITMAP10);
		else if (random == 10)
			m_bitMain.LoadBitmapA(IDB_BITMAP11);
		else if (random == 11)
			m_bitMain.LoadBitmapA(IDB_BITMAP12);
		else if (random == 12)
			m_bitMain.LoadBitmapA(IDB_BITMAP13);
		else if (random == 13)
			m_bitMain.LoadBitmapA(IDB_BITMAP14);
	}
	else
	{
		
		if (random == 0)
			m_bitMain.LoadBitmapA(IDB_BITMAP15);
		else if (random == 1)
			m_bitMain.LoadBitmapA(IDB_BITMAP16);
		else if (random == 2)
			m_bitMain.LoadBitmapA(IDB_BITMAP17);
		else if (random == 3)
			m_bitMain.LoadBitmapA(IDB_BITMAP18);
		else if (random == 4)
			m_bitMain.LoadBitmapA(IDB_BITMAP19);
		else if (random == 5)
			m_bitMain.LoadBitmapA(IDB_BITMAP20);
		else if (random == 6)
			m_bitMain.LoadBitmapA(IDB_BITMAP21);
		else if (random == 7)
			m_bitMain.LoadBitmapA(IDB_BITMAP22);
		else if (random == 8)
			m_bitMain.LoadBitmapA(IDB_BITMAP23);
		else if (random == 9)
			m_bitMain.LoadBitmapA(IDB_BITMAP24);
		else if (random == 10)
			m_bitMain.LoadBitmapA(IDB_BITMAP25);
		else if (random == 11)
			m_bitMain.LoadBitmapA(IDB_BITMAP26);
		else if (random == 12)
			m_bitMain.LoadBitmapA(IDB_BITMAP27);
		else if (random == 13)
			m_bitMain.LoadBitmapA(IDB_BITMAP28);
	}
		
	Invalidate(FALSE);
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	

	SetTimer(0, 1000, NULL);

	SetIcon(m_hIcon, TRUE);         // 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);      // 작은 아이콘을 설정합니다.

								  // TODO: 여기에 추가 초기화 작업을 추가합니다.
	ShowWindow(SW_SHOWMAXIMIZED);
	CMenu *p_menu = this->GetSystemMenu(FALSE);
	p_menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

	SetHook();
	ModifyStyle(WS_CAPTION, NULL); // 타이틀바 없애기

	GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);

	RECT rect2;
	GetWindowRect(&rect2);
	ClipCursor(&rect2);

	HWND hWnd = 0;
	HINSTANCE m_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	if (bLockState)
	{
		if (SearchDrive(m_hInst, m_hWnd))
		{
			Load();
			if (usbCheck)
			{
				bLockState = FALSE;
				UnHook();
				ShellExecute(NULL, "open", "Main2.exe", NULL, NULL, SW_SHOW); //USBDeview 외부 프로그램 실행   
				::PostQuitMessage(WM_QUIT);
			}


		}
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMainDlg::OnPaint()
{

	CPaintDC dc(this);
	//==========================================
	//배경이미지 설정
	//사용할 배경이미지 ID 지정
	CRect tRect;
	GetClientRect(&tRect);
	//------------------------------------------
	//배경이미지 그리기
	CBitmap tBitmap, *pOldBitmap;
	CDC tMemdc;

	tMemdc.CreateCompatibleDC(&dc);

	pOldBitmap = tMemdc.SelectObject(&tBitmap);
	dc.BitBlt(0, 0, tRect.Width(), tRect.Height(), &tMemdc, 0, 0, SRCCOPY);
	tMemdc.SelectObject(pOldBitmap);
	DeleteObject(tBitmap);
	DeleteObject(tMemdc);
	DeleteDC(dc);

	CDialogEx::OnPaint();

}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	//프로세스 킬a

	HANDLE hSnapShot;
	PROCESSENTRY32 pEntry;
	DWORD dwVal = 1;
	//Invalidate();
	HKEY hKey1;


	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	pEntry.dwSize = sizeof(pEntry);

	Process32First(hSnapShot, &pEntry);   // 현재 실행된 프로세스중에 첫 번째 
										  // 프로세스를 가져온다
	/*

	while (1)
	{

		BOOL hRes = Process32Next(hSnapShot, &pEntry);   // 다음번 프로세스의 정보를 
														 // 가져온다.   
		if (hRes == FALSE)
			break;

		
		if (strcmp(pEntry.szExeFile, "Taskmgr.exe") == 0 )    // 프로세스의 이름이 taskmgr.exe(작업관리자)면
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pEntry.th32ProcessID);   // taskmgr.exe의 핸들을 얻음
			if (hProcess)
			{

				if (TerminateProcess(hProcess, 0))      //taskmgr.exe를 종료한다
				{

					unsigned long nCode;
					GetExitCodeProcess(hProcess, &nCode);
				}
				CloseHandle(hProcess);
			}
			break;
		}
		
		
	}
	*/
	   
	CClientDC dc(this);
	CString m_strTime0;
	CString m_strTime1;
	CString m_strTime2;
	CTime t1 = CTime::GetCurrentTime();
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));
	CFont  font1;
	CFont  font2;
	font1.CreateFont(          // object의 생성
		s.cx/10,                  // 문자 폭
		s.cy/10,                  // 문자 높이 
		0,                   // 기울기 각도
		0,                   // 문자 방향
		FW_LIGHT,            // 문자 굵기
		FALSE,               // 문자 기울림 모양
		FALSE,               // 밑 줄
		FALSE,               // 취소선
		DEFAULT_CHARSET,     // 문자 셋
		OUT_DEFAULT_PRECIS,  // 출력 정확도
		CLIP_DEFAULT_PRECIS, // 킬립핑 정확도
		PROOF_QUALITY,     // 출력의 질
		DEFAULT_PITCH,       // 자간 거리
		_T("굴림")               // 문자 모양
	);
	font2.CreateFont(          // object의 생성
		s.cx / 30,                  // 문자 폭
		s.cy / 30,                  // 문자 높이 
		0,                   // 기울기 각도
		0,                   // 문자 방향
		FW_EXTRALIGHT,            // 문자 굵기
		FALSE,               // 문자 기울림 모양
		FALSE,               // 밑 줄
		FALSE,               // 취소선
		DEFAULT_CHARSET,     // 문자 셋
		OUT_DEFAULT_PRECIS,  // 출력 정확도
		CLIP_DEFAULT_PRECIS, // 킬립핑 정확도
		CLEARTYPE_QUALITY,     // 출력의 질
		DEFAULT_PITCH,       // 자간 거리
		_T("굴림")               // 문자 모양
	);
	m_strTime0 = t1.Format("%S"); // 문서포맷 


	if (m_strTime0 == "59")
	{
		Invalidate();
	}
	dc.SelectObject(&font1);
	m_strTime1 = t1.Format("%H:%M"); // 문서포맷 
	dc.TextOut(s.cx / 40, (s.cy)- (s.cy / 4)- (s.cy / 10), m_strTime1);
	dc.SelectObject(&font2);
	m_strTime2 = t1.Format("%m월 %d일"); // 문서포맷

	dc.TextOut(s.cx / 27, (s.cy)-(s.cy/10), m_strTime2);



	HWND wndDlg = ::GetLastActivePopup(m_hWnd);
	if (wndDlg && wndDlg != m_hWnd)
	{
		char buffer[256] = { 0 };
		::GetClassName(wndDlg, buffer, 256);
		if (CString("#32770") == buffer) //메시지 박스는 분명히 다이얼로그이며 클래스명이 #32770
		{
			Sleep(500);
			::EndDialog(wndDlg, IDOK);
		}
	}

}

// 후킹

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	PKBDLLHOOKSTRUCT p;

	if (nCode == HC_ACTION)
	{
		p = (PKBDLLHOOKSTRUCT)lParam;

		if (((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN))
			|| (p->flags & LLKHF_ALTDOWN)
			|| (p->vkCode == VK_TAB)

			)

			return 1;

	}

	return CallNextHookEx(g_hook, nCode, wParam, lParam);



}

BOOL CMainDlg::SetHook()
{
	if (g_hook != NULL)
		UnHook();
	g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, (HINSTANCE)theApp.m_hInstance, NULL);

	return TRUE;
}

BOOL CMainDlg::UnHook()
{
	if (g_hook != NULL)
		UnhookWindowsHookEx(g_hook);
	return TRUE;
}

//파일입출력
void CMainDlg::Load()
{

	HWND hWnd = 0;
	HINSTANCE m_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	CFile file;
	CString str;
	CString Temp;
	string masterKey = "ZYRUS10081800226";
	file.Open(_T("VOCA.dat"), CFile::modeRead);
	CArchive ar(&file, CArchive::load);
	m_cListBox.ResetContent();
	while (ar.ReadString(Temp))  // 파일의 마지막까지 출력
	{
		m_cListBox.AddString(Temp);
	}
	ar.Close();
	file.Close();
	for (int i = 0; i < m_cListBox.GetCount(); i++)
	{
		m_cListBox.GetText(i, strVoca);
		if (masterKey == plainTextIn)
		{
			AfxMessageBox("마스터키 삽입");
			usbCheck = TRUE;
			break;
		}
		else if (strVoca == plainTextIn)
		{
			usbCheck = TRUE;
			break;
		}
		else
		{

			usbCheck = FALSE;

		}

	}

}

HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if (bLockState)
	{
		if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONDBLCLK)
			return TRUE;
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}



BOOL CMainDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	PDEV_BROADCAST_HDR hdr;
	hdr = (PDEV_BROADCAST_HDR)dwData;
	HWND hWnd = 0;
	HINSTANCE m_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	while (true) {
		switch (nEventType)
		{
		case DBT_DEVICEARRIVAL:

			if (bLockState)
			{
				SearchDrive(m_hInst, m_hWnd);
				Load();
				if (usbCheck)
				{

					hide();
					ShellExecute(NULL, "open", "Main2.exe", NULL, NULL, SW_SHOW); //USBDeview 외부 프로그램 실행   
					::PostQuitMessage(WM_QUIT);
				}

			}
			break;
		default:
			break;
		}
		return true;
	}
}




BOOL CMainDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.hMenu = NULL;

	return CDialogEx::PreCreateWindow(cs);
}




BOOL CMainDlg::OnEraseBkgnd(CDC* pDC)
{
	if (!m_bitMain.m_hObject)
		return true;

	CRect rect;
	GetClientRect(&rect);

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&m_bitMain);

	BITMAP bmap;
	m_bitMain.GetBitmap(&bmap);
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, bmap.bmWidth, bmap.bmHeight, SRCCOPY);

	dc.SelectObject(pOldBitmap);
	return true;
}




void CAboutDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(0);
}


void CMainDlg::hide()
{
	BOOL bResult = FALSE;
	LONG lResult = SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);

	bResult = SetLayeredWindowAttributes(0, y, LWA_ALPHA);
	y -= 10;
	if (y != 0)
	{
		Sleep(30);
		hide();
	}
}