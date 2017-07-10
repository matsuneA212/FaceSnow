
// CVDlibTestDlg.h : ヘッダー ファイル
//

#pragma once


// CCVDlibTestDlg ダイアログ
class CCVDlibTestDlg : public CDialogEx
{
// コンストラクション
public:
	CCVDlibTestDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_CVDLIBTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnImage();
	afx_msg void OnBnClickedBtnVideo();
	afx_msg void OnBnClickedChkDrawtriangle();
	afx_msg void OnBnClickedChkResize();
	afx_msg void OnBnClickedChkTimemeasure();
	afx_msg void OnBnClickedChkMouseinner();
};
