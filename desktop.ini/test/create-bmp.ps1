$bmp_16x16 = new-object System.Drawing.Bitmap 16, 16

$canvas = [Drawing.Graphics]::FromImage($bmp_16x16);
$brushRect = new-object Drawing.SolidBrush ([Drawing.Color]::FromArgb(255, 255, 0, 0));
$canvas.FillRectangle($brushRect, [Drawing.Rectangle]::FromLTRB(0, 0, 16, 16));

$bmp_16x16.Save("$(get-location)\16x16.bmp", [System.Drawing.Imaging.ImageFormat]::Bmp);
