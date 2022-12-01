New-Variable -Name "GLSLC" -Visibility Public -Value "$env:VULKAN_SDK/Bin/glslc.exe"

& $GLSLC blit.vert -o blit_vert.spv
& $GLSLC blit.frag -o blit_frag.spv
& $GLSLC mesh_instance.vert -o mesh_instance_vert.spv
& $GLSLC mesh_instance.frag -o mesh_instance_frag.spv
& $GLSLC skeleton_2d.vert -o skeleton_2d_vert.spv
& $GLSLC skeleton_2d.frag -o skeleton_2d_frag.spv
& $GLSLC skybox.vert -o skybox_vert.spv
& $GLSLC skybox.frag -o skybox_frag.spv

# Wait for input.
Write-Host "All jobs finished."
$Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
