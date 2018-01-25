@echo off
@cls
@title ******小程序******
@color 2e
@echo ★ 清除VS打开记录！★
@pause
@REG Delete HKCU\Software\Microsoft\VisualStudio\9.0\FileMRUList /va /f
@REG Delete HKCU\Software\Microsoft\VisualStudio\9.0\ProjectMRUList /va /f
@echo vs2005为8.0,vs2008为9.0
@pause