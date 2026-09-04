<#
.SYNOPSIS
    Install the Competitive Programming Problem Generation skill pack from GitHub.

.DESCRIPTION
    Targets:
      claude   -> ~/.claude/skills/cp-problem-generation                       (Claude Code, same machine)
      cursor   -> ~/.cursor/skills/competitive-programming-problem-generation  (Cursor, personal)
      project  -> ./.agents/skills/competitive-programming-problem-generation  (Cursor, one project)

    An existing clone is updated with `git pull` instead of being clobbered.

.EXAMPLE
    .\install.ps1                      # default target: claude
    .\install.ps1 -Target cursor
    .\install.ps1 -Target project
#>
[CmdletBinding()]
param(
    [ValidateSet('claude', 'cursor', 'project')]
    [string]$Target = 'claude'
)

$ErrorActionPreference = 'Stop'
$Repo   = 'Anhdung1478/Competitive-Programming-Problem-Generation'
$Branch = 'main'

switch ($Target) {
    'claude'  { $Dest = Join-Path $HOME '.claude\skills\cp-problem-generation' }
    'cursor'  { $Dest = Join-Path $HOME '.cursor\skills\competitive-programming-problem-generation' }
    'project' { $Dest = Join-Path (Get-Location) '.agents\skills\competitive-programming-problem-generation' }
}

if (Test-Path (Join-Path $Dest '.git')) {
    Write-Host "==> $Dest already exists; pulling latest"
    git -C $Dest pull --ff-only
    exit 0
}

New-Item -ItemType Directory -Force -Path (Split-Path $Dest) | Out-Null

if (Get-Command git -ErrorAction SilentlyContinue) {
    git clone --branch $Branch "https://github.com/$Repo.git" $Dest
}
else {
    Write-Warning 'git not found; downloading a zip instead'
    $zip = Join-Path $env:TEMP 'competitive-programming-problem-generation.zip'
    Invoke-WebRequest "https://codeload.github.com/$Repo/zip/refs/heads/$Branch" -OutFile $zip
    $tmp = Join-Path $env:TEMP ('cp-problem-generation-' + [guid]::NewGuid().ToString('N'))
    Expand-Archive $zip $tmp
    New-Item -ItemType Directory -Force -Path $Dest | Out-Null
    Copy-Item -Recurse -Force (Join-Path $tmp "$($Repo.Split('/')[-1])-$Branch\*") $Dest
    Remove-Item -Recurse -Force $zip, $tmp
}

Write-Host ''
Write-Host "Installed to: $Dest"
switch ($Target) {
    'claude'  { Write-Host 'Next: open Claude Code and run /reload-plugins (or start a new session).' }
    'cursor'  { Write-Host 'Next: restart Cursor so the new skills are picked up.' }
    'project' { Write-Host 'Next: reload or restart Cursor for this project.' }
}
