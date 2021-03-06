#include "StdAfx.h"
#include "SdkLayout.h"

namespace SdkLayout{

bool CSdkLayout::SetLayout(HWND hWnd, LPCTSTR xml, HINSTANCE hInst)
{
	DeleteLayout();
	
	assert(::IsWindow(hWnd));
	m_hWnd = hWnd;
	CDialogBuilder builder;
	CContainerUI* pRoot = static_cast<CContainerUI*>(
		builder.Create(xml, &m_Manager, hInst ? hInst : GetModuleHandle(NULL)));
	assert(pRoot);
	if(pRoot){
		m_pRoot = pRoot;
		m_pRoot->SetHWND(hWnd, &m_Manager);
		DWORD dwStyle = GetWindowLongPtr(GetHWND(), GWL_STYLE);
		dwStyle |= WS_VSCROLL | WS_HSCROLL;
		SetWindowLongPtr(GetHWND(), GWL_STYLE, dwStyle);
		ShowScrollBar(m_hWnd, SB_VERT, TRUE);
		ShowScrollBar(m_hWnd, SB_HORZ, TRUE);
		_InitializeLayout();
	}
	return !!m_pRoot;
}

bool CSdkLayout::SetLayout( HWND hWnd, UINT id, HINSTANCE hInst/*=NULL*/ )
{
	return SetLayout(hWnd, MAKEINTRESOURCE(id), hInst ? hInst : GetModuleHandle(NULL));
}

void CSdkLayout::DeleteLayout()
{
	delete m_pRoot;
	m_pRoot    = NULL;
	m_hWnd     = NULL;
}

void CSdkLayout::ResizeLayout(const RECT& rc)
{
	if(!m_pRoot || !m_hWnd) return;
	m_pRoot->SetPos(rc);
	_ProcessScrollBar(rc);
	::InvalidateRect(m_hWnd, &rc, FALSE);
	m_rcLast = rc;
}

void CSdkLayout::ResizeLayout()
{
	if(!m_pRoot || !m_hWnd || !IsWindow(m_hWnd)) return;
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	ResizeLayout(rc);
}

CControlUI* CSdkLayout::FindControl(LPCTSTR name)
{
	return m_pRoot ? m_pRoot->FindControl(name) : NULL;
}

void CSdkLayout::_InitializeLayout()
{
	if(!m_pRoot) return;
	
	m_pRoot->DoInit();

	ResizeLayout();
}

void CSdkLayout::_ProcessScrollBar(const CDuiRect& rc)
{
	const SIZE& szPost = GetPostSize();

	bool bSetVert=false,bSetHorz=false;

	if(szPost.cy > rc.GetHeight())
		bSetVert = true;

	if(szPost.cx > rc.GetWidth())
		bSetHorz = true;

	SCROLLINFO si = {0};
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nPos = 0;
	si.nPage = 100;
	if(bSetVert){
		si.nMax = szPost.cy-rc.GetHeight()-1+si.nPage-1;
		SetScrollInfo(GetHWND(), SB_VERT, &si, TRUE);
		ShowScrollBar(GetHWND(), SB_VERT, TRUE);
	}
	else{
		ShowScrollBar(GetHWND(), SB_VERT, FALSE);
	}
	if(bSetHorz){
		si.nMax = szPost.cx-rc.GetWidth()-1+si.nPage-1;
		SetScrollInfo(GetHWND(), SB_HORZ, &si, TRUE);
		ShowScrollBar(GetHWND(), SB_HORZ, TRUE);
	}
	else{
		ShowScrollBar(GetHWND(), SB_HORZ, FALSE);
	}
}

void CSdkLayout::ProcessScrollMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(uMsg == WM_VSCROLL){
		SCROLLINFO si = {0};
		int iVertPos = 0;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(GetHWND(),SB_VERT,&si);
		iVertPos = si.nPos;

		switch(LOWORD(wParam))
		{
		case SB_ENDSCROLL:						break;
		case SB_TOP:		si.nPos = 0;		break;
		case SB_BOTTOM:		si.nPos = si.nMax;	break;
		case SB_LINEUP:		si.nPos --;			break;
		case SB_LINEDOWN:	si.nPos ++;			break;
		case SB_PAGEUP:		si.nPos -= si.nPage; break;
		case SB_PAGEDOWN:	si.nPos += si.nPage; break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(GetHWND(),SB_VERT,&si,TRUE);
		GetScrollInfo(GetHWND(), SB_VERT, &si);

		if(si.nPos != iVertPos){
			ScrollWindow(GetHWND(), 0, (iVertPos-si.nPos), NULL, NULL);
			UpdateWindow(GetHWND());
		}
	}
	else if(uMsg == WM_HSCROLL){
		SCROLLINFO si = {0};
		int iHorzPos = 0;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(GetHWND(),SB_HORZ,&si);
		iHorzPos = si.nPos;

		switch(LOWORD(wParam))
		{
		case SB_ENDSCROLL:						break;
		case SB_LEFT:		si.nPos = 0;		break;
		case SB_RIGHT:		si.nPos = si.nMax;	break;
		case SB_LINELEFT:	si.nPos --;			break;
		case SB_LINERIGHT:	si.nPos ++;			break;
		case SB_PAGELEFT:	si.nPos -= si.nPage; break;
		case SB_PAGERIGHT:	si.nPos += si.nPage; break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(GetHWND(),SB_HORZ,&si,TRUE);
		GetScrollInfo(GetHWND(), SB_HORZ, &si);

		if(si.nPos != iHorzPos){
			ScrollWindow(GetHWND(), (iHorzPos-si.nPos), 0, NULL, NULL);
			UpdateWindow(GetHWND());
		}
	}
}

}
