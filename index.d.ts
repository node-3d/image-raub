import { EventEmitter } from 'events';

declare module "image-raub" {
	namespace image {
		type TSize = Readonly<{ width: number; height: number }>;

		type TImage = TSize & Readonly<{
			data: Buffer;
			noflip?: boolean;
		}>;

		type TEvent = {
			type: string;
			[key: string]: unknown;
		};

		type TEventCb<T extends TEvent> = (event: T) => (void | boolean);
		
		/** Image
		 * It is similar to the web Image.
		 * Extends EventEmitter to provide event-handling.
		*/
		class Image extends EventEmitter {
			constructor(src?: string | null);
			
			/** Is image fully loaded? */
			readonly complete: boolean;
			
			/** Raw image data */
			readonly data: TImage;
			
			/** An Array, containing width and height. */
			readonly wh: [number, number];
			
			/** Alias for width. */
			readonly naturalWidth: number;
			
			/** Alias for height. */
			readonly naturalHeight: number;
			
			/** An Object, containing width and height. */
			readonly size: TSize;
			
			/** Image URI: local file / URL / data-uri */
			src: string;
			
			/** Alias for .on('error', cb).
			 * Setter adds a new callback.
			 * Getter returns an Array of currently existing callbacks.
			*/
			onerror: TEventCb<TEvent> | ReadonlyArray<TEventCb<TEvent>>;
			
			/** Alias for .on('load', cb).
			 * Setter adds a new callback.
			 * Getter returns an Array of currently existing callbacks.
			*/
			onload: TEventCb<TEvent> | ReadonlyArray<TEventCb<TEvent>>;
			
			/** Emit an event on behalf of this Image. */
			emit(name: string, event: TEvent): boolean;
			
			/** Add event listener.
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
				image: TImage,
				sx: number,
				sy: number,
				sWidth: number,
				sHeight: number,
				dx: number,
				dy: number,
				dWidth: number,
				dHeight: number,
			): void;
		}
		
	}
	
	export = image;
}