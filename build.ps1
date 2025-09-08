#setup
# Project Dir
#    └---> Obj
#    └---> Src
#    		└--> Include


# -------- BUILD SETTINGS --------
$C_FLAGS = "-Wall", "-Wextra", "-pedantic", "-std=c++20", "-masm=intel"

$Optimization_flags = "-march=native"
# $Optimization_flags = "-O3", "-mavx512f", "-march=native"
# $Optimization_flags = "-ggdb", "-g3"

$LINKER_FLAGS = "-lmingw32", "-lSDL2main", "-lSDL2", "-s"


$buildAll = $false

$out_file = "LiRaster.exe"

$src_files = "main", "engine", "utils", "surface"

$intermediate_dir = "Intermediate/"

# Include Directories
$include_dir = "Src/Include/"
$sdl_inc_dir = "Libs/SDL2/include/SDL2"
$stb_inc_dir = "Libs/"

# Library Directories
$sdl_lib_dir = "Libs/SDL2/lib"


# -------- BUILD SCRIPT --------
$C_FLAGS += $Optimization_flags

if (!(Test-Path("./Out"))) {
	mkdir Out
}

if (!(Test-Path($intermediate_dir))) {
	mkdir $intermediate_dir
}

# removing previous build
if (Test-Path ./$out_file) {
	Remove-Item $out_file
}


# Compiling
Write-Output "Building:"
foreach ($file in $src_files) {
	if ($file) {

		$modifyCondition = $false

		if (Test-Path "$intermediate_dir/${file}.o") {
			$targetModifiedDate = (Get-Item "$intermediate_dir/${file}.o").LastWriteTime

			# C++ Impl.
			$sourceModifiedDate = (Get-Item "Src/${file}.cpp").LastWriteTime

			# Header if exists
			if (Test-Path "Src/Include/${file}.hpp") {
				$headerModifiedDate = (Get-Item "Src/Include/${file}.hpp").LastWriteTime
				$modifyCondition = $headerModifiedDate -gt $targetModifiedDate
			}

			$modifyCondition = $modifyCondition -or ($sourceModifiedDate -gt $targetModifiedDate)
		}

		else {
			modifyCondition = $true
		}

		if (($modifyCondition -eq $true) -or ($buildAll -eq $true)) {
			Write-Output "    ${file}.cpp"

			if (Test-Path "$intermediate_dir/${file}.o") {
				Remove-Item "$intermediate_dir/${file}.o"
			}

			g++ $C_FLAGS -I $include_dir -I $stb_inc_dir -I $sdl_inc_dir -o "$intermediate_dir/${file}.o" -c "Src/${file}.cpp"

		}
	}
}



# Linking
$obj_files = Get-ChildItem -Path $intermediate_dir

Write-Output "Linking"
g++ $obj_files $C_FLAGS $LINKER_FLAGS -L $sdl_lib_dir $sdl_linkables -o $out_file


# Running current build

if (Test-Path ./$out_file) {
	Write-Output "Build Successfully"
	Write-Output ""
	./LiRaster
}

else {
	Write-Output "ERROR in Building!"
}
