New-Variable -Name "GLSLC" -Visibility Public -Value "$env:VULKAN_SDK/Bin/glslc.exe"

& $GLSLC mesh2d.vert -o mesh2d_vert.spv
& $GLSLC mesh2d.frag -o mesh2d_frag.spv
& $GLSLC mesh3d.vert -o mesh3d_vert.spv
& $GLSLC mesh3d.frag -o mesh3d_frag.spv
& $GLSLC skeleton_2d.vert -o skeleton_2d_vert.spv
& $GLSLC skeleton_2d.frag -o skeleton_2d_frag.spv
& $GLSLC skybox.vert -o skybox_vert.spv
& $GLSLC skybox.frag -o skybox_frag.spv

# Wait for input.
Write-Host "All jobs finished."
$Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
