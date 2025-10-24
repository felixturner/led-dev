# ESP32 Trinity COM Port Detector with Serial Test
# This script finds the COM port and tests if the ESP32 is responding

Write-Host "Searching for ESP32 Trinity..." -ForegroundColor Cyan
Write-Host ""

# Get all serial ports
$allPorts = [System.IO.Ports.SerialPort]::getportnames()

if ($allPorts.Count -eq 0) {
    Write-Host "No COM ports found!" -ForegroundColor Red
    Write-Host "Make sure the ESP32 is plugged in via USB." -ForegroundColor Red
    Write-Host ""
    Write-Host "Press any key to exit..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit
}

Write-Host "Testing COM ports for ESP32..." -ForegroundColor Green
Write-Host "==============================" -ForegroundColor Green
Write-Host ""

$esp32Port = $null

foreach ($portName in $allPorts) {
    Write-Host "Testing $portName..." -ForegroundColor Cyan

    # Get detailed port info from WMI
    $portInfo = Get-WmiObject Win32_SerialPort | Where-Object { $_.DeviceID -eq $portName }

    if ($portInfo) {
        Write-Host "  Description: $($portInfo.Description)" -ForegroundColor Gray
        Write-Host "  Status: $($portInfo.Status)" -ForegroundColor Gray
        $description = $portInfo.Description
    } else {
        # WMI didn't find it, try to get info from Device Manager registry
        $description = "Unknown"
        Write-Host "  Description: Unknown (not in WMI)" -ForegroundColor Gray
    }

    # Skip COM1 (usually system port)
    if ($portName -eq "COM1") {
        Write-Host "  (Skipping - likely system port)" -ForegroundColor Gray
        Write-Host ""
        continue
    }

    # Check if it's a CP210x or CH340 (ESP32 boards use these)
    # OR if description is unknown (CH340 sometimes doesn't show in WMI)
    if ($description -like "*CP210x*" -or $description -like "*CH340*" -or $description -eq "Unknown") {
        if ($description -like "*CP210x*") {
            $chipType = "CP210x"
        } elseif ($description -like "*CH340*") {
            $chipType = "CH340"
        } else {
            $chipType = "Unknown (possibly CH340)"
        }

        Write-Host "  >> This might be a $chipType device" -ForegroundColor Yellow

        # Try to open the port to test if it's accessible
        try {
            $port = New-Object System.IO.Ports.SerialPort $portName, 115200
            $port.ReadTimeout = 1000
            $port.Open()

            Write-Host "  >> Port is ACCESSIBLE and ready!" -ForegroundColor Green
            Write-Host "  >> THIS IS LIKELY YOUR ESP32 TRINITY!" -ForegroundColor Yellow
            $esp32Port = $portName

            $port.Close()
            $port.Dispose()
        }
        catch {
            Write-Host "  >> Port is BUSY or LOCKED" -ForegroundColor Red
            Write-Host "     Error: $($_.Exception.Message)" -ForegroundColor Red
        }
    }
    Write-Host ""
}

Write-Host ""
Write-Host "===== SUMMARY =====" -ForegroundColor Cyan

if ($esp32Port) {
    Write-Host ""
    Write-Host "ESP32 Trinity found on: $esp32Port" -ForegroundColor Green
    Write-Host ""
    Write-Host "Update platformio.ini with:" -ForegroundColor Cyan
    Write-Host "  upload_port = $esp32Port" -ForegroundColor White
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "WARNING: Could not find accessible ESP32!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Possible issues:" -ForegroundColor Yellow
    Write-Host "  1. COM port is locked by another program" -ForegroundColor Yellow
    Write-Host "  2. Driver needs to be reinstalled" -ForegroundColor Yellow
    Write-Host "  3. USB cable is not connected" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Try the Device Manager fix in claude.md" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
