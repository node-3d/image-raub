#include <napi.h>

#include "image.hpp"


Napi::Object init(Napi::Env env, Napi::Object exports) {
	
	Image::init(env, exports);
	
	return exports;
	
}


NODE_API_MODULE(image, init)
