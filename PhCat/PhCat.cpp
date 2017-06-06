// PhCat.cpp : Defines the exported functions for the DLL application.

#include "PhCat.h"

PHCAT_DECLARE_GLOBAL_VARIABLES(PHCAT_DECL_EXTERN);

PWSTR PhCatLoadRCString(UINT uID)
{
  auto s = ce::ceLoadResourceStringW(uID, g_Instance);
  auto ps = new std::wstring(s);
  return (PWSTR)ps->c_str();
}

PPH_EMENU_ITEM PhCatCreateMenuContext(const _In_ PPH_EMENU_ITEM pMenuParent);
void PhCatUpdateMenuContext(const PPH_PLUGIN_MENU_INFORMATION pMenuInfo, const PPH_EMENU_ITEM pMenuContext);

VOID NTAPI CbPluginCallbackLoad(_In_opt_ PVOID pParameter, _In_opt_ PVOID pContext)
{
  ce::ceMsgA(__FUNCTION__);
}

VOID NTAPI CbPluginCallbackMenuItem(_In_opt_ PVOID pParameter, _In_opt_ PVOID pContext)
{
  ce::ceMsgA(__FUNCTION__);

  auto pMenuItem = static_cast<PPH_PLUGIN_MENU_ITEM>(pParameter);
  if (pMenuItem == nullptr) {
    return;
  }

  switch (pMenuItem->Id)
  {
  case ID_ABOUT:
    PhShowInformation(PhMainWndHandle, L"%s (c) %s", PhCatLoadRCString(IDS_NAME), PhCatLoadRCString(IDS_AUTHOR));
    break;
  default:;
  }
}

VOID NTAPI CbGeneralCallbackMainWindowShowing(__in_opt PVOID pParameter, __in_opt PVOID pContext)
{
  ce::ceMsgA(__FUNCTION__);
}

VOID NTAPI CbGeneralCallbackProcessMenuInitializing(_In_opt_ PVOID pParameter, _In_opt_ PVOID pContext)
{
  ce::ceMsgA(__FUNCTION__);

  auto pMenuInfo = static_cast<PPH_PLUGIN_MENU_INFORMATION>(pParameter);
  if (pMenuInfo == nullptr) {
    return;
  }

  auto pMenuContext = PhCatCreateMenuContext(pMenuInfo->Menu);
  if (pMenuContext == nullptr) {
    return;
  }

  PhCatUpdateMenuContext(pMenuInfo, pMenuContext);
}

PPH_EMENU_ITEM PhCatCreateMenuContext(const _In_ PPH_EMENU_ITEM pMenuParent)
{
  if (pMenuParent == nullptr) {
    return nullptr;
  }

  auto s = PhCatLoadRCString(IDS_NAME);
  auto pMenuContext = PhPluginCreateEMenuItem(g_pPlugin, 0, 0, s, nullptr);
  if (pMenuContext == nullptr) {
    return nullptr;
  }

  PPH_EMENU_ITEM pSubMenu = nullptr;

  s = PhCatLoadRCString(IDS_ABOUT);
  pSubMenu = PhPluginCreateEMenuItem(g_pPlugin, 0, ePhCatMenu::ID_ABOUT, s, nullptr);
  PhInsertEMenuItem(pMenuContext, pSubMenu, -1);
  
  auto pSeperatorMenu = PhPluginCreateEMenuItem(g_pPlugin, PH_EMENU_SEPARATOR, 0, nullptr, nullptr);
  auto pMenu = PhFindEMenuItem(pMenuParent, PH_EMENU_FIND_STARTSWITH, L"Window", 0);
  if (pMenu != nullptr) {
    auto iInsertIndex = PhIndexOfEMenuItem(pMenuParent, pMenu);
    PhInsertEMenuItem(pMenuParent, pSeperatorMenu, iInsertIndex + 1);
    PhInsertEMenuItem(pMenuParent, pMenuContext, iInsertIndex + 2);
  }
  else {
    PhInsertEMenuItem(pMenuParent, pSeperatorMenu, -1);
    PhInsertEMenuItem(pMenuParent, pMenuContext, -1);
  }

  return pMenuContext;
}

void PhCatUpdateMenuContext(const PPH_PLUGIN_MENU_INFORMATION pMenuInfo, const PPH_EMENU_ITEM pMenuContext)
{
  auto nProcesses = pMenuInfo->u.Process.NumberOfProcesses;
  if (nProcesses == 0) {
    pMenuContext->Flags |= PH_EMENU_DISABLED;
    return;
  }

  auto s = PhCatLoadRCString(IDS_ABOUT);
  auto pMenu = PhFindEMenuItem(pMenuContext, PH_EMENU_FIND_STARTSWITH, s, ePhCatMenu::ID_ABOUT);
  if (pMenu == nullptr) {
    return;
  }
}
