
/*
	RFC - KWindowTypes.cpp
	Copyright (C) 2013-2017 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/

#include "KWindowTypes.h"


KHotPluggedDialog::KHotPluggedDialog(){}


void KHotPluggedDialog::OnClose()
{
	::EndDialog(compHWND, 0);
}

void KHotPluggedDialog::OnDestroy(){}

KHotPluggedDialog::~KHotPluggedDialog(){}


KOverlappedWindow::KOverlappedWindow()
{
	this->SetText(STATIC_TXT("KOverlapped Window"));
	this->SetStyle(WS_OVERLAPPEDWINDOW);
}

KOverlappedWindow::~KOverlappedWindow(){}


KFrame::KFrame()
{
	this->SetText(STATIC_TXT("KFrame"));
	this->SetStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
}

KFrame::~KFrame(){}



KDialog::KDialog()
{
	this->SetText(STATIC_TXT("KDialog"));
	this->SetStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU);
}

KDialog::~KDialog(){}



KToolWindow::KToolWindow()
{
	this->SetText(STATIC_TXT("KTool Window"));
	this->SetStyle(WS_OVERLAPPED | WS_SYSMENU);
	this->SetExStyle(WS_EX_TOOLWINDOW);
}

KToolWindow::~KToolWindow(){}

