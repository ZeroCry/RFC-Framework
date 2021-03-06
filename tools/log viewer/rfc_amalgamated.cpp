
// ========== Generated With RFC Amalgamator v1.0 ==========


#include"rfc_amalgamated.h"


// =========== KApplication.cpp ===========

/*
	RFC - KApplication.cpp
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


KApplication::KApplication()
{
}

int KApplication::Main(KString **argv, int argc)
{
	return 0;
}

KApplication::~KApplication()
{
}

// =========== rfc.cpp ===========

/*
	RFC - rfc.cpp
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


class InternalVariables
{
public:
	static KComponent *currentComponent;
	static HHOOK wnd_hook;
	static CRITICAL_SECTION g_csComponent; // guard currentComponent!
	static volatile int rfcRefCount;
};

KComponent* InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
volatile int InternalVariables::rfcRefCount = 0;
CRITICAL_SECTION InternalVariables::g_csComponent;

const wchar_t* InternalDefinitions::RFCPropText_Object = L"RFC";
const wchar_t* InternalDefinitions::RFCPropText_OldProc = L"RFCOldProc";

LRESULT CALLBACK RFCCTL_CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode < 0)
		return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

	if(nCode==HCBT_CREATEWND){
		HWND hwnd=(HWND)wParam;
		::SetPropW(hwnd, InternalDefinitions::RFCPropText_Object, (HANDLE)InternalVariables::currentComponent);
		FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
		::SetPropW(hwnd, InternalDefinitions::RFCPropText_OldProc, (HANDLE)lpfnOldWndProc);

		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK GlobalWnd_Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	KComponent *component = (KComponent*)::GetPropW(hwnd, InternalDefinitions::RFCPropText_Object);

	if(!component){ // just for safe!
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );
	}

	if(!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if(msg==WM_NCDESTROY){
		::RemovePropW(hwnd, InternalDefinitions::RFCPropText_Object);

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, InternalDefinitions::RFCPropText_OldProc);
		if (lpfnOldWndProc)
		{
			::RemovePropW(hwnd, InternalDefinitions::RFCPropText_OldProc);
			::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
		}
	}

	return component->WindowProc(hwnd, msg, wParam, lParam);
}

HWND CreateRFCComponent(KComponent* component, bool subClassWindowProc)
{	
	if (subClassWindowProc)
	{
		::EnterCriticalSection(&InternalVariables::g_csComponent);

		InternalVariables::currentComponent = component;

		// install hook to get called before WM_CREATE_WINDOW msg!
		InternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), (const wchar_t*)component->GetComponentClassName(), (const wchar_t*)component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KPlatformUtil::GetInstance()->GetAppHInstance(), 0);

		::UnhookWindowsHookEx(InternalVariables::wnd_hook);

		::LeaveCriticalSection(&InternalVariables::g_csComponent);

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), (const wchar_t*)component->GetComponentClassName(), (const wchar_t*)component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KPlatformUtil::GetInstance()->GetAppHInstance(), 0);

		::SetPropW(hwnd, InternalDefinitions::RFCPropText_Object, (HANDLE)component);

		component->SetHWND(hwnd);

		return hwnd;
	}
}

void DoMessagePump(bool handleTabKey)
{
	MSG msg;
	
	while (::GetMessageW(&msg, NULL, 0, 0)) 
	{
		if(handleTabKey)
		{
			if(msg.message == WM_KEYDOWN)
			{
				if(VK_TAB == msg.wParam) // looking for TAB key!
				{
					if(msg.hwnd)
					{
						HWND parentHWND = ::GetParent(msg.hwnd);
						if(!parentHWND) // nothing selected! (top-level window)
						{
							HWND nextControl = ::GetNextDlgTabItem(msg.hwnd, NULL, FALSE);
							if(nextControl)
							{
								::SetFocus(nextControl);
								continue; // don't pass this message!
							}
						}else // user has already selected component!
						{
							HWND nextControl = ::GetNextDlgTabItem(parentHWND, msg.hwnd, FALSE);
							if(nextControl)
							{
								if((::GetKeyState(VK_CONTROL) & 0x8000) == 0) // user is not hold ctrl key!
								{
									::SetFocus(nextControl);
									continue; // don't pass this message!
								}
							}
						}
					}
				}
			}
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	} 
}

DWORD WINAPI GlobalThread_Proc(LPVOID lpParameter)
{
	if(lpParameter == 0) // for safe!
		return 0;

	KThread* thread = (KThread*)lpParameter;
	thread->SetHandle(::GetCurrentThread());
	thread->Run();

	return 0;	
}

bool CreateRFCThread(KThread* thread)
{
	if(thread)
	{
		HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, 0, NULL);
		if(handle)
			return true;
	}
	return false;
}

int HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KPlatformUtil::GetInstance()->GetAppHInstance(), MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KPlatformUtil::GetInstance()->GetAppHInstance(), MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

INT_PTR CALLBACK GlobalDlg_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if(comp)
		{
			comp->HotPlugInto(hwndDlg, true, true);
		}
		return FALSE;
	}
	return FALSE;
}

void InitRFC(HINSTANCE hInstance)
{
	if (!InternalVariables::rfcRefCount)
	{
		if (!hInstance)
			hInstance = ::GetModuleHandleW(NULL);

		KPlatformUtil::GetInstance()->SetAppHInstance(hInstance); // create instance for first time & initialize Utility class!
		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);

		::CoInitialize(NULL); //Initializes COM as STA.

		::InitializeCriticalSection(&InternalVariables::g_csComponent);
	}
	++InternalVariables::rfcRefCount;
}

void DeInitRFC()
{
	--InternalVariables::rfcRefCount;
	if (!InternalVariables::rfcRefCount)
	{
		::CoUninitialize();

		::DeleteCriticalSection(&InternalVariables::g_csComponent);

		// delete all singletons!
		delete KFont::GetDefaultFont();
		delete KPlatformUtil::GetInstance();
	}
}

// =========== KPerformanceCounter.cpp ===========

/*
	RFC - KPerformanceCounter.cpp
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


KPerformanceCounter::KPerformanceCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);

	this->pcFreq = double(li.QuadPart) / 1000.0;
}

void KPerformanceCounter::StartCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	this->counterStart = li.QuadPart;
}

double KPerformanceCounter::EndCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - this->counterStart) / this->pcFreq;
}

KPerformanceCounter::~KPerformanceCounter()
{

}


// =========== KPlatformUtil.cpp ===========

/*
	RFC - KPlatformUtil.cpp
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


KPlatformUtil* KPlatformUtil::_instance=0;

KPlatformUtil::KPlatformUtil()
{
	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	hInstance = 0;
	::InitializeCriticalSection(&g_csCount);
	menuItemList = new KPointerList<KMenuItem*>();
	timerList = new KPointerList<KTimer*>();
}

KPlatformUtil* KPlatformUtil::GetInstance()
{
	if(_instance)
		return _instance;
	_instance = new KPlatformUtil();
	return _instance;
}

void KPlatformUtil::SetAppHInstance(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
}

HINSTANCE KPlatformUtil::GetAppHInstance()
{
	return hInstance;
}

UINT KPlatformUtil::GenerateControlID()
{
	::EnterCriticalSection(&g_csCount);
	controlCount++;
	::LeaveCriticalSection(&g_csCount);

	return controlCount + 100;
}

UINT KPlatformUtil::GenerateMenuItemID(KMenuItem *menuItem)
{
	::EnterCriticalSection(&g_csCount);
	menuItemCount++;
	menuItemList->AddPointer(menuItem);
	::LeaveCriticalSection(&g_csCount);

	return menuItemCount + 30000;
}

KMenuItem* KPlatformUtil::GetMenuItemByID(UINT id)
{
	return menuItemList->GetPointer(id - 30001);
}

KString KPlatformUtil::GenerateClassName()
{
	::EnterCriticalSection(&g_csCount);

	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'R';
	className[1] = L'F';
	className[2] = L'C';
	className[3] = L'_';

	::_itow((int)hInstance, &className[4], 10);

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow(classCount, &className[lastPos + 1], 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	classCount++;
	::LeaveCriticalSection(&g_csCount);
	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

UINT KPlatformUtil::GenerateTimerID(KTimer *timer)
{
	::EnterCriticalSection(&g_csCount);
	timerCount++;
	timerList->AddPointer(timer);
	::LeaveCriticalSection(&g_csCount);

	return timerCount + 1000;
}

KTimer* KPlatformUtil::GetTimerByID(UINT id)
{
	return timerList->GetPointer(id - 1001);
}

KPlatformUtil::~KPlatformUtil()
{
	delete menuItemList;
	delete timerList;
	::DeleteCriticalSection(&g_csCount);
}

// =========== KRegistry.cpp ===========

/*
	RFC - KRegistry.cpp
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


KRegistry::KRegistry()
{

}

bool KRegistry::CreateKey(HKEY hKeyRoot, const KString& subKey)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	::RegCloseKey(hkey);
	return true;
}

bool KRegistry::DeleteKey(HKEY hKeyRoot, const KString& subKey)
{
	return ::RegDeleteKeyW(hKeyRoot, subKey) == ERROR_SUCCESS ? true : false;
}

bool KRegistry::ReadString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString *result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			if (requiredBytes == 0) // value might be empty
			{
				*result = KString();
			}
			else{
				void* buffer = ::malloc(requiredBytes + 1); // +1 for strings which doesn't have ending null
				::ZeroMemory(buffer, requiredBytes + 1); // ending null might not contain in register

				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)buffer, &requiredBytes);
				*result = KString((wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::WriteString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	int bCount = (value.GetLength() * sizeof(wchar_t)) + 1; // +1 for ending null
	LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_SZ, (LPBYTE)(const wchar_t*)value, bCount);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::ReadDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD *result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, &dwType, (LPBYTE)result, &dwSize);
		::RegCloseKey(hkey);

		if (ret == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool KRegistry::WriteDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	DWORD dwSize = sizeof(DWORD);
	LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_DWORD, (LPBYTE)&value, dwSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::ReadBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void **buffer, DWORD *buffSize)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			*buffSize = requiredBytes;
			if (requiredBytes == 0) // value might be empty
			{
				*buffer = 0;
			}
			else{
				*buffer = ::malloc(requiredBytes);
				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)*buffer, &requiredBytes);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::WriteBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void *buffer, DWORD buffSize)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_BINARY, (LPBYTE)buffer, buffSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

KRegistry::~KRegistry()
{

}

// =========== KThread.cpp ===========

/*
	RFC - KThread.cpp
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



KThread::KThread()
{
	handle = 0;
	isThreadRunning = false;
	threadShouldStop = false;
}

void KThread::SetHandle(HANDLE handle)
{
	this->handle = handle;
}

HANDLE KThread::GetHandle()
{
	return handle;
}

void KThread::Run()
{
	/*

	while(!threadShouldStop)
	{
		// your code goes here...
	}
	isThreadRunning=false;

	*/
}

bool KThread::IsThreadRunning()
{
	return isThreadRunning;
}

void KThread::ThreadShouldStop()
{
	threadShouldStop = true;
}

void KThread::WaitUntilThreadFinish()
{
	while(isThreadRunning) // wait until thread stop!
	{
		::Sleep(10);
	}
}

bool KThread::StartThread()
{
	threadShouldStop = false;
	isThreadRunning = true;
	return ::CreateRFCThread(this);
}

void KThread::uSleep(int waitTime)
{
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while (time2 - time1 < waitTime * freq / 1000000);
}

KThread::~KThread()
{
}

// =========== KTimer.cpp ===========

/*
	RFC - KTimer.cpp
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


KTimer::KTimer()
{
	resolution = 1000;
	started = false;
	listener = 0;
	timerID = KPlatformUtil::GetInstance()->GenerateTimerID(this);
}

void KTimer::SetInterval(int resolution)
{
	this->resolution = resolution;
}

int KTimer::GetInterval()
{
	return resolution;
}

void KTimer::SetTimerWindow(KWindow *window)
{
	this->window = window;
}

void KTimer::SetTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::GetTimerID()
{
	return timerID;
}

void KTimer::StartTimer()
{
	if(started)
		return;

	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			::SetTimer(hwnd, timerID, resolution, 0);
			started = true;
		}
	}
}

void KTimer::StopTimer()
{
	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			if(started)
				::KillTimer(hwnd, timerID);

			started = false;
		}
	}
}

bool KTimer::IsTimerRunning()
{
	return started;
}

void KTimer::OnTimer()
{
	if(listener)
		listener->OnTimer(this);
}

void KTimer::SetListener(KTimerListener *listener)
{
	this->listener = listener;
}

KTimer::~KTimer()
{
	if(started)
		this->StopTimer();
}

// =========== KTimerListener.cpp ===========

/*
	RFC - KTimerListener.cpp
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


KTimerListener::KTimerListener(){}

KTimerListener::~KTimerListener(){}

void KTimerListener::OnTimer(KTimer *timer){}



// =========== KString.cpp ===========

/*
	RFC - KString.cpp
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

#include <stdio.h>

const KString operator+ (const char* const string1, const KString& string2)
{
	KString s(string1);
	s = s + string2;
	return s;
}

const KString operator+ (const wchar_t* const string1, const KString& string2)
{
	KString s(string1);
	s = s + string2;
	return s;
}

const KString operator+ (const KString& string1, const KString& string2)
{
	KString s(string1);
	s = s + string2;
	return s;
}

KString::KString()
{
	stringHolder = 0;
}

KString::KString(const KString& other)
{
	if(other.stringHolder)
	{
		other.stringHolder->AddReference();
		stringHolder = other.stringHolder;
	}else
	{
		stringHolder = 0;
	}
}

KString::KString(const char* const text, UINT codePage)
{
	if (text != 0)
	{
		int count = ::MultiByteToWideChar(codePage, 0, text, -1, 0, 0); // get char count with null character
		if (count)
		{
			wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
			if (::MultiByteToWideChar(codePage, 0, text, -1, w_text, count))
			{
				count--; // ignore null character

				stringHolder = new KStringHolder();
				stringHolder->AddReference();

				stringHolder->w_text = w_text;
				stringHolder->count = count;
				return;
			}else
			{
				::free(w_text);
			}
		}
	}

	stringHolder=0;		
}

KString::KString(const wchar_t* const text, unsigned char behaviour)
{
	if (text != 0)
	{
		int count = (int)::wcslen(text);
		if(count)
		{
			stringHolder = new KStringHolder(behaviour == STATIC_TEXT_DO_NOT_FREE);
			stringHolder->AddReference();

			if( (behaviour == STATIC_TEXT_DO_NOT_FREE) || (behaviour == FREE_TEXT_WHEN_DONE) )
			{
				stringHolder->w_text = (wchar_t*)text;
			}
			else{ // USE_COPY_OF_TEXT or invalid option
				stringHolder->w_text = (wchar_t*)::malloc((count + 1) * sizeof(wchar_t));
				::wcscpy(stringHolder->w_text, text);
			}

			stringHolder->count = count;
			return;
		}
	}

	stringHolder = 0;	
}

KString::KString(const int value,const int radix)
{
	stringHolder = new KStringHolder();
	stringHolder->AddReference();

	stringHolder->w_text = (wchar_t *)::malloc(33 * sizeof(wchar_t)); // max 32 digits
	::_itow(value, stringHolder->w_text, radix);

	stringHolder->count = (int)::wcslen(stringHolder->w_text);
}

KString::KString(const float value, const int numDecimals, bool compact)
{
	// round it to given digits
	char *str_fmtp = (char*)malloc(32);
	char *str_buf = (char*)malloc(64);

	sprintf(str_fmtp, "%%.%df", numDecimals);
	sprintf(str_buf, str_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(str_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (str_buf[len - i] == '0')
				str_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(str_buf) - 1;
		if (str_buf[len] == '.')
			str_buf[len] = 0; // kill it
	}

	int count = ::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, 0, 0); // get char count with null character
	if (count)
	{
		wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
		if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, w_text, count))
		{
			count--; // ignore null character

			stringHolder = new KStringHolder();
			stringHolder->AddReference();

			stringHolder->w_text = w_text;
			stringHolder->count = count;

			::free(str_buf);
			::free(str_fmtp);
			return;
		}
		else
		{
			::free(w_text);
		}
	}

	::free(str_buf);
	::free(str_fmtp);
}

const KString& KString::operator= (const KString& other)
{
	if(stringHolder)
	{
		stringHolder->ReleaseReference();
	}

	if(other.stringHolder)
	{
		other.stringHolder->AddReference();
	}
	
	stringHolder = other.stringHolder;

	return *this;
}

const KString& KString::operator= (const wchar_t* const other)
{
	if(stringHolder)
	{
		stringHolder->ReleaseReference();
	}

	if (other != 0)
	{
		int count = (int)::wcslen(other);
		if(count)
		{
			stringHolder = new KStringHolder();
			stringHolder->AddReference();

			stringHolder->w_text = (wchar_t*)::malloc((count + 1) * sizeof(wchar_t));
			::wcscpy(stringHolder->w_text, other);
			stringHolder->count = count;
			return *this;
		}
	}

	stringHolder = 0;	
	return *this;
}

const KString KString::operator+ (const KString& stringToAppend)
{
	return Append(stringToAppend);
}

const KString KString::operator+ (const wchar_t* const textToAppend)
{
	return Append(KString(textToAppend));
}

KString::operator const char*()const
{
	if(stringHolder)
	{
		return stringHolder->GetAnsiVersion();
	}else
	{
		return "";
	}
}

KString::operator const wchar_t*()const
{
	if(stringHolder)
	{
		return stringHolder->w_text;
	}else
	{
		return L"";
	}
}

const char KString::operator[](const int index)const
{
	if(stringHolder)
	{
		if((0 <= index) && (index <= (stringHolder->count - 1)))
		{
			return stringHolder->GetAnsiVersion()[index];
		}
	}
	return -1;
}

KString KString::Append(const KString& otherString)const
{
	if( (otherString.stringHolder != 0) && (otherString.stringHolder->count != 0) )
	{
		KString result;
		result.stringHolder = new KStringHolder();
		result.stringHolder->AddReference();

		int length = otherString.stringHolder->count;
		int count = stringHolder ? stringHolder->count : 0;

		result.stringHolder->w_text = (wchar_t*)::malloc((length + count + 1) * sizeof(wchar_t));
		if(count) // this string is not empty!
		{
			::wcscpy(result.stringHolder->w_text, stringHolder->w_text);
			::wcsncat(result.stringHolder->w_text, otherString.stringHolder->w_text, length);
		}else
		{
			::wcscpy(result.stringHolder->w_text, otherString.stringHolder->w_text);
		}

		result.stringHolder->count = length + count;
		return result;
	}else
	{
		return *this;
	}
}

KString KString::SubString(int start, int end)const
{
	int count = stringHolder ? stringHolder->count : 0;

	if((0 <= start) && (start <= (count - 1)))
	{
		if((start < end) && (end <= (count - 1)))
		{
			int size = (end - start) + 1;
			wchar_t* buf = (wchar_t*)::malloc((size + 1) * sizeof(wchar_t));
			::wcsncpy(buf, &stringHolder->w_text[start], size);
			buf[size] = 0;

			KString result(buf);
			::free(buf);
			return result;
		}
	}
	return KString();
}

bool KString::EqualsIgnoreCase(const KString& otherString)const
{
	if( (otherString.stringHolder != 0) && (otherString.stringHolder->count != 0) )
	{
		if( (stringHolder != 0) && (stringHolder->count != 0) )
		{
			if(::wcscmp(stringHolder->w_text, otherString.stringHolder->w_text) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool KString::StartsWithChar(wchar_t character)const
{
	if( (stringHolder != 0) && (stringHolder->count != 0) )
	{
		if(stringHolder->w_text[0] == character)
		{
			return true;
		}
	}
	return false;
}

bool KString::StartsWithChar(char character)const
{
	if( (stringHolder != 0) && (stringHolder->count != 0) )
	{
		if(stringHolder->GetAnsiVersion()[0] == character)
		{
			return true;
		}
	}
	return false;
}

bool KString::EndsWithChar(wchar_t character)const
{
	if( (stringHolder != 0) && (stringHolder->count != 0) )
	{
		if(stringHolder->w_text[stringHolder->count - 1] == character)
		{
			return true;
		}
	}
	return false;
}

bool KString::EndsWithChar(char character)const
{
	if( (stringHolder != 0) && (stringHolder->count != 0) )
	{
		if(stringHolder->GetAnsiVersion()[stringHolder->count - 1] == character)
		{
			return true;
		}
	}
	return false;
}

bool KString::IsQuotedString()const
{
	if( (stringHolder != 0) && (stringHolder->count > 1) )
	{
		if(StartsWithChar(L'\"') && EndsWithChar(L'\"'))
		{
			return true;
		}
	}
	return false;
}

wchar_t KString::GetCharAt(int index)const
{
	int count = stringHolder ? stringHolder->count : 0;

	if((0 <= index) && (index <= (count - 1)))
		return stringHolder->w_text[index];
	return -1;
}

int KString::GetLength()const
{
	return stringHolder ? stringHolder->count : 0;
}

bool KString::IsEmpty()const
{
	if(stringHolder ? stringHolder->count : 0)
	{
		return false;
	}
	return true;
}

int KString::GetIntValue()const
{
	if(IsEmpty())
	{
		return 0;
	}
	return ::_wtoi(stringHolder->w_text);
}

KString::~KString()
{
	if(stringHolder)
	{
		stringHolder->ReleaseReference();
	}
}


// =========== KStringHolder.cpp ===========

/*
	RFC - KStringHolder.cpp
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



KStringHolder::KStringHolder(bool isStaticText)
{
	this->isStaticText = isStaticText;
	refCount = 0;
	a_text = 0;
	w_text = 0;
	count = 0;
	::InitializeCriticalSection(&cs_a_text);
}

KStringHolder::~KStringHolder()
{
	::DeleteCriticalSection(&cs_a_text);
}

void KStringHolder::AddReference()
{
	::InterlockedIncrement(&refCount);
}

void KStringHolder::ReleaseReference()
{
	LONG res = ::InterlockedDecrement(&refCount);
	if(res == 0)
	{
		if(a_text)
		{
			::free(a_text);
		}
		if(w_text)
		{
			if (!isStaticText)
				::free(w_text);
		}
		delete this;
	}
}

const char* KStringHolder::GetAnsiVersion(UINT codePage)
{
	::EnterCriticalSection(&cs_a_text);

	if(a_text)
	{
		::LeaveCriticalSection(&cs_a_text);
		return a_text;
	}else
	{
		int length = ::WideCharToMultiByte(codePage, 0, w_text, -1, 0, 0, 0, 0);
		if (length)
		{
			a_text = (char*)::malloc(length);
			if (::WideCharToMultiByte(codePage, 0, w_text, -1, a_text, length, 0, 0))
			{
				::LeaveCriticalSection(&cs_a_text);
				return a_text;
			}
			::free(a_text);
			a_text = 0;
		}

		::LeaveCriticalSection(&cs_a_text);
		return 0; // conversion error
	}
}

// =========== KMD5.cpp ===========

/*
	RFC - KMD5.cpp
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


KMD5::KMD5()
{

}

KString KMD5::GenerateFromString(const KString& text)
{
	if (text.GetLength() == 0)
		return KString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestString((char*)(const char*)text);

	return KString(strMD5);
}

KString KMD5::GenerateFromFile(const KString& fileName)
{
	if (fileName.GetLength() == 0) // empty path
		return KString();

	if (!KFile::IsFileExists(fileName)) // file does not exists
		return KString();

	KFile file;
	file.OpenFile(fileName, KFile::KREAD, false);

	DWORD fileSize = file.GetFileSize();
	file.CloseFile();

	if (fileSize==0) // empty file
		return KString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestFile((char*)(const char*)fileName);

	return KString(strMD5);
}

KMD5::~KMD5()
{

}

// =========== KSHA1.cpp ===========

/*
	RFC - KSHA1.cpp
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


KSHA1::KSHA1()
{

}

KString KSHA1::GenerateFromString(const KString& text)
{
	if (text.GetLength() == 0)
		return KString();

	ExtLibs::CSHA1 sha1;
	sha1.Update((const UINT_8*)(const char*)text, text.GetLength());
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return KString(szReport);
}

KString KSHA1::GenerateFromFile(const KString& fileName)
{
	if (fileName.GetLength() == 0) // empty path
		return KString();

	if (!KFile::IsFileExists(fileName)) // file does not exists
		return KString();

	KFile file;
	file.OpenFile(fileName, KFile::KREAD, false);

	DWORD fileSize = file.GetFileSize();
	file.CloseFile();

	if (fileSize == 0) // empty file
		return KString();

	ExtLibs::CSHA1 sha1;
	sha1.HashFile((const char*)fileName);
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return KString(szReport);
}

KSHA1::~KSHA1()
{

}

// =========== KDirectory.cpp ===========

/*
	RFC - KDirectory.cpp
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



KDirectory::KDirectory(){}

KDirectory::~KDirectory(){}

bool KDirectory::IsDirExists(const KString& dirName)
{
	DWORD dwAttrib = ::GetFileAttributesW((const wchar_t*)dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::CreateDir(const KString& dirName)
{
	return ::CreateDirectoryW((const wchar_t*)dirName, NULL) == 0 ? false : true;
}

bool KDirectory::RemoveDir(const KString& dirName)
{
	return ::RemoveDirectoryW((const wchar_t*)dirName) == 0 ? false : true;
}

KString KDirectory::GetModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	path[0] = 0;
	::GetTempPathW(MAX_PATH + 1, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetApplicationDataDir(bool isAllUsers)
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	path[0] = 0;
	::SHGetFolderPathW(NULL, isAllUsers ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA, NULL, 0, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

// =========== KFile.cpp ===========

/*
	RFC - KFile.cpp
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


KFile::KFile()
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW((const wchar_t*)fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::OpenFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW((const wchar_t*)fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool KFile::CloseFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::GetFileHandle()
{
	return fileHandle;
}

DWORD KFile::ReadFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead=0;

	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::WriteFile(void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten=0;

	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::SetFilePointerToStart()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER ? false : true;
}

bool KFile::SetFilePointerTo(DWORD distance)
{
	return ::SetFilePointer(fileHandle, distance, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER ? false : true;
}

bool KFile::SetFilePointerToEnd()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER ? false : true;
}

DWORD KFile::GetFileSize()
{
	DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return fileSize == INVALID_FILE_SIZE ? 0 : fileSize;
}

void* KFile::ReadAsData()
{
	DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool KFile::WriteString(const KString& text, bool isUnicode)
{
	void* buffer = isUnicode ? (void*)(const wchar_t*)text : (void*)(const char*)text;
	DWORD numberOfBytesToWrite = text.GetLength() * ( isUnicode ? sizeof(wchar_t) : sizeof(char) );

	DWORD numberOfBytesWritten = this->WriteFile(buffer, numberOfBytesToWrite);

	if (numberOfBytesWritten == numberOfBytesToWrite)
		return true;

	return false;
}

KString KFile::ReadAsString(bool isUnicode)
{
	DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize+1); // +1 is for null
		DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			if (isUnicode)
			{
				return KString((const wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}
			else
			{
				KString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return KString();
}

bool KFile::DeleteFile(const KString& fileName)
{
	return ::DeleteFileW((const wchar_t*)fileName) == 0 ? false : true;
}

bool KFile::CopyFile(const KString& sourceFileName, const KString& destFileName)
{
	return ::CopyFileW((const wchar_t*)sourceFileName, (const wchar_t*)destFileName, FALSE) == 0 ? false : true;
}

bool KFile::IsFileExists(const KString& fileName)
{
	DWORD dwAttrib = ::GetFileAttributesW((const wchar_t*)fileName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

KFile::~KFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}

// =========== KLogger.cpp ===========

/*
	RFC - KLogger.cpp
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


KLogger::KLogger(DWORD bufferSize)
{
	buffer = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;
	bufferIndex = 0;
	totalEvents = 0;
	bufferFull = false;
	isFirstCall = true;
}

bool KLogger::WriteNewEvent(unsigned char eventType)
{
	if (!bufferFull)
	{
		if ((bufferIndex + 300) >= bufferSize) // assume each event data is not greater than 300 bytes
		{
			bufferFull = true;
			return false;
		}

		unsigned short secs = 0;
		unsigned short mills = 0;

		if (isFirstCall)
		{
			pCounter.StartCounter();
			isFirstCall = false;
			totalMills = 0;
		}
		else{
			double deltaMills = pCounter.EndCounter();
			totalMills += (unsigned int)deltaMills;

			secs = (unsigned short)(totalMills/1000);
			mills = (unsigned short)(totalMills % 1000);

			pCounter.StartCounter();
		}

		buffer[bufferIndex] = eventType; // write event type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = secs; // write secs
		bufferIndex += sizeof(unsigned short);

		*((unsigned short*)&buffer[bufferIndex]) = mills; // write mills
		bufferIndex += sizeof(unsigned short);

		totalEvents++;

		return true;
	}
	return false;
}

bool KLogger::EndEvent()
{
	if (!bufferFull)
	{
		buffer[bufferIndex] = EVT_END; // write event end
		bufferIndex += sizeof(unsigned char);

		return true;
	}
	return false;
}

bool KLogger::AddTextParam(const char *text, unsigned char textLength)
{
	if( (textLength < 256) && (!bufferFull) )
	{
		buffer[bufferIndex] = PARAM_STRING; // write param type
		bufferIndex += sizeof(unsigned char);

		buffer[bufferIndex] = textLength; // write data size
		bufferIndex += sizeof(unsigned char);

		for (int i = 0; i < textLength; i++) // write data
		{
			buffer[bufferIndex] = text[i];
			bufferIndex += sizeof(unsigned char);
		}

		return true;
	}
	return false;
}

bool KLogger::AddIntParam(int value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_INT32; // write param type
		bufferIndex += sizeof(unsigned char);

		*((int*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(int);

		return true;
	}
	return false;
}

bool KLogger::AddShortParam(unsigned short value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_SHORT16; // write param type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(unsigned short);

		return true;
	}
	return false;
}

bool KLogger::AddFloatParam(float value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_FLOAT; // write param type
		bufferIndex += sizeof(unsigned char);

		*((float*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(float);

		return true;
	}
	return false;
}
	
bool KLogger::AddDoubleParam(double value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_DOUBLE; // write param type
		bufferIndex += sizeof(unsigned char);

		*((double*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(double);

		return true;
	}
	return false;
}

bool KLogger::IsBufferFull()
{
	return bufferFull;
}

bool KLogger::WriteToFile(const KString &filePath)
{
	KFile file;

	if (KFile::IsFileExists(filePath))
		KFile::DeleteFile(filePath);

	if (file.OpenFile(filePath,KFile::KWRITE))
	{
		file.WriteFile((void*)"RLOG", 4);
		file.WriteFile(&totalEvents, 4);
		file.WriteFile(buffer, bufferIndex);

		return true;
	}

	return false;
}

KLogger::~KLogger()
{
	free(buffer);
}

// =========== KSettingsReader.cpp ===========

/*
	RFC - KSettingsReader.cpp
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


KSettingsReader::KSettingsReader()
{

}

bool KSettingsReader::OpenFile(const KString& fileName, int formatID)
{
	if (!KFile::IsFileExists(fileName))
		return false;

	if (!settingsFile.OpenFile(fileName, KFile::KREAD))
		return false;

	settingsFile.SetFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.ReadFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void KSettingsReader::ReadData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.ReadFile(buffer, size);
}

KString KSettingsReader::ReadString()
{
	int size = 0;
	settingsFile.ReadFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.ReadFile(buffer, size);

		return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::ReadInt()
{
	int value = 0;
	settingsFile.ReadFile(&value, sizeof(int));

	return value;
}

float KSettingsReader::ReadFloat()
{
	float value = 0;
	settingsFile.ReadFile(&value, sizeof(float));

	return value;
}

double KSettingsReader::ReadDouble()
{
	double value = 0;
	settingsFile.ReadFile(&value, sizeof(double));

	return value;
}

bool KSettingsReader::ReadBool()
{
	bool value = 0;
	settingsFile.ReadFile(&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader()
{

}

// =========== KSettingsWriter.cpp ===========

/*
	RFC - KSettingsWriter.cpp
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


KSettingsWriter::KSettingsWriter()
{

}

bool KSettingsWriter::OpenFile(const KString& fileName, int formatID)
{
	if (KFile::IsFileExists(fileName))
		KFile::DeleteFile(fileName);

	if (!settingsFile.OpenFile(fileName, KFile::KWRITE))
		return false;

	settingsFile.SetFilePointerToStart();
	settingsFile.WriteFile(&formatID, sizeof(int));

	return true;
}

void KSettingsWriter::WriteData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.WriteFile(buffer, size);
}

void KSettingsWriter::WriteString(const KString& text)
{
	int size = text.GetLength();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.WriteFile(&size, sizeof(int));

		settingsFile.WriteFile((wchar_t*)(const wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.WriteFile(&size, sizeof(int));
	}
}

void KSettingsWriter::WriteInt(int value)
{
	settingsFile.WriteFile(&value, sizeof(int));
}

void KSettingsWriter::WriteFloat(float value)
{
	settingsFile.WriteFile(&value, sizeof(float));
}

void KSettingsWriter::WriteDouble(double value)
{
	settingsFile.WriteFile(&value, sizeof(double));
}

void KSettingsWriter::WriteBool(bool value)
{
	settingsFile.WriteFile(&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter()
{

}

// =========== KButton.cpp ===========

/*
	RFC - KButton.cpp
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


KButton::KButton()
{
	listener = 0;

	compClassName = STATIC_TXT("BUTTON");

	this->SetText(STATIC_TXT("Button"));
	this->SetSize(100, 30);
	this->SetPosition(0, 0);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP);
	this->SetExStyle(WS_EX_WINDOWEDGE);
}

void KButton::SetListener(KButtonListener *listener)
{
	this->listener = listener;
}

KButtonListener* KButton::GetListener()
{
	return listener;
}

void KButton::OnPress()
{
	if(listener)
		listener->OnButtonPress(this);
}

bool KButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
	{
		this->OnPress();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KButton::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KButton::~KButton()
{
}

// =========== KButtonListener.cpp ===========

/*
	RFC - KButtonListener.cpp
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


KButtonListener::KButtonListener(){}

KButtonListener::~KButtonListener(){}

void KButtonListener::OnButtonPress(KButton *button){}

// =========== KCheckBox.cpp ===========

/*
	RFC - KCheckBox.cpp
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


KCheckBox::KCheckBox()
{
	checked = false;

	this->SetText(STATIC_TXT("CheckBox"));
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_NOTIFY | WS_TABSTOP);
}

bool KCheckBox::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KCheckBox::OnPress()
{
	if(::SendMessageW(compHWND, BM_GETCHECK, 0, 0) == BST_CHECKED)
		checked = true;
	else
		checked = false;

	if(listener)
		listener->OnButtonPress(this);
}

bool KCheckBox::IsChecked()
{
	return checked;
}

void KCheckBox::SetCheckedState(bool state)
{
	checked = state;

	if(compHWND)
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
}

KCheckBox::~KCheckBox()
{
}

// =========== KComboBox.cpp ===========

/*
	RFC - KComboBox.cpp
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



KComboBox::KComboBox(bool sort)
{
	listener = 0;
	selectedItemIndex = -1;

	compClassName = STATIC_TXT("COMBOBOX");

	this->SetSize(100, 100);
	this->SetPosition(0, 0);

	this->SetStyle(WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP);

	if(sort)
		this->SetStyle(compDwStyle | CBS_SORT);

	this->SetExStyle(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);

	stringList = new KPointerList<KString*>;
}

void KComboBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KComboBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::RemoveItem(const KString& text)
{
	int itemIndex = this->GetItemIndex(text);
	if(itemIndex>-1)
		this->RemoveItem(itemIndex);
}

int KComboBox::GetItemIndex(const KString& text)
{
	int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if(stringList->GetPointer(i)->EqualsIgnoreCase(text))
				return i;
		}
	}
	return -1;
}

int KComboBox::GetItemCount()
{
	return stringList->GetSize();
}

int KComboBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
		return -1;
	}else
	{
		return -1;
	}	
}

KString KComboBox::GetSelectedItem()
{
	int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

void KComboBox::ClearList()
{
	stringList->DeleteAll(true);
	if(compHWND)
	{
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
	}
}

void KComboBox::SelectItem(int index)
{
	selectedItemIndex = index;
	if(compHWND)
	{
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
	}
}

bool KComboBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		this->OnItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KComboBox::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register COMBOBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(selectedItemIndex > -1)
			::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KComboBox::SetListener(KComboBoxListener *listener)
{
	this->listener = listener;
}

void KComboBox::OnItemSelect()
{
	if(listener)
		listener->OnComboBoxItemSelect(this);
}

KComboBox::~KComboBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}



// =========== KComboBoxListener.cpp ===========

/*
	RFC - KComboBoxListener.h
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


KComboBoxListener::KComboBoxListener(){}

KComboBoxListener::~KComboBoxListener(){}

void KComboBoxListener::OnComboBoxItemSelect(KComboBox *comboBox){}

// =========== KCommonDialogBox.cpp ===========

/*
	RFC - KCommonDialogBox.cpp
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


bool KCommonDialogBox::ShowOpenFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName)
{
	// assumes MAX_PATH * 2 is enough!	
	wchar_t *buff = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	buff[0] = 0;

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags =  OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrTitle = (const wchar_t*)title;

	if(::GetOpenFileNameW(&ofn))
	{
		*fileName = KString(buff, KString::FREE_TEXT_WHEN_DONE);
		return true;
	}else
	{
		::free(buff);
		return false;
	}
}

bool KCommonDialogBox::ShowSaveFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName)
{
	// assumes MAX_PATH * 2 is enough!
	wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	buff[0] = 0;

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags =  OFN_HIDEREADONLY;
	ofn.lpstrTitle = (const wchar_t*)title;

	if(::GetSaveFileNameW(&ofn))
	{
		*fileName = KString(buff, KString::FREE_TEXT_WHEN_DONE);
		return true;
	}else
	{
		::free(buff);
		return false;
	}
}

// =========== KComponent.cpp ===========

/*
	RFC - KComponent.cpp
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




KComponent::KComponent()
{
	isRegistered = false;

	KPlatformUtil *platformUtil = KPlatformUtil::GetInstance();
	compClassName = platformUtil->GenerateClassName();
	compCtlID = platformUtil->GenerateControlID();

	compHWND = 0;
	compParentHWND = 0;
	compDwStyle = 0;
	compDwExStyle = 0;
	cursor = 0;
	compX = CW_USEDEFAULT;
	compY = CW_USEDEFAULT;
	compWidth = CW_USEDEFAULT;
	compHeight = CW_USEDEFAULT;
	compVisible = true;
	compEnabled = true;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = 0;
	wc.lpszMenuName = 0;
	wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIconSm = 0;
	wc.style = 0;
	wc.hInstance = platformUtil->GetAppHInstance();
	wc.lpszClassName = (const wchar_t*)compClassName;

	wc.lpfnWndProc = ::GlobalWnd_Proc;

	compFont = KFont::GetDefaultFont();
}

void KComponent::OnHotPlug()
{

}

void KComponent::HotPlugInto(HWND component, bool fetchInfo, bool subClassWindowProc)
{
	compHWND = component;

	if (fetchInfo)
	{
		wchar_t *clsName = (wchar_t*)::malloc(256 * sizeof(wchar_t));
		clsName[0] = 0;
		::GetClassNameW(compHWND, clsName, 256);
		compClassName = KString(clsName, KString::FREE_TEXT_WHEN_DONE);

		::GetClassInfoExW(KPlatformUtil::GetInstance()->GetAppHInstance(), compClassName, &wc);

		compCtlID = (UINT)::GetWindowLongPtrW(compHWND, GWL_ID);

		RECT rect;
		::GetWindowRect(compHWND, &rect);
		compWidth = rect.right - rect.left;
		compHeight = rect.bottom - rect.top;
		compX = rect.left;
		compY = rect.top;

		compVisible = ::IsWindowVisible(compHWND) ? true : false;
		compEnabled = ::IsWindowEnabled(compHWND) ? true : false;

		compDwStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_STYLE);
		compDwExStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_EXSTYLE);

		compParentHWND = ::GetParent(compHWND);

		wchar_t *buff = (wchar_t*)::malloc(256 * sizeof(wchar_t)); // assume 256 is enough
		buff[0] = 0;
		::GetWindowTextW(compHWND, buff, 256);
		compText = KString(buff, KString::FREE_TEXT_WHEN_DONE);
	}

	::SetPropW(compHWND, InternalDefinitions::RFCPropText_Object, (HANDLE)(KComponent*)this);

	if (subClassWindowProc)
	{
		FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(compHWND, GWLP_WNDPROC);
		::SetPropW(compHWND, InternalDefinitions::RFCPropText_OldProc, (HANDLE)lpfnOldWndProc);

		::SetWindowLongPtrW(compHWND, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...
	}	

	this->OnHotPlug();
}

UINT KComponent::GetControlID()
{
	return compCtlID;
}

void KComponent::SetMouseCursor(KCursor *cursor)
{
	this->cursor = cursor;
	if(compHWND)
		::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());
}

KString KComponent::GetComponentClassName()
{
	return compClassName;
}

bool KComponent::CreateComponent(bool subClassWindowProc)
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered=true;

	::CreateRFCComponent(this, subClassWindowProc);

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		if(cursor)
			::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());

		return true;
	}
	return false;
}

LRESULT KComponent::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, InternalDefinitions::RFCPropText_OldProc);
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)::GlobalWnd_Proc) // it's subclassed standard-control or hot-plugged dialog! RFCOldProc of subclassed control|dialog is not GlobalWnd_Proc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam); // custom control or window
}

bool KComponent::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	return false;
}

void KComponent::SetFont(KFont *compFont)
{
	this->compFont = compFont;
	if(compHWND)
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0));
}

KFont* KComponent::GetFont()
{
	return compFont;
}

KString KComponent::GetText()
{
	return compText;
}

void KComponent::SetText(const KString& compText)
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, (const wchar_t*)this->compText);
}

void KComponent::SetHWND(HWND compHWND)
{
	this->compHWND = compHWND;
}

HWND KComponent::GetHWND()
{
	return compHWND;
}

void KComponent::SetParentHWND(HWND compParentHWND)
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::GetParentHWND()
{
	return compParentHWND;
}

DWORD KComponent::GetStyle()
{
	return compDwStyle;
}

void KComponent::SetStyle(DWORD compStyle)
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::GetExStyle()
{
	return compDwExStyle;
}

void KComponent::SetExStyle(DWORD compDwExStyle)
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

int KComponent::GetX()
{
	return compX; 
}

int KComponent::GetY()
{
	return compY;
}

int KComponent::GetWidth()
{
	return compWidth;
}

int KComponent::GetHeight()
{
	return compHeight;
}

void KComponent::SetSize(int compWidth, int compHeight)
{
	this->compWidth = compWidth;
	this->compHeight = compHeight;

	if(compHWND)
		::SetWindowPos(compHWND, 0, 0, 0, compWidth, compHeight, SWP_NOMOVE | SWP_NOREPOSITION);
}

void KComponent::SetPosition(int compX, int compY)
{
	this->compX = compX;
	this->compY = compY;

	if(compHWND)
		::SetWindowPos(compHWND, 0, compX, compY, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION);
}

void KComponent::SetVisible(bool state)
{
	this->compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::IsVisible()
{
	return compHWND ? this->compVisible : false;
}

bool KComponent::IsEnabled()
{
	return compEnabled;
}

void KComponent::SetEnabled(bool state)
{
	compEnabled=state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::BringToFront()
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::SetKeyboardFocus()
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::Repaint()
{
	if(compHWND)
		::InvalidateRect(compHWND, NULL, TRUE);
}

KComponent::~KComponent()
{
	if(isRegistered)
		::UnregisterClassW((const wchar_t*)compClassName, KPlatformUtil::GetInstance()->GetAppHInstance());
}

// =========== KGlyphButton.cpp ===========

/*
	RFC - KGlyphButton.cpp
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


KGlyphButton::KGlyphButton()
{
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
}

KGlyphButton::~KGlyphButton()
{
}

void KGlyphButton::SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->Repaint();
}

bool KGlyphButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (glyphFont)
	{
		if (msg == WM_NOTIFY)
		{
			if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
			{
				LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)lParam;

				*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

				if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
				{
					*result = CDRF_NOTIFYPOSTPAINT;
				}
				else if (CDDS_POSTPAINT == lpNMCD->dwDrawStage) //  postpaint stage
				{
					RECT rc = lpNMCD->rc;
					bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED | CDIS_GRAYED)) != 0;

					HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, glyphFont->GetFontHandle());
					COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);
					int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

					RECT rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw glyph

					::SetBkMode(lpNMCD->hdc, oldBkMode);
					::SetTextColor(lpNMCD->hdc, oldTextColor);
					::SelectObject(lpNMCD->hdc, oldFont);

					*result = CDRF_DODEFAULT;
				}

				return true; // indicate that we processed this msg & result is valid.
			}
		}
	}

	return KButton::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KGridView.cpp ===========

/*
	RFC - KGridView.cpp
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


KGridView::KGridView(bool sortItems)
{
	itemCount = 0;
	colCount = 0;
	listener = 0;
	compClassName = WC_LISTVIEWW;

	this->SetPosition(0, 0);
	this->SetSize(300, 200);
	this->SetStyle(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL);
	this->SetExStyle(WS_EX_WINDOWEDGE);

	if (sortItems)
		compDwStyle |= LVS_SORTASCENDING;
}

KGridView::~KGridView(){}

void KGridView::SetListener(KGridViewListener *listener)
{
	this->listener = listener;
}

KGridViewListener* KGridView::GetListener()
{
	return listener;
}

void KGridView::InsertRecord(KString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = itemCount;

	int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvi = { 0 };
		lvi.iSubItem = i;
		lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvi);
	}

	itemCount++;
}

void KGridView::InsertRecordTo(int rowIndex, KString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
	lvi.iItem = rowIndex;

	int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvi= { 0 };
		lvi.iSubItem = i;
		lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvi);
	}

	itemCount++;
}

KString KGridView::GetRecordAt(int rowIndex, int columnIndex)
{
	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(compHWND, LVM_GETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
}

int KGridView::GetSelectedRow()
{
	return ListView_GetNextItem(compHWND, -1, LVNI_SELECTED);
}

void KGridView::RemoveRecordAt(int rowIndex)
{
	if (ListView_DeleteItem(compHWND, rowIndex))
		itemCount--;
}

void KGridView::RemoveAll()
{
	ListView_DeleteAllItems(compHWND);
	itemCount = 0;
}

void KGridView::UpdateRecordAt(int rowIndex, int columnIndex, const KString& text)
{
	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.pszText = (wchar_t*)(const wchar_t*)text;

	::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void KGridView::SetColumnWidth(int columnIndex, int columnWidth)
{
	ListView_SetColumnWidth(compHWND, columnIndex, columnWidth);
}

int KGridView::GetColumnWidth(int columnIndex)
{
	return ListView_GetColumnWidth(compHWND, columnIndex);
}

void KGridView::CreateColumn(const KString& text, int columnWidth)
{
	LVCOLUMN lvc = { 0 };

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = columnWidth;
	lvc.pszText = (wchar_t*)(const wchar_t*)text;
	lvc.iSubItem = colCount;

	::SendMessageW(compHWND, LVM_INSERTCOLUMNW, (WPARAM)colCount, (LPARAM)&lvc);

	colCount++;
}

bool KGridView::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				this->OnItemSelect();
				*result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)lParam)->code == NM_RCLICK) // List view item right click
		{
			this->OnItemRightClick();
			*result = 0;
			return true;
		}
		else if (((LPNMHDR)lParam)->code == NM_DBLCLK) // List view item double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KGridView::CreateComponent(bool subClassWindowProc)
{
	if (!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register WC_LISTVIEWW class!

	if (compHWND)
	{
		ListView_SetExtendedListViewStyle(compHWND, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KGridView::OnItemSelect()
{
	if (listener)
		listener->OnGridViewItemSelect(this);
}

void KGridView::OnItemRightClick()
{
	if (listener)
		listener->OnGridViewItemRightClick(this);
}

void KGridView::OnItemDoubleClick()
{
	if (listener)
		listener->OnGridViewItemDoubleClick(this);
}


// =========== KGridViewListener.cpp ===========

/*
	RFC - KGridViewListener.cpp
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


KGridViewListener::KGridViewListener(){}

KGridViewListener::~KGridViewListener(){}

void KGridViewListener::OnGridViewItemSelect(KGridView *gridView){}

void KGridViewListener::OnGridViewItemRightClick(KGridView *gridView){}

void KGridViewListener::OnGridViewItemDoubleClick(KGridView *gridView){}

// =========== KGroupBox.cpp ===========

/*
	RFC - KGroupBox.cpp
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


KGroupBox::KGroupBox()
{
	this->SetText(STATIC_TXT("GroupBox"));
	this->SetSize(100, 100);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_GROUPBOX);
}

KGroupBox::~KGroupBox()
{
}

// =========== KLabel.cpp ===========

/*
	RFC - KLabel.cpp
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


KLabel::KLabel()
{
	compClassName = STATIC_TXT("STATIC");

	this->SetText(STATIC_TXT("Label"));
	this->SetSize(100, 25);
	this->SetPosition(0, 0);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY);
	this->SetExStyle(WS_EX_WINDOWEDGE);
}

bool KLabel::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register Label class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KLabel::~KLabel()
{
}

// =========== KListBox.cpp ===========

/*
	RFC - KListBox.cpp
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


KListBox::KListBox(bool multipleSelection,bool sort,bool vscroll)
{
	this->multipleSelection=multipleSelection;
	listener = 0;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	compClassName = STATIC_TXT("LISTBOX");

	this->SetSize(100, 100);
	this->SetPosition(0, 0);

	this->SetStyle(LBS_NOTIFY | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP);

	if(multipleSelection)
		this->SetStyle(compDwStyle | LBS_MULTIPLESEL);
	if(sort)
		this->SetStyle(compDwStyle | LBS_SORT);
	if(vscroll)
		this->SetStyle(compDwStyle | WS_VSCROLL);

	this->SetExStyle(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);

	stringList = new KPointerList<KString*>;
}

void KListBox::SetListener(KListBoxListener *listener)
{
	this->listener = listener;
}

void KListBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KListBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
}

void KListBox::RemoveItem(const KString& text)
{
	int itemIndex = this->GetItemIndex(text);
	if(itemIndex>-1)
		this->RemoveItem(itemIndex);
}

int KListBox::GetItemIndex(const KString& text)
{
	int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if(stringList->GetPointer(i)->EqualsIgnoreCase(text))
				return i;
		}
	}
	return -1;
}

int KListBox::GetItemCount()
{
	return stringList->GetSize();
}

int KListBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		int index = (int)::SendMessageW(compHWND, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
		return -1;
	}else
	{
		return -1;
	}	
}

KString KListBox::GetSelectedItem()
{
	int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

int KListBox::GetSelectedItems(int* itemArray, int itemCountInArray)
{
	if(compHWND)
	{	 
		int items = (int)::SendMessageW(compHWND, LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);
		if(items != LB_ERR)
			return items;
		return -1;
	}else
	{
		return -1;
	}
}

void KListBox::ClearList()
{
	stringList->DeleteAll(true);

	if(compHWND)
		::SendMessageW(compHWND, LB_RESETCONTENT, 0, 0);
}

void KListBox::SelectItem(int index)
{
	selectedItemIndex = index;

	if(compHWND)
		::SendMessageW(compHWND, LB_SETCURSEL, index, 0);
}

void KListBox::SelectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(compHWND)
			::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool KListBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == LBN_SELCHANGE)) // listbox sel change!
	{
		this->OnItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KListBox::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register LISTBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		int listSize=stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(!multipleSelection) // single selction!
		{
			if(selectedItemIndex > -1)
				::SendMessageW(compHWND, LB_SETCURSEL, selectedItemIndex, 0);
		}else
		{
			if(selectedItemIndex>-1)
				::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(selectedItemIndex, selectedItemEnd));
		}

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KListBox::OnItemSelect()
{
	if(listener)
		listener->OnListBoxItemSelect(this);
}

KListBox::~KListBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}

// =========== KListBoxListener.cpp ===========

/*
	RFC - KListBoxListener.cpp
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


KListBoxListener::KListBoxListener(){}
KListBoxListener::~KListBoxListener(){}

void KListBoxListener::OnListBoxItemSelect(KListBox *listBox){}

// =========== KMenu.cpp ===========

/*
	RFC - KMenu.cpp
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



KMenu::KMenu()
{
	hMenu=::CreatePopupMenu();
}

void KMenu::AddMenuItem(KMenuItem *menuItem)
{
	menuItem->AddToMenu(hMenu);
}

void KMenu::AddSubMenu(const KString& text, KMenu *menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu->GetMenuHandle(), (const wchar_t*)text);
}

void KMenu::AddSeperator()
{
	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_SEPARATOR;

	::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
}

HMENU KMenu::GetMenuHandle()
{
	return hMenu;
}

void KMenu::PopUpMenu(KWindow *window)
{
	if(window)
	{
		POINT p;
		::GetCursorPos(&p);
		::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window->GetHWND(), NULL);
	}
}

KMenu::~KMenu()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuBar.cpp ===========

/*
	RFC - KMenuBar.cpp
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



KMenuBar::KMenuBar()
{
	hMenu = ::CreateMenu();
}

void KMenuBar::AddMenu(const KString& text, KMenu *menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu->GetMenuHandle(), (const wchar_t*)text);
}

void KMenuBar::AddToWindow(KWindow *window)
{
	HWND hwnd = window->GetHWND();
	if(hwnd)
		::SetMenu(hwnd, hMenu);
}

KMenuBar::~KMenuBar()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuButton.cpp ===========

/*
RFC - KMenuButton.cpp
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


KMenuButton::KMenuButton()
{
	buttonMenu = 0;
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
	arrowFont = new KFont(STATIC_TXT("Webdings"), 18);
}	

KMenuButton::~KMenuButton()
{
	delete arrowFont;
}

void KMenuButton::SetMenu(KMenu *buttonMenu)
{
	this->buttonMenu = buttonMenu;
}

void KMenuButton::SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->Repaint();
}

void KMenuButton::OnPress()
{
	if (buttonMenu)
	{
		POINT point = {compX, compY};
		::ClientToScreen(compParentHWND, &point); // get screen cordinates

		::TrackPopupMenu(buttonMenu->GetMenuHandle(), TPM_LEFTBUTTON, point.x, point.y + compHeight, 0, compParentHWND, NULL);
	}
}

bool KMenuButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW) lParam;

			*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				*result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				RECT rc = lpNMCD->rc;
				KGraphics::Draw3dVLine(lpNMCD->hdc, rc.right - 22, rc.top + 6, rc.bottom - 12); // draw line

				bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFont->GetFontHandle());
				COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - 18, rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphFont) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFont->GetFontHandle());
					::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				*result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}

	return KButton::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KMenuItem.cpp ===========

/*
	RFC - KMenuItem.cpp
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



KMenuItem::KMenuItem()
{
	hMenu = 0;
	listener = 0;
	enabled = true;
	checked = false;
	itemID = KPlatformUtil::GetInstance()->GenerateMenuItemID(this);
}

void KMenuItem::AddToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
	mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);

}

bool KMenuItem::IsChecked()
{
	return checked;
}

void KMenuItem::SetCheckedState(bool state)
{
	checked = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool KMenuItem::IsEnabled()
{
	return enabled; 
}

void KMenuItem::SetEnabled(bool state)
{
	enabled = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void KMenuItem::SetText(const KString& text)
{
	itemText = text;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

KString KMenuItem::GetText()
{
	return itemText;
}

UINT KMenuItem::GetItemID()
{
	return itemID;
}

HMENU KMenuItem::GetMenuHandle()
{
	return hMenu;
}

void KMenuItem::SetListener(KMenuItemListener *listener)
{
	this->listener = listener;
}

KMenuItemListener* KMenuItem::GetListener()
{
	return listener;
}

void KMenuItem::OnPress()
{
	if(listener)
		listener->OnMenuItemPress(this);
}

KMenuItem::~KMenuItem()
{
}


// =========== KMenuItemListener.cpp ===========

/*
	RFC - KMenuItemListener.cpp
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


KMenuItemListener::KMenuItemListener(){}

KMenuItemListener::~KMenuItemListener(){}

void KMenuItemListener::OnMenuItemPress(KMenuItem *menuItem){}

// =========== KNumericField.cpp ===========

/*
	RFC - KNumericField.cpp
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


KNumericField::KNumericField():KTextBox(false)
{
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL | ES_NUMBER);
}

KNumericField::~KNumericField(){}

// =========== KPasswordBox.cpp ===========

/*
	RFC - KPasswordBox.cpp
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


KPasswordBox::KPasswordBox(bool readOnly):KTextBox(readOnly)
{
	pwdChar = '*';
	this->SetStyle(compDwStyle | ES_PASSWORD);
}

void KPasswordBox::SetPasswordChar(const char pwdChar)
{
	this->pwdChar = pwdChar;
	if(compHWND)
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		this->Repaint();
	}
}

char KPasswordBox::GetPasswordChar()
{
	return pwdChar;
}

bool KPasswordBox::CreateComponent()
{
	if(KTextBox::CreateComponent())
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		return true;
	}
	return false;
}

KPasswordBox::~KPasswordBox()
{
}

// =========== KProgressBar.cpp ===========

/*
	RFC - KProgressBar.cpp
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


KProgressBar::KProgressBar(bool smooth, bool vertical)
{
	value = 0;

	compClassName = KString(PROGRESS_CLASSW, KString::STATIC_TEXT_DO_NOT_FREE);

	this->SetPosition(0, 0);
	this->SetSize(100, 20);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS);
	this->SetExStyle(WS_EX_WINDOWEDGE);

	if(smooth)
		this->SetStyle(compDwStyle | PBS_SMOOTH);

	if(vertical)
		this->SetStyle(compDwStyle | PBS_VERTICAL);
}

int KProgressBar::GetValue()
{
	return value;
}

void KProgressBar::SetValue(int value)
{
	this->value=value;

	if(compHWND)
		::SendMessageW(compHWND, PBM_SETPOS, value, 0);
}

bool KProgressBar::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register PROGRESS_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(compHWND, PBM_SETPOS, value, 0); // set current value!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;	
}

KProgressBar::~KProgressBar()
{
}

// =========== KPushButton.cpp ===========

/*
	RFC - KPushButton.cpp
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


KPushButton::KPushButton()
{
	this->SetText(STATIC_TXT("Push Button"));
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_NOTIFY | WS_TABSTOP);
}

KPushButton::~KPushButton()
{
}

// =========== KRadioButton.cpp ===========

/*
	RFC - KRadioButton.cpp
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


KRadioButton::KRadioButton()
{
	this->SetText(STATIC_TXT("RadioButton"));
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | BS_RADIOBUTTON | BS_NOTIFY | WS_TABSTOP);
}

KRadioButton::~KRadioButton()
{
}

// =========== KTextArea.cpp ===========

/*
	RFC - KTextArea.cpp
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


KTextArea::KTextArea(bool autoScroll, bool readOnly):KTextBox(readOnly)
{
	this->SetSize(200, 100);
	this->SetStyle(compDwStyle | ES_MULTILINE);

	if(autoScroll)
		this->SetStyle(compDwStyle | ES_AUTOHSCROLL | ES_AUTOVSCROLL);
	else
		this->SetStyle(compDwStyle | WS_HSCROLL | WS_VSCROLL);
}

KTextArea::~KTextArea()
{
}

// =========== KTextBox.cpp ===========

/*
	RFC - KTextBox.cpp
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


KTextBox::KTextBox(bool readOnly)
{
	compClassName = STATIC_TXT("EDIT");

	this->SetSize(100, 20);
	this->SetPosition(0, 0);
	this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL);
	if(readOnly)
		this->SetStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL);

	this->SetExStyle(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
}

KString KTextBox::GetText()
{
	if(compHWND)
	{
		int length = ::GetWindowTextLengthW(compHWND);
		if(length)
		{
			int size = (length + 1) * sizeof(wchar_t);
			wchar_t *text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(compHWND, text, size);
			compText = KString(text, KString::FREE_TEXT_WHEN_DONE);
		}else
		{
			compText = KString();
		}
	}
	return compText;
}


bool KTextBox::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register EDIT class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KTextBox::~KTextBox()
{
}

// =========== KTrackBar.cpp ===========

/*
	RFC - KTrackBar.cpp
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



KTrackBar::KTrackBar(bool showTicks, bool vertical)
{
	listener = 0;
	rangeMin = 0;
	rangeMax = 100;
	value = 0;

	this->SetSize(100, 25);
	this->SetPosition(0, 0);
	this->SetStyle(WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS);
	this->SetExStyle(WS_EX_WINDOWEDGE);

	this->SetStyle(compDwStyle | (showTicks ? TBS_AUTOTICKS : TBS_NOTICKS));
	this->SetStyle(compDwStyle | (vertical ? TBS_VERT : TBS_HORZ));

	compClassName = KString(TRACKBAR_CLASSW, KString::STATIC_TEXT_DO_NOT_FREE);
}

void KTrackBar::SetRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));	
}

void KTrackBar::SetValue(int value)
{
	this->value = value;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);
}

void KTrackBar::OnChange()
{
	value = (int)::SendMessageW(compHWND, TBM_GETPOS, 0, 0);
	if(listener)
		listener->OnTrackBarChange(this);
}

void KTrackBar::SetListener(KTrackBarListener *listener)
{
	this->listener = listener;
}

int KTrackBar::GetValue()
{
	return value;
}

bool KTrackBar::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if( (msg == WM_HSCROLL) || (msg == WM_VSCROLL) )
	{
		int nScrollCode = (int)LOWORD(wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || (TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || (TB_PAGEDOWN == nScrollCode) ) // its trackbar!
		{
			this->OnChange();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KTrackBar::CreateComponent(bool subClassWindowProc)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, subClassWindowProc); // we dont need to register TRACKBAR_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(rangeMin, rangeMax));	
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

KTrackBar::~KTrackBar()
{
}


// =========== KTrackBarListener.cpp ===========

/*
	RFC - KTrackBarListener.cpp
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


KTrackBarListener::KTrackBarListener(){}

KTrackBarListener::~KTrackBarListener(){}

void KTrackBarListener::OnTrackBarChange(KTrackBar *trackBar){}

// =========== KWindow.cpp ===========

/*
	RFC - KWindow.cpp
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


KWindow::KWindow()
{
	this->SetText(STATIC_TXT("KWindow"));
	this->SetSize(400, 200);
	this->SetVisible(false);
	this->SetStyle(WS_POPUP);
	this->SetExStyle(WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT);
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
}

void KWindow::Flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::SetIcon(KIcon *icon)
{
	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)icon->GetHandle());
}

void KWindow::Destroy()
{
	::DestroyWindow(compHWND);
}


void KWindow::OnClose()
{
	this->Destroy();
}

void KWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

void KWindow::CenterScreen()
{
	this->SetPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

bool KWindow::AddComponent(KComponent *component, bool subClassWindowProc)
{
	if(component)
	{
		if(compHWND)
		{
			component->SetParentHWND(compHWND);
			return component->CreateComponent(subClassWindowProc);
		}
	}
	return false;
}

bool KWindow::SetClientAreaSize(int width, int height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		wndRect.right = wndRect.left + width;
		wndRect.bottom = wndRect.top + height;

		::AdjustWindowRect(&wndRect, compDwStyle, ::GetMenu(compHWND) == NULL ? FALSE : TRUE);
		this->SetSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

		return true;
	}
	return false;
}

bool KWindow::GetClientAreaSize(int *width, int *height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		if (width)
			*width = wndRect.right - wndRect.left;

		if (height)
			*height = wndRect.bottom - wndRect.top;

		return true;
	}
	return false;
}

void KWindow::OnMoved()
{

}

void KWindow::OnResized()
{

}

LRESULT KWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent *component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, InternalDefinitions::RFCPropText_Object);
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				KComponent *component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, InternalDefinitions::RFCPropText_Object);
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
			{
				KComponent *component = (KComponent*)::GetPropW((HWND)lParam, InternalDefinitions::RFCPropText_Object);
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}				
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent *component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), InternalDefinitions::RFCPropText_Object);
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}			
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				KComponent *component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, InternalDefinitions::RFCPropText_Object);
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
			{
				KTimer *timer = KPlatformUtil::GetInstance()->GetTimerByID((UINT)wParam);
				if(timer)
				{
					timer->OnTimer();
					break;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compWidth = rect.right - rect.left;
				this->compHeight = rect.bottom - rect.top;

				this->OnResized();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->OnMoved();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == BN_CLICKED) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					KMenuItem *menuItem = KPlatformUtil::GetInstance()->GetMenuItemByID(LOWORD(wParam));
					if(menuItem)
					{
						menuItem->OnPress();
						break;
					}
				}
				else // send to appropriate component
				{
					KComponent *component = (KComponent*)::GetPropW((HWND)lParam, InternalDefinitions::RFCPropText_Object);
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_CLOSE:
			this->OnClose();
			break;

		case WM_DESTROY:
			this->OnDestroy();
			break;

		default:
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow()
{

}

// =========== KWindowTypes.cpp ===========

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


// =========== KBitmap.cpp ===========

/*
	RFC - KBitmap.cpp
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


KBitmap::KBitmap()
{
	hBitmap = 0;
	appHInstance = KPlatformUtil::GetInstance()->GetAppHInstance();
}

bool KBitmap::LoadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(appHInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool KBitmap::LoadFromFile(const KString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(appHInstance, (const wchar_t*)(KString)filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

void KBitmap::DrawOnHDC(HDC hdc, int x, int y, int width, int height)
{
	HDC memHDC = CreateCompatibleDC(hdc);

	SelectObject(memHDC, hBitmap);
	BitBlt(hdc, x, y, width, height, memHDC, 0, 0, SRCCOPY);

	DeleteDC(memHDC);
}

HBITMAP KBitmap::GetHandle()
{
	return hBitmap;
}

KBitmap::~KBitmap()
{
	if(hBitmap)
		::DeleteObject(hBitmap);
}

// =========== KCursor.cpp ===========

/*
	RFC - KCursor.cpp
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


KCursor::KCursor()
{
	hCursor = 0;
	appHInstance = KPlatformUtil::GetInstance()->GetAppHInstance();
}

bool KCursor::LoadFromResource(WORD resourceID)
{
	hCursor = (HCURSOR)::LoadImageW(appHInstance, MAKEINTRESOURCEW(resourceID), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

bool KCursor::LoadFromFile(const KString& filePath)
{
	hCursor = (HCURSOR)::LoadImageW(appHInstance, (const wchar_t*)filePath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

HCURSOR KCursor::GetHandle()
{
	return hCursor;
}

KCursor::~KCursor()
{
	if(hCursor)
		::DestroyCursor(hCursor);
}

// =========== KFont.cpp ===========

/*
	RFC - KFont.cpp
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


KFont* KFont::defaultInstance=0;

KFont::KFont()
{
	hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	customFont = false;
}

KFont::KFont(const KString& face, int size, bool bold, bool italic, bool antiAliased)
{
	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, 0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, (const wchar_t*)face);
	if(hFont)
		customFont = true;
}

KFont* KFont::GetDefaultFont()
{
	if(KFont::defaultInstance)
		return KFont::defaultInstance;
	KFont::defaultInstance = new KFont();
	return KFont::defaultInstance;
}

bool KFont::LoadFont(const KString& path)
{
	return AddFontResourceExW(path, FR_PRIVATE, 0) == 0 ? false : true;
}

void KFont::RemoveFont(const KString& path)
{
	RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT KFont::GetFontHandle()
{
	return hFont;
}

KFont::~KFont()
{
	if(customFont)
		::DeleteObject(hFont);
}

// =========== KGraphics.cpp ===========

/*
	RFC - KGraphics.cpp
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


KGraphics::KGraphics(){}

KGraphics::~KGraphics(){}

void KGraphics::Draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	KGraphics::Draw3dRect(hdc, startX, startY, 2, height, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	KGraphics::Draw3dRect(hdc, startX, startY, width, 2, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void KGraphics::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	KGraphics::FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void KGraphics::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	KGraphics::FillSolidRect(hdc, &rect, color);
}

void KGraphics::FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

// =========== KIcon.cpp ===========

/*
	RFC - KIcon.cpp
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


KIcon::KIcon()
{
	hIcon = 0;
	appHInstance = KPlatformUtil::GetInstance()->GetAppHInstance();
}

bool KIcon::LoadFromResource(WORD resourceID)
{
	hIcon = (HICON)::LoadImageW(appHInstance, MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

bool KIcon::LoadFromFile(const KString& filePath)
{
	hIcon = (HICON)::LoadImageW(appHInstance, (const wchar_t*)filePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

HICON KIcon::GetHandle()
{
	return hIcon;
}

KIcon::~KIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}

// =========== sha1.cpp ===========
/*
	100% free public domain implementation of the SHA-1 algorithm
	by Dominik Reichl <dominik.reichl@t-online.de>
	Web: http://www.dominik-reichl.de/

	Version 1.7 - 2006-12-21
	- Fixed buffer underrun warning which appeared when compiling with
	  Borland C Builder (thanks to Rex Bloom and Tim Gallagher for the
	  patch)
	- Breaking change: ReportHash writes the final hash to the start
	  of the buffer, i.e. it's not appending it to the string any more
	- Made some function parameters const
	- Added Visual Studio 2005 project files to demo project

	Version 1.6 - 2005-02-07 (thanks to Howard Kapustein for patches)
	- You can set the endianness in your files, no need to modify the
	  header file of the CSHA1 class any more
	- Aligned data support
	- Made support/compilation of the utility functions (ReportHash
	  and HashFile) optional (useful when bytes count, for example in
	  embedded environments)

	Version 1.5 - 2005-01-01
	- 64-bit compiler compatibility added
	- Made variable wiping optional (define SHA1_WIPE_VARIABLES)
	- Removed unnecessary variable initializations
	- ROL32 improvement for the Microsoft compiler (using _rotl)

	======== Test Vectors (from FIPS PUB 180-1) ========

	SHA1("abc") =
		A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D

	SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") =
		84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1

	SHA1(A million repetitions of "a") =
		34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/


using namespace ExtLibs;

#ifdef SHA1_UTILITY_FUNCTIONS
#define SHA1_MAX_FILE_BUFFER 8000
#endif

// Rotate x bits to the left
#ifndef ROL32
#ifdef _MSC_VER
#define ROL32(_val32, _nBits) _rotl(_val32, _nBits)
#else
#define ROL32(_val32, _nBits) (((_val32)<<(_nBits))|((_val32)>>(32-(_nBits))))
#endif
#endif

#ifdef SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (m_block->l[i] = \
	(ROL32(m_block->l[i],24) & 0xFF00FF00) | (ROL32(m_block->l[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (m_block->l[i])
#endif

#define SHABLK(i) (m_block->l[i&15] = ROL32(m_block->l[(i+13)&15] ^ m_block->l[(i+8)&15] \
	^ m_block->l[(i+2)&15] ^ m_block->l[i&15],1))

// SHA-1 rounds
#define _R0(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R1(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R2(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5); w=ROL32(w,30); }
#define _R3(v,w,x,y,z,i) { z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5); w=ROL32(w,30); }
#define _R4(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5); w=ROL32(w,30); }

CSHA1::CSHA1()
{
	m_block = (SHA1_WORKSPACE_BLOCK*)m_workspace;

	Reset();
}

CSHA1::~CSHA1()
{
	Reset();
}

void CSHA1::Reset()
{
	// SHA1 initialization constants
	m_state[0] = 0x67452301;
	m_state[1] = 0xEFCDAB89;
	m_state[2] = 0x98BADCFE;
	m_state[3] = 0x10325476;
	m_state[4] = 0xC3D2E1F0;

	m_count[0] = 0;
	m_count[1] = 0;
}

void CSHA1::Transform(UINT_32* pState, const UINT_8* pBuffer)
{
	UINT_32 a = pState[0], b = pState[1], c = pState[2], d = pState[3], e = pState[4];

	memcpy(m_block, pBuffer, 64);

	// 4 rounds of 20 operations each. Loop unrolled.
	_R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
	_R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
	_R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
	_R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
	_R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
	_R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
	_R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
	_R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
	_R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
	_R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
	_R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
	_R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
	_R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
	_R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
	_R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
	_R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
	_R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
	_R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
	_R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
	_R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

	// Add the working vars back into state
	pState[0] += a;
	pState[1] += b;
	pState[2] += c;
	pState[3] += d;
	pState[4] += e;

	// Wipe variables
#ifdef SHA1_WIPE_VARIABLES
	a = b = c = d = e = 0;
#endif
}

// Use this function to hash in binary data and strings
void CSHA1::Update(const UINT_8* pData, UINT_32 uLen)
{
	UINT_32 i, j;

	j = (m_count[0] >> 3) & 63;

	if((m_count[0] += (uLen << 3)) < (uLen << 3))
		m_count[1]++;

	m_count[1] += (uLen >> 29);

	if((j + uLen) > 63)
	{
		i = 64 - j;
		memcpy(&m_buffer[j], pData, i);
		Transform(m_state, m_buffer);

		for( ; (i + 63) < uLen; i += 64)
			Transform(m_state, &pData[i]);

		j = 0;
	}
	else i = 0;

	if((uLen - i) != 0)
		memcpy(&m_buffer[j], &pData[i], uLen - i);
}

#ifdef SHA1_UTILITY_FUNCTIONS
// Hash in file contents
bool CSHA1::HashFile(const char* szFileName)
{
	unsigned long ulFileSize, ulRest, ulBlocks;
	unsigned long i;
	UINT_8 uData[SHA1_MAX_FILE_BUFFER];
	FILE* fIn;

	if(szFileName == NULL) return false;

	fIn = fopen(szFileName, "rb");
	if(fIn == NULL) return false;

	fseek(fIn, 0, SEEK_END);
	ulFileSize = (unsigned long)ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	if(ulFileSize != 0)
	{
		ulBlocks = ulFileSize / SHA1_MAX_FILE_BUFFER;
		ulRest = ulFileSize % SHA1_MAX_FILE_BUFFER;
	}
	else
	{
		ulBlocks = 0;
		ulRest = 0;
	}

	for(i = 0; i < ulBlocks; i++)
	{
		fread(uData, 1, SHA1_MAX_FILE_BUFFER, fIn);
		Update((UINT_8*)uData, SHA1_MAX_FILE_BUFFER);
	}

	if(ulRest != 0)
	{
		fread(uData, 1, ulRest, fIn);
		Update((UINT_8*)uData, ulRest);
	}

	fclose(fIn); fIn = NULL;
	return true;
}
#endif

void CSHA1::Final()
{
	UINT_32 i;
	UINT_8 finalcount[8];

	for(i = 0; i < 8; i++)
		finalcount[i] = (UINT_8)((m_count[((i >= 4) ? 0 : 1)]
			>> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

	Update((UINT_8*)"\200", 1);

	while ((m_count[0] & 504) != 448)
		Update((UINT_8*)"\0", 1);

	Update(finalcount, 8); // Cause a SHA1Transform()

	for(i = 0; i < 20; i++)
		m_digest[i] = (UINT_8)((m_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);

	// Wipe variables for security reasons
#ifdef SHA1_WIPE_VARIABLES
	memset(m_buffer, 0, 64);
	memset(m_state, 0, 20);
	memset(m_count, 0, 8);
	memset(finalcount, 0, 8);
	Transform(m_state, m_buffer);
#endif
}

#ifdef SHA1_UTILITY_FUNCTIONS
// Get the final hash as a pre-formatted string
void CSHA1::ReportHash(char* szReport, unsigned char uReportType) const
{
	unsigned char i;
	char szTemp[16];

	if(szReport == NULL) return;

	if(uReportType == REPORT_HEX)
	{
		sprintf(szTemp, "%02X", m_digest[0]);
		strcpy(szReport, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf(szTemp, "%02X", m_digest[i]);
			strcat(szReport, szTemp);
		}
	}
	else if(uReportType == REPORT_DIGIT)
	{
		sprintf(szTemp, "%u", m_digest[0]);
		strcpy(szReport, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf(szTemp, " %u", m_digest[i]);
			strcat(szReport, szTemp);
		}
	}
	else strcpy(szReport, "Error: Unknown report type!");
}
#endif

// Get the raw message digest
void CSHA1::GetHash(UINT_8* puDest) const
{
	memcpy(puDest, m_digest, 20);
}