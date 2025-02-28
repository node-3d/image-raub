'use strict';

const assert = require('node:assert').strict;
const { describe, it } = require('node:test');
const Image = require('..');


const TEST_IMAGE_WIDTH = 283;
const TEST_IMAGE_HEIGHT = 71;
const TEST_STRETCH_WIDTH = 512;
const TEST_STRETCH_HEIGHT = 128;
const TEST_IMAGE_LENGTH = 80372;

const props = [
	'complete', 'width', 'height', 'naturalWidth', 'naturalHeight',
	'wh', 'size', 'src', 'onerror', 'onload',
];

const methods = ['on', 'save', 'drawImage'];

const loadImageAsync = async (name) => {
	const image = new Image();
	image.src = `${__dirname}/${name}`;
	
	await new Promise((res, rej) => {
		image.on('load', () => res());
		image.on('error', rej);
	});
	
	return image;
};


describe('Image', () => {
	it('exports an object', () => {
		assert.strictEqual(typeof Image, 'function');
	});
	
	it('can be created', () => {
		assert.ok((new Image()) instanceof Image);
	});
	
	
	props.forEach((prop) => {
		const image = new Image();
		it(`#${prop} property exposed`, () => {
			assert.ok(image[prop] !== undefined);
		});
	});
	
	methods.forEach((method) => {
		const image = new Image();
		it(`#${method}() method exposed`, () => {
			assert.strictEqual(typeof image[method], 'function');
		});
	});
	
	
	it('emits "load" for the early listener', async () => {
		const image = new Image();
		
		const loaded = await new Promise((res, rej) => {
			image.on('load', () => res(true));
			image.on('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		assert.ok(loaded);
	});
	
	
	it('emits "load" for the late listener', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		const loaded = await new Promise((res, rej) => {
			image.on('load', () => res(true));
			image.on('error', rej);
		});
		
		assert.ok(loaded);
	});
	
	
	it('early #addEventListener() calls back with the correct scope', async () => {
		const image = new Image();
		
		const that = await new Promise((res, rej) => {
			image.addEventListener('load', function () { res(this); });
			image.addEventListener('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		assert.strictEqual(that, image);
	});
	
	
	it('late #addEventListener() calls back with the correct scope', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		const that = await new Promise((res, rej) => {
			image.addEventListener('load', function () { res(this); });
			image.addEventListener('error', rej);
		});
		
		assert.strictEqual(that, image);
	});
	
	
	it('early #on() calls back with the correct scope', async () => {
		const image = new Image();
		
		const that = await new Promise((res, rej) => {
			image.on('load', function () { res(this); });
			image.on('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		assert.strictEqual(that, image);
	});
	
	
	it('late #on() calls back with the correct scope', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		const that = await new Promise((res, rej) => {
			image.on('load', function () { res(this); });
			image.on('error', rej);
		});
		
		assert.strictEqual(that, image);
	});
	
	
	it('early #once() calls back with the correct scope', async () => {
		const image = new Image();
		
		const that = await new Promise((res, rej) => {
			image.once('load', function () { res(this); });
			image.once('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		assert.strictEqual(that, image);
	});
	
	
	it('late #once() calls back with the correct scope', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		const that = await new Promise((res, rej) => {
			image.once('load', function () { res(this); });
			image.once('error', rej);
		});
		
		assert.strictEqual(that, image);
	});
	
	it('has accessible JPG data', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		assert.ok(!!image.data);
		assert.strictEqual(image.data.length, TEST_IMAGE_LENGTH);
	});
	
	it('has accessible PNG data', async () => {
		const image = await loadImageAsync('freeimage.png');
		
		assert.ok(!!image.data);
		assert.strictEqual(image.data.length, TEST_IMAGE_LENGTH);
	});
	
	it('has accessible GIF data', async () => {
		const image = await loadImageAsync('freeimage.gif');
		
		assert.ok(!!image.data);
		assert.strictEqual(image.data.length, TEST_IMAGE_LENGTH);
	});
	
	
	it('has correct dimensions', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		assert.strictEqual(image.width, TEST_IMAGE_WIDTH);
		assert.strictEqual(image.naturalWidth, TEST_IMAGE_WIDTH);
		
		assert.strictEqual(image.height, TEST_IMAGE_HEIGHT);
		assert.strictEqual(image.naturalHeight, TEST_IMAGE_HEIGHT);
	});
	
	
	it('has correct `complete` when empty', async () => {
		const image = new Image();
		assert.strictEqual(image.complete, false);
	});
	
	
	it('has correct `complete` when loaded', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		assert.ok(image.complete);
	});
	
	
	it('has correct `complete` after dropping `src`', async () => {
		const image = await loadImageAsync('freeimage.jpg');
		
		let status = '';
		image.on('load', () => status += image.complete);
		
		assert.ok(image.complete);
		
		image.src = '';
		await new Promise((res) => setTimeout(res, 10));
		
		assert.ok(!image.complete);
		assert.strictEqual(status, 'truefalse');
	});
	
	
	it('can draw a stretched image', async () => {
		const src = await loadImageAsync('freeimage.jpg');
		
		const dest = new Image();
		dest.drawImage(
			src,
			0, 0, src.width, src.height,
			0, 0, TEST_STRETCH_WIDTH, TEST_STRETCH_HEIGHT
		);
		
		assert.strictEqual(dest.width, TEST_STRETCH_WIDTH);
		assert.strictEqual(dest.height, TEST_STRETCH_HEIGHT);
	});
});
