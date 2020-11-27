
// USBDiskThiefDlg.h : header file
//

#pragma once


// CUSBDiskThiefDlg dialog
class CUSBDiskThiefDlg : public CDialogEx
{
// Construction
public:
	CUSBDiskThiefDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USBDISKTHIEF_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonChoosePath();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnBnClickedButtonStartWatch();
	afx_msg void OnBnClickedButtonEndWatch();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void CUSBDiskThiefDlg::OnBnClickedCancel();
	afx_msg void OnBnClickedButtonShowLog();
};
