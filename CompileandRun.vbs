
exe_name = "myApp.exe "
Proj_src_folder = "C:\Portables\VisualStudioCode_Projects\wxwidgets_CMD"

wxWidgets_incl = " -I" & "C:\Portables\CodeBlocks20.03\wxWidgets-3.0.5\include"
wxWidgets_lib = " -I" & "C:\Portables\CodeBlocks20.03\wxWidgets-3.0.5\lib\gcc_lib\mswu"
wxWidgets_lib_dir = " -L" & "C:\Portables\CodeBlocks20.03\wxWidgets-3.0.5\lib\gcc_lib"
mingw_bin = "C:\Portables\CodeBlocks20.03\MingwDistrox64_GCC 9.2.0_Boost 1.71.0\MinGW\bin"
libs = " resource.res -s -mthreads  -lwxmsw30u -lwxpng -lwxjpeg -lwxtiff -lwxzlib -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lcomctl32 -lwsock32 -lodbc32 -mwindows"
windres_o = " -J rc -O coff -i "
winres_e  =" -o resource.res"

compile_cpp_opt = "g++.exe -Wall -std=gnu++17 -pipe -mthreads -D__GNUWIN32__ -D__WXMSW__ -DwxUSE_UNICODE -O2"

'check and SET mingw_bin 
Set wshShell = CreateObject( "WScript.Shell" )
arrEnvironments = Array( "USER")
ALL_PT = ""
For Each strEnv In arrEnvironments
	Set wshEnv = wshShell.Environment( strEnv )
	ALL_PT = wshEnv( "PATH" )
Next
Set wshEnv   = Nothing
Set wshShell = Nothing

If InStr(ALL_PT, mingw_bin ) = 0 Then
	Set objShell = CreateObject("WScript.Shell")
	Set objEnv = objShell.Environment("USER")
	'What we want to add
	PathToAdd = mingw_bin
	'Get the current value of Path
	oldSystemPath = objEnv("PATH")
	'Build the new Path
	newSystemPath = oldSystemPath & ";" & PathToAdd
	'Set the new Path
	objEnv("PATH") = newSystemPath
End If
'!check and SET mingw_bin 

Set objFSO = CreateObject("Scripting.FileSystemObject") 
objStartFolder = Proj_src_folder
Set objFolder = objFSO.GetFolder(objStartFolder)
Set colFiles = objFolder.Files

cmp_txt = "cd " & objStartFolder & vbNewLine
'resource file'
For Each objFile in colFiles 
    strFileName = objFile.Name
    If objFSO.GetExtensionName(strFileName) = "rc" Then 
	   cmp_txt = cmp_txt & "windres.exe " & wxWidgets_incl & wxWidgets_lib & windres_o &  objFile.path & winres_e & vbNewLine
    End If
Next
'cpp files'
For Each objFile in colFiles 
    strFileName = objFile.Name
    If objFSO.GetExtensionName(strFileName) = "cpp" Then 
		cmp_txt = cmp_txt & compile_cpp_opt & wxWidgets_incl & wxWidgets_lib & " -c " &  objFile.path & " -o " & strFileName & ".o"  & vbNewLine
       
    End If
Next

onjs = ""

'o files'
For Each objFile in colFiles 
    strFileName = objFile.Name
    If objFSO.GetExtensionName(strFileName) = "o" Then 
		onjs = onjs & strFileName & " " 
    End If
Next
'linker abs rub
cmp_txt = cmp_txt &  "g++.exe " & wxWidgets_lib_dir & " -o " & exe_name &  onjs &  libs & vbNewLine & exe_name & vbNewLine
Set FSO = CreateObject("Scripting.FileSystemObject")
Set File = FSO.CreateTextFile(objStartFolder&"\compile.bat",True)
File.Write cmp_txt
File.Close

CreateObject("WScript.Shell").Run objStartFolder&"\compile.bat"
