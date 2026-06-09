echo off
:INI
..\..\bossac.exe --port=COM3 -U true -e -w -v GEVINO_TFT_1.0_Test.ino.arduino_zero.bin -R

pause
cls
GOTO INI


