'use strict';

const { expect } = require('chai');

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
		expect(Image).to.be.a('function');
	});
	
	it('can be created', () => {
		expect(new Image()).to.be.an.instanceof(Image);
	});
	
	
	props.forEach(prop => {
		it(`#${prop} property exposed`, () => {
			expect(new Image()).to.have.property(prop);
		});
	});
	
	methods.forEach(method => {
		it(`#${method}() method exposed`, () => {
			expect(new Image()).to.respondTo(method);
		});
	});
	
	
	it('emits "load" for the early listener', async () => {
		
		const image = new Image();
		
		const loaded = await new Promise((res, rej) => {
			image.on('load', () => res(true));
			image.on('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		expect(loaded).to.be.true;
		
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
		
		expect(loaded).to.be.true;
		
	});
	
	
	it('early #addEventListener() calls back with the correct scope', async () => {
		
		const image = new Image();
		
		const that = await new Promise((res, rej) => {
			image.addEventListener('load', function () { res(this); });
			image.addEventListener('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		expect(that).to.be.equal(image);
		
	});
	
	
	it('late #addEventListener() calls back with the correct scope', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		const that = await new Promise((res, rej) => {
			image.addEventListener('load', function () { res(this); });
			image.addEventListener('error', rej);
		});
		
		expect(that).to.be.equal(image);
		
	});
	
	
	it('early #on() calls back with the correct scope', async () => {
		
		const image = new Image();
		
		const that = await new Promise((res, rej) => {
			image.on('load', function () { res(this); });
			image.on('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		expect(that).to.be.equal(image);
		
	});
	
	
	it('late #on() calls back with the correct scope', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		const that = await new Promise((res, rej) => {
			image.on('load', function () { res(this); });
			image.on('error', rej);
		});
		
		expect(that).to.be.equal(image);
		
	});
	
	
	it('early #once() calls back with the correct scope', async () => {
		
		const image = new Image();
		
		const that = await new Promise((res, rej) => {
			image.once('load', function () { res(this); });
			image.once('error', rej);
			image.src = `${__dirname}/freeimage.jpg`;
		});
		
		expect(that).to.be.equal(image);
		
	});
	
	
	it('late #once() calls back with the correct scope', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		const that = await new Promise((res, rej) => {
			image.once('load', function () { res(this); });
			image.once('error', rej);
		});
		
		expect(that).to.be.equal(image);
		
	});
	
	
	it('has accessible data', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		const data = await new Promise((res, rej) => {
			image.on('load', () => res(image.data));
			image.on('error', rej);
		});
		
		expect(data).to.exist;
		expect(data.length).to.be.equal(TEST_IMAGE_LENGTH);
		
	});
	
	
	it('has correct dimensions', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		await new Promise((res, rej) => {
			image.on('load', res);
			image.on('error', rej);
		});
		
		expect(image.width).to.be.equal(TEST_IMAGE_WIDTH);
		expect(image.naturalWidth).to.be.equal(TEST_IMAGE_WIDTH);
		
		expect(image.height).to.be.equal(TEST_IMAGE_HEIGHT);
		expect(image.naturalHeight).to.be.equal(TEST_IMAGE_HEIGHT);
		
	});
	
	
	it('has correct `complete` when empty', async () => {
		
		const image = new Image();
		
		expect(image.complete).to.be.equal(false);
		
	});
	
	
	it('has correct `complete` when loaded', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		await new Promise((res, rej) => {
			image.on('load', res);
			image.on('error', rej);
		});
		
		expect(image.complete).to.be.equal(true);
		
	});
	
	
	it('has correct `complete` after dropping `src`', async () => {
		
		const image = new Image();
		image.src = `${__dirname}/freeimage.jpg`;
		
		let status = '';
		image.on('load', () => status += image.complete);
		
		await new Promise((res, rej) => {
			image.once('load', res);
			image.once('error', rej);
		});
		
		expect(image.complete).to.be.equal(true);
		
		image.src = '';
		
		await new Promise(res => setTimeout(res, 10));
		
		expect(image.complete).to.be.equal(false);
		expect(status).to.be.equal('truefalse');
		
	});
	
	
	it('can draw a stretched image', async () => {
		
		const src = new Image();
		src.src = `${__dirname}/freeimage.jpg`;
		
		await new Promise((res, rej) => {
			src.once('load', res);
			src.once('error', rej);
		});
		
		const dest = new Image();
		dest.drawImage(src, 0, 0, src.width, src.height, 0, 0, TEST_STRETCH_WIDTH, TEST_STRETCH_HEIGHT);
		
		expect(dest.width).to.be.equal(TEST_STRETCH_WIDTH);
		expect(dest.height).to.be.equal(TEST_STRETCH_HEIGHT);
		
	});
	
});
