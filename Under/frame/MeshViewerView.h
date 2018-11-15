
// MeshViewerView.h: CMeshViewerView 类的接口
//

#pragma once

#include "Point3D.h"


class CMeshViewerView : public CView
{
protected: // 仅从序列化创建
	CMeshViewerView();
	DECLARE_DYNCREATE(CMeshViewerView)

// 特性
public:
	CMeshViewerDoc* GetDocument() const;

	BOOL InitializeOpenGL();
	BOOL SetupPixelFormat();

	void MakeCurrent(BOOL makeCurrent)
	{
		if (makeCurrent)
		{
			wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		}
		else
		{
			wglMakeCurrent(nullptr, nullptr);
		}

	}

	CPoint3D WindowToObject(CPoint point);


// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMeshViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CDC * m_pDC;
	HGLRC m_hRC;
	int m_nWinWidth, m_nWinHeight;

	CPoint m_ptLast;

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDisplaySmooth();
	afx_msg void OnDisplayWireframe();
	afx_msg void OnDisplayPointset();
	afx_msg void OnUpdateDisplaySmooth(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDisplayWireframe(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDisplayPointset(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // MeshViewerView.cpp 中的调试版本
inline CMeshViewerDoc* CMeshViewerView::GetDocument() const
   { return reinterpret_cast<CMeshViewerDoc*>(m_pDocument); }
#endif

