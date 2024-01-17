#setup
# Project Dir
#    └---> Obj
#    └---> Src
#    		└--> Include


# -------- BUILD SETTINGS --------
$C_FLAGS = "-Wall", "-Wextra", "-pedantic", "-std=c++20"

$out_file = "LiRaster.exe"
$main_files = "Src/main.cpp"

$src_files =  "engine","vec", "color", "utils", "surface"

$buildAll = $true
$buildDebug = $false


$include_dir = "Src/Include/"

$sdl_inc_dir = "Libs/SDL2/include/SDL2"
$stb_inc_dir = "Libs/"


$sdl_lib_dir = "Libs/SDL2/lib"

$sdl_linkables = "-lmingw32", "-lSDL2main", "-lSDL2"


# -------- BUILD SCRIPT --------

# Specifying a Debug or Release Build 
if ($buildDebug) {
	$C_FLAGS += "-g3", "-ggdb"	
} else {
	$C_FLAGS += "-s", "-O3"
}

if (!(Test-Path("./Out"))) {
	mkdir Out
}


if (!(Test-Path("./Asm"))) {
	mkdir Asm
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

			$modifyCondition = $true
			if (Test-Path "Asm/${file}.asm") {				
				$sourceModifiedDate = (Get-Item "Src/${file}.cpp").LastWriteTime
				$targetModifiedDate = (Get-Item "Asm/${file}.asm").LastWriteTime
				$modifyCondition = ($sourceModifiedDate -gt $targetModifiedDate)
			}

			if (($modifyCondition -eq $true) -or ($buildAll -eq $true)) {
				Write-Output "    ${file}.cpp"
			
				if (Test-Path "Asm/${file}.asm") {
					Remove-Item Asm/${file}.asm
				}

				g++ $C_FLAGS  -I $include_dir -I $stb_inc_dir -I $sdl_inc_dir -L $sdl_lib_dir $sdl_linkables -o Asm/${file}.s -S Src/${file}.cpp
			}

		}
	}
}



# Linking
$obj_files = Get-ChildItem -Path Asm/

Write-Output "Linking"
g++ $main_files $obj_files $C_FLAGS -o $out_file -I $include_dir -I $stb_inc_dir -I $sdl_inc_dir -L $sdl_lib_dir $sdl_linkables


# Running current build

if (Test-Path ./$out_file) {
	Write-Output "Build Successfully"
	Write-Output ""
	./LiRaster
}

else {
	Write-Output "ERROR in Building!"
}
