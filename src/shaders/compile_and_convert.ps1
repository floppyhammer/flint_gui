Remove-Item -Path "generated" -Recurse

New-Item -Path "generated" -ItemType Directory

New-Variable -Name "GLSLC" -Visibility Public -Value "$env:VULKAN_SDK/Bin/glslc.exe"

# Compile shaders.
& $GLSLC blit.vert -o generated/blit_vert.spv
& $GLSLC blit.frag -o generated/blit_frag.spv

Copy-Item "blit.vert" "generated"
Copy-Item "blit.frag" "generated"

Set-Location "generated"

# Generate headers.
python ../convert_files_to_header.py vert
python ../convert_files_to_header.py frag
python ../convert_files_to_header.py spv

# Remove intermediate files.
Get-ChildItem -Recurse -File | Where { ($_.Extension -ne ".h") } | Remove-Item

# Wait for input.
Write-Host "All jobs finished."
$Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
