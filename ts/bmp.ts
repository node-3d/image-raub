const DIB_SIZE = 40;
const HEADER_SKIP = 14;
const HEADER_SIZE = HEADER_SKIP + DIB_SIZE;
const SOME_BYTES = 0x0ec4;

/**
 * Creates a Buffer filled according to BMP file format.
 *
 * As if read directly from a file. Can write it or feed into image loaders.
 * See https://en.wikipedia.org/wiki/BMP_file_format
 */
export const createBmpFromPixels = (
	width: number,
	height: number,
	bpp: number,
	pixels: Buffer,
): Buffer => {
	const memSize = width * height * Math.floor(bpp / 8);
	const bmpSize = HEADER_SIZE + memSize;
	const fakeBmp = Buffer.allocUnsafeSlow(bmpSize);
	let pos = 0;
	
	fakeBmp.write('BM', pos, 2, 'ascii');
	pos += 2;
	
	fakeBmp.writeUInt32LE(bmpSize, pos);
	pos += 4;
	
	pos += 4;
	
	fakeBmp.writeUInt32LE(HEADER_SIZE, pos);
	pos += 4;
	
	fakeBmp.writeUInt32LE(DIB_SIZE, pos);
	pos += 4;
	
	fakeBmp.writeInt32LE(width, pos);
	pos += 4;
	
	fakeBmp.writeInt32LE(height, pos);
	pos += 4;
	
	fakeBmp.writeUInt16LE(1, pos);
	pos += 2;
	
	fakeBmp.writeUInt16LE(bpp, pos);
	pos += 2;
	
	fakeBmp.writeUInt32LE(0, pos);
	pos += 4;
	
	fakeBmp.writeUInt32LE(memSize, pos);
	pos += 4;
	
	fakeBmp.writeUInt32LE(SOME_BYTES, pos);
	pos += 4;
	
	fakeBmp.writeUInt32LE(SOME_BYTES, pos);
	pos += 4;
	
	fakeBmp.writeUInt32LE(0, pos);
	pos += 4;
	
	fakeBmp.writeUInt32LE(0, pos);
	pos += 4;
	
	pixels.copy(fakeBmp, pos);
	
	return fakeBmp;
};
