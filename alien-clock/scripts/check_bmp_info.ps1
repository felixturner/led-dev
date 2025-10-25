# BMP File Info Checker
# Shows dimensions, bit depth, and file size for BMP files

Write-Host "=== BMP File Analysis ===" -ForegroundColor Green
Write-Host ""

$bmpFiles = Get-ChildItem -Path "data\" -Filter "*.bmp"

if ($bmpFiles.Count -eq 0) {
    Write-Host "No BMP files found in data folder" -ForegroundColor Red
    exit
}

foreach ($file in $bmpFiles) {
    $bytes = [System.IO.File]::ReadAllBytes($file.FullName)

    # Check BMP signature
    if ($bytes[0] -ne 0x42 -or $bytes[1] -ne 0x4D) {
        Write-Host "Not a valid BMP: $($file.Name)" -ForegroundColor Red
        continue
    }

    # Read dimensions and bit depth
    $width = [BitConverter]::ToInt32($bytes, 18)
    $height = [BitConverter]::ToInt32($bytes, 22)
    $bitsPerPixel = [BitConverter]::ToInt16($bytes, 28)
    $fileSize = $file.Length

    Write-Host "File: $($file.Name)" -ForegroundColor Cyan
    Write-Host "  Dimensions: ${width}x${height}" -ForegroundColor White
    Write-Host "  Bit Depth: $bitsPerPixel-bit" -ForegroundColor White
    $sizeKB = [math]::Round($fileSize/1KB, 2)
    Write-Host "  File Size: $sizeKB KB" -ForegroundColor White

    # Check if optimal
    if ($width -eq 64 -and $height -eq 64 -and $bitsPerPixel -eq 24) {
        if ($fileSize -gt 15000) {
            Write-Host "  WARNING: Larger than expected! Should be ~12 KB" -ForegroundColor Yellow
        } else {
            Write-Host "  Status: Optimal size" -ForegroundColor Green
        }
    } elseif ($width -gt 64 -or $height -gt 64) {
        Write-Host "  WARNING: Larger than 64x64, will be cropped!" -ForegroundColor Yellow
    }
    Write-Host ""
}

$totalSize = ($bmpFiles | Measure-Object -Property Length -Sum).Sum
$totalKB = [math]::Round($totalSize/1KB, 2)
$remainingKB = [math]::Round((1500 - $totalSize/1KB), 2)

Write-Host "=== Summary ===" -ForegroundColor Green
Write-Host "Total Files: $($bmpFiles.Count)" -ForegroundColor White
Write-Host "Total Size: $totalKB KB" -ForegroundColor White
Write-Host "Available Space: ~1,500 KB" -ForegroundColor Cyan
Write-Host "Remaining: ~$remainingKB KB" -ForegroundColor Cyan
Write-Host ""
