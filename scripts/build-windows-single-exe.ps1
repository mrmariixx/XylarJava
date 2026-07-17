# Builds a single distributable XylarJava.exe (no sibling DLLs / helper EXEs).
# Runtime files are embedded and extracted to %LocalAppData%\XylarJava\app on first run.

param(
    [string]$BuildDir = "$PSScriptRoot\..\build-win",
    [switch]$SkipCmakeBuild
)

$ErrorActionPreference = "Stop"
$root = (Resolve-Path "$PSScriptRoot\..").Path
$buildPath = Join-Path $root "build-win"
$dist = Join-Path $root "dist-single"
$payload = Join-Path $dist "payload"
$zip = Join-Path $dist "payload.zip"
$packer = Join-Path $root "tools\SingleExePacker"
$finalDir = Join-Path $dist "XylarJava-Windows"
$env:PATH = "C:\msys64\clang64\bin;C:\msys64\usr\bin;" + $env:PATH

Get-Process XylarJava -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Seconds 1

if (-not $SkipCmakeBuild) {
    cmake --build $buildPath --parallel --target XylarJava
}
cmake --install $buildPath --component Runtime | Out-Null
cmake --install $buildPath --component bundle | Out-Null

if (Test-Path $dist) { Remove-Item $dist -Recurse -Force }
New-Item -ItemType Directory -Path $payload | Out-Null

Copy-Item (Join-Path $buildPath "XylarJava.exe") $payload
Get-ChildItem (Join-Path $buildPath "*.dll") -File | Copy-Item -Destination $payload
foreach ($d in @("platforms","imageformats","iconengines","styles","tls","jars","themes")) {
    $src = Join-Path $buildPath $d
    if (Test-Path $src) { Copy-Item $src (Join-Path $payload $d) -Recurse }
}
foreach ($f in @("qt.conf","qtlogging.ini","auth-settings.ini","portable.txt")) {
    $src = Join-Path $buildPath $f
    if (-not (Test-Path $src)) { $src = Join-Path $root $f }
    if (Test-Path $src) { Copy-Item $src $payload }
}
if (-not (Test-Path (Join-Path $payload "portable.txt"))) {
    Set-Content (Join-Path $payload "portable.txt") "portable" -NoNewline
}
Get-ChildItem $payload -Filter "*.exe" -Recurse | Where-Object { $_.Name -ne "XylarJava.exe" } | Remove-Item -Force

& "C:\Program Files\7-Zip\7z.exe" a -tzip -mx=7 $zip "$payload\*" | Out-Null
Copy-Item $zip (Join-Path $packer "payload.zip") -Force

$publish = Join-Path $dist "publish"
dotnet publish $packer -c Release -r win-x64 --self-contained true `
    -p:PublishSingleFile=true `
    -p:IncludeNativeLibrariesForSelfExtract=true `
    -p:EnableCompressionInSingleFile=true `
    -o $publish

New-Item -ItemType Directory -Path $finalDir | Out-Null
Copy-Item (Join-Path $publish "XylarJava.exe") (Join-Path $finalDir "XylarJava.exe")
Write-Host "Single EXE: $(Join-Path $finalDir 'XylarJava.exe')"
Get-Item (Join-Path $finalDir "XylarJava.exe") | Format-List FullName, Length
