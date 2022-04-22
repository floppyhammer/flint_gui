C:/VulkanSDK/1.3.204.0/Bin/glslc.exe blit.vert -o blit_vert.spv
C:/VulkanSDK/1.3.204.0/Bin/glslc.exe blit.frag -o blit_frag.spv
C:/VulkanSDK/1.3.204.0/Bin/glslc.exe mesh_instance.vert -o mesh_instance_vert.spv
C:/VulkanSDK/1.3.204.0/Bin/glslc.exe mesh_instance.frag -o mesh_instance_frag.spv

# Wait for input.
Write-Host "All jobs finished."
$Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
