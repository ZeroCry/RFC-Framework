
/*
	RFC - KComboBox.h
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

#ifndef _RFC_KCOMBOBOX_H_
#define _RFC_KCOMBOBOX_H_

#include "KComponent.h"
#include "../containers/KPointerList.h"


class KComboBoxListener;

class RFC_API KComboBox : public KComponent
{
protected:
	KPointerList<KString*> *stringList;
	int selectedItemIndex;
	KComboBoxListener *listener;

public:
	KComboBox(bool sort=false);

	virtual void AddItem(const KString& text);

	virtual void RemoveItem(int index);

	virtual void RemoveItem(const KString& text);

	virtual int GetItemIndex(const KString& text);

	virtual int GetItemCount();

	virtual int GetSelectedItemIndex();

	virtual KString GetSelectedItem();

	virtual void ClearList();

	virtual void SelectItem(int index);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void SetListener(KComboBoxListener *listener);

	virtual void OnItemSelect();

	virtual ~KComboBox();
};

#endif