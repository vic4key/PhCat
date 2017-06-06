// PhCat.cpp : Defines the exported functions for the DLL application.

#include "PhCat.h"

PHCAT_DECLARE_GLOBAL_VARIABLES(PHCAT_DECL_EXTERN);

class CTotalProcessesResources
{
public:
  typedef struct _TOTAL_RESOURCE
  {
    ULONGLONG PrivateUsage;
    ULONGLONG PeakPagefileUsage;
    ULONGLONG WorkingSetSize;
    ULONGLONG PeakWorkingSetSize;

    _TOTAL_RESOURCE() :
      PrivateUsage(0),
      PeakPagefileUsage(0),
      WorkingSetSize(0),
      PeakWorkingSetSize(0) {};

    _TOTAL_RESOURCE& operator+=(const PPH_PROCESS_ITEM pProcess)
    {
      this->PrivateUsage += pProcess->VmCounters.PrivateUsage;
      this->PeakPagefileUsage += pProcess->VmCounters.PeakPagefileUsage;
      this->WorkingSetSize += pProcess->VmCounters.WorkingSetSize;
      this->PeakWorkingSetSize += pProcess->VmCounters.PeakWorkingSetSize;
      return *this;
    }
  } TTotalResources;

  CTotalProcessesResources()
  {
    m_PIDs.clear();
  }

  const TTotalResources& operator()(const PPH_PROCESS_ITEM pProcess, const PPH_LIST pProcessList, bool recursive = true)
  {
    if (pProcess == nullptr)
    {
      return m_TotalResources;
    }

    auto IsPIDUsed = [this](const HANDLE PID)
    {
      return (std::find(m_PIDs.cbegin(), m_PIDs.cend(), PID) != m_PIDs.cend());
    };

    if (IsPIDUsed(pProcess->ProcessId))
    {
      return m_TotalResources;
    }

    m_TotalResources += pProcess;
    m_PIDs.push_back(pProcess->ProcessId);

    if (!recursive)
    {
      return m_TotalResources;
    }

    for (ULONG i = 0; i < pProcessList->Count; i++)
    {
      auto pItem = static_cast<PPH_PROCESS_ITEM>(pProcessList->Items[i]);
      if (
        (pItem != nullptr) &&
        (pItem->ProcessId != 0) &&
        (pItem->ParentProcessId == pProcess->ProcessId) &&
        (!IsPIDUsed(pItem->ProcessId))
      )
      {
        auto pParentProcess = PhReferenceProcessItemForParent(
          pItem->ParentProcessId,
          pItem->ProcessId,
          &pItem->CreateTime
        );
        if (pParentProcess != nullptr)
        {
          (*this)(pItem, pProcessList, recursive);
        }
      }
    }

    return m_TotalResources;
  }

private:
  TTotalResources m_TotalResources;
  std::vector<const HANDLE> m_PIDs;
};

#define PHCAT_FORMAT(o, m, d) auto s ## m = std::wstring(d) + L" : " + std::wstring(PhFormatSize(o.m, -1)->Buffer) + L"\n"

void PhCatTotalProcessesResources(bool recursive)
{
  ULONG nProcessItems;
  PPH_PROCESS_ITEM* pProcessItems;
  PhEnumProcessItems(&pProcessItems, &nProcessItems);

  auto pProcessList = PhCreateList(nProcessItems);
  PhAddItemsList(pProcessList, (PVOID*)pProcessItems, nProcessItems);

  CTotalProcessesResources totalProcessesResources;
  CTotalProcessesResources::TTotalResources totalResources;

  ULONG nProcesses;
  PPH_PROCESS_ITEM* pProcesses;
  PhGetSelectedProcessItems(&pProcesses, &nProcesses);
  for (ULONG i = 0; i < nProcesses; i++)
  {
    totalResources = totalProcessesResources(pProcesses[i], pProcessList, recursive);
  }

  PHCAT_FORMAT(totalResources, PrivateUsage, PhCatLoadRCString(IDS_TR_PRIVATE_BYTES));
  PHCAT_FORMAT(totalResources, PeakPagefileUsage, PhCatLoadRCString(IDS_TR_PEAK_PRIVATE_BYTES));
  PHCAT_FORMAT(totalResources, WorkingSetSize, PhCatLoadRCString(IDS_TR_WORKING_SET));
  PHCAT_FORMAT(totalResources, PeakWorkingSetSize, PhCatLoadRCString(IDS_TR_PEAK_WORKING_SET));

  std::wstring s = L"";
  s += sPrivateUsage;
  s += sPeakPagefileUsage;
  s += L"\n";
  s += sWorkingSetSize;
  s += sPeakWorkingSetSize;

  PhShowInformation(PhMainWndHandle, (PWSTR)s.c_str());

  PhFree(pProcessItems);
}

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
  ce::ceMsgA(__FUNCSIG__);
}

VOID NTAPI CbPluginCallbackMenuItem(_In_opt_ PVOID pParameter, _In_opt_ PVOID pContext)
{
  ce::ceMsgA(__FUNCSIG__);

  auto pMenuItem = static_cast<PPH_PLUGIN_MENU_ITEM>(pParameter);
  if (pMenuItem == nullptr) {
    return;
  }

  switch (pMenuItem->Id)
  {
  case ePhCatMenu::ID_TOTAL_RESOURCES:
    {
      PhCatTotalProcessesResources(false);
    }
    break;
  case ePhCatMenu::ID_TOTAL_RESOURCES_RECURSIVE:
    {
      PhCatTotalProcessesResources(true);
    }
    break;
  case ePhCatMenu::ID_ABOUT:
    PhShowInformation(
      PhMainWndHandle,
      L"%s (c) %s %s",
      PhCatLoadRCString(IDS_NAME),
      PhCatLoadRCString(IDS_AUTHOR),
      PhCatLoadRCString(IDS_YEAR)
    );
    break;
  default:;
  }
}

VOID NTAPI CbGeneralCallbackMainWindowShowing(__in_opt PVOID pParameter, __in_opt PVOID pContext)
{
  ce::ceMsgA(__FUNCSIG__);
}

VOID NTAPI CbGeneralCallbackProcessMenuInitializing(_In_opt_ PVOID pParameter, _In_opt_ PVOID pContext)
{
  ce::ceMsgA(__FUNCSIG__);

  auto pMenuInfo = static_cast<PPH_PLUGIN_MENU_INFORMATION>(pParameter);
  if (pMenuInfo == nullptr)
  {
    return;
  }

  auto pMenuContext = PhCatCreateMenuContext(pMenuInfo->Menu);
  if (pMenuContext == nullptr)
  {
    return;
  }

  PhCatUpdateMenuContext(pMenuInfo, pMenuContext);
}

PPH_EMENU_ITEM PhCatCreateMenuContext(const _In_ PPH_EMENU_ITEM pMenuParent)
{
  if (pMenuParent == nullptr)
  {
    return nullptr;
  }

  auto s = PhCatLoadRCString(IDS_NAME);
  auto pMenuContext = PhPluginCreateEMenuItem(g_pPlugin, 0, 0, s, nullptr);
  if (pMenuContext == nullptr)
  {
    return nullptr;
  }

  PPH_EMENU_ITEM pSubMenu = nullptr;

  s = PhCatLoadRCString(IDS_TOTAL_RESOURCES);
  pSubMenu = PhPluginCreateEMenuItem(g_pPlugin, 0, ePhCatMenu::ID_TOTAL_RESOURCES, s, nullptr);
  PhInsertEMenuItem(pMenuContext, pSubMenu, -1);

  s = PhCatLoadRCString(IDS_TOTAL_RESOURCES_RECURSIVE);
  pSubMenu = PhPluginCreateEMenuItem(g_pPlugin, 0, ePhCatMenu::ID_TOTAL_RESOURCES_RECURSIVE, s, nullptr);
  PhInsertEMenuItem(pMenuContext, pSubMenu, -1);

  s = PhCatLoadRCString(IDS_ABOUT);
  pSubMenu = PhPluginCreateEMenuItem(g_pPlugin, 0, ePhCatMenu::ID_ABOUT, s, nullptr);
  PhInsertEMenuItem(pMenuContext, pSubMenu, -1);

  auto pSeperatorMenu = PhPluginCreateEMenuItem(g_pPlugin, PH_EMENU_SEPARATOR, 0, nullptr, nullptr);
  auto pMenu = PhFindEMenuItem(pMenuParent, PH_EMENU_FIND_STARTSWITH, L"Window", 0);
  if (pMenu != nullptr)
  {
    auto iInsertIndex = PhIndexOfEMenuItem(pMenuParent, pMenu);
    PhInsertEMenuItem(pMenuParent, pSeperatorMenu, iInsertIndex + 1);
    PhInsertEMenuItem(pMenuParent, pMenuContext, iInsertIndex + 2);
  }
  else
  {
    PhInsertEMenuItem(pMenuParent, pSeperatorMenu, -1);
    PhInsertEMenuItem(pMenuParent, pMenuContext, -1);
  }

  return pMenuContext;
}

void PhCatUpdateMenuContext(const PPH_PLUGIN_MENU_INFORMATION pMenuInfo, const PPH_EMENU_ITEM pMenuContext)
{
  if (pMenuInfo->u.Process.NumberOfProcesses == 0)
  {
    pMenuContext->Flags |= PH_EMENU_DISABLED;
  }
}
