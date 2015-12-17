@echo off



set itemA=DGIndex_pf.exe
set itemB=DGIndex.pdb


set itemAA=DGIndex_pf.exe  debug
set itemBB=DGIndex.pdb  debug


set srcdir=.\DGIndex_pf\Debug\
::set dstdir=C:\A\DTV\EpgDataCap_Bon\Write\Write_PF_2test\DGIndex_pf\
set dstdir=C:\A\DTV\EDCB\Write\Write_PF_3debug\DGIndex_pf\


del  "%dstdir%%itemAA%" 
del  "%dstdir%%itemBB%"

copy  /y  "%srcdir%%itemA%"  "%dstdir%%itemA%" 
copy  /y  "%srcdir%%itemB%"  "%dstdir%%itemB%" 
copy  /y  "%srcdir%%itemA%"  "%dstdir%%itemAA%" 
copy  /y  "%srcdir%%itemB%"  "%dstdir%%itemBB%" 


::ping localhost -n 2 > nul
::pause
exit






