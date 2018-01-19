#include <cstdlib>
#include <vector>
#include <iostream>

#include "image.hpp"

using namespace v8;
using namespace node;
using namespace std;

#define THIS_IMAGE                                                            \
	Image *image = ObjectWrap::Unwrap<Image>(info.This());

#define USE_UINT32_ARG(I, VAR, DEF)                                           \
	CHECK_LET_ARG(I, IsUint32(), "uint32");                                   \
	unsigned int VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I]->Uint32Value();


static vector<Image*> images;


static void registerImage(Image *obj) {
	images.push_back(obj);
}


static void unregisterImage(Image* obj) {
	vector<Image*>::iterator it = images.begin();
	while (it != images.end()) {
		if (*it == obj) {
			images.erase(it);
			break;
		}
		it++;
	}
}


Nan::Persistent<v8::Function> Image::_constructor;


NAN_MODULE_INIT(Image::init) {
	
	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(newCtor);
	
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(JS_STR("Image"));
	
		// prototype
	Nan::SetPrototypeMethod(ctor, "save", save);
	Nan::SetPrototypeMethod(ctor, "load", load);
	
	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
	ACCESSOR_R(proto, width);
	ACCESSOR_R(proto, height);
	
	_constructor.Reset(Nan::GetFunction(ctor).ToLocalChecked());
	Nan::Set(target, JS_STR("Image"), Nan::GetFunction(ctor).ToLocalChecked());
	
	FreeImage_Initialise(true);
	
}


void Image::_emit(int argc, Local<Value> argv[]) {
	
	if ( ! Nan::New(_emitter)->Has(JS_STR("emit")) ) {
		return;
	}
	
	Nan::Callback callback(Nan::New(_emitter)->Get(JS_STR("emit")).As<Function>());
	
	if ( ! callback.IsEmpty() ) {
		callback.Call(argc, argv);
	}
	
}


NAN_METHOD(Image::newCtor) {
	
	CTOR_CHECK("Image");
	
	REQ_OBJ_ARG(0, emitter);
	
	Image *image = new Image();
	image->_emitter.Reset(emitter);
	
	image->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


NAN_GETTER(Image::widthGetter) { THIS_IMAGE;
	
	RET_VALUE(JS_INT(FreeImage_GetWidth(image->_bitmap)));
	
}


NAN_GETTER(Image::heightGetter) { THIS_IMAGE;
	
	RET_VALUE(JS_INT(FreeImage_GetHeight(image->_bitmap)));
	
}


NAN_METHOD(Image::load) { THIS_IMAGE;
	
	REQ_UTF8_ARG(0, src);
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(*src, 0);
	FIBITMAP *tmp = FreeImage_Load(format, *src, 0);
	image->_bitmap = FreeImage_ConvertTo32Bits(tmp);
	FreeImage_Unload(tmp);
	
	// adjust internal fields
	size_t num_pixels = FreeImage_GetWidth(image->_bitmap) * FreeImage_GetHeight(image->_bitmap);
	BYTE *pixels = FreeImage_GetBits(image->_bitmap);
	size_t num_bytes = num_pixels * 4;
	
	// FreeImage stores data in BGR
	// Convert from BGR to RGB
	for (size_t i = 0; i < num_pixels; i++) {
		size_t i4 = i << 2;
		BYTE temp = pixels[i4 + 0];
		pixels[i4 + 0] = pixels[i4 + 2];
		pixels[i4 + 2] = temp;
	}
	
	Nan::MaybeLocal<v8::Object> buffer = Nan::NewBuffer((int)num_bytes);
	
	std::memcpy(node::Buffer::Data(buffer.ToLocalChecked()), pixels, (int)num_bytes);
	
	Nan::Set(info.This(), JS_STR("data"), buffer.ToLocalChecked());
	
	Local<Value> argv[2] = { JS_STR("load"), info[0] };
	image->_emit(2, argv);
	
}


NAN_METHOD(Image::save) { THIS_IMAGE;
	
	REQ_UTF8_ARG(0, dest)
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*dest);
	
	void *buffer = node::Buffer::Data(info[1]);
	
	REQ_UINT32_ARG(2, width);
	REQ_UINT32_ARG(3, height);
	
	USE_UINT32_ARG(4, pitch, width * 4);
	USE_UINT32_ARG(5, bpp, 32);
	USE_UINT32_ARG(6, redMask, 0xFF000000);
	USE_UINT32_ARG(7, greenMask, 0x00FF0000);
	USE_UINT32_ARG(8, blueMask, 0x0000FF00);
	
	FIBITMAP *output = FreeImage_ConvertFromRawBits(
		(BYTE*)buffer,
		width, height,
		pitch, bpp,
		redMask, greenMask, blueMask
	);
	
	if (format == FIF_JPEG && bpp != 24) {
		FIBITMAP *old = output;
		output = FreeImage_ConvertTo24Bits(output);
		FreeImage_Unload(old);
	}
	
	bool ret = FreeImage_Save(format, output, *dest) == 1;
	FreeImage_Unload(output);
	
	RET_VALUE(Nan::New<Boolean>(ret));
	
}


Image::~Image() {
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
	}
	
	unregisterImage(this);
	
}


void Image::deinit() {
	
	vector<Image*>::iterator it = images.begin();
	while (it != images.end()) {
		Image *img = *it;
		
		if (img->_bitmap) {
			FreeImage_Unload(img->_bitmap);
			img->_bitmap = NULL;
		}
		
		it++;
	}
	
	FreeImage_DeInitialise();
	
}
