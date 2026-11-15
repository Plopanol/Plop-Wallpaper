#Requires -Version 7.0
[CmdletBinding(SupportsShouldProcess)]
param([switch] $Execute)

$ErrorActionPreference = 'Stop'
$root = Get-Item -LiteralPath (Join-Path $PSScriptRoot '..') -Force
$repoPath = $root.FullName
if ($root.Attributes -band [IO.FileAttributes]::ReparsePoint) {
    throw "La racine du dépôt est un lien : $repoPath"
}

$gitRoot = git -C $repoPath rev-parse --show-toplevel
if ($LASTEXITCODE -ne 0 -or [IO.Path]::GetFullPath($gitRoot) -ne $repoPath) {
    throw "Racine Git introuvable ou différente : $repoPath"
}

$directories = 'build', 'target', 'debug', 'release', 'logs', '.cache', '.vs', 'CMakeFiles'
$entries = Get-ChildItem -LiteralPath $repoPath -Force | Where-Object {
    ($_.PSIsContainer -and $_.Name -in $directories) -or
    (-not $_.PSIsContainer -and ($_.Extension -eq '.log' -or $_.Name -eq 'CMakeCache.txt'))
}
$removals = [Collections.Generic.List[IO.FileSystemInfo]]::new()
$localAssets = (Join-Path 'build' 'icon-edge-profile'), (Join-Path 'build' 'icon-render.png')

function Add-CleanupCandidate {
    param([IO.FileSystemInfo] $Item)

    $relative = [IO.Path]::GetRelativePath($repoPath, $Item.FullName)
    if (($Item.Attributes -band [IO.FileAttributes]::ReparsePoint) -or
        $Item.Name -in @('config.ini', '.git', 'data', 'livrable') -or
        $relative -in $localAssets) {
        Write-Host "Conservé : $relative"
        return $false
    }
    if ($tracked.Contains($relative)) {
        Write-Host "Suivi par Git, conservé : $relative"
        return $false
    }

    $start = $removals.Count
    $canRemove = $true
    if ($Item.PSIsContainer) {
        $children = Get-ChildItem -LiteralPath $Item.FullName -Force
        if ($children.Name -contains '.git') {
            Write-Host "Dépôt imbriqué conservé : $relative"
            return $false
        }
        foreach ($child in $children) {
            if (-not (Add-CleanupCandidate $child)) { $canRemove = $false }
        }
    }

    if ($canRemove) {
        # Collapse an entirely disposable tree into one deletion.
        $removals.RemoveRange($start, $removals.Count - $start)
        $removals.Add($Item)
    }
    return $canRemove
}

$tracked = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
$index = git -C $repoPath -c core.quotePath=false ls-files -z
if ($LASTEXITCODE -ne 0) { throw 'Impossible de lire les fichiers suivis par Git.' }
foreach ($path in ($index -join "`n").Split([char]0, [StringSplitOptions]::RemoveEmptyEntries)) {
    [void] $tracked.Add($path.Replace('/', [IO.Path]::DirectorySeparatorChar))
}

foreach ($entry in $entries) { [void] (Add-CleanupCandidate $entry) }
if ($removals.Count -eq 0) { Write-Host 'Aucun artefact à nettoyer.' }

foreach ($item in $removals) {
    $relative = [IO.Path]::GetRelativePath($repoPath, $item.FullName)
    if (-not $Execute) {
        Write-Host "À supprimer : $relative"
        continue
    }
    if ($PSCmdlet.ShouldProcess($item.FullName, 'Supprimer un artefact local')) {
        $resolved = (Resolve-Path -LiteralPath $item.FullName).ProviderPath
        if ($resolved -ne (Join-Path $repoPath $relative) -or
            -not $resolved.StartsWith($repoPath + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Cible hors du dépôt : $resolved"
        }
        Remove-Item -LiteralPath $resolved -Recurse:$item.PSIsContainer -Force
        Write-Host "Supprimé : $relative"
    }
}
