@echo off

rmdir /s /q debug
rem rmdir /s /q release
del release\*.exe
del release\*.pdb
del release\*.ipdb
del release\*.iobj

attrib -r -s -h *.ncb
attrib -r -s -h *.suo
attrib -r -s -h *.db
del *.ncb
del *.suo
del *.db

attrib -r -s -h .vs
rmdir /s /q .vs

rmdir /s /q client\debug
rmdir /s /q client\release

rmdir /s /q server\debug
rmdir /s /q server\release

del client\*.user
del server\*.user