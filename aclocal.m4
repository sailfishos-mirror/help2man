dnl Local macros
dnl $Id: aclocal.m4,v 1.1 2000-07-07 05:41:47 bod Exp $

changequote()
define(RIGHT_BRACKET, ])
changequote([, ])

dnl AC_PROG_PERL([MIN-VERSION])
AC_DEFUN(AC_PROG_PERL,
[# find perl binary
AC_MSG_CHECKING([for perl])
AC_CACHE_VAL(ac_cv_prog_PERL,
[ifelse([$1],,,[echo "configure:__oline__: ...version $1 required" >&AC_FD_CC
  ])# allow user to override
  if test -n "$PERL"; then
    ac_try="$PERL"
  else
    ac_try="perl perl5"
  fi

  for ac_prog in $ac_try; do
    echo "configure:__oline__: trying $ac_prog" >&AC_FD_CC
    if ($ac_prog -e 'printf "found version %g\n",$]RIGHT_BRACKET[dnl
ifelse([$1],,,[;exit($]RIGHT_BRACKET[<$1)])') 1>&AC_FD_CC 2>&1; then
      ac_cv_prog_PERL=$ac_prog
      break
    fi
  done])dnl
PERL="$ac_cv_prog_PERL"
if test -n "$PERL"; then
  AC_MSG_RESULT($PERL)
else
  AC_MSG_RESULT(no)
fi
AC_SUBST(PERL)dnl
])dnl
