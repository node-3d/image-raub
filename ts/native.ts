import { createRequire } from 'node:module';
import { getBin } from '@node-3d/addon-tools';
import '@node-3d/segfault';
import '@node-3d/deps-freeimage';

export type TNativeImage = {
	readonly isDestroyed: boolean;
	readonly width: number;
	readonly height: number;
	destroy(): void;
	_load(data: Buffer, noflip?: boolean): void;
	_unload(): void;
	save(dest: string): boolean;
	drawImage(image: TNativeImage, dx: number, dy: number): void;
	drawImage(image: TNativeImage, dx: number, dy: number, dWidth: number, dHeight: number): void;
	drawImage(
		image: TNativeImage,
		sx: number,
		sy: number,
		sWidth: number,
		sHeight: number,
		dx: number,
		dy: number,
		dWidth: number,
		dHeight: number,
	): void;
};

type TNative = {
	Image: new () => TNativeImage;
};

const loadAddon = createRequire(import.meta.url);

export const native = loadAddon(`../${getBin()}/image.node`) as TNative;
