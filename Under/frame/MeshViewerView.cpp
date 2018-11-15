
// MeshViewerView.cpp: CMeshViewerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MeshViewer.h"
#endif

#include "MeshViewerDoc.h"
#include "MeshViewerView.h"

#include "OpenGLRender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMeshViewerView

IMPLEMENT_DYNCREATE(CMeshViewerView, CView)

BEGIN_MESSAGE_MAP(CMeshViewerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMeshViewerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_COMMAND(ID_DISPLAY_WIREFRAME, &CMeshViewerView::OnDisplayWireframe)
	ON_COMMAND(ID_DISPLAY_POINTSET, &CMeshViewerView::OnDisplayPointset)
	ON_COMMAND(ID_DISPLAY_SMOOTH, &CMeshViewerView::OnDisplaySmooth)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SMOOTH, &CMeshViewerView::OnUpdateDisplaySmooth)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_POINTSET, &CMeshViewerView::OnUpdateDisplayPointset)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME, &CMeshViewerView::OnUpdateDisplayWireframe)
END_MESSAGE_MAP()

// CMeshViewerView 构造/析构

CMeshViewerView::CMeshViewerView()
{
	// TODO: 在此处添加构造代码

}

CMeshViewerView::~CMeshViewerView()
{
}

BOOL CMeshViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	return CView::PreCreateWindow(cs);
}

// CMeshViewerView 绘图

void CMeshViewerView::OnDraw(CDC* pDC)
{
	CMeshViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	MakeCurrent(true);

	// Clear out the color & depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	COpenGLRender::GetInstance().RenderScene();

	// Tell OpenGL to flush its pipeline
	glFinish();

	// Now Swap the buffers
	SwapBuffers(pDC->GetSafeHdc());

	MakeCurrent(false);
}


// CMeshViewerView 打印


void CMeshViewerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMeshViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMeshViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMeshViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CMeshViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMeshViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMeshViewerView 诊断

#ifdef _DEBUG
void CMeshViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CMeshViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMeshViewerDoc* CMeshViewerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMeshViewerDoc)));
	return (CMeshViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMeshViewerView 消息处理程序


int CMeshViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	InitializeOpenGL();
	return 0;
}

BOOL CMeshViewerView::InitializeOpenGL()
{
	m_pDC = new CClientDC(this);

	if (m_pDC == NULL)
	{
		AfxMessageBox(_T("Error Obtaining DC"));
		return FALSE;
	}
	//Failure to set the pixel format
	if (!SetupPixelFormat())
	{
		AfxMessageBox(_T("Error setup pixel format"));
		return FALSE;
	}
	//Create Rendering Context
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	//Failure to Create Rendering Context
	if (m_hRC == 0)
	{
		AfxMessageBox(_T("Error Creating RC"));
		return FALSE;
	}
	//Make the RC Current
	if (::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC) == FALSE)
	{
		AfxMessageBox(_T("Error making RC Current"));
		return FALSE;
	}

	//Specify Black as the clear color
	glClearColor(0.6f, 0.6f, 0.6f, 0.0f);

	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Specify the back of the buffer as clear depth
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	//lighting
	// 	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//smooth shading
	glShadeModel(GL_SMOOTH);

	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);

	return TRUE;
}

BOOL CMeshViewerView::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |            // support OpenGL
		PFD_DOUBLEBUFFER,                // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		16,                             // 16-bit z-buffer
		8,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int nPixelFormat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (nPixelFormat == 0)
	{
		return FALSE;
	}
	if (::SetPixelFormat(m_pDC->GetSafeHdc(), nPixelFormat, &pfd) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

void CMeshViewerView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	MakeCurrent(false);

	//Delete the rendering context
	if (::wglDeleteContext(m_hRC) == FALSE)
	{
		MessageBox(_T("Could not delete RC"));
	}

	//Delete the DC
	if (m_pDC)
	{
		delete m_pDC;
	}
	//Set it to NULL
	m_pDC = NULL;
}


BOOL CMeshViewerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}


void CMeshViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (0 >= cx || 0 >= cy)
	{
		return;
	}

	COpenGLRender::GetInstance().SetWindowSize(cx, cy);

	MakeCurrent(true);

	// select the full client area
	m_nWinWidth = cx;
	m_nWinHeight = cy;
	//m_render.SetWindowSize(cx, cy);

	glViewport(0, 0, cx, cy);

	// compute the aspect ratio
	// this will keep all dimension scales equal
	GLdouble aspect_ratio = (GLdouble)m_nWinWidth / (GLdouble)m_nWinHeight;

	// select the projection matrix and clear it
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// select the viewing volume
	double width = 1, height = 1;
	if (m_nWinWidth > m_nWinHeight)
		width *= aspect_ratio;
	else
		height /= aspect_ratio;

	glOrtho(-width, width, -height, height, 1.0, 6.0);

	//glFrustum(-width, width, -height, height, 1, 3);	

	// switch back to the modelview matrix and clear it
	glMatrixMode(GL_MODELVIEW);

	MakeCurrent(false);
}


void CMeshViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	COpenGLRender::GetInstance().GetArcball().Click(point.x, point.y);
	CView::OnLButtonDown(nFlags, point);
}


void CMeshViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	COpenGLRender &render = COpenGLRender::GetInstance();
	if (nFlags & MK_LBUTTON)
	{
		render.GetArcball().Drag(point.x, point.y);
		Invalidate(FALSE);

	}
	else if (nFlags & MK_MBUTTON)
	{
		CVector3D offset = WindowToObject(point) - WindowToObject(m_ptLast);
		render.Translate(offset);

		Invalidate(FALSE);
	}

	m_ptLast = point;

	CView::OnMouseMove(nFlags, point);
}


BOOL CMeshViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	COpenGLRender::GetInstance().Scale(1 + zDelta * 0.001);

	Invalidate(FALSE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMeshViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_ptLast = point;
	CView::OnMButtonDown(nFlags, point);
}

CPoint3D CMeshViewerView::WindowToObject(CPoint point)
{
	MakeCurrent(true);

	double modelView[16];
	double projMtrx[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projMtrx);
	int viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);

	double ptObj[3];
	gluUnProject(point.x, m_nWinHeight - point.y, 0, modelView, projMtrx, viewPort, &ptObj[0], &ptObj[1], &ptObj[2]);

	MakeCurrent(false);

	return CPoint3D(ptObj);
}

void CMeshViewerView::OnDisplaySmooth()
{
	// TODO: 在此添加命令处理程序代码
	COpenGLRender::GetInstance().SetDisplayMode(SmoothMode);
	Invalidate(FALSE);
}


void CMeshViewerView::OnDisplayWireframe()
{
	// TODO: 在此添加命令处理程序代码
	COpenGLRender::GetInstance().SetDisplayMode(WireframeMode);
	Invalidate(FALSE);
}


void CMeshViewerView::OnDisplayPointset()
{
	// TODO: 在此添加命令处理程序代码
	COpenGLRender::GetInstance().SetDisplayMode(PointSetMode);
	Invalidate(FALSE);
}


void CMeshViewerView::OnUpdateDisplaySmooth(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(COpenGLRender::GetInstance().GetDisplayMode() == SmoothMode);
}


void CMeshViewerView::OnUpdateDisplayWireframe(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(COpenGLRender::GetInstance().GetDisplayMode() == WireframeMode);
}


void CMeshViewerView::OnUpdateDisplayPointset(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(COpenGLRender::GetInstance().GetDisplayMode() == PointSetMode);
}
