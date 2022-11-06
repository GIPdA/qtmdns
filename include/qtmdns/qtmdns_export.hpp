#pragma once

#include <QtCore/qglobal.h>

#if defined(BUILD_SHARED_LIBS)
#  if defined(QTMDNS_LIBRARY)
#    define QTMDNS_EXPORT Q_DECL_EXPORT
#  else
#    define QTMDNS_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QTMDNS_EXPORT
#endif
