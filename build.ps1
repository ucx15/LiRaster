#setup
# Project Dir
#    └---> Obj
#    └---> Src
#    		└--> Include


# -------- BUILD SETTINGS --------
$C_FLAGS = "-Wall", "-Wextra", "-pedantic", "-std=c++20", "-masm=intel"

$Optimization_flags = "-O3", "-mavx512f", "-march=native"
# $Optimization_flags = "-ggdb", "-g3"

$LINKER_FLAGS = "-lmingw32", "-lSDL2main", "-lSDL2", "-s"


$buildAll = $true

$out_file = "LiRaster.exe"

$src_files = "main", "engine","vec", "color", "utils", "surface"


$include_dir = "Src/Include/"

$sdl_inc_dir = "Libs/SDL2/include/SDL2"
$stb_inc_dir = "Libs/"


$sdl_lib_dir = "Libs/SDL2/lib"


# -------- BUILD SCRIPT --------
$C_FLAGS += $Optimization_flags

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

		$modifyCondition = $true
		if (Test-Path "Asm/${file}.s") {
			$sourceModifiedDate = (Get-Item "Src/${file}.cpp").LastWriteTime
			$targetModifiedDate = (Get-Item "Asm/${file}.s").LastWriteTime
			$modifyCondition = ($sourceModifiedDate -gt $targetModifiedDate)
		}

		if (($modifyCondition -eq $true) -or ($buildAll -eq $true)) {
			Write-Output "    ${file}.cpp"
		
			if (Test-Path "Asm/${file}.s") {
				Remove-Item Asm/${file}.s
			}

			g++ $C_FLAGS -I $include_dir -I $stb_inc_dir -I $sdl_inc_dir -L $sdl_lib_dir $sdl_linkables -o Asm/${file}.s -S Src/${file}.cpp

		}
	}
}



# Linking
$obj_files = Get-ChildItem -Path Asm/

Write-Output "Linking"
g++ $obj_files $C_FLAGS $LINKER_FLAGS -o $out_file -L $sdl_lib_dir


# Running current build

if (Test-Path ./$out_file) {
	Write-Output "Build Successfully"
	Write-Output ""
	./LiRaster
}

else {
	Write-Output "ERROR in Building!"
}
