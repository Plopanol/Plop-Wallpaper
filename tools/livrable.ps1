#Requires -Version 7.0
$ErrorActionPreference = 'Stop'

Push-Location -LiteralPath (Join-Path $PSScriptRoot '..')
try {
    $version = (Get-Content -LiteralPath VERSION -Raw).Trim()
    if ($version -notmatch '^\d+\.\d+\.\d+$') {
        throw "Version invalide dans VERSION : $version"
    }
    $name = "Plop-Wallpaper-$version-windows-x64.zip"
    $output = Join-Path 'livrable' $name
    if ((Test-Path -LiteralPath $output) -or (Test-Path -LiteralPath "$output.sha256")) {
        throw "Un livrable existe déjà pour cette version : $output"
    }

    $build = 'build/portable'
    cmake -S . -B $build -G 'Visual Studio 18 2026' -A x64
    if ($LASTEXITCODE -ne 0) { throw 'La configuration CMake a échoué.' }
    cmake --build $build --config Release
    if ($LASTEXITCODE -ne 0) { throw 'La compilation Release a échoué.' }

    $stage = Join-Path $build ([Guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $stage | Out-Null
    $archive = Join-Path $stage $name
    try {
        $files = @("$build/Release/PlopWallpaper.exe", 'docs/config.example.ini', 'CHANGELOG.md')
        Compress-Archive -LiteralPath $files -DestinationPath $archive
        $hash = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
        Set-Content -LiteralPath "$archive.sha256" -Value "$hash  $name" -Encoding ascii

        & ./tools/commun/Verify-Livrable.ps1 -Path $archive -Project 'Plop-Wallpaper' -Version $version `
            -Expected 'PlopWallpaper.exe', 'config.example.ini', 'CHANGELOG.md' -NoNotices
        if ($LASTEXITCODE -ne 0) { throw 'La vérification du paquet a échoué.' }

        New-Item -ItemType Directory -Path 'livrable' -Force | Out-Null
        [IO.File]::Move((Convert-Path -LiteralPath $archive), (Join-Path $PWD.ProviderPath $output))
        [IO.File]::Move((Convert-Path -LiteralPath "$archive.sha256"), (Join-Path $PWD.ProviderPath "$output.sha256"))
    } finally {
        foreach ($file in @($archive, "$archive.sha256")) {
            if (Test-Path -LiteralPath $file) { Remove-Item -LiteralPath $file }
        }
        Remove-Item -LiteralPath $stage
    }
} finally {
    Pop-Location
}
