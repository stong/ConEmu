﻿
/*
Copyright (c) 2011-present Maximus5
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define HIDE_USE_EXCEPTION_INFO
#define SHOWDEBUGSTR

#include "Header.h"
#include "../common/EnvVar.h"
#include "../common/MGuiMacro.h"
#include "../common/MStrSafe.h"
#include "../common/ProcessSetEnv.h"
#include "../common/WFiles.h"

#include <set>

#include "AboutDlg.h"
#include "Attach.h"
#include "ConEmu.h"
#include "Inside.h"
#include "Macro.h"
#include "MacroImpl.h"
#include "Menu.h"
#include "Options.h"
#include "OptionsClass.h"
#include "RealConsole.h"
#include "SetCmdTask.h"
#include "SetColorPalette.h"
#include "SetDlgButtons.h"
#include "SystemEnvironment.h"
#include "TabBar.h"
#include "TrayIcon.h"
#include "Update.h"
#include "VConGroup.h"
#include "VirtualConsole.h"


/* ********************************* */
/* ****** Forward definitions ****** */
/* ********************************* */
namespace ConEmuMacro
{
	typedef CEStr (*MacroFunction)(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);

	struct SyncExecMacroParm
	{
		MacroFunction pfn;
		GuiMacro* p;
		CRealConsole* pRCon;
		bool bFromPlugin;
	};

	static bool mb_ChangeContext = false;
	static UINT mn_ChangeContextTab = 0, mn_ChangeContextSplit = 0;
	CRealConsole* ChangeContext(CRealConsole* apRCon, UINT nTabIndex, UINT nSplitIndex, CVConGuard& VCon, CEStr& pszError);

	/* ****************************** */
	/* ****** Macros functions ****** */
	/* ****************************** */

	// Диалог About(["Tab"])
	CEStr About(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// AffinityPriority([Affinity,Priority])
	CEStr AffinityPriority(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// AltNumber([Base])
	CEStr AltNumber(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Attach - console or ChildGui by PID
	CEStr Attach(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Break (0=CtrlC; 1=CtrlBreak)
	CEStr Break(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Закрыть/прибить текущую консоль
	CEStr Close(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Change execution context (apRCon)
	CEStr Context(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Copy (<What>)
	CEStr Copy(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Debug (<Action>)
	CEStr Debug(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Detach
	CEStr Detach(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Environment variables
	CEStr EnvironmentReload(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	CEStr EnvironmentList(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Найти окно и активировать его. // int nWindowType/*Panels=1, Viewer=2, Editor=3*/, asName
	CEStr FindEditor(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	CEStr FindFarWindow(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	CEStr FindViewer(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	CEStr FindFarWindowHelper(CEFarWindowType anWindowType/*Panels=1, Viewer=2, Editor=3*/, const CEStr& asName, CRealConsole* apRCon, bool abFromPlugin); // helper, это не макро-фукнция
	// Flash
	CEStr Flash(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Изменить имя основного шрифта. string
	CEStr FontSetName(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Изменить размер шрифта. int nRelative, int N
	CEStr FontSetSize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// GetInfo("<Var1>"[,"<Var2>"[,...]])
	CEStr GetInfo(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// GetOption("<Name>")
	CEStr GetOption(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// GroupInput [<cmd>]
	CEStr GroupInput(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Change 'Highlight row/col' under mouse. Locally in current VCon.
	CEStr HighlightMouse(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Проверка, есть ли ConEmu GUI. Функцию мог бы и сам плагин обработать, но для "общности" возвращаем "Yes" здесь
	CEStr IsConEmu(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Проверка, активна ли RealConsole
	CEStr IsConsoleActive(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Проверка, видима ли RealConsole
	CEStr IsRealVisible(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Keys("<Combo1>"[,"<Combo2>"[...]])
	CEStr Keys(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Menu(Type)
	CEStr Menu(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// MsgBox(ConEmu,asText,asTitle,anType) // CEStr asText [, CEStr asTitle[, int anType]]
	CEStr MsgBox(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Palette([<Cmd>[,"<NewPalette>"]])
	CEStr Palette(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Paste (<Cmd>[,"<Text>"[,"<Text2>"[...]]])
	CEStr Paste(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// PasteExplorerPath (<DoCd>,<SetFocus>)
	CEStr PasteExplorerPath(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// PasteFile (<Cmd>[,"<File>"[,"<CommentMark>"]])
	CEStr PasteFile(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Pause
	CEStr Pause(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// print("<Text>") - alias for Paste(2,"<Text>")
	CEStr Print(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Progress(<Type>[,<Value>])
	CEStr Progress(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Recreate(<Action>[,<Confirm>[,<AsAdmin>]]), alias "Create"
	CEStr Recreate(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Rename(<Type>,"<Title>")
	CEStr Rename(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Scroll(<Type>,<Direction>,<Count=1>)
	CEStr Scroll(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Select(<Type>,<DX>,<DY>)
	CEStr Select(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// SetDpi(dpi)
	CEStr SetDpi(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// SetFocus
	CEStr SetFocus(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// SetOption("<Name>",<Value>)
	CEStr SetOption(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// SetParentHWND(<NewParentHWND>)
	CEStr SetParentHWND(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Open Settings dialog
	CEStr Settings(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Shell (ShellExecute)
	CEStr Shell(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Sleep (ms)
	CEStr Sleep(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Split
	CEStr Split(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Status
	CEStr Status(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Tabs
	CEStr Tab(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Task
	CEStr Task(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// TaskAdd("Name","Commands"[,"GuiArgs"[,Flags]])
	CEStr TaskAdd(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// TermMode(<Mode>[,<Action>])
	CEStr TermMode(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Transparency
	CEStr Transparency(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	CEStr TransparencyHelper(int nCmd, int nValue); // helper, это не макро-фукнция
	// Unfasten
	CEStr Unfasten(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Update
	CEStr Update(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Wiki(["Page"])
	CEStr Wiki(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Fullscreen
	CEStr WindowFullscreen(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Maximize
	CEStr WindowMaximize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Минимизировать окно (можно насильно в трей) // [int nForceToTray=0/1]
	CEStr WindowMinimize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Вернуть текущий статус: NOR/MAX/FS/MIN/TSA
	CEStr WindowMode(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Change window pos and size, same as ‘Apply’ button in the ‘Size & Pos’ page of the Settings dialog
	CEStr WindowPosSize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Write all strings to console. Use carefully!
	CEStr Write(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);
	// Установить Zoom для шрифта. 100% и т.п.
	CEStr Zoom(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin);

	typedef DWORD GuiMacroFlags;
	const GuiMacroFlags
		gmf_MainThread           = 1,
		gmf_PostponeWhenActive   = 2,
		gmf_None                 = 0;

	/* ******************************* */
	/* ****** Macro enumeration ****** */
	/* ******************************* */
	struct {
		MacroFunction pfn;
		LPCWSTR Alias[5]{}; // Some function can be called with different names (usability)
		GuiMacroFlags Flags{ gmf_None };
	} Functions[] = {
		// List all functions
		{About, {L"About"}, gmf_MainThread},
		{AffinityPriority, {L"AffinityPriority"}, gmf_PostponeWhenActive|gmf_MainThread},
		{AltNumber, {L"AltNumber", L"AltNumbers", L"AltNumpad"}},
		{Attach, {L"Attach"}, gmf_MainThread},
		{Break, {L"Break"}},
		{Close, {L"Close"}, gmf_MainThread},
		{Context, {L"Context"}},
		{Copy, {L"Copy"}},
		{Debug, {L"Debug"}, gmf_MainThread},
		{Detach, {L"Detach"}, gmf_MainThread},
		{EnvironmentReload, {L"EnvironmentReload", L"EnvReload"}, gmf_MainThread},
		{EnvironmentList, {L"EnvironmentList", L"EnvList"}},
		{FindEditor, {L"FindEditor"}},
		{FindFarWindow, {L"FindFarWindow"}},
		{FindViewer, {L"FindViewer"}},
		{Flash, {L"Flash", L"FlashWindow"}},
		{FontSetName, {L"FontSetName"}, gmf_MainThread},
		{FontSetSize, {L"FontSetSize"}, gmf_MainThread},
		{GetInfo, {L"GetInfo"}},
		{GetOption, {L"GetOption"}},
		{GroupInput, {L"GroupInput"}},
		{HighlightMouse, {L"HighlightMouse"}},
		{IsConEmu, {L"IsConEmu"}},
		{IsConsoleActive, {L"IsConsoleActive"}},
		{IsRealVisible, {L"IsRealVisible"}},
		{Keys, {L"Keys"}, gmf_PostponeWhenActive},
		{Menu, {L"Menu"}, gmf_MainThread},
		{MsgBox, {L"MsgBox"}, gmf_MainThread},
		{Palette, {L"Palette"}},
		{Paste, {L"Paste"}, gmf_PostponeWhenActive},
		{PasteFile, {L"PasteFile"}, gmf_PostponeWhenActive},
		{PasteExplorerPath, {L"PasteExplorerPath"}, gmf_PostponeWhenActive},
		{Pause, {L"Pause"}, gmf_PostponeWhenActive},
		{Print, {L"Print"}, gmf_PostponeWhenActive},
		{Progress, {L"Progress"}},
		{Recreate, {L"Recreate", L"Create"}, gmf_MainThread},
		{Rename, {L"Rename"}, gmf_MainThread},
		{Scroll, {L"Scroll"}, gmf_MainThread},
		{Select, {L"Select"}},
		{SetDpi, {L"SetDpi"}, gmf_MainThread},
		{SetFocus, {L"SetFocus"}, gmf_MainThread},
		{SetOption, {L"SetOption"}, gmf_MainThread},
		{SetParentHWND, {L"SetParentHWND"}, gmf_MainThread},
		{Settings, {L"Settings"}, gmf_MainThread},
		{Shell, {L"Shell", L"ShellExecute"}, gmf_MainThread},
		{Sleep, {L"Sleep"}},
		{Split, {L"Split", L"Splitter"}, gmf_MainThread},
		{Status, {L"Status", L"StatusBar", L"StatusControl"}, gmf_MainThread},
		{Tab, {L"Tab", L"Tabs", L"TabControl"}, gmf_MainThread},
		{Task, {L"Task"}, gmf_MainThread},
		{TaskAdd, {L"TaskAdd"}, gmf_MainThread},
		{TermMode, {L"TermMode"}},
		{Transparency, {L"Transparency"}},
		{Unfasten, {L"Unfasten"}, gmf_MainThread},
		{Update, {L"Update"}, gmf_MainThread},
		{Wiki, {L"Wiki"}},
		{WindowFullscreen, {L"WindowFullscreen"}, gmf_MainThread},
		{WindowMaximize, {L"WindowMaximize"}, gmf_MainThread},
		{WindowMinimize, {L"WindowMinimize"}, gmf_MainThread},
		{WindowMode, {L"WindowMode"}, gmf_MainThread},
		{WindowPosSize, {L"WindowPosSize"}, gmf_MainThread},
		{Write, {L"Write"}},
		{Zoom, {L"Zoom"}, gmf_MainThread},
		// End
		{nullptr}
	};

};



/* ************************* */
/* ****** Realization ****** */  // NOLINT(clang-diagnostic-missing-field-initializers)
/* ************************* */

CEStr ConEmuMacro::ConvertMacro(LPCWSTR asMacro, BYTE FromVersion, bool bShowErrorTip /*= true*/)
{
	_ASSERTE(FromVersion == 0);

	if (!asMacro || !*asMacro)
	{
		return lstrdup(L"");
	}

	CEStr pszBuf(asMacro);
	if (!pszBuf)
	{
		_ASSERTE(FALSE && "Memory allocation failed");
		return nullptr;
	}

	wchar_t* pszMacro = pszBuf.data();
	CEStr pszError;
	GuiMacro* pMacro = GetNextMacro(pszMacro, true, &pszError);
	pszBuf.Release();

	if (!pMacro)
	{
		if (bShowErrorTip)
		{
			const wchar_t pszPrefix[] = L"Failed to convert macro\n";
			const CEStr pszFull(pszPrefix, pszError, L"\n", asMacro);
			if (pszFull)
			{
				Icon.ShowTrayIcon(pszFull, tsa_Config_Error);
			}
		}

		// Return unchanged macro-string
		return lstrdup(asMacro);
	}

	CEStr pszCvt = pMacro->AsString();
	if (!pszCvt)
	{
		_ASSERTE(FALSE && "pMacro->AsString failed!");
		pszCvt = lstrdup(asMacro);
	}
	free(pMacro);

	return pszCvt;
}

CRealConsole* ConEmuMacro::ChangeContext(CRealConsole* apRCon, UINT nTabIndex, UINT nSplitIndex, CVConGuard& VCon, CEStr& pszError)
{
	if (!nTabIndex && !nSplitIndex)
	{
		// Just change context to current VCon
		CVConGroup::GetActiveVCon(&VCon);
	}
	else
	{
		CVConGuard VConTab, VConSplit;

		// Special (non-active) tab or split was selected
		if (nTabIndex)
		{
			gpConEmu->mp_TabBar->GetVConFromTab(nTabIndex - 1, &VConTab, nullptr);
		}
		else if (apRCon)
		{
			VConTab.Attach(apRCon->VCon());
		}
		if (!VConTab.VCon())
		{
			pszError = lstrdup(L"InvalidTabIndex");;
			return nullptr;
		}

		// And split
		CVConGroup* pGr = nullptr;
		if (nSplitIndex && CVConGroup::isGroup(VConTab.VCon(), &pGr))
		{
			MArray<CVConGuard*> Panes;
			int iCount = pGr->GetGroupPanes(&Panes);
			if ((int)nSplitIndex > iCount)
			{
				CVConGroup::FreePanesArray(Panes);
				pszError = lstrdup(L"InvalidSplitIndex");
				return nullptr;
			}
			VConSplit.Attach(Panes[nSplitIndex-1]->VCon());
			CVConGroup::FreePanesArray(Panes);
			if (!VConSplit.VCon())
			{
				pszError = lstrdup(L"InvalidSplit");
				return nullptr;
			}
			VCon.Attach(VConSplit.VCon());
		}
		else
		{
			VCon.Attach(VConTab.VCon());
		}
	}

	if (!VCon.VCon())
	{
		pszError = lstrdup(L"NoActiveCon");
		return nullptr;
	}

	return VCon->RCon();
}



// Общая функция, для обработки любого известного макроса
CEStr ConEmuMacro::ExecuteMacro(CEStr&& macro, CRealConsole* apRCon, bool abFromPlugin /*= false*/, CESERVER_REQ_GUIMACRO* Opt /*= nullptr*/)
{
	wchar_t* asMacro = macro.data();
	CRealConsole* pMacroRCon = apRCon;
	CVConGuard VConTab;

	if (Opt && (Opt->nTabIndex || Opt->nSplitIndex))
	{
		CEStr pszError;
		pMacroRCon = ChangeContext(pMacroRCon, Opt->nTabIndex, Opt->nSplitIndex, VConTab, pszError);
		if (!pMacroRCon)
		{
			return pszError ? pszError : lstrdup(L"InvalidTabIndex");
		}
	}

	CEStr pszErr;
	GuiMacro* p = GetNextMacro(asMacro, false, &pszErr);
	if (!p)
	{
		return pszErr ? pszErr : lstrdup(L"Invalid Macro");
	}

	CEStr pszAllResult;

	const bool bIsMainThread = isMainThread();

	while (p)
	{
		CEStr pszResult;
		LPCWSTR pszFunction = p->szFunc;
		_ASSERTE(pszFunction && !wcschr(pszFunction,L';'));
		// To force execution of that function when RCon successfully starts
		bool bPostpone = false;
		if (*pszFunction == L'@')
		{
			bPostpone = true;
			pszFunction++;
		}

		// Поехали
		MacroFunction pfn = nullptr;
		for (size_t f = 0; Functions[f].pfn && !pfn; f++)
		{
			for (size_t n = 0; (n < countof(Functions[f].Alias)) && Functions[f].Alias[n]; n++)
			{
				if (lstrcmpi(pszFunction, Functions[f].Alias[n]) == 0)
				{
					if ((bPostpone || (Functions[f].Flags & gmf_PostponeWhenActive)) && !(pMacroRCon && pMacroRCon->isConsoleReady()))
					{
						if (!pMacroRCon)
						{
							CEStr pszMacro = p->AsString();
							gpConEmu->AddPostGuiRConMacro(pszMacro);
							pszResult = lstrdup(L"PostponedVCon");
						}
						else
						{
							pszResult = pMacroRCon->PostponeMacro(p->AsString());
						}
					}
					else if ((Functions[f].Flags & gmf_MainThread) && !bIsMainThread)
					{
						SyncExecMacroParm parm = {Functions[f].pfn, p, pMacroRCon, abFromPlugin};
						auto* result = reinterpret_cast<CEStr*>(gpConEmu->SyncExecMacro(f, reinterpret_cast<LPARAM>(&parm)));
						if (!result)
						{
							pszResult.Release();
						}
						else
						{
							pszResult = std::move(*result);
							delete result;
						}
					}
					else
					{
						pszResult = Functions[f].pfn(p, pMacroRCon, abFromPlugin);
					}
					goto executed;
				}
			}
		}

		_ASSERTE(FALSE && "Unknown macro function!");
		pszResult = lstrdup(L"UnknownMacro"); // Неизвестная функция

	executed:
		if (mb_ChangeContext)
		{
			// Usage example: print("abc"); Split(2); Context(); print("def")
			CRealConsole* pChangedRCon = ChangeContext(pMacroRCon, mn_ChangeContextTab, mn_ChangeContextSplit, VConTab, pszResult);
			mb_ChangeContext = false; mn_ChangeContextTab = 0; mn_ChangeContextSplit = 0;
			if (pChangedRCon)
			{
				pMacroRCon = pChangedRCon;
			}
			if (!pszResult)
			{
				pszResult = pChangedRCon ? lstrdup(L"OK") : lstrdup(L"FAILED");
			}
		}

		if (pszResult == nullptr)
		{
			_ASSERTE(FALSE && "MacroFunction must returns anything");
			pszResult = lstrdup(L"");
		}

		if (!pszAllResult)
		{
			pszAllResult = pszResult;
		}
		else
		{
			// Добавить результат через разделитель ';'
			const CEStr pszAll(pszAllResult, L";", pszResult);
			if (!pszAll)
			{
				_ASSERTE(pszAll!=nullptr);
				break;
			}
			pszAllResult = pszAll;
		}

		free(p);
		p = GetNextMacro(asMacro, false, nullptr/*&pszErr*/);
	}

	// Fin
	SafeFree(p);
	return pszAllResult;
}

LRESULT ConEmuMacro::ExecuteMacroSync(WPARAM wParam, LPARAM lParam)
{
	if (static_cast<INT_PTR>(wParam) < 0 || wParam >= countof(Functions))
	{
		_ASSERTE(static_cast<INT_PTR>(wParam) >= 0 && wParam < countof(Functions));
		return 0;
	}

	SyncExecMacroParm* parm = reinterpret_cast<SyncExecMacroParm*>(lParam);
	_ASSERTE(parm->pfn == Functions[wParam].pfn);

	CEStr pszResult = Functions[wParam].pfn(parm->p, parm->pRCon, parm->bFromPlugin);
	if (!pszResult)
		return 0;
	return reinterpret_cast<LRESULT>(new CEStr(std::move(pszResult)));
}

/* ***  Теперь - собственно макросы  *** */

// Диалог About(["Tab"])
CEStr ConEmuMacro::About(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszPageName;
	if (p->IsStrArg(0))
		p->GetStrArg(0, pszPageName);

	ConEmuAbout::OnInfo_About(pszPageName);

	return lstrdup(L"OK");
}

CEStr ConEmuMacro::AffinityPriority(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc;
	CEStr pszAffinity, pszPriority;

	p->GetStrArg(0, pszAffinity);
	p->GetStrArg(1, pszPriority);

	if (apRCon && apRCon->ChangeAffinityPriority(pszAffinity, pszPriority))
		pszRc = lstrdup(L"OK");

	return pszRc ? pszRc : lstrdup(L"FAILED");
}

// AltNumber([Base]) -- Base is 0 or 10 or 16
CEStr ConEmuMacro::AltNumber(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nBase = 0;
	if (!p->GetIntArg(0, nBase) || !(nBase == 0 || nBase == 10 || nBase == 16))
		nBase = 16;
	gpConEmu->EnterAltNumpadMode(nBase);
	return lstrdup(L"OK");
}

CEStr ConEmuMacro::Attach(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nPID = 0, nAlt = 0;
	if (!p->GetIntArg(0, nPID))
	{
		PostMessage(ghWnd, WM_SYSCOMMAND, IDM_ATTACHTO, 0);
		return lstrdup(L"DialogShowed");
	}

	p->GetIntArg(1, nAlt);

	const CAttachDlg::AttachMacroRet nRc = CAttachDlg::AttachFromMacro(nPID, (nAlt!=0));
	switch (nRc)
	{
	case CAttachDlg::amr_Success:
		return lstrdup(L"OK");
	case CAttachDlg::amr_Ambiguous:
		return lstrdup(L"Ambiguous");
	case CAttachDlg::amr_WindowNotFound:
		return lstrdup(L"WindowNotFound");
	case CAttachDlg::amr_Unexpected:
	default:
		return lstrdup(L"Unexpected");
	}
}

// Проверка, есть ли ConEmu GUI. Функцию мог бы плагин и сам обработать,
// но для "общности" возвращаем "Yes" здесь
CEStr ConEmuMacro::IsConEmu(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	return lstrdup(L"Yes");
}

// Проверка, видима ли RealConsole
CEStr ConEmuMacro::IsRealVisible(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult;

	if (apRCon && IsWindowVisible(apRCon->ConWnd()))
		pszResult = lstrdup(L"Yes");
	else
		pszResult = lstrdup(L"No");

	return pszResult;
}

// Проверка, активна ли RealConsole
CEStr ConEmuMacro::IsConsoleActive(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult;

	if (apRCon && apRCon->isActive(false))
		pszResult = lstrdup(L"Yes");
	else
		pszResult = lstrdup(L"No");

	return pszResult;
}

// Break (0=CtrlC; 1=CtrlBreak)
CEStr ConEmuMacro::Break(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	bool bRc = false;
	int nEvent = 0, nGroup = 0;

	if (p->GetIntArg(0, nEvent))
		p->GetIntArg(1, nGroup);

	if ((nEvent == 0 || nEvent == 1) && apRCon)
	{
		bRc = apRCon->PostCtrlBreakEvent(nEvent, nGroup);
	}

	return bRc ? lstrdup(L"OK") : lstrdup(L"InvalidArg");
}

CEStr ConEmuMacro::Close(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult;
	int nCmd = 0, nFlags = 0;

	if (p->GetIntArg(0, nCmd))
		p->GetIntArg(1, nFlags);

	switch (nCmd)
	{
	case 0: // close current console (0), without confirmation (0,1)
		if (apRCon)
		{
			apRCon->CloseConsole(false, (nFlags & 1)==0);
			pszResult = lstrdup(L"OK");
		}
		break;
	case 1: // terminate active process (1), no confirm (1,1)
		if (apRCon)
		{
			bool bTerminated = apRCon->TerminateActiveProcess(((nFlags & 1) == 0), 0);
			pszResult = lstrdup(bTerminated ? L"OK" : L"FAIL");
		}
		break;
	case 2: // close ConEmu window (2), no confirm (2,1)
	{
		bool bClose = true, bMsgConfirmed = false;
		if (!(nFlags & 1) && gpSet->nCloseConfirmFlags && !CVConGroup::OnCloseQuery(&bMsgConfirmed))
			bClose = false;
		else
			gpConEmu->SilentMacroClose = true;
		if (bClose)
		{
			gpConEmu->PostScClose();
			pszResult = lstrdup(L"OK");
		}
		break;
	}
	case 3: // close active tab (3)
		if (apRCon)
		{
			apRCon->CloseTab();
			pszResult = lstrdup(L"OK");
		}
		break;
	case 4: // close active group (4)
	case 7: // close all active processes of the active group (7)
		if (apRCon)
		{
			CVConGroup::CloseGroup(apRCon->VCon(), (nCmd==7));
			pszResult = lstrdup(L"OK");
		}
		break;
	case 5: // close all tabs but active (5), no confirm (5,1)
	case 8: // close all tabs (8), no confirm (8,1)
	case 9: // close all zombies (9), no confirm (9,1)
	case 11:// close to the right (11), no confirm (11,1)
		if (apRCon)
		{
			switch (nCmd)
			{
			case 5:
				CVConGroup::CloseAllButActive(apRCon->VCon(), CVConGroup::CloseSimple, (nFlags & 1)==1); break;
			case 8:
				CVConGroup::CloseAllButActive(nullptr, CVConGroup::CloseSimple, (nFlags & 1)==1); break;
			case 9:
				CVConGroup::CloseAllButActive(nullptr, CVConGroup::CloseZombie, (nFlags & 1)==1); break;
			case 11:
				CVConGroup::CloseAllButActive(apRCon->VCon(), CVConGroup::Close2Right, (nFlags & 1)==1); break;
			}
			pszResult = lstrdup(L"OK");
		}
		break;
	case 6: // close active tab or group (6)
		if (apRCon)
		{
			if (gpSet->isOneTabPerGroup && CVConGroup::isGroup(apRCon->VCon()))
				CVConGroup::CloseGroup(apRCon->VCon());
			else
				apRCon->CloseTab();
			pszResult = lstrdup(L"OK");
		}
		break;
	case 10: // terminate all but shell process (10), no confirm (10,1)
		if (apRCon)
		{
			pszResult = apRCon->TerminateAllButShell((nFlags & 1)==0) ? lstrdup(L"OK") : lstrdup(L"FAILED");
		}
		break;
	}

	return pszResult ? pszResult : lstrdup(L"Failed");
}

CEStr ConEmuMacro::Debug(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int iAction = 0;
	CEStr pszResult;
	if (apRCon)
	{
		p->GetIntArg(0, iAction);
		switch (iAction)
		{
		case 0:
			gpConEmu->StartDebugActiveProcess();
			break;
		case 1:
			gpConEmu->MemoryDumpActiveProcess(false/*abProcessTree*/);
			break;
		case 2:
			gpConEmu->MemoryDumpActiveProcess(true/*abProcessTree*/);
			break;
		default:
			pszResult = lstrdup(L"BadAction");
		}
	}
	else
	{
		pszResult = lstrdup(L"NoConsole");
	}
	return pszResult ? pszResult : lstrdup(L"OK");
}

CEStr ConEmuMacro::Detach(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult;
	if (apRCon)
	{
		int iOptions = 0;
		p->GetIntArg(0, iOptions);
		apRCon->DetachRCon((iOptions & 2) != 0, false, (iOptions & 1) == 1);
		pszResult = lstrdup(L"OK");
	}
	return pszResult ? pszResult : lstrdup(L"Failed");
}

CEStr ConEmuMacro::Unfasten(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult;
	if (apRCon)
	{
		apRCon->Unfasten();
		pszResult = lstrdup(L"OK");
	}
	return pszResult ? pszResult : lstrdup(L"Failed");
}

CEStr ConEmuMacro::Update(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!gpConEmu->CheckUpdates(UpdateCallMode::Manual))
		return lstrdup(L"NotAllowed");
	return lstrdup(L"Started");
}

CEStr ConEmuMacro::Context(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int iVal = 0;
	mb_ChangeContext = true;
	if (p->GetIntArg(0, iVal))
	{
		mn_ChangeContextTab = iVal;
		if (p->GetIntArg(1, iVal))
		{
			mn_ChangeContextSplit = iVal;
		}
	}
	return lstrdup(L"");
}

CEStr ConEmuMacro::EnvironmentReload(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	gpConEmu->ReloadEnvironmentVariables();
	return lstrdup(L"OK");
}

namespace {
void AppendExpandedValue(CEStrConcat& ss, const bool expandable, const wchar_t* data)
{
	if (expandable)
	{
		const CEStr value(ExpandEnvStr(data));
		if (!value.IsEmpty())
		{
			ss.Append(value.c_str(L""));
			return;
		}
	}
	ss.Append(data);
}
};

CEStr ConEmuMacro::EnvironmentList(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStrConcat ss;

	// Pairs `name=value` from registry
	const std::shared_ptr<SystemEnvironment> data_ptr = gpConEmu->GetEnvironmentVariables();
	const auto& data = data_ptr->env_data;
	for (const auto& key : data_ptr->GetKeys())
	{
		const auto& v = data.at(key);
		ss.Append(v.name.c_str());
		ss.Append(L"=");
		AppendExpandedValue(ss, v.expandable, v.data.c_str());
		ss.Append(L"\n");
	}

	// Pairs `name=value` from ConEmu settings
	if (gpSet->psEnvironmentSet)
	{
		CProcessEnvCmd env;
		env.AddLines(gpSet->psEnvironmentSet->c_str(), true);

		class Apply : public CStartEnvBase
		{
		private:
			CEStrConcat& ss_;
		public:
			Apply(CEStrConcat& ss) : ss_(ss) {}
			void Alias(LPCWSTR asName, LPCWSTR asValue) override {}
			void ChCp(LPCWSTR asCP) override {}
			void Echo(LPCWSTR asSwitches, LPCWSTR asText) override {}
			void Title(LPCWSTR asTitle) override {}
			void Type(LPCWSTR asSwitches, LPCWSTR asFile) override {}
			void Set(LPCWSTR asName, LPCWSTR asValue) override
			{
				if (asName && *asName && asValue)
				{
					ss_.Append(asName);
					ss_.Append(L"=");
					AppendExpandedValue(ss_, true, asValue);
					ss_.Append(L"\n");
				}
			}
		};

		Apply apply(ss);
		env.Apply(&apply);
	}

	return ss.GetData().Detach();
}

// Найти окно и активировать его. // LPWSTR asName
CEStr ConEmuMacro::FindEditor(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszName;

	if (!p->GetStrArg(0, pszName))
		return lstrdup(L"");

	// Пустые строки не допускаются
	if (!pszName || !*pszName)
		return lstrdup(L"");

	return FindFarWindowHelper(3/*WTYPE_EDITOR*/, pszName, apRCon, abFromPlugin);
}
// Найти окно и активировать его. // LPWSTR asName
CEStr ConEmuMacro::FindViewer(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszName;

	if (!p->GetStrArg(0, pszName))
		return lstrdup(L"");

	// Пустые строки не допускаются
	if (!pszName || !*pszName)
		return lstrdup(L"");

	return FindFarWindowHelper(2/*WTYPE_VIEWER*/, pszName, apRCon, abFromPlugin);
}
// Найти окно и активировать его. // int nWindowType, LPWSTR asName
CEStr ConEmuMacro::FindFarWindow(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nWindowType = 0;
	CEStr pszName;

	if (!p->GetIntArg(0, nWindowType))
		return lstrdup(L"InvalidArg");

	if (!p->GetStrArg(1, pszName))
		return lstrdup(L"InvalidArg");

	// Пустые строки не допускаются
	if (!pszName || !*pszName)
		return lstrdup(L"InvalidArg");

	return FindFarWindowHelper(nWindowType, pszName, apRCon, abFromPlugin);
}

CEStr ConEmuMacro::FindFarWindowHelper(
	CEFarWindowType anWindowType/*Panels=1, Viewer=2, Editor=3, |(Elevated=0x100), |(NotElevated=0x200), |(Modal=0x400)*/,
	const CEStr& asName, CRealConsole* apRCon, bool abFromPlugin /*= false*/)
{
	CVConGuard VCon;

	// Need to get active con index BEFORE switching to found window
	const int iActiveCon = CVConGroup::ActiveConNum() + 1; //need 1-based value

	int iFoundCon = 0;
	const int iFoundWnd = CVConGroup::isFarExist(anWindowType|fwt_FarFullPathReq|(abFromPlugin?fwt_ActivateOther:fwt_ActivateFound), asName, &VCon);

	if ((iFoundWnd <= 0) && VCon.VCon())
		iFoundCon = -1; // редактор есть, но заблокирован модальным диалогом/другим редактором
	else if (iFoundWnd)
		iFoundCon = gpConEmu->isVConValid(VCon.VCon()); //1-based

	const int cchSize = 32; //-V112
	CEStr pszResult;
	if (!pszResult.GetBuffer(cchSize))
	{
		return lstrdup(L"NotEnoughMem");
	}

	if ((iFoundCon > 0) && (iFoundCon == iActiveCon))
		swprintf_c(pszResult.data(), pszResult.GetMaxCount(), L"Active:%i", (iFoundWnd - 1)); // Need return 0-based Far window index
	else if (iFoundCon > 0)
		swprintf_c(pszResult.data(), pszResult.GetMaxCount(), L"Found:%i", iFoundCon);
	else if (iFoundCon == -1)
		pszResult.Set(L"Blocked");
	else
		pszResult.Set(L"NotFound");

	return pszResult;
}

// Диалог Wiki(["Page"])
CEStr ConEmuMacro::Wiki(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszPage;
	p->GetStrArg(0, pszPage);

	ConEmuAbout::OnInfo_OnlineWiki(pszPage);

	return lstrdup(L"OK");
}

// Fullscreen
CEStr ConEmuMacro::WindowFullscreen(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = WindowMode(nullptr, nullptr, false);

	gpConEmu->DoFullScreen();

	return pszRc;
}

// WindowMaximize
CEStr ConEmuMacro::WindowMaximize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = WindowMode(nullptr, nullptr, false);

	int nStyle = 0; //
	p->GetIntArg(0, nStyle);

	switch (nStyle)
	{
	case 1:
		// By width
		gpConEmu->ChangeTileMode(cwc_TileWidth); break;
	case 2:
		// By height
		gpConEmu->ChangeTileMode(cwc_TileHeight); break;
	default:
		gpConEmu->DoMaximizeRestore();
	}

	return pszRc;
}

// Минимизировать окно (можно насильно в трей) // [int nForceToTray=0/1]
CEStr ConEmuMacro::WindowMinimize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = WindowMode(nullptr, nullptr, false);

	int nForceToTray = 0;
	p->GetIntArg(0, nForceToTray);

	if (nForceToTray)
		Icon.HideWindowToTray();
	else
		PostMessage(ghWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);

	return pszRc;
}

// Опционально установить новый статус
// Вернуть текущий/новый статус: NOR/MAX/FS/MIN/TSA/TLEFT/TRIGHT/THEIGHT
CEStr ConEmuMacro::WindowMode(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = nullptr;
	CEStr pszMode = nullptr;

	LPCWSTR sFS  = L"FS";
	LPCWSTR sMAX = L"MAX";
	LPCWSTR sMIN = L"MIN";
	LPCWSTR sTSA = L"TSA";
	LPCWSTR sNOR = L"NOR";
	LPCWSTR sTileLeft = L"TLEFT";
	LPCWSTR sTileRight = L"TRIGHT";
	LPCWSTR sTileWidth = L"TWIDTH";
	LPCWSTR sTileHeight = L"THEIGHT";
	LPCWSTR sMonitorPrev = L"MPREV";
	LPCWSTR sMonitorNext = L"MNEXT";
	LPCWSTR sBringHere = L"HERE";

	ConEmuWindowCommand Cmd = cwc_Current;

	// без аргументов - ничего не менять, вернуть текущее состояние
	if (p && p->argc)
	{
		if (p->IsIntArg(0))
		{
			int iCmd = 0;
			if (p->GetIntArg(0, iCmd) && (iCmd >= cwc_Current && iCmd < cwc_LastCmd))
			{
				Cmd = static_cast<ConEmuWindowCommand>(iCmd);
			}
		}
		else if (p->IsStrArg(0) && p->GetStrArg(0, pszMode))
		{
			if (lstrcmpi(pszMode, sFS) == 0)
				Cmd = cwc_FullScreen;
			else if (lstrcmpi(pszMode, sMAX) == 0)
				Cmd = cwc_Maximize;
			else if (lstrcmpi(pszMode, sMIN) == 0)
				Cmd = cwc_Minimize;
			else if (lstrcmpi(pszMode, sTSA) == 0)
				Cmd = cwc_MinimizeTSA;
			else if (lstrcmpi(pszMode, sTileLeft) == 0)
				Cmd = cwc_TileLeft;
			else if (lstrcmpi(pszMode, sTileRight) == 0)
				Cmd = cwc_TileRight;
			else if (lstrcmpi(pszMode, sTileWidth) == 0)
				Cmd = cwc_TileWidth;
			else if (lstrcmpi(pszMode, sTileHeight) == 0)
				Cmd = cwc_TileHeight;
			else if (lstrcmpi(pszMode, sMonitorPrev) == 0)
				Cmd = cwc_PrevMonitor;
			else if (lstrcmpi(pszMode, sMonitorNext) == 0)
				Cmd = cwc_NextMonitor;
			else if (lstrcmpi(pszMode, sBringHere) == 0)
				Cmd = cwc_BringHere;
			else //if (lstrcmpi(pszMode, sNOR) == 0)
				Cmd = cwc_Restore;
		}
	}

	switch (Cmd)
	{
	case cwc_Restore:
		gpConEmu->SetWindowMode(wmNormal);
		break;
	case cwc_Minimize:
		gpConEmu->LogString(L"GuiMacro: WindowMode(cwc_Minimize)");
		PostMessage(ghWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		break;
	case cwc_MinimizeTSA:
		Icon.HideWindowToTray();
		break;
	case cwc_Maximize:
		gpConEmu->SetWindowMode(wmMaximized);
		break;
	case cwc_FullScreen:
		gpConEmu->SetWindowMode(wmFullScreen);
		break;
	case cwc_TileLeft:
	case cwc_TileRight:
	case cwc_TileHeight:
	case cwc_TileWidth:
		gpConEmu->ChangeTileMode(Cmd);
		break;
	case cwc_PrevMonitor:
	case cwc_NextMonitor:
		gpConEmu->JumpNextMonitor(Cmd==cwc_NextMonitor);
		break;
	case cwc_BringHere:
		gpConEmu->DoBringHere();
		break;
	case cwc_Current:
	case cwc_LastCmd:
	default:
		; // GCC fix
	}

	if (!IsWindowVisible(ghWnd))
		pszRc = sTSA;
	else if (gpConEmu->isFullScreen())
		pszRc = sFS;
	else if (gpConEmu->isZoomed())
		pszRc = sMAX;
	else if (gpConEmu->isIconic())
		pszRc = sMIN;
	else
	{
		const ConEmuWindowCommand tiled = gpConEmu->GetTileMode(true);
		switch (tiled)  // NOLINT(clang-diagnostic-switch-enum)
		{
		case cwc_TileLeft:
			pszRc = sTileLeft; break;
		case cwc_TileRight:
			pszRc = sTileRight; break;
		case cwc_TileHeight:
			pszRc = sTileHeight; break;
		default:
			pszRc = sNOR;
		}
	}

	return pszRc;
}

// Change window pos and size, same as ‘Apply’ button in the ‘Size & Pos’ page of the Settings dialog
CEStr ConEmuMacro::WindowPosSize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszX = nullptr, pszY = nullptr, pszW = nullptr, pszH = nullptr;

	p->GetStrArg(0, pszX);
	p->GetStrArg(1, pszY);
	p->GetStrArg(2, pszW);
	p->GetStrArg(3, pszH);

	if (!gpConEmu->SetWindowPosSize(pszX, pszY, pszW, pszH))
		return lstrdup(L"FAILED");

	return lstrdup(L"OK");
}

// Menu(Type)
CEStr ConEmuMacro::Menu(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nType = 0;
	p->GetIntArg(0, nType);

	POINT ptCur = {-32000,-32000};

	if (isPressed(VK_LBUTTON) || isPressed(VK_MBUTTON) || isPressed(VK_RBUTTON))
		GetCursorPos(&ptCur);

	switch (nType)
	{
	case 0:
		WARNING("учитывать gpCurrentHotKey, если оно по клику мышки - показывать в позиции курсора");
		LogString(L"ShowSysmenu called from (GuiMacro)");
		gpConEmu->mp_Menu->ShowSysmenu(ptCur.x, ptCur.y);
		return lstrdup(L"OK");

	case 1:
		if (apRCon)
		{
			WARNING("учитывать gpCurrentHotKey, если оно по клику мышки - показывать в позиции курсора");
			if (ptCur.x == -32000)
				ptCur = gpConEmu->mp_Menu->CalcTabMenuPos(apRCon->VCon());
			gpConEmu->mp_Menu->ShowPopupMenu(apRCon->VCon(), ptCur);
			return lstrdup(L"OK");
		}
		break;
	}

	return lstrdup(L"InvalidArg");
}

// MsgBox(ConEmu,asText,asTitle,anType) // LPWSTR asText [, LPWSTR asTitle[, int anType]]
CEStr ConEmuMacro::MsgBox(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszText = nullptr, pszTitle = nullptr;
	int nButtons = 0;

	if (p->GetStrArg(0, pszText))
		if (p->GetStrArg(1, pszTitle))
			p->GetIntArg(2, nButtons);

	int nRc = ::MsgBox(pszText ? pszText : L"", nButtons, pszTitle ? pszTitle : L"ConEmu Macro", nullptr, false);

	switch(nRc)
	{
		case IDABORT:
			return lstrdup(L"Abort");
		case IDCANCEL:
			return lstrdup(L"Cancel");
		case IDIGNORE:
			return lstrdup(L"Ignore");
		case IDNO:
			return lstrdup(L"No");
		case IDOK:
			return lstrdup(L"OK");
		case IDRETRY:
			return lstrdup(L"Retry");
		case IDYES:
			return lstrdup(L"Yes");
	}

	return nullptr;
}

CEStr ConEmuMacro::Flash(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int iMode = 0, iFlags = 0, iCount = 0;
	p->GetIntArg(0, iMode);

	CESERVER_REQ_FLASHWINFO flash = {};

	if (apRCon)
		flash.hWnd = apRCon->ConWnd();

	if (iMode == 0)
	{
		flash.fType = eFlashSimple;
		flash.bInvert = (p->GetIntArg(1, iFlags)) && (iFlags != 0);
	}
	else
	{
		if (p->GetIntArg(1, iFlags))
			flash.dwFlags = iFlags;

		if (p->GetIntArg(2, iCount) && (iCount > 0))
			flash.uCount = iCount;
		else
			flash.uCount = 3;
	}

	gpConEmu->DoFlashWindow(&flash, true);

	return lstrdup(L"OK");
}

// Изменить размер шрифта, в процентах 100%==оригинал
CEStr ConEmuMacro::Zoom(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nValue = 0;

	if (p->GetIntArg(0, nValue))
	{
		gpConEmu->PostFontSetSize(2, nValue);
		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

// Изменить размер шрифта.
// FontSetSize(nRelative, N)
// для nRelative==0: N - высота
// для nRelative==1: N - +-1, +-2
// для nRelative==2: N - Zoom в процентах
// Возвращает - OK или InvalidArg
CEStr ConEmuMacro::FontSetSize(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nRelative = 0, nValue = 0;

	if (p->GetIntArg(0, nRelative)
		&& p->GetIntArg(1, nValue))
	{
		gpConEmu->PostFontSetSize(nRelative, nValue);
		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

// Изменить имя основного шрифта. string. Как бонус - можно сразу поменять и размер
// <FontName>[,<Height>[,<Width>]]
CEStr ConEmuMacro::FontSetName(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszFontName = nullptr;
	int nHeight = 0, nWidth = 0;

	if (p->GetStrArg(0, pszFontName))
	{
		if (!p->GetIntArg(1, nHeight))
			nHeight = 0;
		else if (!p->GetIntArg(2, nWidth))
			nWidth = 0;

		gpConEmu->PostMacroFontSetName(pszFontName.Detach(), nHeight, nWidth, FALSE);

		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

// Copy (<What>)
CEStr ConEmuMacro::Copy(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nWhat = 0, nFormat;

	if (!apRCon)
		return lstrdup(L"InvalidArg");

	bool bCopy = false;
	CECopyMode CopyMode;
	CEStr szDstBuf;
	CEStr pszDstFile = nullptr;

	if (p->GetIntArg(0, nWhat))
	{
		CEStr pszTemp;
		if (!p->GetIntArg(1, nFormat))
		{
			nFormat = gpSet->isCTSHtmlFormat;
		}
		else if (p->GetStrArg(2, pszTemp))
		{
			pszDstFile = pszTemp;
			// Cygwin style path?
			if (wcschr(pszTemp, L'/'))
			{
				if (MakeWinPath(pszTemp, apRCon ? apRCon->GetMntPrefix() : nullptr, szDstBuf))
					pszDstFile = szDstBuf.ms_Val;
			}
		}

		switch (nWhat)
		{
		case 0:
			CopyMode = cm_CopySel; break;
		case 1:
			CopyMode = cm_CopyAll; break;
		case 2:
			CopyMode = cm_CopyVis; break;
		default:
			goto wrap;
		}

		bCopy = apRCon->DoSelectionCopy(CopyMode, nFormat, pszDstFile);

		if (bCopy)
			return lstrdup(L"OK");
	}

wrap:
	return lstrdup(L"InvalidArg");
}

// Paste (<Cmd>[,"<Text>"[,"<Text2>"[...]]])
CEStr ConEmuMacro::Paste(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nCommand = 0;
	CEStr pszText;

	if (!apRCon)
		return lstrdup(L"InvalidArg");

	if (p->GetIntArg(0, nCommand))
	{
		CEPasteMode PasteMode = (nCommand & 1) ? pm_FirstLine : pm_Standard;
		bool bNoConfirm = (nCommand & 2) != 0;
		PosixPasteMode ppm = pxm_Intact;

		CEStr pszChooseBuf;

		if (!(nCommand >= 0 && nCommand <= 10))
		{
			return lstrdup(L"InvalidArg");
		}

		if (p->GetRestStrArgs(1, pszText))
		{
			// Пустая строка допускается только при выборе файла/папки для вставки
			if (!*pszText)
			{
				if (!((nCommand >= 4) && (nCommand <= 7)))
					return lstrdup(L"InvalidArg");
				else
					pszText = nullptr;
			}
		}
		else
		{
			pszText = nullptr;
		}

		if ((nCommand >= 4) && (nCommand <= 7))
		{
			CEStr szDir;
			CEStr pszDefPath = apRCon->GetConsoleCurDir(szDir, true);

			if ((nCommand == 4) || (nCommand == 6))
				pszChooseBuf = SelectFile(L"Choose file pathname for paste", pszText, pszDefPath, ghWnd, nullptr, sff_AutoQuote|((nCommand == 6)?sff_Cygwin:sff_Default));
			else
				pszChooseBuf = SelectFolder(L"Choose folder path for paste", pszText?pszText:pszDefPath, ghWnd, sff_AutoQuote|((nCommand == 6)?sff_Cygwin:sff_Default));

			if (!pszChooseBuf)
				return lstrdup(L"Cancelled");

			pszText = pszChooseBuf;
			PasteMode = pm_FirstLine;
			bNoConfirm = true;
		}
		else if (nCommand == 8)
		{
			PasteMode = pm_FirstLine;
			ppm = pxm_Convert;
			bNoConfirm = true;
		}
		else if (nCommand == 9 || nCommand == 10)
		{
			PasteMode = pm_OneLine;
			_ASSERTE((nCommand != 10) || bNoConfirm);
		}

		apRCon->Paste(PasteMode, pszText, bNoConfirm, (nCommand == 8)/*abCygWin*/);

		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

CEStr ConEmuMacro::Pause(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nCommand = 0;
	CEStr pszResult = nullptr;

	p->GetIntArg(0, nCommand);

	if (apRCon && (nCommand >= 0 && nCommand <= CEPause_Off))
	{
		CEPauseCmd result = apRCon->Pause((CEPauseCmd)nCommand);
		pszResult = lstrdup(result==CEPause_On ? L"PAUSED" : result==CEPause_Off ? L"UNPAUSED" : L"UNKNOWN");
	}

	return pszResult ? pszResult : lstrdup(L"FAILED");
}

// GroupInput [<cmd>]
CEStr ConEmuMacro::GroupInput(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"NoActiveConsole");

	int nCommand = 0;
	p->GetIntArg(0, nCommand);

	// Active splits (visible panes)
	if (nCommand >= 0 && nCommand <= 2)
	{
		// 0 - toggle, 1 - group, 2 - ungroup
		CVConGroup::GroupInput(apRCon->VCon(), (GroupInputCmd)nCommand);
		return lstrdup(L"OK");
	}
	// All VCons (active/inactive/invisible)
	else if (nCommand >= 3 && nCommand <= 5)
	{
		// 3 - toggle, 4 - group, 5 - ungroup
		CVConGroup::ResetGroupInput(apRCon->Owner(), (GroupInputCmd)(nCommand-3));
		return lstrdup(L"OK");
	}
	// Toggle group mode on active console
	else if (nCommand == 6)
	{
		CVConGroup::GroupSelectedInput(apRCon->VCon());
		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

// PasteExplorerPath (<DoCd>,<SetFocus>)
CEStr ConEmuMacro::PasteExplorerPath(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int iDoCd = 1, iSetFocus = 1;

	if (!apRCon)
		return lstrdup(L"InvalidArg");

	if (p->IsIntArg(0))
	{
		p->GetIntArg(0, iDoCd);

		if (p->IsIntArg(1))
			p->GetIntArg(1, iSetFocus);
	}

	apRCon->PasteExplorerPath((iDoCd!=0), (iSetFocus!=0));

	return lstrdup(L"OK");
}

// PasteFile (<Cmd>[,"<File>"[,"<CommentMark>"]])
CEStr ConEmuMacro::PasteFile(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	bool bOk = false;
	int nCommand = 0;
	CEStr pszFile = nullptr;
	CEStr ptrBuf = nullptr;
	DWORD nBufSize = 0, nErrCode = 0;

	if (!apRCon)
		return lstrdup(L"InvalidArg");

	if (p->GetIntArg(0, nCommand))
	{
		CEPasteMode PasteMode = (nCommand & 1) ? pm_FirstLine : pm_Standard;
		const bool bNoConfirm = (nCommand & 2) != 0;

		CEStr pszChooseBuf = nullptr;

		if (!(nCommand >= 0 && nCommand <= 10))
		{
			return lstrdup(L"InvalidArg");
		}

		if (!p->GetStrArg(1, pszFile) || !pszFile || !*pszFile)
		{
			pszChooseBuf = SelectFile(L"Choose file for paste", nullptr, nullptr, nullptr, nullptr, sff_Default);
			if (!pszChooseBuf)
				return lstrdup(L"NoFileSelected");
			pszFile = pszChooseBuf;
		}

		if (nCommand == 9 || nCommand == 10)
		{
			PasteMode = pm_OneLine;
			_ASSERTE((nCommand != 10) || bNoConfirm);
		}

		if ((ReadTextFile(pszFile, 0x100000, ptrBuf, nBufSize, nErrCode) == 0) && nBufSize)
		{
			// Need to drop lines with comment marks?
			CEStr pszCommentMark = nullptr;
			if (p->GetStrArg(2, pszCommentMark) && *pszCommentMark)
			{
				StripLines(ptrBuf.data(), pszCommentMark);
			}

			// And PASTE
			apRCon->Paste(PasteMode, ptrBuf, bNoConfirm);
			bOk = true;
		}

		return bOk ? lstrdup(L"OK") : lstrdup(L"ReadFileFailed");
	}

	return lstrdup(L"InvalidArg");
}

// print("<Text>"[,"<Text2>"[...]]) - alias for Paste(2,"<Text>")
CEStr ConEmuMacro::Print(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"InvalidArg");

	CEStr pszText = nullptr;
	if (p->GetRestStrArgs(0, pszText))
	{
		if (!*pszText)
			return lstrdup(L"InvalidArg");
	}
	else
	{
		pszText = nullptr;
	}

	apRCon->Paste(pm_Standard, pszText, true);

	return lstrdup(L"OK");
}

// Keys("<Combo1>"[,"<Combo2>"[...]])
CEStr ConEmuMacro::Keys(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	// Наверное имеет смысл поддержать синтаксис AHK (допускаю, что многие им пользуются)
	// Но по хорошему, проще всего разрешить что-то типа
	// Keys("CtrlC","ControlV","ShiftIns")

	if (!apRCon)
		return lstrdup(L"RConRequired");

	CEStr keyBuf = nullptr;
	int iKeyNo = 0;
	wchar_t szSeq[4];

	while (p->GetStrArg(iKeyNo++, keyBuf))
	{
		// GetStrArg never returns true & nullptr
		wchar_t* pszKey = keyBuf.data();
		_ASSERTE(pszKey);
		// Modifiers (for Example RCtrl+Shift+LAlt)
		DWORD dwControlState = 0, dwScan = 0;
		int iScanCode = -1;
		bool  bRight = false;
		wchar_t* p;
		// + the key
		UINT VK = 0;
		// Parse modifiers
		while (pszKey[0] && pszKey[1])
		{
			switch (pszKey[0])
			{
			case L'<':
				bRight = false;
				break;
			case L'>':
				bRight = true;
				break;
			case L'^':
				dwControlState |= bRight ? RIGHT_CTRL_PRESSED : LEFT_CTRL_PRESSED;
				bRight = false;
				break;
			case L'!':
				dwControlState |= bRight ? RIGHT_ALT_PRESSED : LEFT_ALT_PRESSED;
				bRight = false;
				break;
			case L'+':
				dwControlState |= SHIFT_PRESSED;
				bRight = false;
				break;
			default:
				// Post {pszKey + dwControlState} to console
				goto DoPost;
			}
			pszKey++;
		}

	DoPost:
		if (*pszKey == L'{' && ((p = wcschr(pszKey+2,L'}')) != nullptr))
		{
			// Trim brackets from "{Enter}" for example
			pszKey++; *p = 0;
		}
		VK = ConEmuChord::GetVkByKeyName(pszKey, &iScanCode, &dwControlState);
		switch (VK)
		{
		case VK_WHEEL_UP: case VK_WHEEL_DOWN: case VK_WHEEL_LEFT: case VK_WHEEL_RIGHT:
			return lstrdup(L"NotImplementedYet");
		}
		if (VK || (iScanCode != -1))
		{
			if (pszKey[1] != 0)
				pszKey = nullptr;

			if (dwControlState & (RIGHT_ALT_PRESSED|LEFT_ALT_PRESSED|RIGHT_CTRL_PRESSED|LEFT_CTRL_PRESSED))
			{
				BYTE pressed[256] = {};
				if (dwControlState & RIGHT_ALT_PRESSED)  { pressed[VK_MENU] = 0x81;    pressed[VK_RMENU] = 0x81; }
				if (dwControlState & LEFT_ALT_PRESSED)   { pressed[VK_MENU] = 0x81;    pressed[VK_LMENU] = 0x81; }
				if (dwControlState & RIGHT_CTRL_PRESSED) { pressed[VK_CONTROL] = 0x81; pressed[VK_RCONTROL] = 0x81; }
				if (dwControlState & LEFT_CTRL_PRESSED)  { pressed[VK_CONTROL] = 0x81; pressed[VK_LCONTROL] = 0x81; }
				if (dwControlState & SHIFT_PRESSED)      { pressed[VK_SHIFT] = 0x81;   pressed[VK_LSHIFT] = 0x81; }

				if (VK > 0 && VK <= 255)
					pressed[VK] = 0x81;

				int iMapRc = ToUnicode(VK, (iScanCode != -1) ? iScanCode : 0, pressed, szSeq, countof(szSeq), 0);

				if (iMapRc == 1)
					pszKey = szSeq;
				else
					pszKey = nullptr;
			}

			if (!(dwControlState & (SHIFT_PRESSED|RIGHT_ALT_PRESSED|LEFT_ALT_PRESSED|RIGHT_CTRL_PRESSED|LEFT_CTRL_PRESSED))
				&& !(pszKey && *pszKey))
			{
				switch (VK)
				{
				case VK_RETURN:
					wcscpy_c(szSeq, L"\r"); pszKey = szSeq; break;
				case VK_TAB:
					wcscpy_c(szSeq, L"\t"); pszKey = szSeq; break;
				case VK_BACK:
					wcscpy_c(szSeq, L"\b"); pszKey = szSeq; break;
				}
			}

			// Send it...
			apRCon->PostKeyPress(VK, dwControlState, pszKey ? *pszKey : 0, iScanCode);
			continue;
		}
		return lstrdup(L"UnknownKey");
	}

	return lstrdup(L"OK");

	// Префиксы:
	//	# - Win
	//	! - Alt
	//	^ - Control
	//	+ - Shift
	//	% - Apps
	//	< - use Left modifier
	//	> - use Right modifier
	//	<^>! - special combo for AltGr!
	//	Down/Up - Used for emulating of series keypresses.

	// KeyNames? (From AHK "List of Keys, Mouse Buttons, and Joystick Controls")
	/*
	WheelDown Turn the wheel downward (toward you).
	WheelUp Turn the wheel upward (away from you).

	WheelLeft
	WheelRight [v1.0.48+]: Scroll to the left or right.


	CapsLock Caps lock
	Space Space bar
	Tab Tab key
	Enter (or Return) Enter key
	Escape (or Esc) Esc key
	Backspace (or BS) Backspace


	ScrollLock Scroll lock
	Delete (or Del) Delete key
	Insert (or Ins) Insert key
	Home Home key
	End End key
	PgUp Page Up key
	PgDn Page Down key
	Up Up arrow key
	Down Down arrow key
	Left Left arrow key
	Right Right arrow key


	NumLock
	Numpad0 NumpadIns 0 / Insert key
	Numpad1 NumpadEnd 1 / End key
	Numpad2 NumpadDown 2 / Down arrow key
	Numpad3 NumpadPgDn 3 / Page Down key
	Numpad4 NumpadLeft 4 / Left arrow key
	Numpad5 NumpadClear 5 / typically does nothing
	Numpad6 NumpadRight 6 / Right arrow key
	Numpad7 NumpadHome 7 / Home key
	Numpad8 NumpadUp 8 / Up arrow key
	Numpad9 NumpadPgUp 9 / Page Up key
	NumpadDot NumpadDel Decimal separation / Delete key
	NumpadDiv NumpadDiv Divide
	NumpadMult NumpadMult Multiply
	NumpadAdd NumpadAdd Add
	NumpadSub NumpadSub Subtract
	NumpadEnter NumpadEnter Enter key


	F1 - F24 The 12 or more function keys at the top of most keyboards.


	LWin Left Windows logo key. Corresponds to the <# hotkey prefix.
	RWin Right Windows logo key. Corresponds to the ># hotkey prefix.
	Control (or Ctrl) Control key. As a hotkey (Control::) it fires upon release unless it has the tilde prefix. Corresponds to the ^ hotkey prefix.
	Alt Alt key. As a hotkey (Alt::) it fires upon release unless it has the tilde prefix. Corresponds to the ! hotkey prefix.
	Shift Shift key. As a hotkey (Shift::) it fires upon release unless it has the tilde prefix. Corresponds to the + hotkey prefix.
	LControl (or LCtrl) Left Control key. Corresponds to the <^ hotkey prefix.
	RControl (or RCtrl) Right Control key. Corresponds to the >^ hotkey prefix.
	LShift Left Shift key. Corresponds to the <+ hotkey prefix.
	RShift Right Shift key. Corresponds to the >+ hotkey prefix.
	LAlt Left Alt key. Corresponds to the <! hotkey prefix.
	RAlt Right Alt key. Corresponds to the >! hotkey prefix.
		Note: If your keyboard layout has AltGr instead of RAlt, you can probably use it as a hotkey prefix via <^>! as described here. In addition, LControl & RAlt:: would make AltGr itself into a hotkey.

	Browser_Back Back
	Browser_Forward Forward
	Browser_Refresh Refresh
	Browser_Stop Stop
	Browser_Search Search
	Browser_Favorites Favorites
	Browser_Home Homepage
	Volume_Mute Mute the volume
	Volume_Down Lower the volume
	Volume_Up Increase the volume
	Media_Next Next Track
	Media_Prev Previous Track
	Media_Stop Stop
	Media_Play_Pause Play/Pause
	Launch_Mail Launch default e-mail program
	Launch_Media Launch default media player
	Launch_App1 Launch My Computer
	Launch_App2 Launch Calculator
		Note: The function assigned to each of the keys listed above can be overridden by modifying the Windows registry. This table shows the default function of each key on most versions of Windows.


	AppsKey Menu key. This is the key that invokes the right-click context menu.
	PrintScreen Print screen
	CtrlBreak

	Pause Pause key
	Break Break key. Since this is synonymous with Pause, use ^CtrlBreak in hotkeys instead of ^Pause or ^Break.
	Help Help key. This probably doesn't exist on most keyboards. It's usually not the same as F1.
	Sleep Sleep key. Note that the sleep key on some keyboards might not work with this.
	SCnnn Specify for nnn the scan code of a key. Recognizes unusual keys not mentioned above. See Special Keys for details.
	VKnn Specify for nn the hexadecimal virtual key code of a key.
	*/
}

// Palette([<Cmd>[,"<NewPalette>"]])
// Cmd=0 - return palette from ConEmu settings
// Cmd=1 - change palette in ConEmu settings, returns prev palette
// Cmd=2 - return palette from current console
// Cmd=3 - change palette in current console, returns prev palette
CEStr ConEmuMacro::Palette(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = nullptr;
	int nCmd = 0, iPal;
	CEStr pszNewName = nullptr;
	const ColorPalette* pPal;

	p->GetIntArg(0, nCmd);

	switch (nCmd)
	{
	case 0:
	case 1:
		// Return current palette name
		pPal = gpSet->PaletteFindCurrent(true);
		_ASSERTE(pPal!=nullptr); // If find failed - it returns "<Current color scheme>"
		pszRc = lstrdup(pPal ? pPal->pszName : L"Unexpected");
		// And change to new, if asked
		if ((nCmd == 1) && (p->argc > 1))
		{
			// Allow name or index
			if (p->IsStrArg(1) && p->GetStrArg(1, pszNewName) && pszNewName && *pszNewName)
				iPal = gpSet->PaletteGetIndex(pszNewName);
			else if (!p->IsIntArg(1) || !p->GetIntArg(1, iPal))
				iPal = -1;
			// Index is valid?
			if (iPal < 0)
			{
				pszRc.Set(L"InvalidArg");
			}
			else
			{
				gpSetCls->ChangeCurrentPalette(gpSet->PaletteGet(iPal), true);
			}
		}
		break;
	case 2:
	case 3:
		if (apRCon)
		{
			// Return current palette name (in the active console)
			iPal = apRCon->VCon()->GetPaletteIndex();
			pPal = (iPal >= 0)
				? gpSet->PaletteGet(iPal)
				: gpSet->PaletteFindCurrent(true);
			_ASSERTE(pPal!=nullptr); // At least "<Current color scheme>" must be returned
			pszRc = lstrdup(pPal ? pPal->pszName : L"Unexpected");
			// And change to new, if asked
			if ((nCmd == 3) && (p->argc > 1))
			{
				// Allow name or index
				if (p->IsStrArg(1) && p->GetStrArg(1, pszNewName) && pszNewName && *pszNewName)
					apRCon->VCon()->ChangePalette(gpSet->PaletteGetIndex(pszNewName));
				else if (p->IsIntArg(1) && p->GetIntArg(1, iPal))
					apRCon->VCon()->ChangePalette(iPal);
			}
		}
		else
		{
			pszRc = lstrdup(L"InvalidArg");
		}
		break;
	default:
		pszRc = lstrdup(L"InvalidArg");
	}

	return pszRc;
}

// Progress(<Type>[,<Value>])
CEStr ConEmuMacro::Progress(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nState = 0, nValue = 0;
	CEStr pszName;
	if (!apRCon)
		return lstrdup(L"InvalidArg");

	if (p->GetIntArg(0, nState))
	{
		const auto state = static_cast<AnsiProgressStatus>(nState);
		if (!(state >= AnsiProgressStatus::None && state <= AnsiProgressStatus::LongRunStop))
		{
			return lstrdup(L"InvalidArg");
		}

		if (state == AnsiProgressStatus::Running || state == AnsiProgressStatus::Error || state == AnsiProgressStatus::Paused)
		{
			p->GetIntArg(1, nValue);
			if (nValue < 0 || nValue > 100)
				return lstrdup(L"InvalidArg");
		}
		else if (state == AnsiProgressStatus::LongRunStart || state == AnsiProgressStatus::LongRunStop)
		{
			p->GetStrArg(1, pszName);
			if (!pszName || !*pszName)
				return lstrdup(L"InvalidArg");
		}

		apRCon->SetProgress(state, static_cast<short>(nValue), pszName);

		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

// Recreate(<Action>[,<Confirm>[,<AsAdmin>]]), alias "Create"
CEStr ConEmuMacro::Recreate(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	RecreateActionParm Action = gpSetCls->GetDefaultCreateAction();
	bool Confirm = gpSet->isMultiNewConfirm;
	RConBoolArg AsAdmin = crb_Undefined;

	int i = 0;
	if (p->GetIntArg(0, i))
	{
		if ((i >= cra_CreateTab) && (i <= cra_CreateWindow))
			Action = (RecreateActionParm)i;
		if (p->GetIntArg(1, i))
			Confirm = (i != 0);
		if (p->GetIntArg(2, i))
			AsAdmin = i ? crb_On : crb_Off;
	}

	CEStr pszRc = gpConEmu->RecreateAction(Action, Confirm, AsAdmin)
		? lstrdup(L"CREATED")
		: lstrdup(L"FAILED");
	return pszRc;
}

// Rename(<Type>,"<Title>")
CEStr ConEmuMacro::Rename(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nType = 0;
	CEStr pszTitle = nullptr;

	if (!apRCon)
		return lstrdup(L"InvalidArg");

	if (p->GetIntArg(0, nType))
	{
		if (!(nType == 0 || nType == 1))
		{
			return lstrdup(L"InvalidArg");
		}

		if (!p->GetStrArg(1, pszTitle) || !*pszTitle)
		{
			pszTitle = nullptr;
		}

		switch (nType)
		{
		case 0:
			apRCon->RenameTab(pszTitle);
			break;
		case 1:
			apRCon->RenameWindow(pszTitle);
			break;
		}

		return lstrdup(L"OK");
	}

	return lstrdup(L"InvalidArg");
}

// Scroll(<Type>,<Direction>,<Count=1>)
//  Type: 0; Value: ‘-1’=Up, ‘+1’=Down
//  Type: 1; Value: ‘-1’=PgUp, ‘+1’=PgDown
//  Type: 2; Value: ‘-1’=HalfPgUp, ‘+1’=HalfPgDown
//  Type: 3; Value: ‘-1’=Top, ‘+1’=Bottom
//  Type: 4; No arguments; Go to cursor line
//  Type: 5; Value: ‘-1’=PrevPrompt, ‘+1’=NextPrompt - search for executed prompts
CEStr ConEmuMacro::Scroll(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"No console");

	int nType = 0, nDir = 0, nCount = 1;

	if (!p->GetIntArg(0, nType))
		return lstrdup(L"InvalidArg");
	if (nType != 4)
	{
		if (!p->GetIntArg(1, nDir) || (nDir == 0))
			return lstrdup(L"InvalidArg");
		if (!p->GetIntArg(2, nCount) || (nCount < 1))
			nCount = 1;
	}

	switch (nType)
	{
	case 0:
		apRCon->DoScroll((nDir < 0) ? SB_LINEUP : SB_LINEDOWN, nCount);
		break;
	case 1:
		apRCon->DoScroll((nDir < 0) ? SB_PAGEUP : SB_PAGEDOWN, nCount);
		break;
	case 2:
		apRCon->DoScroll((nDir < 0) ? SB_HALFPAGEUP : SB_HALFPAGEDOWN, nCount);
		break;
	case 3:
		apRCon->DoScroll((nDir < 0) ? SB_TOP : SB_BOTTOM);
		break;
	case 4:
		apRCon->DoScroll(SB_GOTOCURSOR);
		break;
	case 5:
		// Don't scroll RealConsole, just virtually position our view rect
		apRCon->DoScroll((nDir < 0) ? SB_PROMPTUP : SB_PROMPTDOWN, 1);
		break;
	default:
		return lstrdup(L"InvalidArg");
	}

	return lstrdup(L"OK");
}

// Select(<Type>,<DX>,<DY>,<HE>)
//  Type: 0 - Text, 1 - Block, 2 - Stop selection
//    DX: select text horizontally: -1/+1
//    DY: select text vertically: -1/+1
//    HE: to-home(-1)/to-end(+1) with text selection
CEStr ConEmuMacro::Select(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"No console");

	int nType = 0; // 0 - text, 1 - block
	int nDX = 0, nDY = 0, nHomeEnd = 0;

	p->GetIntArg(0, nType);
	if (nType == 2)
	{
		// ref gh-1299
		if (!apRCon->isSelectionPresent())
			return lstrdup(L"Selection was not started");
		apRCon->DoSelectionFinalize();
		return lstrdup(L"OK");
	}

	if (apRCon->isSelectionPresent())
		return lstrdup(L"Selection was already started");

	p->GetIntArg(1, nDX);
	p->GetIntArg(2, nDY);
	p->GetIntArg(3, nHomeEnd);

	bool bText = (nType == 0);

	COORD cr = {};
	apRCon->GetConsoleCursorInfo(nullptr, &cr);

	#ifdef _DEBUG
	CONSOLE_SCREEN_BUFFER_INFO csbi = {};
	apRCon->GetConsoleScreenBufferInfo(&csbi);
	#endif

	//WARNING("TODO: buffered selection!");
	//if (cr.X >= csbi.srWindow.Left)
	//{
	//	cr.X -= csbi.srWindow.Left;
	//}
	//else
	//{
	//	_ASSERTE(cr.X >= csbi.srWindow.Left);
	//}

	//if (cr.Y >= csbi.srWindow.Top)
	//{
	//	cr.Y -= csbi.srWindow.Top;
	//}
	//else
	//{
	//	_ASSERTE(cr.Y >= csbi.srWindow.Top);
	//}

	if (nType == 0)
	{
		if ((nDX < 0) && cr.X)
			cr.X--;
	}

	DWORD nAnchorFlag =
		(nDY < 0) ? CONSOLE_RIGHT_ANCHOR :
		(nDY > 0) ? CONSOLE_LEFT_ANCHOR :
		(nDX < 0) ? CONSOLE_RIGHT_ANCHOR :
		(nDX > 0) ? CONSOLE_LEFT_ANCHOR :
		0;

	apRCon->StartSelection(bText, cr.X, cr.Y, false, nAnchorFlag);

	if (nType == 1)
	{
		if (nDY)
			apRCon->ExpandSelection(cr.X, cr.Y + nDY);
	}
	else if (nType == 0)
	{
		switch (nHomeEnd)
		{
		case -1:
			apRCon->ChangeSelectionByKey(VK_HOME, false, true); break;
		case +1:
			apRCon->ChangeSelectionByKey(VK_END, false, true); break;
		case -2:
			apRCon->ChangeSelectionByKey(VK_LEFT, true, true); break;
		case +2:
			apRCon->ChangeSelectionByKey(VK_RIGHT, true, true); break;
		}
	}

	return lstrdup(L"OK");
}

// SetDpi(dpi)
CEStr ConEmuMacro::SetDpi(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nValue = 0;

	if (!p->GetIntArg(0, nValue))
		return lstrdup(L"InvalidArg");

	RECT rcCurrent = gpConEmu->CalcRect(CER_MAIN);
	gpConEmu->OnDpiChanged(nValue, nValue, &rcCurrent, true, dcs_Macro);

	return lstrdup(L"OK");
}

CEStr ConEmuMacro::Int2Str(UINT nValue, bool bSigned)
{
	wchar_t szNumber[20];
	if (bSigned)
		swprintf_c(szNumber, L"%i", (int)nValue);
	else
		swprintf_c(szNumber, L"%u", nValue);
	return CEStr(szNumber);
}

// GetInfo("<Var1>"[,"<Var2>"[,...]])
CEStr ConEmuMacro::GetInfo(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult;
	CEStr pszName;
	CEStr szBuf;
	int idx = 0;

	while (p->GetStrArg(idx++, pszName))
	{
		wchar_t szTemp[MAX_PATH] = L"";
		LPCWSTR pszVal = szTemp;

		// Globals
		if (lstrcmpi(pszName, L"PID") == 0)
			ultow_s(GetCurrentProcessId(), szTemp, 10);
		else if (lstrcmpi(pszName, L"HWND") == 0)
			msprintf(szTemp, countof(szTemp), L"0x%08X", LODWORD(ghWnd));
		else if (lstrcmpi(pszName, L"Build") == 0)
			wcscpy_c(szTemp, gpConEmu->ms_ConEmuBuild);
		else if (lstrcmpi(pszName, L"Drive") == 0)
			GetDrive(gpConEmu->ms_ConEmuExeDir, szTemp, countof(szTemp));
		else if (lstrcmpi(pszName, L"Dir") == 0)
			wcscpy_c(szTemp, gpConEmu->ms_ConEmuExeDir);
		else if (lstrcmpi(pszName, L"BaseDir") == 0)
			wcscpy_c(szTemp, gpConEmu->ms_ConEmuBaseDir);
		else if (lstrcmpi(pszName, L"Hooks") == 0)
			wcscpy_c(szTemp, (gpSet->isUseInjects) ? L"Enabled" : L"OFF");
		else if (lstrcmpi(pszName, L"ANSI") == 0)
			wcscpy_c(szTemp, (gpSet->isUseInjects && gpSet->isProcessAnsi) ? L"ON" : L"OFF");
		else if (lstrcmpi(pszName, L"Args") == 0)
			pszVal = gpConEmu->opt.cfgSwitches;
		else if (lstrcmpi(pszName, L"Args2") == 0)
			pszVal = gpConEmu->opt.cmdRunCommand;
		else if (lstrcmpi(pszName, L"Config") == 0)
			pszVal = gpSetCls->GetConfigName();
		else if (lstrcmpi(pszName, L"ConfigPath") == 0)
			pszVal = gpSetCls->GetConfigPath();
		// RCon related
		else if (apRCon)
			pszVal = apRCon->GetConsoleInfo(pszName, szBuf);
		else if (lstrcmpi(pszName, L"Root") == 0 || lstrcmpi(pszName, L"RootInfo") == 0)
			pszVal = CreateRootInfoXml(nullptr/*asRootExeName*/, nullptr, szBuf);

		// Concat the string
		pszResult.Append(pszResult ? L"\n" : nullptr, pszVal);
	}

	return pszResult ? pszResult : lstrdup(L"");
}

// GetOption("<Name>")
CEStr ConEmuMacro::GetOption(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult = nullptr;
	CEStr pszName;
	int nID = 0;

	if (p->GetIntArg(0, nID))
	{
		// TODO: ...
	}
	else if (p->GetStrArg(0, pszName))
	{
		if (!lstrcmpi(pszName, L"QuakeStyle") || !lstrcmpi(pszName, L"QuakeAutoHide"))
		{
			pszResult = Int2Str(gpSet->isQuakeStyle, false);
		}
		else if (!lstrcmpi(pszName, L"FarGotoEditorPath"))
		{
			pszResult = lstrdup(gpSet->sFarGotoEditor);
		}
		else if (!lstrcmpi(pszName, L"TabSelf"))
		{
			pszResult = Int2Str(gpSet->isTabSelf, false);
		}
		else if (!lstrcmpi(pszName, L"TabRecent"))
		{
			pszResult = Int2Str(gpSet->isTabRecent, false);
		}
		else if (!lstrcmpi(pszName, L"TabLazy"))
		{
			pszResult = Int2Str(gpSet->isTabLazy, false);
		}
	}

	return pszResult ? pszResult : lstrdup(L"");
}

// SetOption("<Name>",<Value>)
CEStr ConEmuMacro::SetOption(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult = nullptr;
	CEStr pszName;
	CEStr pszValue = nullptr;
	int nValue = 0;
	int nRel = 0;
	int CB = 0, uCheck = 0;

	if (!p->GetStrArg(0, pszName))
		return lstrdup(L"InvalidArg");

	// Check option by CheckBox/RadioButton ID
	if (!lstrcmpi(pszName, L"Check"))
	{
		if (!p->GetIntArg(1, CB) || (CB <= 0))
			pszResult = lstrdup(L"InvalidID");
		else if (!p->GetIntArg(2, uCheck) || (uCheck < (int)BST_UNCHECKED || uCheck >= (int)BST_INDETERMINATE))
			pszResult = lstrdup(L"InvalidValue");
		else
			pszResult = CSetDlgButtons::CheckButtonMacro(CB, uCheck);
		return pszResult;
	}

	// Other named options specially implemented
	if (!lstrcmpi(pszName, L"QuakeAutoHide"))
	{
		// SetOption("QuakeAutoHide",0) - Hide on demand only
		// SetOption("QuakeAutoHide",1) - Enable autohide on focus lose
		// SetOption("QuakeAutoHide",2) - Switch autohide
		p->GetIntArg(1, nValue);
		if (gpSet->isQuakeStyle)
		{
			switch (nValue)
			{
			case 0:
				gpConEmu->SetQuakeMode(1);
				break;
			case 1:
				gpConEmu->SetQuakeMode(2);
				break;
			case 2:
				gpConEmu->SetQuakeMode((gpSet->isQuakeStyle == 1) ? 2 : 1);
				break;
			}
		}
		pszResult = lstrdup(L"OK");
	}
	else if (!lstrcmpi(pszName, L"bgImageDarker"))
	{
		if (p->GetIntArg(1, nValue))
		{
			if (p->GetIntArg(2, nRel) && nRel) // Use relative values?
				nValue = std::max(0,std::min(255,(((int)(UINT)gpSet->bgImageDarker)+nValue)));
			if (nValue >= 0 && nValue < 256 && nValue != (int)(UINT)gpSet->bgImageDarker)
			{
				gpSetCls->SetBgImageDarker(nValue, true);
			}
		}
		pszResult = lstrdup(L"OK");
	}
	else if (!lstrcmpi(pszName, L"AlwaysOnTop"))
	{
		// SetOption("AlwaysOnTop",0) - Disable top-most
		// SetOption("AlwaysOnTop",1) - Enable top-most
		// SetOption("AlwaysOnTop",2) - Switch top-most
		p->GetIntArg(1, nValue);
		switch (nValue)
		{
		case 0:
			gpSet->isAlwaysOnTop = false;
			break;
		case 1:
			gpSet->isAlwaysOnTop = true;
			break;
		case 2:
			gpSet->isAlwaysOnTop = !gpSet->isAlwaysOnTop;
			break;
		}
		gpConEmu->DoAlwaysOnTopSwitch();
		pszResult = lstrdup(L"OK");
	}
	else if (!lstrcmpi(pszName, L"AlphaValue"))
	{
		if (p->GetIntArg(1, nValue))
			pszResult = TransparencyHelper((p->GetIntArg(2, nRel) && nRel) ? 1 : 0, nValue);
	}
	else if (!lstrcmpi(pszName, L"AlphaValueInactive"))
	{
		if (p->GetIntArg(1, nValue))
			pszResult = TransparencyHelper((p->GetIntArg(2, nRel) && nRel) ? 3 : 2, nValue);
	}
	else if (!lstrcmpi(pszName, L"AlphaValueSeparate"))
	{
		if (p->GetIntArg(1, nValue))
			pszResult = TransparencyHelper(4, nValue);
	}
	else if (!lstrcmpi(pszName, L"CursorBlink"))
	{
		if (p->GetIntArg(1, nValue))
			pszResult = gpSetCls->SetOption(pszName, nValue) ? lstrdup(L"OK") : nullptr;
	}
	else if (!lstrcmpi(pszName, L"FarGotoEditorPath")
		|| !lstrcmpi(pszName, L"BackGround Image") || !lstrcmpi(pszName, L"bgImage")
		|| !lstrcmpi(pszName, L"Scheme"))
	{
		if (p->GetStrArg(1, pszValue))
			pszResult = gpSetCls->SetOption(pszName.c_str(), pszValue.c_str()) ? lstrdup(L"OK") : nullptr;
	}
	else if (!lstrcmpi(pszName, L"VConScheme"))
	{
		if (apRCon && p->GetStrArg(1, pszValue))
		{
			CVConGuard VCon(apRCon ? apRCon->VCon() : nullptr);
			pszResult = VCon->ChangePalette(pszValue.c_str()) ? lstrdup(L"OK") : nullptr;
		}
	}
	else
	{
		//TODO: More options on demand
		_ASSERTE(FALSE && "Option is not supported yet");
	}

	return pszResult ? pszResult : lstrdup(L"UnknownOption");
}

// SetFocus
CEStr ConEmuMacro::SetFocus(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int command = 0;
	if (!p->GetIntArg(0, command))
		command = 0;

	if ((command < 0) || static_cast<SwitchGuiFocusOp>(command) >= SwitchGuiFocusOp::Last)
		return lstrdup(L"InvalidArg");

	gpConEmu->OnSwitchGuiFocus(static_cast<SwitchGuiFocusOp>(command));

	return lstrdup(L"OK");
}

// SetParentHWND(<NewParentHWND>)
CEStr ConEmuMacro::SetParentHWND(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!gpConEmu->isInside())
		return lstrdup(L"InsideModeRequired");

	HWND2 hNewParent{0};
	if (p->IsIntArg(0))
	{
		int hwnd_int = 0;
		p->GetIntArg(0, hwnd_int);
		hNewParent.u = static_cast<DWORD>(hwnd_int);
	}

	gpConEmu->SetParent(hNewParent);
	gpConEmu->mp_Inside->InitInside(false, false, nullptr, 0, hNewParent);

	return lstrdup(L"OK");
}

// Settings Dialog
CEStr ConEmuMacro::Settings(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int IdShowPage = 0;
	if (p->IsIntArg(0))
		p->GetIntArg(0, IdShowPage);

	CSettings::Dialog(IdShowPage);

	return lstrdup(L"OK");
}

// ShellExecute
// <oper>,<app>[,<parm>[,<dir>[,<showcmd>]]]
CEStr ConEmuMacro::Shell(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = nullptr;
	CEStr pszOper = nullptr, pszFile = nullptr, pszParm = nullptr, pszDir = nullptr;
	CEStr pszBuf = nullptr;
	CEStr szRConCD;
	bool bDontQuote = false;
	int nShowCmd = SW_SHOWNORMAL;

	// Without any arguments - just starts default shell
	p->GetStrArg(0, pszOper);

	// And now, process the action
	{
		CVConGuard VCon(apRCon ? apRCon->VCon() : nullptr);

		if (p->GetStrArg(1, pszFile))
		{
			if (!p->GetStrArg(2, pszParm))
				pszParm = nullptr;
			else if (!p->GetStrArg(3, pszDir))
				pszDir = nullptr;
			else if (!p->GetIntArg(4, nShowCmd))
				nShowCmd = SW_SHOWNORMAL;
		}

		bool bNewConsoleVerb = (pszOper && (wmemcmp(pszOper, L"new_console", 11) == 0));
		bool bForceDuplicate = false;
		bool bDontDuplicate = false;
		if (bNewConsoleVerb)
		{
			RConStartArgsEx args; args.pszSpecialCmd = CEStr(L"\"-", pszOper, L"\"").Detach();
			args.ProcessNewConArg();
			// new_console:I
			bForceDuplicate = (args.ForceInherit == crb_On) && (apRCon != nullptr);
			// RunAs - does not supported in "duplicate"
			bDontDuplicate = ((args.RunAsAdministrator == crb_On) || (args.RunAsRestricted == crb_On) || args.pszUserName);
			_ASSERTE(!(bForceDuplicate && bDontDuplicate));
		}


		if (apRCon && (bForceDuplicate || (!(pszFile && *pszFile) && !(pszParm && *pszParm))))
		{
			LPCWSTR pszCmd;

			if (bNewConsoleVerb && !bDontDuplicate)
			{
				CEStr pszNewConsoleArgs(L"\"-", pszOper, L"\"");
				bool bOk = apRCon->DuplicateRoot(true/*bSkipMsg*/, false/*bRunAsAdmin*/, pszNewConsoleArgs, pszFile, pszParm);
				if (bOk)
				{
					pszRc = lstrdup(L"OK");
					goto wrap;
				}
				else
				{
					pszRc = lstrdup(L"VConCreateFailed");
					goto wrap;
				}
			}

			pszCmd = apRCon->GetCmd();
			if (pszCmd && *pszCmd)
			{
				pszBuf = lstrdup(pszCmd);
				pszFile = pszBuf;
				bDontQuote = true;
			}
		}

		if (!pszFile)
		{
			pszBuf = lstrdup(L"");
			pszFile = pszBuf;
		}

		if (!(pszFile && *pszFile) && !(pszParm && *pszParm))
		{
			struct Impl {
				static LRESULT CallNewConsoleDlg(LPARAM lParam)
				{
					return static_cast<LRESULT>(gpConEmu->RecreateAction(cra_CreateTab, true));
				};
			};
			pszRc = gpConEmu->CallMainThread(true, Impl::CallNewConsoleDlg, 0) ? lstrdup(L"CREATED") : lstrdup(L"FAILED");
		}
		else
		{

			bool bNewTaskGroup = false;

			if (*pszFile == CmdFilePrefix || *pszFile == TaskBracketLeft)
			{
				CEStr pszDataW = gpConEmu->LoadConsoleBatch(pszFile);
				if (pszDataW)
				{
					bNewTaskGroup = true;
				}
			}

			// 120830 - пусть shell("","cmd") запускает новую вкладку в ConEmu
			bool bNewOper = bNewTaskGroup || !pszOper || !*pszOper || (wmemcmp(pszOper, L"new_console", 11) == 0);

			if (bNewOper || (pszParm && wcsstr(pszParm, L"-new_console")))
			{
				size_t nAllLen;
				RConStartArgsEx args;

				// It's allowed now to append user arguments to named task contents
				{
					nAllLen = _tcslen(pszFile) + (pszParm ? _tcslen(pszParm) : 0) + 16;

					if (bNewOper)
					{
						size_t nOperLen = pszOper ? _tcslen(pszOper) : 0;
						if ((nOperLen > 11) && (pszOper[11] == L':'))
							nAllLen += (nOperLen + 6);
						else
							bNewOper = false;
					}

					args.pszSpecialCmd = (wchar_t*)malloc(nAllLen*sizeof(wchar_t));
					args.pszSpecialCmd[0] = 0;

					if (*pszFile)
					{
						if (!bDontQuote && (*pszFile != L'"') && (wcschr(pszFile, L' ') != nullptr))
						{
							args.pszSpecialCmd[0] = L'"';
							_wcscpy_c(args.pszSpecialCmd+1, nAllLen-1, pszFile);
							_wcscat_c(args.pszSpecialCmd, nAllLen, (pszParm && *pszParm) ? L"\" " : L"\"");
						}
						else
						{
							_wcscpy_c(args.pszSpecialCmd, nAllLen, pszFile);
							if (pszParm && *pszParm)
							{
								_wcscat_c(args.pszSpecialCmd, nAllLen, L" ");
							}
						}
					}

					if (pszParm && *pszParm)
					{
						_wcscat_c(args.pszSpecialCmd, nAllLen, pszParm);
					}

					// Параметры запуска консоли могли передать в первом аргуементе (например "new_console:b")
					if (bNewOper)
					{
						_wcscat_c(args.pszSpecialCmd, nAllLen, L" \"-");
						_wcscat_c(args.pszSpecialCmd, nAllLen, pszOper);
						_wcscat_c(args.pszSpecialCmd, nAllLen, L"\"");
					}
				}

				// Support CD from ACTIVE console
				if (pszDir && (lstrcmpi(pszDir, L"%CD%") == 0))
				{
					if (apRCon)
						apRCon->GetConsoleCurDir(szRConCD, true);
					if (szRConCD.IsEmpty())
						szRConCD.Set(gpConEmu->WorkDir());
					pszDir = szRConCD.ms_Val;
				}
				if (pszDir)
					args.pszStartupDir = lstrdup(pszDir).Detach();

				// Don't use PostCreateCon here, otherwise Context function will fail
				gpConEmu->CreateCon(args, true);

				pszRc = lstrdup(L"OK");
				goto wrap;
			}
			else
			{
				INT_PTR nRc = (INT_PTR)ShellExecuteW(ghWnd, pszOper, pszFile, pszParm, pszDir, nShowCmd);

				if (nRc <= 32)
				{
					switch (nRc)
					{
					case 0:
						pszRc = lstrdup(L"OUT_OF_MEMORY"); break;
					case ERROR_FILE_NOT_FOUND:
						pszRc = lstrdup(L"ERROR_FILE_NOT_FOUND"); break;
					case ERROR_PATH_NOT_FOUND:
						pszRc = lstrdup(L"ERROR_PATH_NOT_FOUND"); break;
					case ERROR_BAD_FORMAT:
						pszRc = lstrdup(L"ERROR_BAD_FORMAT"); break;
					case SE_ERR_ACCESSDENIED:
						pszRc = lstrdup(L"SE_ERR_ACCESSDENIED"); break;
					case SE_ERR_SHARE:
						pszRc = lstrdup(L"SE_ERR_SHARE"); break;
					default:
						{
							size_t cchSize = 16;
							if (pszRc.GetBuffer(cchSize))
								swprintf_c(pszRc.data(), pszRc.GetMaxCount(), L"Failed:%i", nRc);
						}
					}
				}
				else
				{
					pszRc = lstrdup(L"OK");
				}

				goto wrap;
			}
		}
	}

wrap:
	return pszRc ? pszRc : lstrdup(L"InvalidArg");
}

CEStr ConEmuMacro::Sleep(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszRc = nullptr;
	int ms = 0;
	wchar_t szStatus[32];

	// Max - 10sec to avoid hungs
	if (p->GetIntArg(0, ms) && (ms > 0) && (ms < 10000))
	{
		swprintf_c(szStatus, L"Sleeping %u ms", ms);
		if (apRCon) apRCon->SetConStatus(szStatus, CRealConsole::cso_Critical);
		DWORD dwStartTick = GetTickCount(), dwDelta, dwNow;
		MSG Msg;
		if (!isMainThread())
		{
			::Sleep(ms);
		}
		else
		{
			while (true)
			{
				::Sleep(std::min(ms,5));
				dwDelta = (dwNow = GetTickCount()) - dwStartTick;
				// Too long waiting may be noticed as "hangs"
				if (dwDelta < (DWORD)ms)
				{
					while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
					{
						if (!ProcessMessage(Msg))
						{
							return lstrdup(L"Terminated");
						}
						dwDelta = (dwNow = GetTickCount()) - dwStartTick;
						if (dwDelta >= (DWORD)ms)
							break;
					}
				}
				// Sleep more?
				if (dwDelta >= (DWORD)ms)
					break;
			}
		}
		if (apRCon) apRCon->SetConStatus(nullptr);
		pszRc = lstrdup(L"OK");
	}

	return pszRc ? pszRc : lstrdup(L"InvalidArg");
}

CEStr ConEmuMacro::Split(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult = nullptr;
	int nCmd = 0;
	int nHorz = 0;
	int nVert = 0;

	if (apRCon && p->GetIntArg(0, nCmd))
	{
		if (!p->GetIntArg(1, nHorz))
			nHorz = 0;
		if (!p->GetIntArg(2, nVert))
			nVert = 0;

		if (nCmd == 0)
		{
			// Аналог
			// Duplicate active «shell» split to right: Shell("new_console:sHn")
			// или
			// Duplicate active «shell» split to bottom: Shell("new_console:sVn")
			wchar_t szMacro[32] = L"";
			if (nHorz > 0 && nHorz < 100 && nVert == 0)
				swprintf_c(szMacro, L"Shell(\"new_console:s%iHn\")", nHorz);
			else if (nVert > 0 && nVert < 100 && nHorz == 0)
				swprintf_c(szMacro, L"Shell(\"new_console:s%iVn\")", nVert);

			if (szMacro[0])
				pszResult = ExecuteMacro(szMacro, apRCon);
		}
		else if (nCmd == 1)
		{
			// Move splitter between panes
			// Horizontal, Vertical, or both directions may be specified
			if (nHorz != 0 || nVert != 0)
			{
				CVConGroup::ReSizeSplitter(apRCon->VCon(), nHorz, nVert);
				pszResult = lstrdup(L"OK");
			}
		}
		else if (nCmd == 2)
		{
			// Put cursor to the nearest pane
			// You may choose preferred direction with Horz and Vert parameters
			pszResult = CVConGroup::ActivateNextPane(apRCon->VCon(), nHorz, nVert) ? lstrdup(L"OK") : lstrdup(L"Failed");
		}
		else if (nCmd == 3)
		{
			// Maximize/Restore active pane
			CVConGroup::PaneMaximizeRestore(apRCon->VCon());
		}
		else if (nCmd == 4)
		{
			// Exchange active with nearest pane using preferred direction defined by Horz and Vert parameters
			pszResult = CVConGroup::ExchangePanes(apRCon->VCon(), nHorz, nVert) ? lstrdup(L"OK") : lstrdup(L"Failed");
		}
	}

	return pszResult ? pszResult : lstrdup(L"InvalidArg");
}

// Status(0[,<Parm>]) - Show/Hide status bar, Parm=1 - Show, Parm=2 - Hide
// Status(1[,"<Text>"]) - Set status bar text
CEStr ConEmuMacro::Status(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult = nullptr;
	int nStatusCmd = 0;
	int nParm = 0;
	CEStr pszText = nullptr;

	if (!p->GetIntArg(0, nStatusCmd))
		nStatusCmd = 0;

	switch (nStatusCmd)
	{
	case csc_ShowHide:
		p->GetIntArg(1, nParm);
		gpConEmu->StatusCommand(csc_ShowHide, nParm);
		pszResult = lstrdup(L"OK");
		break;
	case csc_SetStatusText:
		if (apRCon)
		{
			p->GetStrArg(1, pszText);
			gpConEmu->StatusCommand(csc_SetStatusText, 0, pszText, apRCon);
			pszResult = lstrdup(L"OK");
		}
		break;
	}

	return pszResult ? pszResult : lstrdup(L"InvalidArg");
}

// TabControl
// <Cmd>[,<Parm>]
// Cmd: 0 - Show/Hide tabs
//		1 - commit lazy changes
//		2 - switch next (eq. CtrlTab)
//		3 - switch prev (eq. CtrlShiftTab)
//      4 - switch tab direct (no recent mode), parm=(1,-1)
//      5 - switch tab recent, parm=(1,-1)
//      6 - switch console direct (no recent mode), parm=(1,-1)
//      7 - activate console by number, parm=(one-based console index)
//		8 - show tabs list menu (indiffirent Far/Not Far)
// Returns "OK" on success
CEStr ConEmuMacro::Tab(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszResult = nullptr;

	int nTabCmd = 0;
	if (p->GetIntArg(0, nTabCmd))
	{
		int nParm = 0;
		p->GetIntArg(1, nParm);

		switch (nTabCmd)
		{
		case ctc_ShowHide:
		case ctc_SwitchCommit: // commit lazy changes
		case ctc_SwitchNext:
		case ctc_SwitchPrev:
			gpConEmu->TabCommand((ConEmuTabCommand)nTabCmd);
			pszResult = lstrdup(L"OK");
			break;
		case ctc_SwitchDirect: // switch tab direct (no recent mode), parm=(1,-1)
			if (nParm == 1)
			{
				gpConEmu->mp_TabBar->SwitchNext(gpSet->isTabRecent);
				pszResult = lstrdup(L"OK");
			}
			else if (nParm == -1)
			{
				gpConEmu->mp_TabBar->SwitchPrev(gpSet->isTabRecent);
				pszResult = lstrdup(L"OK");
			}
			break;
		case ctc_SwitchRecent: // switch tab recent, parm=(1,-1)
			if (nParm == 1)
			{
				gpConEmu->mp_TabBar->SwitchNext(!gpSet->isTabRecent);
				pszResult = lstrdup(L"OK");
			}
			else if (nParm == -1)
			{
				gpConEmu->mp_TabBar->SwitchPrev(!gpSet->isTabRecent);
				pszResult = lstrdup(L"OK");
			}
			break;
		case ctc_SwitchConsoleDirect: // switch console direct (no recent mode), parm=(1,-1)
			{
				int nActive = gpConEmu->ActiveConNum(); // 0-based
				if (nParm == 1)
				{
					// Прокрутка вперед (циклически)
					if (CVConGroup::isVConExists(nActive+1))
					{
						gpConEmu->ConActivate(nActive+1);
						pszResult = lstrdup(L"OK");
					}
					else if (nActive)
					{
						gpConEmu->ConActivate(0);
						pszResult = lstrdup(L"OK");
					}
				}
				else if (nParm == -1)
				{
					// Прокрутка назад (циклически)
					if (nActive > 0)
					{
						gpConEmu->ConActivate(nActive-1);
						pszResult = lstrdup(L"OK");
					}
					else
					{
						int nNew = gpConEmu->GetConCount()-1;
						if (nNew > nActive)
						{
							gpConEmu->ConActivate(nNew);
							pszResult = lstrdup(L"OK");
						}
					}
				}
			}
			break;
		case ctc_ActivateConsole: // activate console by number, parm=(one-based console index)
			if ((nParm >= 1) && CVConGroup::isVConExists(nParm-1))
			{
				gpConEmu->ConActivate(nParm-1);
				pszResult = lstrdup(L"OK");
			}
			break;
		case ctc_ActivateByName: // activate console by renamed title, console title, active process name, root process name
			if (p->IsStrArg(1))
			{
				CEStr pszName;
				if (p->GetStrArg(1, pszName))
				{
					if (gpConEmu->ConActivateByName(pszName))
						pszResult = lstrdup(L"OK");
					else
						pszResult = lstrdup(L"NotFound");
				}
			}
			break;
		case ctc_ShowTabsList: //
			CConEmuCtrl::key_ShowTabsList(NullChord, false, nullptr, nullptr/*чтобы не зависимо от фара показала меню*/);
			break;
		case ctc_CloseTab:
			if (apRCon)
			{
				apRCon->CloseTab();
				pszResult = lstrdup(L"OK");
			}
			break;
		case ctc_SwitchPaneDirect: // switch visible panes direct (no recent mode), parm=(1,-1), like ctc_SwitchConsoleDirect but for Splits
			if (apRCon && (nParm == 1 || nParm == -1))
			{
				bool bNext = (nParm == 1);
				if (CVConGroup::PaneActivateNext(bNext))
				{
					pszResult = lstrdup(L"OK");
				}
			}
			break;
		case ctc_GetTabsList: // return list of tabs in format "1: name1\n2: name2\n..."
			{
				CEStrConcat data;
				CVConGroup::EnumVCon(evf_All, [&data](CVirtualConsole* pVCon, LPARAM lParam){
					data.Append(pVCon->IndexStr());
					data.Append(L": ");
					data.Append(pVCon->RCon()->GetTitle());
					data.Append(L"\n");
					return true;
				}, 0);
				return data.GetData().Detach();
			}
			break;
		}
	}

	return pszResult ? pszResult : lstrdup(L"InvalidArg");
}

// Task(Index)
// Task("Name")
CEStr ConEmuMacro::Task(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	LPCWSTR pszResult = nullptr;
	CEStr pszName;
	int nTaskIndex = 0;

	if (p->argc < 1)
		return lstrdup(L"InvalidArg");

	if (p->IsStrArg(0))
	{
		CEStr pszArg = nullptr;
		if (p->GetStrArg(0, pszArg))
		{
			if (*pszArg && (*pszArg != TaskBracketLeft))
			{
				const wchar_t left[] = { TaskBracketLeft , 0 }, right[] = { TaskBracketRight , 0 };
				pszName = CEStr(left, pszArg, right);
			}
			else
			{
				pszName = std::move(pszArg);
			}
		}
	}
	else
	{
		if (p->GetIntArg(0, nTaskIndex) && (nTaskIndex > 0))
		{
			const CommandTasks* pTask = gpSet->CmdTaskGet(nTaskIndex - 1);
			if (pTask)
				pszName.Set(pTask->pszName);
		}
	}

	if (pszName && *pszName)
	{
		RConStartArgsEx args;
		args.pszSpecialCmd = lstrdup(pszName).Detach();

		CEStr pszDir = nullptr;
		if (p->GetStrArg(1, pszDir) && pszDir && *pszDir)
			args.pszStartupDir = lstrdup(pszDir).Detach();

		// Don't use PostCreateCon here, otherwise Context function will fail
		gpConEmu->CreateCon(args, true);

		pszResult = L"OK";
	}

	return pszResult ? lstrdup(pszResult) : lstrdup(L"InvalidArg");
}

// TaskAdd("Name","Commands"[,"GuiArgs"[,Flags]])
CEStr ConEmuMacro::TaskAdd(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	CEStr pszName;
	CEStr pszCommands = nullptr;
	CEStr pszGuiArgs = nullptr;
	CETASKFLAGS aFlags = CETF_DONT_CHANGE;
	int iVal;

	if (p->GetStrArg(0, pszName) && p->GetStrArg(1, pszCommands))
	{
		p->GetStrArg(2, pszGuiArgs);
		if (p->GetIntArg(3, iVal))
			aFlags = static_cast<CETASKFLAGS>(iVal & CETF_FLAGS_MASK);
	}
	else
	{
		return lstrdup(L"InvalidArg");
	}

	// Append new task
	const int nTaskIdx = gpSet->CmdTaskSet(-1, pszName, pszGuiArgs, pszCommands, aFlags|CETF_MAKE_UNIQUE);
	const CommandTasks* pGrp = gpSet->CmdTaskGet(nTaskIdx);
	if (pGrp && pGrp->pszName && *pGrp->pszName)
	{
		// Save tasks, if it's not a `-basic` mode
		if (!gpConEmu->IsResetBasicSettings())
		{
			gpSet->SaveCmdTasks(nullptr);
		}

		// Return created task name
		wchar_t szIndex[16] = L"";
		swprintf_c(szIndex, L"%i: ", nTaskIdx+1); // 1-based index
		return CEStr(szIndex, pGrp->pszName);
	}

	return lstrdup(L"Failed");
}

// Change or switch term mode
// TermMode(<Mode>[,<Action>])
CEStr ConEmuMacro::TermMode(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"NoActiveCon");

	int Mode, Action;
	if (!p->GetIntArg(0, Mode))
		return lstrdup(L"InvalidArg");
	if (!p->GetIntArg(1, Action))
		Action = 2;

	if (apRCon->StartStopTermMode((TermModeCommand)Mode, (ChangeTermAction)Action))
		return lstrdup(L"OK");

	return lstrdup(L"Failed");
}

// Change 'Highlight row/col' under mouse. Locally in current VCon.
CEStr ConEmuMacro::HighlightMouse(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"NoActiveCon");

	int nWhat, nSwitch;
	if (!p->GetIntArg(0, nWhat))
		return lstrdup(L"InvalidArg");

	if (!p->GetIntArg(1, nSwitch))
		nSwitch = 2;

	apRCon->VCon()->ChangeHighlightMouse(nWhat, nSwitch);
	return lstrdup(L"OK");
}

CEStr ConEmuMacro::Transparency(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	int nCmd, nValue;
	if (!p->GetIntArg(0, nCmd) || !(nCmd >= 0 && nCmd <= 4))
		return lstrdup(L"InvalidArg");
	if (!p->GetIntArg(1, nValue))
		return lstrdup(L"InvalidArg");

	return TransparencyHelper(nCmd, nValue);
}

CEStr ConEmuMacro::TransparencyHelper(int nCmd, int nValue)
{
	int oldValue, newValue;

	switch (nCmd)
	{
	case 0:
		// Absolute value, "AlphaValue" (active)
		oldValue = gpSet->nTransparent;
		gpSet->nTransparent = newValue = std::max(MIN_ALPHA_VALUE, std::min(MAX_ALPHA_VALUE, nValue));
		break;
	case 1:
		// Relative value, "AlphaValue" (active)
		oldValue = gpSet->nTransparent;
		gpSet->nTransparent = newValue = std::max(MIN_ALPHA_VALUE, std::min(MAX_ALPHA_VALUE, gpSet->nTransparent+nValue));
		break;
	case 2:
		// Absolute value, "AlphaValueInactive" (inactive)
		oldValue = gpSet->nTransparentInactive;
		gpSet->nTransparentInactive = newValue = std::max(MIN_INACTIVE_ALPHA_VALUE, std::min(MAX_ALPHA_VALUE, nValue));
		break;
	case 3:
		// Relative value, "AlphaValueInactive" (inactive)
		oldValue = gpSet->nTransparentInactive;
		gpSet->nTransparentInactive = newValue = std::max(MIN_INACTIVE_ALPHA_VALUE, std::min(MAX_ALPHA_VALUE, gpSet->nTransparentInactive+nValue));
		break;
	case 4:
		// "AlphaValueSeparate"
		oldValue = gpSet->isTransparentSeparate;
		newValue = gpSet->isTransparentSeparate = (nValue != 0);
		break;
	default:
		return lstrdup(L"InvalidArg");
	}

	if (oldValue != newValue)
	{
		gpConEmu->OnTransparent();
	}

	return lstrdup(L"OK");
}

// Write all strings to console. Use carefully!
CEStr ConEmuMacro::Write(GuiMacro* p, CRealConsole* apRCon, bool abFromPlugin)
{
	if (!apRCon)
		return lstrdup(L"FAILED:NO_RCON");

	CEStr pszText = nullptr;
	if (!p->GetRestStrArgs(0, pszText))
		return lstrdup(L"FAILED:NO_TEXT");

	if (!apRCon->Write(pszText))
		return lstrdup(L"FAILED");

	return lstrdup(L"OK");
}
