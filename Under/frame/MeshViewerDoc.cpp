
// MeshViewerDoc.cpp: CMeshViewerDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MeshViewer.h"
#endif

#include "MeshViewerDoc.h"

#include <propkey.h>

#include "Model.h"
#include "FileManager.h"
#include "OpenGLRender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMeshViewerDoc

IMPLEMENT_DYNCREATE(CMeshViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CMeshViewerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CMeshViewerDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CMeshViewerDoc::OnFileSave)
END_MESSAGE_MAP()


// CMeshViewerDoc 构造/析构

CMeshViewerDoc::CMeshViewerDoc()
{
	// TODO: 在此添加一次性构造代码

}

CMeshViewerDoc::~CMeshViewerDoc()
{
}

BOOL CMeshViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	CModel::GetInstance().clear();
	return TRUE;
}




// CMeshViewerDoc 序列化

void CMeshViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMeshViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMeshViewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMeshViewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMeshViewerDoc 诊断

#ifdef _DEBUG
void CMeshViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMeshViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMeshViewerDoc 命令


void CMeshViewerDoc::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE, _T(".pcd"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All files (*.*)|*.*|Mesh files (*.obj)|*.obj||"));
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();
		//convert to ascii
		CStringA pathA(path.GetBuffer());

		BeginWaitCursor();
		CEntity *pEntity = CFileManager::Read(pathA.GetBuffer());
		if (pEntity)
		{
			CModel::GetInstance().AddEntity(pEntity);
			COpenGLRender::GetInstance().ZoomToFit();
			UpdateAllViews(nullptr);
		}
		EndWaitCursor();
	}
}


void CMeshViewerDoc::OnFileSave()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(FALSE, _T(".pcd"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All files (*.*)|*.*|Mesh files (*.obj)|*.obj||"));
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();
		//convert to ascii
		CStringA pathA(path.GetBuffer());

		BeginWaitCursor();

		if (CFileManager::Write(pathA.GetBuffer()))
		{
			AfxMessageBox(_T("save successfully"));
		}
		else
		{
			AfxMessageBox(_T("Faild to save ") + path);
		}
		EndWaitCursor();

	}
}
