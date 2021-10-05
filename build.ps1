
 param (
    [switch]$optimize = $false
 )

$OutputDirectory = "bin"
$SourceFiles = "../main.c ../parser.c"
$OutputBinary = "main.exe"

if ((Test-Path -Path $OutputDirectory) -eq $false) {
    New-Item -ItemType "directory" -Path $OutputDirectory | Out-Null
}

Push-Location $OutputDirectory

if (Get-Command "cl.exe" -ErrorAction SilentlyContinue) { 
   Write-Host "Found MSVC."

    $CompilerFlags = "-Od -Zi"

    if ($optimize) {
        Write-Output "Optimize build enabled."
        $CompilerFlags = "-O2 -Zi"
    }

    cl -nologo -D_CRT_SECURE_NO_WARNINGS $SourceFiles.Split(" ") $CompilerFlags.Split(" ") -EHsc -Fe"$OutputBinary"
    Write-Output "Build Finished."
} elseif (Get-Command "clang" -ErrorAction SilentlyContinue) {
    Write-Host "Found CLANG."

    $CompilerFlags = "-Od -gcodeview"

    if ($optimize) {
        Write-Output "Optimize build enabled."
        $CompilerFlags = "-O2 -gcodeview"
    }

    clang -Wno-switch -Wno-pointer-sign -Wno-enum-conversion -D_CRT_SECURE_NO_WARNINGS $SourceFiles.Split(" ") $CompilerFlags.Split(" ") -o "$OutputBinary"
    Write-Output "Build Finished."
} elseif (Get-Command "gcc.exe" -ErrorAction SilentlyContinue) {
    Write-Output "Found GCC :pepeChrist:"
    $compilerFlags="-g"
    gcc $SourceFiles $CompilerFlags.Split(" ") -o "$OutputBinary"
    
} else {
    Write-Error "Compiler not found."
}

Pop-Location
