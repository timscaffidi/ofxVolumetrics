ofxVolumetrics
==============

Description
-----------

Render volumetric data on the GPU using GLSL and openFrameworks.

Written by Timothy Scaffidi (http://timothyscaffidi.com)
Volumetric rendering algorithm adapted from Peter Trier (http://www.daimi.au.dk/~trier/?page_id=98)
Single pass rendering method adapted from Phillip Rideout (http://prideout.net/blog/?p=64)

Usage
-----

~~copy or make symbolic links to the shaders in your app's bin/data/shaders folder.~~
Shader source is now embedded into the addon. No need for shader files now.

*setup:*
`void setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize)`;
`voxelSize` is the size of each voxel, W:H:D, used for stretching the volume, think pixel aspect ratio in video. If there is a data set that looks squashed with 1:1:1, try 1:1:2. `usePowerOfTwoTexSize` sets the cropped power-of-two texture mode. This mode will allocate a volume with power-of-two sized texture memory. For example, if you ask for 640x480x200, it will create a volume of size 1024x512x256, but only render a 640x480x200 voxel portion. This mode uses more memory but can greatly increase performance on some hardware. You may want to use this mode if resizing your data to power-of-two sizes is not possible.

*load with data:*
Create a voxel buffer `unsigned char array`. This must be RGBA! rendering voxels without an alpha channel defeats the point.

`void updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset);`
Upload the entire volume at once, or sub-portions of the volume using the x y and z offset amounts to change the size of upload.
Note that moving large amounts of data from system memory to GFX memory is quite slow! Updating the entire volume every frame will be very slow, only update what has changed if you want to do animation.

*Render:*
Perform any transformations as per usual, and draw the volume as you would any other 3d object. in this case, a box.
'void drawVolume(float x, float y, float z, float size, int zTexOffset);'
'zTexOffset' is used to slide the texture coordinates around on the z axis, which may be useful for animation by uploading one slice at a time.
