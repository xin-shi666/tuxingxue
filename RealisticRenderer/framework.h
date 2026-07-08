#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxdialogex.h>
#include <afxcontrolbars.h>
#include <afxcmn.h>
#include <gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <algorithm>
#include <cmath>

#undef min
#undef max
