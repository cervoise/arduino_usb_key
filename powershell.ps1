# --- Configuration ---
$portName = "COM3"
$baudRate = 9600
$fileName = "image.png" 
$filePath = "C:\path\to\your\file.png"

$port = New-Object System.IO.Ports.SerialPort $portName, $baudRate, None, 8, one
$port.Open()
Start-Sleep -Seconds 2 # Wait for Arduino reset

try {
    # 1. Send Filename
    $port.WriteLine("START_FILE:$fileName")
    Start-Sleep -Milliseconds 500

    # 2. Convert File to Base64
    Write-Host "Encoding file to Base64..." -ForegroundColor Cyan
    $fileBytes = [System.IO.File]::ReadAllBytes($filePath)
    $base64Content = [Convert]::ToBase64String($fileBytes)

    # 3. Send Data Header
    $port.WriteLine("START_DATA:")
    
    # 4. Send Base64 in chunks (to avoid overloading Arduino buffer)
    $chunkSize = 32 
    for ($i = 0; $i -lt $base64Content.Length; $i += $chunkSize) {
        $end = [Math]::Min($chunkSize, $base64Content.Length - $i)
        $chunk = $base64Content.Substring($i, $end)
        $port.Write($chunk) # Use .Write to keep it as a single continuous stream
        Start-Sleep -Milliseconds 20
    }
    
    # Send a newline to signify the end of the Base64 stream
    $port.WriteLine("") 

    # 5. Send End Command
    $port.WriteLine("END_FILE")
    Write-Host "Transfer Complete!" -ForegroundColor Green
}
finally {
    $port.Close()
}
