#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_


#include <addon-tools.hpp>

#include <FreeImage.h>


class Image : public node::ObjectWrap {
	
public:
	static void init(v8::Handle<v8::Object> target);
	static void deinit();
	
	int GetWidth();
	int GetHeight();
	int GetPitch();
	void *GetData();
	void Load (const char *filename);
	
	
protected:
	static NAN_METHOD(New);
	static NAN_GETTER(WidthGetter);
	static NAN_GETTER(HeightGetter);
	static NAN_GETTER(SrcGetter);
	static NAN_SETTER(SrcSetter);
	static NAN_SETTER(OnloadSetter);
	static NAN_GETTER(PitchGetter);
	static NAN_METHOD(save);
	
	virtual ~Image();
	
	
private:
	static Nan::Persistent<v8::Function> _constructor;
	
	FIBITMAP *image_bmp;
	char *filename;
	void *data;
	
};

#endif // _IMAGE_HPP_
