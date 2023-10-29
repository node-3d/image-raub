declare module "image-raub" {
	namespace image {
		type TSize = Readonly<{ width: number; height: number }>;
		
		type TImageData = TSize & Readonly<{
			data: Buffer;
			noflip?: boolean;
		}>;
		
		type TEvent = {
			type: string;
			[key: string]: unknown;
		};
		
		type TEventCb<T extends TEvent> = (event: T) => (void | boolean);
		
		type EventEmitter = import('node:events').EventEmitter;
		
		/**
		 * Image
		 *
		 * It is similar to the web Image.
		 * Extends EventEmitter to provide event-handling.
		*/
		class ImageJs implements EventEmitter {
			constructor(src?: string | null);
			
			/** Is image fully loaded? */
			readonly complete: boolean;
			
			/** Raw image data */
			readonly data: TImageData;
			
			/** An Array, containing width and height. */
			readonly wh: [number, number];
			
			/** Image width. */
			readonly width: number;
			
			/** Image height. */
			readonly height: number;
			
			/** Alias for width. */
			readonly naturalWidth: number;
			
			/** Alias for height. */
			readonly naturalHeight: number;
			
			/** An Object, containing width and height. */
			readonly size: TSize;
			
			/** Image URI: local file / URL / data-uri */
			src: string;
			
			/**
			 * Similar to .on('error', cb).
			 *
			 * Setter adds a new callback. If passed `null`, removes ALL callbacks.
			 * Getter returns an Array of currently existing callbacks.
			*/
			onerror: null | undefined | TEventCb<TEvent> | ReadonlyArray<TEventCb<TEvent>>;
			
			/**
			 * Similar to .on('load', cb).
			 *
			 * Setter adds a new callback. If passed `null`, removes ALL callbacks.
			 * Getter returns an Array of currently existing callbacks.
			*/
			onload: null | undefined | TEventCb<TEvent> | ReadonlyArray<TEventCb<TEvent>>;
			
			/** Emit an event on behalf of this Image. */
			emit(name: string, event: TEvent): boolean;
			
			/**
			 * Add event listener.
			 *
			 * 'error' - something went wrong.
			 * 'load' - image has been loaded.
			*/
			on(name: string, cb: (event: TEvent) => (void | boolean)): this;
			
			/** Alias for emit. */
			dispatchEvent(event: TEvent): void;
			
			/** Alias for on. */
			addEventListener(name: string): void;
			
			/** Alias for removeListener. */
			removeEventListener(name: string): void;
			
			/** Save the image to a file. */
			save(dest: string): void;
			
			/** Draws other image onto this one.
			 * @see https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage
			*/
			drawImage(
				image: TImageData,
				sx: number,
				sy: number,
				sWidth: number,
				sHeight: number,
				dx: number,
				dy: number,
				dWidth: number,
				dHeight: number,
			): void;
			
			static fromPixels(width: number, height: number, bpp: number, pixels: Buffer): ImageJs;
			static loadAsync(src: string): Promise<ImageJs>;
			
			// ------ implements EventEmitter
			
			addListener(eventName: string | symbol, listener: (...args: any[]) => void): this;
			on(eventName: string | symbol, listener: (...args: any[]) => void): this;
			once(eventName: string | symbol, listener: (...args: any[]) => void): this;
			removeListener(eventName: string | symbol, listener: (...args: any[]) => void): this;
			off(eventName: string | symbol, listener: (...args: any[]) => void): this;
			removeAllListeners(event?: string | symbol): this;
			setMaxListeners(n: number): this;
			getMaxListeners(): number;
			listeners(eventName: string | symbol): Function[];
			rawListeners(eventName: string | symbol): Function[];
			emit(eventName: string | symbol, ...args: any[]): boolean;
			listenerCount(eventName: string | symbol, listener?: Function): number;
			prependListener(eventName: string | symbol, listener: (...args: any[]) => void): this;
			prependOnceListener(eventName: string | symbol, listener: (...args: any[]) => void): this;
			eventNames(): Array<string | symbol>;
		}
		
	}
	
	export = image.ImageJs;
}