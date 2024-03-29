
// VlcPlayerCsdnDlg.h: 头文件
//

#pragma once

#include "AVPlayer.h"
const int WM_RECORD = WM_USER + 100;
// CVlcPlayerCsdnDlg 对话框
class CVlcPlayerCsdnDlg : public CDialogEx
{
// 构造
public:
	CVlcPlayerCsdnDlg(CWnd* pParent = nullptr);	// 标准构造函数
	afx_msg LRESULT OnRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPosChanged(WPARAM wParam, LPARAM lParam);
	
	CSliderCtrl m_slider;
public:
	CComPtr<ISpRecognizer>m_pSREngine;// 语音识别引擎(recognition)的接口。
	CComPtr<ISpRecoContext>m_pSRContext;// 识别引擎上下文(context)的接口。
	CComPtr<ISpRecoGrammar>m_pSRGrammar;// 识别文法(grammar)的接口。
	CComPtr<ISpStream>m_pInputStream;// 流()的接口。
	CComPtr<ISpObjectToken>m_pToken;// 语音特征的(token)接口。
	CComPtr<ISpAudio>m_pAudio;// 音频(Audio)的接口。(用来保存原来默认的输入流)
	ULONGLONG ullGrammerID;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VLCPLAYERCSDN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	private:
	CAVPlayer   m_cAVPlayer;        // 播放器类

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChangedScrollbar1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedPlay();
};
