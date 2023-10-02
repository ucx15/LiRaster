#setup
# Project Dir
#    └---> Obj
#    └---> Src
#    		└--> Include


# -------- BUILD SETTINGS --------
$C_FLAGS = "-Wall", "-Wextra", "-pedantic", "-std=c++20", "-s", "-O3"

$out_file = "LiRaster.exe"
$main_files = "Src/main.cpp"

$src_files =  "engine","vec", "color", "utils", "surface"
# $src_files = "engine", "surface"


$include_dir = "Src/Include/"
$stb_inc_dir = "Libs/STB/"


# -------- BUILD SCRIPT --------
if (!(Test-Path("./Out"))) {
	mkdir Out
}


# removing previous build
if (Test-Path ./$out_file) {
	Remove-Item $out_file
}


# Compiling
Write-Output "Building:"
foreach ($file in $src_files) {

	if ($file) {
		if ("Src/${file}.cpp" -notin $main_files) {
			Write-Output "    ${file}.cpp"

			if (Test-Path "Obj/${file}.o") {
				Remove-Item Obj/${file}.o
			}

			g++ $C_FLAGS  -I $include_dir -I $stb_inc_dir -o Obj/${file}.o -c Src/${file}.cpp
		}
	}
}



# Linking
$obj_files = Get-ChildItem -Path Obj/

Write-Output "Linking"
g++ $main_files $obj_files $C_FLAGS -o $out_file -I $include_dir 


# Running current build

if (Test-Path ./$out_file) {
	Write-Output "Build Successfully"
	Write-Output ""
	& ./$out_file
}
else {
	Write-Output "ERROR in Building!"
}
