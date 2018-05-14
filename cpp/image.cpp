#include <cstdlib>
#include <vector>
#include <iostream>

#include "image.hpp"


using namespace v8;
using namespace node;
using namespace std;


// ------ Aux macros

#define THIS_IMAGE                                                            \
	Image *image = ObjectWrap::Unwrap<Image>(info.This());
	
#define THIS_CHECK                                                            \
	if (image->_isDestroyed) return;

#define USE_UINT32_ARG(I, VAR, DEF)                                           \
	CHECK_LET_ARG(I, IsUint32(), "uint32");                                   \
	unsigned int VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I]->Uint32Value();


// ------ Constructor and Destructor

Image::~Image() {
	
	_destroy();
	
}


void Image::_destroy() { DES_CHECK;
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
		_bitmap = nullptr;
	}
	
	_isDestroyed = true;
	
	EventEmitter::_destroy();
	
}


// ------ Methods and props

NAN_GETTER(Image::widthGetter) { THIS_IMAGE; THIS_CHECK;
	
	RET_VALUE(JS_INT(image->_bitmap ? FreeImage_GetWidth(image->_bitmap) : 0));
	
}


NAN_GETTER(Image::heightGetter) { THIS_IMAGE; THIS_CHECK;
	
	RET_VALUE(JS_INT(image->_bitmap ? FreeImage_GetHeight(image->_bitmap) : 0));
	
}


NAN_METHOD(Image::load) { THIS_IMAGE; THIS_CHECK;
	
	REQ_OBJ_ARG(0, file);
	
	BYTE *bufferData = reinterpret_cast<BYTE*>(Buffer::Data(file));
	size_t bufferLength = Buffer::Length(file);
	
	FIMEMORY *memStream = FreeImage_OpenMemory(bufferData, bufferLength);
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memStream, bufferLength);
	FIBITMAP *tmpBitmap = FreeImage_LoadFromMemory(format, memStream);
	
	if (image->_bitmap) {
		FreeImage_Unload(image->_bitmap);
	}
	
	image->_bitmap = FreeImage_ConvertTo32Bits(tmpBitmap);
	
	FreeImage_Unload(tmpBitmap);
	
	FreeImage_CloseMemory(memStream);
	
	// adjust internal fields
	size_t num_pixels = FreeImage_GetWidth(image->_bitmap) * FreeImage_GetHeight(image->_bitmap);
	BYTE *pixels = FreeImage_GetBits(image->_bitmap);
	int num_bytes = static_cast<int>(num_pixels * 4);
	
	// FreeImage stores data in BGR. Convert from BGR to RGB.
	for (size_t i = 0; i < num_pixels; i++) {
		size_t i4 = i << 2;
		BYTE temp = pixels[i4 + 0];
		pixels[i4 + 0] = pixels[i4 + 2];
		pixels[i4 + 2] = temp;
	}
	
	V8_VAR_OBJ buffer = Nan::NewBuffer(num_bytes).ToLocalChecked();
	
	memcpy(Buffer::Data(buffer), pixels, num_bytes);
	
	Nan::Set(info.This(), JS_STR("_data"), buffer);
	
	image->emit("load");
	
}


NAN_METHOD(Image::unload) { THIS_IMAGE; THIS_CHECK;
	
	if (image->_bitmap) {
		FreeImage_Unload(image->_bitmap);
		image->_bitmap = nullptr;
	}
	
	Nan::Set(info.This(), JS_STR("_data"), Nan::Null());
	
	image->emit("load");
	
}


NAN_METHOD(Image::save) { THIS_IMAGE; THIS_CHECK;
	
	REQ_UTF8_ARG(0, dest)
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*dest);
	
	void *buffer = Buffer::Data(info[1]);
	
	REQ_UINT32_ARG(2, width);
	REQ_UINT32_ARG(3, height);
	
	USE_UINT32_ARG(4, pitch, width * 4);
	USE_UINT32_ARG(5, bpp, 32);
	USE_UINT32_ARG(6, redMask, 0xFF000000);
	USE_UINT32_ARG(7, greenMask, 0x00FF0000);
	USE_UINT32_ARG(8, blueMask, 0x0000FF00);
	
	FIBITMAP *output = FreeImage_ConvertFromRawBits(
		reinterpret_cast<BYTE*>(buffer),
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


// ------ System methods and props for ObjectWrap

V8_STORE_FT Image::_protoImage;
V8_STORE_FUNC Image::_ctorImage;


void Image::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	// class Image inherits EventEmitter
	V8_VAR_FT parent = Nan::New(EventEmitter::_protoEventEmitter);
	proto->Inherit(parent);
	
	proto->InstanceTemplate()->SetInternalFieldCount(1);
	proto->SetClassName(JS_STR("Image"));
	
	
	// Accessors
	V8_VAR_OT obj = proto->PrototypeTemplate();
	ACCESSOR_R(obj, isDestroyed);
	
	ACCESSOR_R(obj, width);
	ACCESSOR_R(obj, height);
	
	// -------- dynamic
	
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	Nan::SetPrototypeMethod(proto, "save", save);
	Nan::SetPrototypeMethod(proto, "_load", load);
	Nan::SetPrototypeMethod(proto, "_unload", unload);
	
	// -------- static
	
	V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
	
	_protoImage.Reset(proto);
	_ctorImage.Reset(ctor);
	
	Nan::Set(target, JS_STR("Image"), ctor);
	
	
}


bool Image::isImage(V8_VAR_OBJ obj) {
	return Nan::New(_protoImage)->HasInstance(obj);
}


NAN_METHOD(Image::newCtor) {
	
	CTOR_CHECK("Image");
	
	Image *image = new Image();
	image->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


NAN_METHOD(Image::destroy) { THIS_IMAGE; THIS_CHECK;
	
	image->emit("destroy");
	
	image->_destroy();
	
}


NAN_GETTER(Image::isDestroyedGetter) { THIS_IMAGE;
	
	RET_VALUE(JS_BOOL(image->_isDestroyed));
	
}

