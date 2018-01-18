'use strict';

const util = require('util');
const EventEmitter = require('events');

const Image = require('../core');


class JsImage extends EventEmitter {
	
	constructor() {
		
		super();
		
		this.emit = this.emit.bind(this);
		
		this._image = new Image(this);
		
	}
	
	
	get src() { return this._image.src; }
	set src(v) { this._image.src = v; }
	
	
	[util.inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Image { src: [${this.src}] }`
	}
	
}


module.exports = JsScene;
