dnl ----------------------------------------------------
dnl CHECK_WX_BUILT_WITH_GTK2
dnl check gtk version wx windows was compiled
dnl ----------------------------------------------------

AC_DEFUN([CHECK_WX_BUILT_WITH_GTK2],
[
  AC_MSG_CHECKING(if wxWindows was linked with GTK2)
  if $WX_CONFIG_NAME --cppflags | grep -q 'gtk2' ; then
     GTK_USEDVERSION=2
     AC_MSG_RESULT(yes)
  else
     AC_MSG_RESULT(no)
  fi

  AC_SUBST(GTK_USEDVERSION)
])