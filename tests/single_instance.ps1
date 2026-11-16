param(
    [Parameter(Mandatory)]
    [string] $Executable
)

$ErrorActionPreference = 'Stop'
$mutex = [System.Threading.Mutex]::new($false, 'Local\Plop-Wallpaper.SingleInstance')
try {
    $process = Start-Process -FilePath $Executable -PassThru -WindowStyle Hidden
    try {
        if (-not $process.WaitForExit(5000)) {
            throw 'Le deuxième lancement reste actif.'
        }
        if ($process.ExitCode -ne 0) {
            throw "Le deuxième lancement a renvoyé le code $($process.ExitCode)."
        }
    } finally {
        if (-not $process.HasExited) {
            $process.Kill()
            $process.WaitForExit()
        }
        $process.Dispose()
    }
} finally {
    $mutex.Dispose()
}
