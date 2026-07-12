param(
    [string]$BuildDir = "$PSScriptRoot\..\build-win",
    [string]$OutputDir = "$PSScriptRoot\..\dist-windows",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

$root = (Resolve-Path "$PSScriptRoot\..").Path
$buildPath = (Resolve-Path $BuildDir).Path
$outputPath = [System.IO.Path]::GetFullPath($OutputDir)

if (-not $outputPath.StartsWith($root, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputDir must stay inside the project folder: $root"
}

if (-not $SkipBuild) {
    cmake --build $buildPath --target XylarJava
}

if (Test-Path $outputPath) {
    Remove-Item -LiteralPath $outputPath -Recurse -Force
}
New-Item -ItemType Directory -Path $outputPath | Out-Null

$launcherExe = Join-Path $buildPath "XylarJava.exe"
if (-not (Test-Path $launcherExe)) {
    throw "Missing launcher executable: $launcherExe"
}
Copy-Item -LiteralPath $launcherExe -Destination $outputPath

Get-ChildItem -LiteralPath $buildPath -Filter "*.dll" -File | Copy-Item -Destination $outputPath
$distDllDir = Join-Path $buildPath "dist"
if (Test-Path $distDllDir) {
    Get-ChildItem -LiteralPath $distDllDir -Filter "*.dll" -File | Copy-Item -Destination $outputPath -Force
}

foreach ($dirName in @("platforms", "imageformats", "iconengines", "styles", "tls", "jars")) {
    $source = Join-Path $buildPath $dirName
    if (Test-Path $source) {
        Copy-Item -LiteralPath $source -Destination (Join-Path $outputPath $dirName) -Recurse
    }
}

foreach ($fileName in @("qt.conf", "qtlogging.ini")) {
    $source = Join-Path $buildPath $fileName
    if (Test-Path $source) {
        Copy-Item -LiteralPath $source -Destination $outputPath
    }
}

$authSettings = Join-Path $root "auth-settings.ini"
if (Test-Path $authSettings) {
    $authDestination = Join-Path $outputPath "auth-settings.ini"
    Copy-Item -LiteralPath $authSettings -Destination $authDestination
    (Get-Item -LiteralPath $authDestination).Attributes = "Hidden"
}

$externalExecutables = Get-ChildItem -LiteralPath $outputPath -Filter "*.exe" -File | Where-Object { $_.Name -ne "XylarJava.exe" }
if ($externalExecutables) {
    $names = ($externalExecutables | ForEach-Object { $_.Name }) -join ", "
    throw "Unexpected external executables in package: $names"
}

$zipPath = Join-Path $root "XylarJava-Windows-x64-portable.zip"
if (Test-Path $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -Path (Join-Path $outputPath "*") -DestinationPath $zipPath -Force

Write-Host "Windows package: $outputPath"
Write-Host "Windows zip: $zipPath"
