#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_


#include <addon-tools.hpp>

#include <FreeImage.h>


class Image : public node::ObjectWrap {
	
public:
	static NAN_MODULE_INIT(init);
	static void deinit();
	
	void *GetData();
	void Load (const char *filename);
	
	
protected:
	
	static NAN_METHOD(newCtor);
	
	static NAN_GETTER(widthGetter);
	static NAN_GETTER(heightGetter);
	
	static NAN_METHOD(load);
	static NAN_METHOD(save);
	
	virtual ~Image();
	
	
private:
	static Nan::Persistent<v8::Function> _constructor;
	
	Nan::Persistent<v8::Object> _emitter;
	inline void _emit(int argc, v8::Local<v8::Value> argv[]);
	
	FIBITMAP *_bitmap;
	
};

#endif // _IMAGE_HPP_
