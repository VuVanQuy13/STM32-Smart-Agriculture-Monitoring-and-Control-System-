param([Parameter(Mandatory = $true)][string]$Path)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($env:STM32_BUILD_CC)) {
    throw 'Missing build configuration; invoke this script through Makefile.'
}
$configuration = [ordered]@{
    Compiler = $env:STM32_BUILD_CC
    CompileFlags = $env:STM32_BUILD_CFLAGS
    LinkFlags = $env:STM32_BUILD_LDFLAGS
    ObjectCopy = $env:STM32_BUILD_OBJCOPY
    Sources = $env:STM32_BUILD_SOURCES
} | ConvertTo-Json

$target = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Path)
if ((Test-Path -LiteralPath $target) -and
    [System.IO.File]::ReadAllText($target) -ceq $configuration) {
    return
}
[System.IO.Directory]::CreateDirectory([System.IO.Path]::GetDirectoryName($target)) | Out-Null
[System.IO.File]::WriteAllText($target, $configuration)
