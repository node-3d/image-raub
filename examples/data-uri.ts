import fs from 'node:fs';

const dataUris = JSON.parse(
	fs.readFileSync(new URL('assets/data-uri.json', import.meta.url), 'utf8'),
) as readonly string[];

export const dataUri = dataUris[0] ?? '';
