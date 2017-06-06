// dllmain.cpp : Defines the entry point for the DLL application.

#ifdef _MSC_VER
  #pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include "PhCat.h"

PHCAT_DECLARE_GLOBAL_VARIABLES(PHCAT_DECL_NONE);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
  switch (ulReason)
  {
  case DLL_PROCESS_ATTACH:
    {
      g_Instance = hModule;

      auto s = PhCatLoadRCString(IDS_INTERNAL);

      PPH_PLUGIN_INFORMATION pInfo = nullptr;
      g_pPlugin = PhRegisterPlugin(s, hModule, &pInfo);
      if (g_pPlugin == nullptr) {
        return FALSE;
      }

      pInfo->DisplayName = PhCatLoadRCString(IDS_NAME);
      pInfo->Author      = PhCatLoadRCString(IDS_AUTHOR);
      pInfo->Url         = PhCatLoadRCString(IDS_WEBSITE);
      pInfo->Description = PhCatLoadRCString(IDS_DESCRIPTION);
      pInfo->HasOptions  = FALSE;

      PHCAT_REGISTER_PLUGIN_CALLBACK(PluginCallbackLoad);
      PHCAT_REGISTER_PLUGIN_CALLBACK(PluginCallbackMenuItem);
      PHCAT_REGISTER_GENERAL_CALLBACK(GeneralCallbackMainWindowShowing);
      PHCAT_REGISTER_GENERAL_CALLBACK(GeneralCallbackProcessMenuInitializing);
    }
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE;
}
