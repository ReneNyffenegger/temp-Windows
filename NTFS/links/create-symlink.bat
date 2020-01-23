@echo off
rem
rem      Requires administrator privileges unless in Developer mode
rem

if exist dir-two\sym-linked-file.txt del dir-two\sym-linked-file.txt

mklink  dir-two\sym-linked-file.txt dir-one\file.txt
