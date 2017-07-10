
// CVDlibTestDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CVDlibTest.h"
#include "CVDlibTestDlg.h"
#include "afxdialogex.h"
#include "CCPUTimer.h"

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

#include "opencv2\\opencv.hpp"
#include "opencv2\\imgproc\\imgproc.hpp"

using namespace dlib;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#pragma comment(lib,"dlibd.lib")            // dlibデバッグ用
#else 
#pragma comment(lib,"dlibr.lib")            // dlibリリース用
#endif

#ifdef _DEBUG
//Debugモードの場合
#pragma comment(lib,"opencv_core310d.lib")            // opencv_core
#pragma comment(lib,"opencv_imgproc310d.lib")        // opencv_imgproc
#pragma comment(lib,"opencv_highgui310d.lib")        // opencv_highgui
#pragma comment(lib,"opencv_imgcodecs310d.lib")        // opencv_imagecodec
#pragma comment(lib,"opencv_videoio310d.lib")        // opencv_videoio
//#pragma comment(lib,"opencv_objdetect231d.lib")    // opencv_objdetect
#else 
#pragma comment(lib,"opencv_core310.lib")            // opencv_core
#pragma comment(lib,"opencv_imgproc310.lib")        // opencv_imgproc (cvCircle,cvRectangle, cvSubdiv2D,line)
#pragma comment(lib,"opencv_highgui310.lib")        // opencv_highgui
#pragma comment(lib,"opencv_imgcodecs310.lib")        // opencv_imagecodec (imread)
#pragma comment(lib,"opencv_videoio310.lib")        // opencv_videoio 　実行にはffmpegも必要
//  #pragma comment(lib,"opencv_objdetect310.lib")    // opencv_objdetect
#endif


#define CAPTURE_WIDTH    640    //キャプチャサイズ(幅)
#define CAPTURE_HEIGHT  480    //キャプチャサイズ(高さ)

unsigned char g_img[CAPTURE_WIDTH*CAPTURE_HEIGHT * 4];
COLORREF g_col[] = { RGB(0, 0, 0), RGB(255, 255, 255), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };

CString g_str;

//キャプチャ画像
int key;
CvCapture* g_capture = NULL;
IplImage* g_cvimg = NULL;

//静止画像
cv::Mat		g_staticMat;
IplImage* g_staticimg = NULL;
cv::Point2f g_staticGravityPoint;
std::vector<cv::Point2f> g_TargetPoints;	//顔特徴点位置


CCPUTimer g_cputimer;		//時間測定用タイマ
BOOL g_bTimeMeas = FALSE;

BOOL g_bMouseInner = TRUE;

//DLIB
frontal_face_detector g_detector;
shape_predictor g_pose_model; //dlib 顔特徴量

std::vector<cv::Point3i> g_Triangles;		//ポリゴンを構成する三角形
cv::Point3i g_triplets[] = {		//ポリゴンを構成する３点の組の番号 

	//左目下
	cv::Point3i(0, 1, 36), cv::Point3i(1, 36, 41), cv::Point3i(1, 2, 41), cv::Point3i(2, 41, 31),
	cv::Point3i(2, 31, 48), cv::Point3i(2, 3, 48), cv::Point3i(3, 4, 48), cv::Point3i(4, 5, 48),
	cv::Point3i(5, 6, 48),
	//口下
	cv::Point3i(6, 48, 59), cv::Point3i(6, 7, 59), cv::Point3i(7, 59, 58), cv::Point3i(7, 8, 58),
	cv::Point3i(8, 58, 57), cv::Point3i(8, 57, 56), cv::Point3i(8, 9, 56), cv::Point3i(9, 56, 55),
	cv::Point3i(9, 10, 55), cv::Point3i(10, 55, 54),
	//右目下
	cv::Point3i(10, 11, 54), cv::Point3i(11, 54, 12), cv::Point3i(12, 54, 13), cv::Point3i(13, 54, 35),
	cv::Point3i(13, 35, 14), cv::Point3i(14, 35, 15), cv::Point3i(15, 35, 46), cv::Point3i(15, 46, 45),
	cv::Point3i(15, 45, 16),
	//右目上
	cv::Point3i(16, 45, 26), cv::Point3i(26, 45, 25), cv::Point3i(25, 45, 44), cv::Point3i(25, 44, 24),
	cv::Point3i(24, 44, 43), cv::Point3i(24, 43, 23), cv::Point3i(23, 43, 22), cv::Point3i(22, 42, 27),
	cv::Point3i(22, 42, 43),
	//眉間
	cv::Point3i(21, 22, 23), cv::Point3i(20, 21, 23), cv::Point3i(20, 23, 24), cv::Point3i(22, 27, 21),
	//左目上
	cv::Point3i(21, 27, 39), cv::Point3i(21, 39, 20), cv::Point3i(20, 39, 38), cv::Point3i(19, 20, 38),
	cv::Point3i(19, 38, 37), cv::Point3i(19, 37, 18), cv::Point3i(18, 37, 36), cv::Point3i(17, 18, 36),
	cv::Point3i(17, 0, 36),
	//左目
	cv::Point3i(37, 36, 41), cv::Point3i(37, 41, 40), cv::Point3i(37, 38, 40), cv::Point3i(38, 40, 39),
	//右目
	cv::Point3i(42, 43, 47), cv::Point3i(43, 44, 47), cv::Point3i(44, 47, 46), cv::Point3i(44, 46, 45),
	//鼻
	cv::Point3i(40, 41, 31), cv::Point3i(39, 40, 31), cv::Point3i(39, 29, 31), cv::Point3i(39, 28, 29),
	cv::Point3i(39, 28, 27), cv::Point3i(27, 28, 42), cv::Point3i(28, 29, 42), cv::Point3i(29, 42, 47),
	cv::Point3i(29, 35, 47), cv::Point3i(35, 47, 46), cv::Point3i(29, 30, 31), cv::Point3i(29, 30, 35),
	cv::Point3i(30, 31, 32), cv::Point3i(30, 32, 33), cv::Point3i(30, 33, 34), cv::Point3i(30, 34, 35),
	//鼻下
	cv::Point3i(31, 48, 49), cv::Point3i(31, 49, 50), cv::Point3i(31, 32, 50), cv::Point3i(32, 33, 50),
	cv::Point3i(33, 50, 51), cv::Point3i(33, 51, 52), cv::Point3i(33, 34, 52), cv::Point3i(34, 35, 52),
	cv::Point3i(35, 52, 53), cv::Point3i(35, 53, 54),
	//上唇
	cv::Point3i(48, 49, 60), cv::Point3i(49, 60, 61), cv::Point3i(49, 50, 61), cv::Point3i(50, 51, 61),
	cv::Point3i(51, 61, 62), cv::Point3i(51, 62, 63), cv::Point3i(51, 52, 63), cv::Point3i(52, 53, 63),
	cv::Point3i(53, 63, 64), cv::Point3i(53, 54, 64),
	//下唇
	cv::Point3i(48, 59, 60), cv::Point3i(59, 60, 67), cv::Point3i(58, 59, 67), cv::Point3i(58, 67, 66),
	cv::Point3i(57, 58, 66), cv::Point3i(56, 57, 66), cv::Point3i(56, 65, 66), cv::Point3i(55, 56, 65),
	cv::Point3i(55, 64, 65), cv::Point3i(54, 55, 64),
};

cv::Point3i g_mouseInTriplets[] = {		//ポリゴンを構成する３点の組の番号(口の中) 
	cv::Point3i(60, 61, 62), cv::Point3i(62, 63, 64), cv::Point3i(64, 65, 66), cv::Point3i(66, 67, 60),
	cv::Point3i(60, 62, 64), cv::Point3i(60, 66, 64),
};

BOOL g_bTriangles = TRUE;

//変形
BOOL g_bDrawTriangles = FALSE;
BOOL g_bResize = TRUE;

BOOL IsInTriangle(cv::Point2f pnt, std::vector<cv::Point2f> &coord, cv::Point3i &triplet);						//指定する座標pntが三角形(座標：coord,添字triplet)の内部にあるか判定
void CalcAlphaBeta(cv::Point2f pnt, std::vector<cv::Point2f> &coord, cv::Point3i &triplet, cv::Point2f &coef);	//内挿点への係数を算出
void TriangleTest();


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCVDlibTestDlg ダイアログ




CCVDlibTestDlg::CCVDlibTestDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CCVDlibTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCVDlibTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCVDlibTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CCVDlibTestDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_IMAGE, &CCVDlibTestDlg::OnBnClickedBtnImage)
	ON_BN_CLICKED(IDC_BTN_VIDEO, &CCVDlibTestDlg::OnBnClickedBtnVideo)
	ON_BN_CLICKED(IDC_CHK_DRAWTRIANGLE, &CCVDlibTestDlg::OnBnClickedChkDrawtriangle)
	ON_BN_CLICKED(IDC_CHK_RESIZE, &CCVDlibTestDlg::OnBnClickedChkResize)
	ON_BN_CLICKED(IDC_CHK_TIMEMEASURE, &CCVDlibTestDlg::OnBnClickedChkTimemeasure)
	ON_BN_CLICKED(IDC_CHK_MOUSEINNER, &CCVDlibTestDlg::OnBnClickedChkMouseinner)
END_MESSAGE_MAP()


// CCVDlibTestDlg メッセージ ハンドラー

BOOL CCVDlibTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。


	//	frontal_face_detector g_detector = get_frontal_face_detector();
	g_detector = get_frontal_face_detector();
	try
	{
		dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> g_pose_model;
	}
	catch (serialization_error& e)
	{
		CString str;
		str.Format(_T("Deserializatio error:%s"), e.what());
		AfxMessageBox(str);
	}

	((CButton*)GetDlgItem(IDC_CHK_RESIZE))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHK_MOUSEINNER))->SetCheck(1);
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CCVDlibTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CCVDlibTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CCVDlibTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCVDlibTestDlg::OnBnClickedBtnCapture()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	cv::VideoCapture cap(0);
	if (!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．	
	{
		AfxMessageBox(_T("Cant open video"));
		//読み込みに失敗したときの処理
		return;
	}
	cv::Mat frame;
	cap >> frame; // get a new frame from camera
	cap >> frame; // get a new frame from camera
	cap >> frame; // get a new frame from camera

	int key;
	key = cv::waitKey(1);
	while (key != 'q')//無限ループ
	{
		key = cv::waitKey(1);

		cap >> frame; // get a new frame from camera
		if (frame.empty()) break;
		cv::imshow("capture", frame);//カメラ画像を表示．


		IplImage tmp = frame;
		g_cvimg = &tmp;


		cv_image<bgr_pixel> cimg(g_cvimg);
		// Detect faces 
		std::vector<rectangle> faces = g_detector(cimg); // Dlibにかける

		//顔が見つからなければ次のフレーム
		if (faces.size() == 0) continue;

		//顔領域の表示
		CvPoint pntl, pntr;
		if (faces.size() == 1){
			pntr.x = faces[0].bl_corner().x();
			pntr.y = faces[0].bl_corner().y();
			pntl.x = faces[0].tr_corner().x();
			pntl.y = faces[0].tr_corner().y();
			//cvRectangle(g_cvimg, pntl, pntr, CV_RGB(255, 0, 0), 2);

			cvSetImageROI(g_cvimg, cv::Rect(pntl, pntr)); // 画像の切り出し

		}



		// Find the pose of each face.
		std::vector<full_object_detection> shapes;
		shapes.clear();
		CString str;
		for (unsigned long i = 0; i < faces.size(); ++i){
			full_object_detection shape = g_pose_model(cimg, faces[i]);
			shapes.push_back(shape);
		}

		//顔特徴点の表示
		std::vector<cv::Point2f> points;
		points.clear();
		cv::Point2f gravityPoint;

		for (unsigned long i = 0; i < shapes.size(); ++i){
			for (unsigned long k = 0; k < shapes[i].num_parts(); k++){
				CvPoint pnt;
				pnt.x = shapes[i].part(k).x();
				pnt.y = shapes[i].part(k).y();
				cvCircle(g_cvimg, pnt, 3, CV_RGB(255, 0, 0), 2);
				//fprintf(fp,"%d,%d,", pnt.x, pnt.y);
				points.push_back(cv::Point2f(pnt.x, pnt.y));	//ドロネー用
				gravityPoint.x += pnt.x;
				gravityPoint.y += pnt.y;
			}
		}
		//fprintf(fp,"\n");

		gravityPoint.x /= points.size();
		gravityPoint.y /= points.size();
		std::vector<cv::Point2f> displacement;
		displacement.clear();
		cv::Mat cpyMat;
		cpyMat = g_staticMat.clone();
		IplImage staticImg;
		staticImg = cpyMat;
		for (unsigned long i = 0; i < points.size(); ++i){
			displacement.push_back(points[i] - gravityPoint + g_staticGravityPoint);
			//displacement.push_back(points[i] - gravityPoint);
			//cvCircle( &staticImg, displacement[i]+g_staticGravityPoint, 3, CV_RGB(0, 0, 255), 2);
		}
		cv::imshow("image", cv::cvarrToMat(&staticImg));//静止画像を表示．

		if (g_bDrawTriangles){			//三角形表示チェックボックス
			if (shapes.size() != 0){		//顔が無ければ分割不要
				//ドロネー分割
				float max_x, max_y, min_x, min_y;
				max_x = min_x = points[0].x;
				max_y = min_y = points[0].y;
				for (auto it = points.begin(); it != points.end(); it++){
					if (it->x > max_x) max_x = it->x;
					else if (it->x < min_x) min_x = it->x;
					if (it->y > max_y) max_y = it->y;
					else if (it->y < min_y) min_y = it->y;
				}
				pntr.x = max_x, pntr.y = max_y;
				pntl.x = min_x, pntl.y = min_y;
				cvRectangle(g_cvimg, pntl, pntr, CV_RGB(255, 0, 0), 2);
				cv::Subdiv2D subdiv;
				//subdiv.initDelaunay(cv::Rect(0, 0, g_cvimg->width, g_cvimg->height));
				subdiv.initDelaunay(cv::Rect(min_x, min_y, max_x, max_y));
				subdiv.insert(points);
				std::vector<cv::Vec6f> triangles;
				subdiv.getTriangleList(triangles);
				BOOL bDrawLine = TRUE;
				BOOL bP1 = FALSE, bP2 = FALSE, bP3 = FALSE;
				for (auto it = triangles.begin(); it != triangles.end(); it++) {
					cv::Vec6f &vec = *it;
					cv::Point p1(vec[0], vec[1]);
					cv::Point p2(vec[2], vec[3]);
					cv::Point p3(vec[4], vec[5]);
					for (auto itt = points.begin(); itt != points.end(); itt++){
						if ((p1.x == itt->x) && (p1.y == itt->y)) bP1 = TRUE;
						if ((p2.x == itt->x) && (p2.y == itt->y)) bP2 = TRUE;
						if ((p3.x == itt->x) && (p3.y == itt->y)) bP3 = TRUE;
					}
					if (bP1 && bP2 && bP3){
						cv::line(cv::cvarrToMat(g_cvimg), p1, p2, cv::Scalar(64, 255, 128));
						cv::line(cv::cvarrToMat(g_cvimg), p2, p3, cv::Scalar(64, 255, 128));
						cv::line(cv::cvarrToMat(g_cvimg), p3, p1, cv::Scalar(64, 255, 128));
					}
					bP1 = bP2 = bP3 = FALSE;
				}
			}
		}

		//ここから変形
		cv::Point2f pos;
		cv::Point2f npnt;
		cv::Point2f coef;
		//cv::Mat deformedMat;
		//deformedMat = g_staticMat.clone();
		int height = g_staticMat.rows;
		int width = g_staticMat.cols;
		//cv::Mat deformedMat(width, height, CV_8U, cv::Scalar(0));
		cv::Mat deformedMat(width, height, CV_8UC3, CV_RGB(0, 0, 0));
		std::vector<cv::Point3i> vecTriplet(g_triplets, std::end(g_triplets));
		std::vector<cv::Point3i> vecMouseTriplet(g_mouseInTriplets, std::end(g_mouseInTriplets));

		if (g_bMouseInner) {
			vecTriplet.insert(vecTriplet.end(), vecMouseTriplet.begin(), vecMouseTriplet.end());
		}

		for (int j = 0; j < height; j++){
			for (int i = 0; i < width; i++){
				pos.x = i, pos.y = j;
				cv::Point3i triplet;
				for (auto itr = vecTriplet.begin(); itr != vecTriplet.end(); itr++) {
					triplet = *itr;
					if (IsInTriangle(pos, displacement, triplet)){
						CalcAlphaBeta(pos, displacement, triplet, coef);
						npnt = coef.x * (g_TargetPoints[triplet.x] - g_TargetPoints[triplet.z]) + coef.y * (g_TargetPoints[triplet.y] - g_TargetPoints[triplet.z]) + g_TargetPoints[triplet.z];//旧座標
						//npnt = coef.x * (points[triplet.x] - points[triplet.z]) + coef.y * (points[triplet.y] - points[triplet.z]) + points[triplet.z];//旧座標
						//deformedMat.at<unsigned char>(pos) = cpyMat.at<cv::Vec3b>(npnt)[0];
						//deformedMat.at<cv::Vec3b>(pos) = cpyMat.at<cv::Vec3b>(npnt); // ニアレストレイバー
						
						// バイリニア補間
						double n_xy1 = ((int)npnt.x + 1 - npnt.x) * ((int)npnt.y + 1 - npnt.y);
						double n_xy2 = ((int)npnt.x + 1 - npnt.x) * (npnt.y - (int)npnt.y);
						double n_xy3 = (npnt.x - (int)npnt.x) * ((int)npnt.y + 1 - npnt.y);
						double n_xy4 = (npnt.x - (int)npnt.x) * (npnt.y - (int)npnt.y);
						//std::wstringstream ss;
						//ss << xy1 << "\n" << xy2 << "\n" << xy3 << "\n" << xy4 << "\n";
						//OutputDebugString((LPCWSTR)(ss.str().c_str()));
						double b = n_xy1 *  (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + (int)npnt.x * 3])
							+ n_xy2 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + (int)npnt.x * 3])
							+ n_xy3 * (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + ((int)npnt.x + 1) * 3])
							+ n_xy4 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + ((int)npnt.x + 1) * 3]);

						double g = n_xy1 *  (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + (int)npnt.x * 3 + 1])
							+ n_xy2 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + (int)npnt.x * 3 + 1])
							+ n_xy3 * (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + ((int)npnt.x + 1) * 3 + 1])
							+ n_xy4 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + ((int)npnt.x + 1) * 3 + 1]);

						double r = n_xy1 *  (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + (int)npnt.x * 3 + 2])
							+ n_xy2 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + (int)npnt.x * 3 + 2])
							+ n_xy3 * (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + ((int)npnt.x + 1) * 3 + 2])
							+ n_xy4 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + ((int)npnt.x + 1) * 3 + 2]);

						cv::Vec3b new_color = cv::Vec3b(b, g, r);

						deformedMat.at<cv::Vec3b>(pos) = new_color; // ここまでバイリニア補間
						
						TRACE("POS:pos(%f,%f),points0(%f,%f),npt(%f,%f)", pos.x, pos.y, points[triplet.x].x, points[triplet.x].y, npnt.x, npnt.y);
						//fprintf(fp,"POS:pos(%f,%f),points0(%f,%f),npt(%f,%f)\n", pos.x, pos.y, points[triplet.x].x, points[triplet.x].y, npnt.x, npnt.y);
						if (g_bDrawTriangles){
							cv::line(cpyMat, g_TargetPoints[triplet.x], g_TargetPoints[triplet.y], cv::Scalar(255));
							cv::line(cpyMat, g_TargetPoints[triplet.y], g_TargetPoints[triplet.z], cv::Scalar(255));
							cv::line(cpyMat, g_TargetPoints[triplet.z], g_TargetPoints[triplet.x], cv::Scalar(255));
							//cv::line(deformedMat, displacement[triplet.x], displacement[triplet.y], cv::Scalar(255));
							//cv::line(deformedMat, displacement[triplet.y], displacement[triplet.z], cv::Scalar(255));
							//cv::line(deformedMat, displacement[triplet.z], displacement[triplet.x], cv::Scalar(255));
							cv::line(deformedMat, displacement[triplet.x], displacement[triplet.y], CV_RGB(255, 255, 255));  //color
							cv::line(deformedMat, displacement[triplet.y], displacement[triplet.z], CV_RGB(255, 255, 255));
							cv::line(deformedMat, displacement[triplet.z], displacement[triplet.x], CV_RGB(255, 255, 255));
						}
					}
				}
			}
		}

		if (g_bResize){
			cv::Mat resizedMat;
			cv::resize(deformedMat, resizedMat, deformedMat.size() * 2);
			cv::imshow("resized", resizedMat);
		}
		else {
			cv::imshow("dst", deformedMat);
		}
		//cv::imshow("cpy", cpyMat);


		cv::imshow("face", cv::cvarrToMat(g_cvimg));// 顔領域を表示．
		//writer << deformedMat;

	}// while(key)	
}


void CCVDlibTestDlg::OnBnClickedBtnImage()
{
	CString str;
	char filename[512];
	CFileDialog loadDLG(TRUE, _T("jpg"), _T("*.jpg"), OFN_FILEMUSTEXIST | OFN_READONLY,
		_T("JPGデータ(*.jpg)|*.jpg|pngデータ(*.png)|*.png|BMPデータ(*.bmp)|*.bmp|(*.*)|*.*|"));//FALSE:保存, TRUE:読み込み
	if (loadDLG.DoModal() == IDOK){
		strncpy(filename, CT2CA(loadDLG.GetPathName()), 512);

		g_staticMat = cv::imread(filename, 1);	// 画像を読込んでcv::Matに代入
		cv::Mat tmpMat;
		tmpMat = g_staticMat.clone();
		IplImage tmpimg = tmpMat;  // cv::MatからIplImageに変換


		cv_image<bgr_pixel> cimg(tmpimg); // IplImageからcv_imageに変換
		// Detect faces 
		std::vector<rectangle> faces = g_detector(cimg);

		//顔領域の表示
		CvPoint pntl, pntr;
		pntr.x = faces[0].bl_corner().x();
		pntr.y = faces[0].bl_corner().y();
		pntl.x = faces[0].tr_corner().x();
		pntl.y = faces[0].tr_corner().y();
		//cvRectangle(g_cvimg,pntl,pntr,CV_RGB(255,0,0), 2);

		// Find the pose of each face.
		std::vector<full_object_detection> shapes;

		for (unsigned long i = 0; i < faces.size(); ++i){
			full_object_detection shape = g_pose_model(cimg, faces[i]);
			shapes.push_back(shape);
		}

		//顔特徴点の表示
		g_staticGravityPoint = cv::Point2f(0.0, 0.0);
		g_TargetPoints.clear();

		for (unsigned long i = 0; i < shapes.size(); ++i){
			for (unsigned long k = 0; k < shapes[i].num_parts(); k++){
				CvPoint pnt;
				pnt.x = shapes[i].part(k).x();
				pnt.y = shapes[i].part(k).y();
				//g_staticPoints.push_back(cv::Point2f(pnt.x, pnt.y));	//ドロネー用
				cvCircle(&tmpimg, pnt, 1, CV_RGB(255, 0, 0), 2);
				g_TargetPoints.push_back(pnt);
				g_staticGravityPoint.x += pnt.x;
				g_staticGravityPoint.y += pnt.y;
			}
		}
		g_staticGravityPoint.x /= 68.0;
		g_staticGravityPoint.y /= 68.0;
		cvCircle(&tmpimg, g_staticGravityPoint, 3, CV_RGB(0, 0, 255), 2);		//重心の表示

		if (shapes.size() != 0){		//顔が無ければ分割不要
			//ドロネー分割
			float max_x, max_y, min_x, min_y;
			max_x = min_x = g_TargetPoints[0].x;
			max_y = min_y = g_TargetPoints[0].y;
			for (auto it = g_TargetPoints.begin(); it != g_TargetPoints.end(); it++){
				if (it->x > max_x) max_x = it->x;
				else if (it->x < min_x) min_x = it->x;
				if (it->y > max_y) max_y = it->y;
				else if (it->y < min_y) min_y = it->y;
			}
			pntr.x = max_x, pntr.y = max_y;
			pntl.x = min_x, pntl.y = min_y;
			cvRectangle(&tmpimg, pntl, pntr, CV_RGB(255, 0, 0), 2);
			cv::Subdiv2D subdiv;
			//subdiv.initDelaunay(cv::Rect(0, 0, g_cvimg->width, g_cvimg->height));
			subdiv.initDelaunay(cv::Rect(min_x, min_y, max_x, max_y));
			subdiv.insert(g_TargetPoints);
			std::vector<cv::Vec6f> triangles;
			subdiv.getTriangleList(triangles);
			BOOL bDrawLine = TRUE;
			BOOL bP1 = FALSE, bP2 = FALSE, bP3 = FALSE;
			g_Triangles.resize(0);
			cv::Point3i idx;
			for (auto it = triangles.begin(); it != triangles.end(); it++) {
				cv::Vec6f &vec = *it;
				cv::Point p1(vec[0], vec[1]);
				cv::Point p2(vec[2], vec[3]);
				cv::Point p3(vec[4], vec[5]);
				//int idx[3];
				for (auto itt = g_TargetPoints.begin(); itt != g_TargetPoints.end(); itt++){
					if ((p1.x == itt->x) && (p1.y == itt->y)) {
						idx.x = find(g_TargetPoints.begin(), g_TargetPoints.end(), *itt) - g_TargetPoints.begin();
						bP1 = TRUE;
					}
					if ((p2.x == itt->x) && (p2.y == itt->y)) {
						idx.y = find(g_TargetPoints.begin(), g_TargetPoints.end(), *itt) - g_TargetPoints.begin();
						bP2 = TRUE;
					}
					if ((p3.x == itt->x) && (p3.y == itt->y)) {
						idx.z = find(g_TargetPoints.begin(), g_TargetPoints.end(), *itt) - g_TargetPoints.begin();
						bP3 = TRUE;
					}
				}
				if (bP1 && bP2 && bP3){
					g_Triangles.push_back(idx);
					cv::line(tmpMat, p1, p2, cv::Scalar(64, 255, 128));
					cv::line(tmpMat, p2, p3, cv::Scalar(64, 255, 128));
					cv::line(tmpMat, p3, p1, cv::Scalar(64, 255, 128));
				}
				bP1 = bP2 = bP3 = FALSE;
			}

		}
		cv::imshow("image", tmpMat);//画像を表示．

		//三角形テスト
		//std::vector <cv::Point3i> vectmp(g_tmpval, std::end(g_tmpval));
		//g_Triangles=vectmp;
		CString strtmp;
		for (auto it = g_Triangles.begin(); it != g_Triangles.end(); it++) {
			strtmp.Format(_T("%d %d %d\n"), it->x, it->y, it->z);
			str += strtmp;
		}
		strtmp.Format(_T("num=%d"), g_Triangles.size());
		//str = strtmp;
		//AfxMessageBox(str);
		if (g_Triangles.size() == 107){
			g_bTriangles = TRUE;
			//g_TargetPoints = points;
		}

	}
}


void CCVDlibTestDlg::OnBnClickedBtnVideo()
{
	//TriangleTest();

	char filename[512];
	long duration;
	CString tmpstr;

	CFileDialog loadDLG(TRUE, _T("mp4"), _T("*.mp4"), OFN_FILEMUSTEXIST | OFN_READONLY,
		_T("mp4データ(*.mp4)|*.mp4|aviデータ(*.avi)|*.avi|(*.*)|*.*|"));//FALSE:保存, TRUE:読み込み

	if (loadDLG.DoModal() == IDOK){
		/*//cv::VideoWriter writer("videofile.avi", CV_FOURCC_MACRO('D', 'I', 'B', ' '), 5.0, g_staticMat.size() , true);
		//cv::VideoWriter writer("video.avi", -1, 15, g_staticMat.size(), true);
		//cv::VideoWriter writer("video.avi", CV_FOURCC_DEFAULT, 15, g_staticMat.size(), true);
		//cv::VideoWriter writer("videofile.avi", cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 29.97, g_staticMat.size(), true);
		//if (!writer.isOpened()){
		AfxMessageBox(_T("Cant open writer."));
		}
		*/
		if (g_bTimeMeas){
			g_cputimer.Start();
		}
		strncpy(filename, CT2CA(loadDLG.GetPathName()), 512);
		cv::VideoCapture cap(filename);

		if (!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．	
		{
			AfxMessageBox(L"Cant open video");
			return;
		}
		if (g_bTimeMeas){
			duration = g_cputimer.GetElapsedTimeReset();
			g_str.Format(_T("VideoOpen:%.2f ms\n"), duration / 1000.0);
		}

		//FILE *fp; // ファイルポインタを作成
		//fp = fopen("D:/Matsune/Face/data/DlibResult.csv", "w");

		//FILE *fp2; // ファイルポインタを作成
		//fp2 = fopen("D:/Matsune/Face/data/DlibResult_x10.csv", "w");

		int key;
		key = cv::waitKey(1);
		while (key != 'q')//無限ループ
		{
			key = cv::waitKey(1);

			cv::Mat frame;
			cap >> frame; // get a new frame from camera
			if (frame.empty()) break;
			cv::imshow("video", frame);// 原動画を表示．

			if (g_bTimeMeas){
				duration = g_cputimer.GetElapsedTimeReset();
				tmpstr.Format(_T("VideoRead:%.2f ms\n"), duration / 1000.0);
				g_str += tmpstr;
			}

			IplImage tmp = frame;
			g_cvimg = &tmp;

			cv::Mat mat = cv::cvarrToMat(g_cvimg);  // IplImageからcv::Matに変換
			cv::Mat rmat;
			cv::resize(mat, rmat, mat.size() * 10, CV_INTER_LINEAR);
			IplImage rimg = rmat;

			cv_image<bgr_pixel> cimg(g_cvimg);
			// Detect faces 
			std::vector<rectangle> faces = g_detector(cimg);
			if (g_bTimeMeas){
				duration = g_cputimer.GetElapsedTimeReset();
				tmpstr.Format(_T("FaceDetection:%.2f ms\n"), duration / 1000.0);
				g_str += tmpstr;
			}


			//顔領域の表示
			CvPoint pntl, pntr;
			if (faces.size() == 1){
				pntr.x = faces[0].bl_corner().x();
				pntr.y = faces[0].bl_corner().y();
				pntl.x = faces[0].tr_corner().x();
				pntl.y = faces[0].tr_corner().y();
				//cvRectangle(g_cvimg, pntl, pntr, CV_RGB(255, 0, 0), 2);

				cvSetImageROI(g_cvimg, cv::Rect(pntl, pntr)); // 画像の切り出し

			}
			// Find the pose of each face.
			std::vector<full_object_detection> shapes;
			shapes.clear();
			CString str;
			for (unsigned long i = 0; i < faces.size(); ++i){
				full_object_detection shape = g_pose_model(cimg, faces[i]);
				shapes.push_back(shape);
			}
			if (g_bTimeMeas){
				duration = g_cputimer.GetElapsedTimeReset();
				tmpstr.Format(_T("FeaturePointsDetection:%.2f ms\n"), duration / 1000.0);
				g_str += tmpstr;
			}

			//顔特徴点の表示
			std::vector<cv::Point2f> points;
			points.clear();
			cv::Point2f gravityPoint;
			for (unsigned long i = 0; i < shapes.size(); ++i){
				for (unsigned long k = 0; k < shapes[i].num_parts(); k++){
					CvPoint pnt;
					pnt.x = shapes[i].part(k).x();
					pnt.y = shapes[i].part(k).y();
					cvCircle(g_cvimg, pnt, 3, CV_RGB(255, 0, 0), 2);
					//fprintf(fp, "%d,%d,", pnt.x, pnt.y); // pn.xとpnt.yをfpに保存
					points.push_back(cv::Point2f(pnt.x, pnt.y));	//ドロネー用
					gravityPoint.x += pnt.x;
					gravityPoint.y += pnt.y;
				}
			}
			//fprintf(fp, "\n"); // 改行 
			if (g_bTimeMeas){
				duration = g_cputimer.GetElapsedTimeReset();
				tmpstr.Format(_T("FeaturePointsView:%2.f ms\n"), duration / 1000.0);
				g_str += tmpstr;
			}

			gravityPoint.x /= points.size();
			gravityPoint.y /= points.size();
			std::vector<cv::Point2f> displacement;
			displacement.clear();
			cv::Mat cpyMat;
			cpyMat = g_staticMat.clone();
			IplImage staticImg;
			staticImg = cpyMat;
			for (unsigned long i = 0; i < points.size(); ++i){
				displacement.push_back(points[i] - gravityPoint + g_staticGravityPoint);
				//displacement.push_back(points[i] - gravityPoint);
				//cvCircle( &staticImg, displacement[i]+g_staticGravityPoint, 3, CV_RGB(0, 0, 255), 2);
			}
			//cv::imshow("image", cv::cvarrToMat(&staticImg));//画像を表示．

			if (g_bDrawTriangles){			//三角形表示チェックボックス
				if (shapes.size() != 0){		//顔が無ければ分割不要
					//ドロネー分割
					float max_x, max_y, min_x, min_y;
					max_x = min_x = points[0].x;
					max_y = min_y = points[0].y;
					for (auto it = points.begin(); it != points.end(); it++){
						if (it->x > max_x) max_x = it->x;
						else if (it->x < min_x) min_x = it->x;
						if (it->y > max_y) max_y = it->y;
						else if (it->y < min_y) min_y = it->y;
					}
					pntr.x = max_x, pntr.y = max_y;
					pntl.x = min_x, pntl.y = min_y;
					cvRectangle(g_cvimg, pntl, pntr, CV_RGB(255, 0, 0), 2);
					cv::Subdiv2D subdiv;
					//subdiv.initDelaunay(cv::Rect(0, 0, g_cvimg->width, g_cvimg->height));
					subdiv.initDelaunay(cv::Rect(min_x, min_y, max_x, max_y));
					subdiv.insert(points);
					std::vector<cv::Vec6f> triangles;
					subdiv.getTriangleList(triangles);
					BOOL bDrawLine = TRUE;
					BOOL bP1 = FALSE, bP2 = FALSE, bP3 = FALSE;
					for (auto it = triangles.begin(); it != triangles.end(); it++) {
						cv::Vec6f &vec = *it;
						cv::Point p1(vec[0], vec[1]);
						cv::Point p2(vec[2], vec[3]);
						cv::Point p3(vec[4], vec[5]);
						for (auto itt = points.begin(); itt != points.end(); itt++){
							if ((p1.x == itt->x) && (p1.y == itt->y)) bP1 = TRUE;
							if ((p2.x == itt->x) && (p2.y == itt->y)) bP2 = TRUE;
							if ((p3.x == itt->x) && (p3.y == itt->y)) bP3 = TRUE;
						}
						if (bP1 && bP2 && bP3){
							cv::line(cv::cvarrToMat(g_cvimg), p1, p2, cv::Scalar(64, 255, 128));
							cv::line(cv::cvarrToMat(g_cvimg), p2, p3, cv::Scalar(64, 255, 128));
							cv::line(cv::cvarrToMat(g_cvimg), p3, p1, cv::Scalar(64, 255, 128));
						}
						bP1 = bP2 = bP3 = FALSE;
					}
				}
				if (g_bTimeMeas){
					duration = g_cputimer.GetElapsedTimeReset();
					tmpstr.Format(_T("TrianglesView:%.2f ms\n"), duration / 1000.0);
					g_str += tmpstr;
				}
			}


			//ここから変形
			cv::Point2f pos;
			cv::Point2f npnt;
			cv::Point2f coef;
			//cv::Mat deformedMat;
			//deformedMat = g_staticMat.clone();
			int height = g_staticMat.rows;
			int width = g_staticMat.cols;
			//cv::Mat deformedMat(width, height, CV_8U, cv::Scalar(0));
			cv::Mat deformedMat(width, height, CV_8UC3, CV_RGB(0, 0, 0));
			std::vector<cv::Point3i> vecTriplet(g_triplets, std::end(g_triplets));

			for (int j = 0; j < height; j++){
				for (int i = 0; i < width; i++){
					pos.x = i, pos.y = j;
					cv::Point3i triplet;
					for (int k = 0; k < vecTriplet.size(); k++){
						triplet = g_triplets[k];
						if (IsInTriangle(pos, displacement, triplet)){
							CalcAlphaBeta(pos, displacement, triplet, coef);
							npnt = coef.x * (g_TargetPoints[triplet.x] - g_TargetPoints[triplet.z]) + coef.y * (g_TargetPoints[triplet.y] - g_TargetPoints[triplet.z]) + g_TargetPoints[triplet.z];//旧座標
							//npnt = coef.x * (points[triplet.x] - points[triplet.z]) + coef.y * (points[triplet.y] - points[triplet.z]) + points[triplet.z];//旧座標
							//deformedMat.at<unsigned char>(pos) = cpyMat.at<cv::Vec3b> (npnt)[0];
							//deformedMat.at<cv::Vec3b>(pos) = cpyMat.at<cv::Vec3b>(npnt); // ニアレストネイバー

							// バイリニア補間
							double n_xy1 = ((int)npnt.x + 1 - npnt.x) * ((int)npnt.y + 1 - npnt.y);
							double n_xy2 = ((int)npnt.x + 1 - npnt.x) * (npnt.y - (int)npnt.y);
							double n_xy3 = (npnt.x - (int)npnt.x) * ((int)npnt.y + 1 - npnt.y);
							double n_xy4 = (npnt.x - (int)npnt.x) * (npnt.y - (int)npnt.y);
							//std::wstringstream ss;
							//ss << xy1 << "\n" << xy2 << "\n" << xy3 << "\n" << xy4 << "\n";
							//OutputDebugString((LPCWSTR)(ss.str().c_str()));
							double b = n_xy1 *  (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + (int)npnt.x * 3])
								+ n_xy2 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + (int)npnt.x * 3])
								+ n_xy3 * (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + ((int)npnt.x + 1) * 3])
								+ n_xy4 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + ((int)npnt.x + 1) * 3]);

							double g = n_xy1 *  (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + (int)npnt.x * 3 + 1])
								+ n_xy2 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + (int)npnt.x * 3 + 1])
								+ n_xy3 * (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + ((int)npnt.x + 1) * 3 + 1])
								+ n_xy4 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + ((int)npnt.x + 1) * 3 + 1]);

							double r = n_xy1 *  (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + (int)npnt.x * 3 + 2])
								+ n_xy2 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + (int)npnt.x * 3 + 2])
								+ n_xy3 * (unsigned char)(staticImg.imageData[staticImg.widthStep * (int)npnt.y + ((int)npnt.x + 1) * 3 + 2])
								+ n_xy4 * (unsigned char)(staticImg.imageData[staticImg.widthStep * ((int)npnt.y + 1) + ((int)npnt.x + 1) * 3 + 2]);

							cv::Vec3b new_color = cv::Vec3b(b, g, r);

							deformedMat.at<cv::Vec3b>(pos) = new_color; // ここまでバイリニア補間

							TRACE("POS:pos(%f,%f),points0(%f,%f),npt(%f,%f)", pos.x, pos.y, points[triplet.x].x, points[triplet.x].y, npnt.x, npnt.y); // printf
							//fprintf(fp,"POS:pos(%f,%f),points0(%f,%f),npt(%f,%f)\n", pos.x, pos.y, points[triplet.x].x, points[triplet.x].y, npnt.x, npnt.y);
							if (g_bDrawTriangles){
								cv::line(cpyMat, g_TargetPoints[triplet.x], g_TargetPoints[triplet.y], cv::Scalar(255));
								cv::line(cpyMat, g_TargetPoints[triplet.y], g_TargetPoints[triplet.z], cv::Scalar(255));
								cv::line(cpyMat, g_TargetPoints[triplet.z], g_TargetPoints[triplet.x], cv::Scalar(255));
								//cv::line(deformedMat, displacement[triplet.x], displacement[triplet.y], cv::Scalar(255)); // monocrhome
								//cv::line(deformedMat, displacement[triplet.y], displacement[triplet.z], cv::Scalar(255)); 
								//cv::line(deformedMat, displacement[triplet.z], displacement[triplet.x], cv::Scalar(255));
								cv::line(deformedMat, displacement[triplet.x], displacement[triplet.y], CV_RGB(255, 255, 255));  //color
								cv::line(deformedMat, displacement[triplet.y], displacement[triplet.z], CV_RGB(255, 255, 255));
								cv::line(deformedMat, displacement[triplet.z], displacement[triplet.x], CV_RGB(255, 255, 255));
							}
						}
					}
				}
			}
			if (g_bTimeMeas){
				duration = g_cputimer.GetElapsedTimeReset();
				tmpstr.Format(_T("Deformation:%.2f ms\n\n"), duration / 1000.0);
				g_str += tmpstr;
			}

			if (g_bResize){
				cv::Mat resizedMat;
				cv::resize(deformedMat, resizedMat, deformedMat.size() * 2);
				cv::imshow("resized", resizedMat); // 2倍にリサイズした動く静止画を表示
			}
			else {
				cv::imshow("dst", deformedMat);
			}
			//cv::imshow("cpy", cpyMat); // 静止画像のコピーを表示
			//cv::imshow("sample", cv::cvarrToMat(g_cvimg));//顔領域のみを表示．

			/* バイリニア補間 切り出した顔領域を10倍表示させて補間する
			int new_w = abs(pntr.x - pntl.x);
			int new_h = abs(pntr.y - pntl.y);
			IplImage* create = cvCreateImage(CvSize(new_w * 10, new_h * 10), IPL_DEPTH_8U, 3);

			for (int h = 0; h < new_h * 10; h++) {
				for (int w = 0; w < new_w * 10; w++) {
					double y = pntl.y + (h * 0.1);
					double x = pntr.x + (w * 0.1);

					//create->imageData[create->widthStep * h + w * 3]     = g_cvimg->imageData[g_cvimg->widthStep * (int)y + (int)x * 3];        // B
					//create->imageData[create->widthStep * h + w * 3 + 1] = g_cvimg->imageData[g_cvimg->widthStep * (int)y + (int)x * 3 + 1];    // G
					//create->imageData[create->widthStep * h + w * 3 + 2] = g_cvimg->imageData[g_cvimg->widthStep * (int)y + (int)x * 3 + 2];    // R

					double xy1 = ((int)x + 1 - x) * ((int)y + 1 - y);
					double xy2 = ((int)x + 1 - x) * (y - (int)y);
					double xy3 = (x - (int)x) * ((int)y + 1 - y);
					double xy4 = (x - (int)x) * (y - (int)y);
					//std::wstringstream ss;
					//ss << xy1 << "\n" << xy2 << "\n" << xy3 << "\n" << xy4 << "\n";
					//OutputDebugString((LPCWSTR)(ss.str().c_str()));

					for (int i = 0; i < 3; i++) {
						double create_color = xy1 *  (unsigned char)(g_cvimg->imageData[g_cvimg->widthStep * (int)y + (int)x * 3 + i])
							+ xy2 * (unsigned char)(g_cvimg->imageData[g_cvimg->widthStep * ((int)y + 1) + (int)x * 3 + i])
							+ xy3 * (unsigned char)(g_cvimg->imageData[g_cvimg->widthStep * (int)y + ((int)x + 1) * 3 + i])
							+ xy4 * (unsigned char)(g_cvimg->imageData[g_cvimg->widthStep * ((int)y + 1) + ((int)x + 1) * 3 + i]);

						// if ((g_cvimg->imageData[g_cvimg->widthStep * (int)y + (int)x * 3 + i]) < 0) {

							// std::wstringstream ss;
							// ss << (unsigned char)(g_cvimg->imageData[g_cvimg->widthStep * (int)y + (int)x * 3 + i]) << "\n";
							// OutputDebugString((LPCWSTR)(ss.str().c_str()));
						// }

						// create_color = (create_color > 255) ? 255 : create_color;
						// create_color = (create_color < 0) ? 0 : create_color;
						// create_color = abs(create_color);

						// if (create_color > 255) {
							// std::wstringstream ss;
							// ss << create_color;
							// OutputDebugString((LPCWSTR)(ss.str().c_str()));
							// create_color = 255;
						// }
						// else if (create_color < 0) {
							// std::wstringstream ss;
							// ss << create_color;
							// OutputDebugString((LPCWSTR)(ss.str().c_str()));
							// create_color = 0;
						// }

						create->imageData[create->widthStep * h + w * 3 + i] = create_color;
					}
				}
			}
			*/
			
			/*

			cv_image<bgr_pixel> cimg2(rimg);
			// Detect faces 
			std::vector<rectangle> big_faces = g_detector(cimg2); // Dlibにかける

			//cv::imshow("big", cv::cvarrToMat(create));//画像を表示．

			//顔が見つからなければ次のフレーム
			if (big_faces.size() == 0) continue;

			// Find the pose of each face.
			std::vector<full_object_detection> new_shapes;
			new_shapes.clear();
			CString new_str;
			for (unsigned long i = 0; i < big_faces.size(); ++i){
				full_object_detection new_shape = g_pose_model(cimg2, big_faces[i]);
				new_shapes.push_back(new_shape);
			}

			//顔特徴点の保存
			std::vector<cv::Point2f> new_points;
			new_points.clear();
			cv::Point2f new_gravityPoint;

			for (unsigned long i = 0; i < new_shapes.size(); ++i){
				for (unsigned long k = 0; k < new_shapes[i].num_parts(); k++){
					CvPoint new_pnt;
					new_pnt.x = new_shapes[i].part(k).x();
					new_pnt.y = new_shapes[i].part(k).y();
					//fprintf(fp2, "%d,%d,", new_pnt.x, new_pnt.y); // pn.xとpnt.yをfp2に保存
					new_points.push_back(cv::Point2f(new_pnt.x, new_pnt.y));	//ドロネー用
					new_gravityPoint.x += new_pnt.x;
					new_gravityPoint.y += new_pnt.y;
				}
			}
			//cv::imshow("big", cv::cvarrToMat(create));//画像を表示．
			//cv::imshow("cimg2", cv::cvarrToMat(&rimg)); // 全体を10倍にリサイズした画像を表示
			//fprintf(fp2, "\n"); // 改行 

			cv::imshow("face", cv::cvarrToMat(g_cvimg));// 顔領域を表示．
			//writer << deformedMat; // ビデオの保存?
			*/
		}// while(key)

		//fclose(fp);
		//fclose(fp2); // ファイルを閉じる


		//writer.release();

		if (g_bTimeMeas){
			AfxMessageBox(g_str);
			CStdioFile    stdFile;
			int err = 0;
			if (!stdFile.Open(_T("C:\\tmp\\fileTest.txt"), CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
				err = 1;
			if (!err){
				TRY{ stdFile.WriteString(g_str); }
				CATCH(CFileException, eP) { err = 1; }
				END_CATCH
			}

		}
	}//if( loadDLG.DoModal() == IDOK)
}

BOOL IsInTriangle(cv::Point2f pnt, std::vector<cv::Point2f> &coord, cv::Point3i &triplet)
{
	cv::Point2f vec[3];
	double z[3];
	int tri[3];
	tri[0] = triplet.x;
	tri[1] = triplet.y;
	tri[2] = triplet.z;

	for (int i = 0; i < 3; i++){
		vec[i].x = coord[tri[i]].x - pnt.x;
		vec[i].y = coord[tri[i]].y - pnt.y;
		//if (pnt.x == vec[i].x && pnt.y == vec[i].y) return TRUE;	//一致していたら内部
		//TRACE("coord[%d]:(%.1f,%.1f) tri[]=%d\n", i, vec[i].x, vec[i].y, tri[i]);
	}
	for (int i = 0; i < 3; i++){
		z[i] = vec[i].x*vec[(i + 1) % 3].y - vec[i].y*vec[(i + 1) % 3].x;
	}
	if (z[0] >= 0.0 && z[1] >= 0.0 && z[2] >= 0.0){
		return TRUE;
	}
	if (z[0] <= 0.0 && z[1] <= 0.0 && z[2] <= 0.0){
		return TRUE;
	}
	return FALSE;
}
void CalcAlphaBeta(cv::Point2f pnt, std::vector<cv::Point2f> &coord, cv::Point3i &triplet, cv::Point2f &coef)
//coef.x : alpha
//coec.y : beta
{
	cv::Point2f vec[2], p;

	int tri[3];
	tri[0] = triplet.x;
	tri[1] = triplet.y;
	tri[2] = triplet.z;
	for (int i = 0; i < 2; i++){
		vec[i].x = coord[tri[i]].x - coord[tri[2]].x;
		vec[i].y = coord[tri[i]].y - coord[tri[2]].y;
	}
	p.x = pnt.x - coord[tri[2]].x;
	p.y = pnt.y - coord[tri[2]].y;

	cv::Mat b(cv::Size(1, 2), CV_32F);
	cv::Mat A(cv::Size(2, 2), CV_32F);
	cv::Mat x(cv::Size(1, 2), CV_32F);
	b.at<float>(0, 0) = p.dot(vec[0]);
	b.at<float>(1, 0) = p.dot(vec[1]);
	A.at<float>(0, 0) = vec[0].dot(vec[0]);
	A.at<float>(1, 0) = vec[0].dot(vec[1]);
	A.at<float>(0, 1) = vec[0].dot(vec[1]);
	A.at<float>(1, 1) = vec[1].dot(vec[1]);
	x = A.inv() * b;

	coef.x = x.at<float>(0, 0);
	coef.y = x.at<float>(1, 0);
}

void TriangleTest()
//[1] 10x10画素の各画素が３角形内部か否かを判定し１と０で出力する
//[2] alpha,betaの計算結果を確認する
{
	cv::Point3i triplet(0, 1, 2);
	std::vector<cv::Point2f> points;

	cv::Point2f p1(3, 3);
	cv::Point2f p2(3, 8);
	cv::Point2f p3(8, 3);

	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);

	cv::Point2f pnt(2, 2);
	/*
	BOOL flag;
	flag = IsInTriangle(pnt, points, triplet);
	if (flag)
	AfxMessageBox(_T("In"));
	else
	AfxMessageBox(_T("Out"));
	*/
	BOOL flag;
	char tmp[10 * 10];
	for (int j = 0; j < 10; j++){
		for (int i = 0; i < 10; i++){
			pnt.x = i;	pnt.y = j;
			flag = IsInTriangle(pnt, points, triplet);
			if (flag){
				tmp[i + j * 10] = 1;
				_RPT0(_CRT_WARN, "1");
			}
			else {
				tmp[i + j * 10] = 0;
				_RPT0(_CRT_WARN, "0");
			}
		}
		_RPT0(_CRT_WARN, "\n");
	}

	//[2]
	cv::Point2f npnt;
	cv::Point2f coef;

	//	points.clear();
	//	p1 = cv::Point2f(0, 2);
	//	p2 = cv::Point2f(4, 0);
	//	p3 = cv::Point2f(0, 0);
	//	points.push_back(p1);
	//	points.push_back(p2);
	//	points.push_back(p3);
	pnt.x = 2;	pnt.y = 2;
	for (int j = 0; j < 10; j++){
		for (int i = 0; i < 10; i++){
			pnt.x = i;	pnt.y = j;

			CalcAlphaBeta(pnt, points, triplet, coef);
			TRACE("[%f, %f]\n", coef.x, coef.y);
			npnt = coef.x * (points[triplet.x] - points[triplet.z]) + coef.y * (points[triplet.y] - points[triplet.z]) + points[triplet.z];
			TRACE("[%f, %f]=(%f, %f)\n", pnt.x, pnt.y, npnt.x, npnt.y);
		}
	}

	//[3] 変形
#define SIZEX 100
#define SIZEY 100
	int width = SIZEX, height = SIZEY;
	points.clear();
	p1 = cv::Point2f(10, 80);
	p2 = cv::Point2f(80, 10);
	p3 = cv::Point2f(10, 10);
	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);
	std::vector<cv::Point2f> dstpnt;
	p1 = cv::Point2f(80, 80);
	p2 = cv::Point2f(80, 10);
	p3 = cv::Point2f(10, 10);
	dstpnt.push_back(p1);
	dstpnt.push_back(p2);
	dstpnt.push_back(p3);
	unsigned char img[SIZEX*SIZEY];// , dst[SIZEX*SIZEY];
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			img[i + j * width] = ((i / 10) % 2) * 200;
		}
	}
	cv::Mat ImgMat(height, width, CV_8U, img), ImgDst(height, width, CV_8U);
	for (int i = 0; i < 3; i++)
		cv::circle(ImgMat, points[i], 2, CV_RGB(0, 0, 255), 2);
	cv::imshow("src", ImgMat);
	cv::Point2f pos;
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			pos.x = i, pos.y = j;
			if (IsInTriangle(pos, dstpnt, triplet)){
				CalcAlphaBeta(pos, dstpnt, triplet, coef);
				npnt = coef.x * (points[triplet.x] - points[triplet.z]) + coef.y * (points[triplet.y] - points[triplet.z]) + points[triplet.z];//旧座標
				ImgDst.at<unsigned char>(pos) = ImgMat.at<unsigned char>(npnt);
			}
		}
	}
	cv::imshow("dst", ImgDst);


}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}


void CCVDlibTestDlg::OnBnClickedChkDrawtriangle()
{
	if (((CButton*)GetDlgItem(IDC_CHK_DRAWTRIANGLE))->GetCheck()){
		g_bDrawTriangles = TRUE;
	}
	else{
		g_bDrawTriangles = FALSE;
	}
}


void CCVDlibTestDlg::OnBnClickedChkResize()
{
	if (((CButton*)GetDlgItem(IDC_CHK_RESIZE))->GetCheck()){
		g_bResize = TRUE;
	}
	else{
		g_bResize = FALSE;
	}
}


void CCVDlibTestDlg::OnBnClickedChkTimemeasure()
{
	if (((CButton*)GetDlgItem(IDC_CHK_TIMEMEASURE))->GetCheck()){
		g_bTimeMeas = TRUE;
	}
	else{
		g_bTimeMeas = FALSE;
	}
}


void CCVDlibTestDlg::OnBnClickedChkMouseinner()
{

	if (((CButton*)GetDlgItem(IDC_CHK_MOUSEINNER))->GetCheck()){
		g_bMouseInner = TRUE;
	}
	else{
		g_bMouseInner = FALSE;
	}
}
