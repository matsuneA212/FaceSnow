
// CVDlibTest.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CCVDlibTestApp:
// このクラスの実装については、CVDlibTest.cpp を参照してください。
//

class CCVDlibTestApp : public CWinApp
{
public:
	CCVDlibTestApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CCVDlibTestApp theApp;