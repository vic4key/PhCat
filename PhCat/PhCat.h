#ifndef _PH_CAT_H
#define _PH_CAT_H

#include <phdk.h>
#include <Windows.h>

#include <CatEngine.h>
#pragma comment(lib, "CatEngine.lib")

#include "resource.h"

enum ePhCatMenu : ULONG
{
  ID_ABOUT = 1000,
  ID_TOTAL_RESOURCES,
  ID_TOTAL_RESOURCES_RECURSIVE
};

/* Declare Function Callback */

#define PHCAT_DECLARE_FUNCTION_CALLBACK(declName)\
VOID NTAPI Cb ## declName (__in_opt PVOID pParameter, __in_opt PVOID pContext)

PHCAT_DECLARE_FUNCTION_CALLBACK(PluginCallbackLoad);
PHCAT_DECLARE_FUNCTION_CALLBACK(PluginCallbackMenuItem);
PHCAT_DECLARE_FUNCTION_CALLBACK(GeneralCallbackMainWindowShowing);
PHCAT_DECLARE_FUNCTION_CALLBACK(GeneralCallbackProcessMenuInitializing);

/* Declare Global Variable */

#define PHCAT_DECL_NONE
#define PHCAT_DECL_EXTERN extern

#define PHCAT_DECLARE_GLOBAL_VARIABLES(declType)\
declType PPH_PLUGIN g_pPlugin;\
declType PH_CALLBACK_REGISTRATION g_PluginCallbackLoadRegistration;\
declType PH_CALLBACK_REGISTRATION g_PluginCallbackMenuItemRegistration;\
declType PH_CALLBACK_REGISTRATION g_GeneralCallbackMainWindowShowingRegistration;\
declType PH_CALLBACK_REGISTRATION g_GeneralCallbackProcessMenuInitializingRegistration;\
declType HMODULE g_Instance;

/* Register Plugin/General Callback */

#define PHCAT_REGISTER_PLUGIN_CALLBACK(Name)\
PhRegisterCallback(\
  PhGetPluginCallback(g_pPlugin, Name),\
  PPH_CALLBACK_FUNCTION(Cb ## Name),\
  NULL,\
  &g_ ## Name ## Registration\
);

#define PHCAT_REGISTER_GENERAL_CALLBACK(Name)\
PhRegisterCallback(\
  PhGetGeneralCallback(Name),\
  PPH_CALLBACK_FUNCTION(Cb ## Name),\
  NULL,\
  &g_ ## Name ## Registration\
);

/* Others */

PWSTR PhCatLoadRCString(UINT uID);

#endif // _PH_CAT_H
