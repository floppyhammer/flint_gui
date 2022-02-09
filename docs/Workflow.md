# Vulkan workflow

## Set up command queues
* Allocate command buffers
* Fill it with commands

## Set up semaphores and fences for synchronization

## Main loop
* Wait for fences
* Acquire next image
* Wait for fences
* Update uniform buffer
* Reset fences
* Submit queue
* Present queue
