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
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		// Async barrier
		await new Promise((res, rej) => {
			image.on('load', () => res(true));
			image.on('error', rej);
		});
		
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
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
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
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
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
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		const that = await new Promise((res, rej) => {
			image.once('load', function () { res(this); });
			image.once('error', rej);
		});
		
		assert.strictEqual(that, image);
		
	});
	
	
	it('has accessible data', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		const data = await new Promise((res, rej) => {
			image.on('load', () => res(image.data));
			image.on('error', rej);
		});
		
		assert.ok(!!data);
		assert.strictEqual(data.length, TEST_IMAGE_LENGTH);
		
	});
	
	
	it('has correct dimensions', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		await new Promise((res, rej) => {
			image.on('load', res);
			image.on('error', rej);
		});
		
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
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		await new Promise((res, rej) => {
			image.on('load', res);
			image.on('error', rej);
		});
		
		assert.ok(image.complete);
		
	});
	
	const setSrc = (image, src) => { image.src = src; };
	it('has correct `complete` after dropping `src`', async () => {
		
		const image = new Image();
		setSrc(image, `${__dirname}/freeimage.jpg`);
		
		let status = '';
		image.on('load', () => status += image.complete);
		
		await new Promise((res, rej) => {
			image.once('load', res);
			image.once('error', rej);
		});
		
		assert.ok(image.complete);
		
		setSrc(image, '');
		
		await new Promise((res) => setTimeout(res, 10));
		
		assert.strictEqual(image.complete, false);
		assert.strictEqual(status, 'truefalse');
		
	});
	
	
	it('can draw a stretched image', async () => {
		
		const src = new Image();
		src.src = `${__dirname}/freeimage.jpg`;
		
		await new Promise((res, rej) => {
			src.once('load', res);
			src.once('error', rej);
		});
		
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
