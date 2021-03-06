
// Management_USBDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Management_USB.h"
#include "Management_USBDlg.h"
#include "afxdialogex.h"
#include <string>
#include "dbt.h"
#include "des.h"
#include "resource.h"
#include "string.h"
#include <string>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

char  *sArr[10] = { NULL, };
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
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CManagementUSBDlg 대화 상자



CManagementUSBDlg::CManagementUSBDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MANAGEMENT_USB_DIALOG, pParent)
	, m_strInput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManagementUSBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cListBox);
	DDX_Text(pDX, IDC_EDIT1, m_strInput);
}

BEGIN_MESSAGE_MAP(CManagementUSBDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CManagementUSBDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CManagementUSBDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CManagementUSBDlg::OnBnClickedOk)
	ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()


// CManagementUSBDlg 메시지 처리기

char* GetDriveInfo(HKEY Key, LPCSTR lpSubKey, LPCSTR lpValueName)
{
#include "resource.h"
	HKEY	hKey;
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
			for (int i = 0; i<256; i += 2) {		//필요한 정보는 1byte 간격으로 있기 
													//때문에 i의 값을 2씩 증가(사이사이에 NULL값이 포함되어 있다)
				data_buffer[j] = data_buffer[i];		//원하는 정보로 data_buffer 초기화
				j++;
				if (strchr(data_buffer, '}'))		//USB정보를 읽던 중 정보의 끝부분('}')에 
													//다다르면 루프 종료
					break;
			}
			data_buffer[j] = NULL;		//문자열의 마지막에 NULL을 넣고




			int i = 0;                     // 문자열 포인터 배열의 인덱스로 사용할 변수

			char *ptr = strtok(data_buffer, "#");   // 공백 문자열을 기준으로 문자열을 자름

			while (ptr != NULL)            // 자른 문자열이 나오지 않을 때까지 반복
			{
				sArr[i] = ptr;             // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
				i++;                       // 인덱스 증가

				ptr = strtok(NULL, "#");   // 다음 문자열을 잘라서 포인터를 반환
			}


			// 자른 문자열 출력
			usbData += sArr[2];

			for (int j = 0; j < 16; j++) {
				plainTextIn[j] = usbData.at(j);//스트링을 한글자씩 끊어 배열에 저장


			}
			

			


			return sArr[2];		//포인터를 리턴

		}
	}

}

bool SearchDrive(HINSTANCE m_hInst, HWND m_hWnd)
{

	DWORD myDrivesBitMask;
	char driveName = 'A';	//드라이브 경로
	char * message;
	myDrivesBitMask = GetLogicalDrives();		//모든 드라이브 정보를가져온다.


	while (myDrivesBitMask) {
		if (myDrivesBitMask) {
			char sDriveName[20] = { "\0" };
			char path[40] = "\\DosDevices\\";
			char cRecordFile[512];

			sDriveName[0] = driveName;		//현재 탐색중인 드라이브 경로를 	
											//저장(A,B,C,D,...)
			strcat_s(sDriveName, 4, ":");
			strcat_s(path, 15, sDriveName);
			if (GetDriveTypeA(sDriveName) == DRIVE_REMOVABLE) {
				strcat_s(sDriveName, "\\USB.data");		// 해당 경로에서 설정 파일 찾는다.
				memset(cRecordFile, 0, sizeof(cRecordFile));

				GetPrivateProfileString((LPCSTR)"Key", (LPCSTR)"Serial", NULL, cRecordFile, 512,
					(LPCSTR)sDriveName);	//해당 USB 드라이브에서 암호화 파일을
											//(USB.data) 찾는다.
				GetDriveInfo(HKEY_LOCAL_MACHINE,
					"SYSTEM\\MountedDevices", path);	//USB 정보를 레지스트리에서  얻어온다.


				if (true) {


					return TRUE;

				}

			}
			driveName++;
			myDrivesBitMask >>= 1;
		}
	}
	return FALSE;

}

BOOL CManagementUSBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	HWND hWnd = 0;
	HINSTANCE m_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
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
	
	SearchDrive(m_hInst, m_hWnd);

	pEdit->SetWindowTextA(plainTextIn);
	
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	
	Load();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CManagementUSBDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CManagementUSBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CManagementUSBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CManagementUSBDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	PDEV_BROADCAST_HDR hdr;
	hdr = (PDEV_BROADCAST_HDR)dwData;
	HWND hWnd = 0;
	HINSTANCE m_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);

	while (true) {
		switch (nEventType)
		{
		case DBT_DEVICEARRIVAL:

			SearchDrive(m_hInst, m_hWnd);
				
			pEdit->SetWindowTextA(plainTextIn);
			break;
		case true:

			SearchDrive(m_hInst, m_hWnd);

			pEdit->SetWindowTextA(plainTextIn);


			break;
		default:



			break;
		}
		return true;
	}
}

void CManagementUSBDlg::OnBnClickedButton1()
{
	CString str1;
	CString str2;
	int count = 0;
	GetDlgItemText(IDC_EDIT1, str2);
	for (int i = 0; i < m_cListBox.GetCount(); i++)
	{
		m_cListBox.GetText(i, str1);
		if (str1 == str2)
		{
			count++;
			AfxMessageBox("이미 저장된 usb입니다");
			break;
		}
	}
	
	if (count == 0) 
	{
		UpdateData(TRUE);   //Edit Box에 있는 문자열을 변수에 저장
		m_cListBox.AddString(m_strInput);    //리스트상자의 변수에 AddString을 사용하여 문자열을 추가
		m_strInput = "";     //EditBox에 있던 값이 리스트상자에 추가되고 난 후 빈 상자로 만들기위해 NULL값 지정
		UpdateData(FALSE);    //변수에 있는 값을 에디터 상자로 보냄
	}
}


void CManagementUSBDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	int nList = 0;
	nList = m_cListBox.GetCurSel();    //선택된 목록의 인덱스를 리턴한다.
	m_cListBox.DeleteString(nList);     //목록의 인덱스를 삭제한다.


}

void CManagementUSBDlg::Save() 
{ 
//파일을 쓸 준비를 한다.
	CStdioFile file;
	CString qwer;
	file.Open("VOCA.dat", CStdioFile::modeCreate | CStdioFile::modeWrite | CStdioFile::typeText);

	// 리스트 컨트롤의 루프를 돌면서 값들을 가져온다.
	CString strVoca;
	for (int i = 0; i < m_cListBox.GetCount(); i++) 
	{
		m_cListBox.GetText(i, strVoca);
		file.WriteString(strVoca);
	}
	file.Close();
	AfxMessageBox("파일이 저장되었습니다");
}

void CManagementUSBDlg::Load() 
{ 
	m_cListBox.ResetContent();
	CFile file;
	CString Temp;
	file.Open(_T("VOCA.dat"), CFile::modeRead);
	CArchive ar(&file, CArchive::load);
	while (ar.ReadString(Temp))  // 파일의 마지막까지 출력
	{
		m_cListBox.AddString(Temp);
	}
	ar.Close();
	file.Close();
}

void CManagementUSBDlg::OnBnClickedOk()
{
	Save();
	::PostQuitMessage(WM_QUIT);
}
