# Image for Node.js

Image crossplatform addon with minimized dependencies.


## Install

```
npm i -s node-image-raub
```

Note: as this is a compiled addon, compilation tools must be in place on your system.
Such as MSVS13 for Windows, where `npm install --global windows-build-tools` most probably helps.


## Use

This module implements Image class suitable for loading FreeImage supported formats.

Example usage:

```js
const Image = require('node-image-raub');
const image = new Image();

image.onload = () => {
	gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
	gl.generateMipmap(gl.TEXTURE_2D);
	gl.bindTexture(gl.TEXTURE_2D, null);
};

image.src = `${__dirname}/texture.jpg`;
```

Here `Image` is used to load a texture. Constructed object receives src property,
then the file is read and `'load'` event emitted. After this `image.data` is
available as a Buffer, containing whole pixel data, and `image.width`/`image.height`
contain image dimensions.
