@echo off

reg add HKCR\tq84-directory-desktop-ini-test

rem
rem Is this really necessary?
rem
reg add HKCR\tq84-directory-desktop-ini-test /v CanUseForDirectory /d ""

reg add HKCR\tq84-directory-desktop-ini-test\shell
reg add HKCR\tq84-directory-desktop-ini-test\shell\TQ84Verb
reg add HKCR\tq84-directory-desktop-ini-test\shell\TQ84Verb\command
reg add HKCR\tq84-directory-desktop-ini-test\shell\TQ84Verb\command /ve /d "%systemroot%\system32\notepad.exe %1\desktop.ini" /f

attrib +s +h Desktop.ini
