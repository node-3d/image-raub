'use strict';

const { expect } = require('chai');

const Image = require('image-raub');


const props = [
	'complete','width','height','naturalWidth','naturalHeight',
	'wh','size','src','onerror','onload',
];
const methods = ['on','save'];


describe('Image', () => {
	
	it(`exports an object`, () => {
		expect(Image).to.be.a('function');
	});
	
	it(`can be created`, () => {
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
	
});
