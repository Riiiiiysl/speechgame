
// VlcPlayerCsdnDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VlcPlayerCsdn.h"
#include "VlcPlayerCsdnDlg.h"
#include "afxdialogex.h"

#include "vlc.h"
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")


#define WM_USER_POS_CHANGED WM_USER + 1
#define TIMERID 8
int score = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void CALLBACK Test(HWND hWnd, UINT para1, UINT_PTR para2, DWORD para3)

{


}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};






// CVlcPlayerCsdnDlg 对话框



CVlcPlayerCsdnDlg::CVlcPlayerCsdnDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VLCPLAYERCSDN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVlcPlayerCsdnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
}

BEGIN_MESSAGE_MAP(CVlcPlayerCsdnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECORD, OnRecord)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CVlcPlayerCsdnDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_USER_POS_CHANGED, OnPosChanged)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CVlcPlayerCsdnDlg::OnNMCustomdrawSlider1)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_PLAY, &CVlcPlayerCsdnDlg::OnStnClickedPlay)
END_MESSAGE_MAP()






void CallbackPosChanged(void *data, int iPos)
{
	CAVPlayer *pAVPlayer = (CAVPlayer *)data;

	if (pAVPlayer)
	{
		HWND hWnd = pAVPlayer->GetHWND();

		if (::IsWindow(hWnd) && ::IsWindow(::GetParent(hWnd)))
		{
			::PostMessage(::GetParent(hWnd), WM_USER_POS_CHANGED, (WPARAM)data, iPos);
		}
	}
}

CStringA UnicodeToUTF8(const CStringW& strWide)
{
	CStringA strUTF8;
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, strWide, -1, NULL, 0, NULL, NULL);

	if (nLen > 1)
	{
		::WideCharToMultiByte(CP_UTF8, 0, strWide, -1, strUTF8.GetBuffer(nLen - 1), nLen, NULL, NULL);
		strUTF8.ReleaseBuffer();
	}

	return strUTF8;
}

LRESULT CVlcPlayerCsdnDlg::OnPosChanged(WPARAM wParam, LPARAM lParam)
{
	m_slider.SetPos(m_cAVPlayer.GetPos());
	CString t;
	int min = m_cAVPlayer.GetTime() / 1000;



	t.Format(L"%04d", min);
	GetDlgItem(IDC_STATIC)->SetWindowText(t);


	return TRUE;
}



// CVlcPlayerCsdnDlg 消息处理程序

BOOL CVlcPlayerCsdnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//ShowWindow(SW_MAXIMIZE);//对话框默认最大化弹出
	

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_cAVPlayer.SetHWND(GetDlgItem(IDC_PLAY)->GetSafeHwnd());   // 设置播放器的窗口句柄
	//m_cAVPlayer.SetCallback(CallbackPosChanged);

	/**********************************************************************************/
	m_cAVPlayer.Play("res\\1.mp4");
	
	/**********************************************************************************/

	// TODO: 在此添加额外的初始化代码
	CoInitialize(NULL);
	HRESULT hr;
	
	hr = m_pSREngine.CoCreateInstance(CLSID_SpInprocRecognizer);
	if (FAILED(hr)) {
		MessageBox(L"m_pSREngine.CoCreateInstance");
	}/*else{
	 MessageBox(L"success");
	 }*/
	hr = m_pSREngine->CreateRecoContext(&m_pSRContext);//建立上下文
	if (FAILED(hr)) {
		MessageBox(L" m_pSREngine->CreateRecoContext ");
	}/*else{
	 MessageBox(L"success");
	 }*/
	 //这里是设置事件
	HWND hwnd = GetSafeHwnd();
	hr = m_pSRContext->SetNotifyWindowMessage(hwnd, WM_RECORD, 0, 0);
	if (FAILED(hr)) {
		MessageBox(L"SetNotifyWindowMessage");
	}/*else{
	 MessageBox(L"success");
	 }*/
	hr = m_pSRContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));


	//这里是设置默认的音频输入
	hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &m_pAudio);
	m_pSREngine->SetInput(m_pAudio, true);


	//这里是加载默认的语法规则
	ullGrammerID = 1000;
	hr = m_pSRContext->CreateGrammar(ullGrammerID, &m_pSRGrammar);
	if (FAILED(hr)) {
		MessageBox(L"CreateGrammar");
	}/*else{
	 MessageBox(L"success");
	 }*/
	WCHAR wszXMLFile[30] = L"";
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)"res\\good.xml", -1, wszXMLFile, 256);  //这里修改XML的目录
	hr = m_pSRGrammar->LoadCmdFromFile(wszXMLFile, SPLO_DYNAMIC);
	if (FAILED(hr)) {
		MessageBox(L"LoadCmdFromFile");
	}/*else{
	 MessageBox(L"success");
	 }*/


	 //开启语音识别
	m_pSRGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
	hr = m_pSREngine->SetRecoState(SPRST_ACTIVE);
	if (FAILED(hr)) {
		MessageBox(L"SetRecoState");
	}/*else{
	 MessageBox(L"success");
	 }*/
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


LRESULT CVlcPlayerCsdnDlg::OnRecord(WPARAM wParam, LPARAM lParam) {
	USES_CONVERSION;
	CSpEvent event;
	CString finish("Finish!");
	//MessageBox(L"onrecord");
	

	if (m_pSRContext)
	{
		
		while (event.GetFrom(m_pSRContext) == S_OK) {
			
			if (score == -3) {
				score = 0;
			}
			switch (event.eEventId)
			{
			case SPEI_RECOGNITION:
			{
				//识别出了语音输入 
				//m_bGotReco = TRUE; 
				static const WCHAR wszUnrecognized[] = L"<Unrecognized>";
				CSpDynamicString dstrText;
				////取得识别结果 
				if (FAILED(event.RecoResult()->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL)))
				{
					dstrText = wszUnrecognized;
				}

				BSTR SRout;
				dstrText.CopyToBSTR(&SRout);
				
				CString Recstring;
				Recstring.Empty();
				Recstring = SRout;
				char* lpszText2 = _com_util::ConvertBSTRToString(SRout);
				//这里开始处理语音识别的消息。
				//MessageBox(Recstring);
				
				//m_cAVPlayer.Play("res\\1.avi");
				
					if (strcmp(lpszText2,"漂亮") ==0|| strcmp(lpszText2, "美") == 0 || strcmp(lpszText2, "真好") == 0 || strcmp(lpszText2, "可爱") == 0 || strcmp(lpszText2, "加油") == 0 || strcmp(lpszText2, "喜欢") == 0 || strcmp(lpszText2, "长大") == 0 || strcmp(lpszText2, "浇水") == 0 || strcmp(lpszText2, "开花") == 0 || strcmp(lpszText2, "好看") == 0 || strcmp(lpszText2, "有趣") == 0 || strcmp(lpszText2, "阳光") == 0 || strcmp(lpszText2, "温暖") == 0 || strcmp(lpszText2, "宝贝") == 0 || strcmp(lpszText2, "我爱你") == 0 || strcmp(lpszText2, "最棒") == 0 || strcmp(lpszText2, "开心") == 0 || strcmp(lpszText2, "一定行") == 0 || strcmp(lpszText2, "优秀") == 0 || strcmp(lpszText2, "挺好") == 0)
					{
						if (score == 0) {//出苗
							m_cAVPlayer.SeekTo(50);
							m_cAVPlayer.Play();
							m_cAVPlayer.Timetopause(2);
							score += 1;
							continue;
						}
						if (score == 1) {//到出苞
							m_cAVPlayer.SeekTo(52);
							m_cAVPlayer.Timetopause(3);
							score += 5;
							continue;
						}
						if (score == 6) {//到半开花
							m_cAVPlayer.SeekTo(55);
							m_cAVPlayer.Timetopause(4);
							score += 5;
							continue;
						}
						if (score == 11) {//到全开花
							m_cAVPlayer.SeekTo(59);
							m_cAVPlayer.Timetopause(5);
							score += 5;
							continue;
						}
						if (score == 2) {//苞枯到苞半枯
							m_cAVPlayer.SeekTo(153);
							m_cAVPlayer.Timetopause(4);
							score += 2;
							continue;
						}
						if (score == 4) {//苞半枯到苞
							m_cAVPlayer.SeekTo(157);
							m_cAVPlayer.Timetopause(4);
							score += 2;
							continue;
						}
						if (score == 7) {//半开花枯到半枯
							m_cAVPlayer.SeekTo(170);
							m_cAVPlayer.Timetopause(4);
							score += 2;
							continue;
						}
						if (score == 9) {//半开花半枯到半开花
							m_cAVPlayer.SeekTo(174);
							m_cAVPlayer.Timetopause(4);
							score += 2;
							continue;
						}
						if (score == -1) {//苞枯到半枯
							m_cAVPlayer.SeekTo(136);
							m_cAVPlayer.Timetopause(4);
							score += 2;
							continue;
						}
						if (score == 16) {//到全盛
							m_cAVPlayer.SeekTo(65);
							m_cAVPlayer.Timetopause(4);
							score =0;
							continue;
						}
						if (score == 7) {//到全盛
							m_cAVPlayer.SeekTo(187);
							m_cAVPlayer.Timetopause(3);
							score +=2;
							continue;
						}
						if (score == 9) {//到全盛
							m_cAVPlayer.SeekTo(190);
							m_cAVPlayer.Timetopause(3);
							score += 2;
							continue;
						}
					}
							//-3死
					if (strcmp(lpszText2, "没意思") == 0 || strcmp(lpszText2, "难过") == 0 || strcmp(lpszText2, "不漂亮") == 0 || strcmp(lpszText2, "不可爱") == 0 || strcmp(lpszText2, "丑") == 0 || strcmp(lpszText2, "恶心") == 0 || strcmp(lpszText2, "烦") == 0 || strcmp(lpszText2, "废物") == 0 || strcmp(lpszText2, "不喜欢") == 0 || strcmp(lpszText2, "无聊") == 0 || strcmp(lpszText2, "丢脸") == 0 || strcmp(lpszText2, "垃圾") == 0 || strcmp(lpszText2, "坏") == 0 || strcmp(lpszText2, "不要脸") == 0 || strcmp(lpszText2, "讨厌") == 0 || strcmp(lpszText2, "尴尬") == 0 || strcmp(lpszText2, "傻逼") == 0 || strcmp(lpszText2, "死") == 0 || strcmp(lpszText2, "枯") == 0 || strcmp(lpszText2, "不好") == 0)
					{	
						if (score == 0) {//出苗
							m_cAVPlayer.SeekTo(50);
							m_cAVPlayer.Play();
							m_cAVPlayer.Timetopause(2);
							score += 1;
							continue;
						}
						if (score == 1) {//苗半枯
							m_cAVPlayer.SeekTo(83);
							m_cAVPlayer.Timetopause(2);
							score -= 2;
							continue;
						}
						if (score == -1) {//苗枯
							m_cAVPlayer.SeekTo(85);
							m_cAVPlayer.Timetopause(4);
							score -= 2;
							continue;
						}
						
						if (score == 6) {//苞半枯
							m_cAVPlayer.SeekTo(93);
							m_cAVPlayer.Timetopause(5);
							score -= 2;
							continue;
						}
						if (score == 4) {//苞枯
							m_cAVPlayer.SeekTo(98);
							m_cAVPlayer.Timetopause(4);
							score -= 2;
							continue;
						}
						if (score == 2) {//苞死
							m_cAVPlayer.SeekTo(102);
							m_cAVPlayer.Timetopause(2);
							score -= 5;
							continue;
						}
						if (score == 11) {//半开花半枯
							m_cAVPlayer.SeekTo(106);
							m_cAVPlayer.Timetopause(4);
							score -= 2;
							continue;
						}
						if (score == 9) {//半开花枯
							m_cAVPlayer.SeekTo(110);
							m_cAVPlayer.Timetopause(4);
							score -= 2;
							continue;
						}
						if (score == 7) {//半开花死
							m_cAVPlayer.SeekTo(114);
							m_cAVPlayer.Timetopause(4);
							score -= 10;
							continue;
						}
						if (score == 11) {//花半谢
							m_cAVPlayer.SeekTo(122);
							m_cAVPlayer.Timetopause(4);
							score -= 2;
							continue;
						}
						if (score == 9) {//花枯
							m_cAVPlayer.SeekTo(126);
							m_cAVPlayer.Timetopause(4);
							score -= 2;
							continue;
						}
						if (score == 7) {//花死
							m_cAVPlayer.SeekTo(130);
							m_cAVPlayer.Timetopause(3);
							score -= 10;
							continue;
						}
						//continue;
					}
			}
			}
			continue;
		}
	}
	return NULL;
}



void CVlcPlayerCsdnDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVlcPlayerCsdnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVlcPlayerCsdnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CVlcPlayerCsdnDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

		m_cAVPlayer.Pause();
		CString s;
		int time = m_cAVPlayer.GetTime() / 1000;
		//int min = m_cAVPlayer.GetTime() / 1000 / 60;
		//int sec = m_cAVPlayer.GetTime() / 1000 % 60;

		//int min2 = m_cAVPlayer.GetLength() / 1000 / 60;
		//int sec2 = m_cAVPlayer.GetLength() / 1000 % 60;

		//增加自动下一曲播放


		s.Format(L"%04d", time);
		MessageBox(s);
}


void CVlcPlayerCsdnDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}







void CVlcPlayerCsdnDlg::OnStnClickedPlay()
{
	// TODO: 在此添加控件通知处理程序代码
}
