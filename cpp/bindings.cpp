#include <cstdlib>

#include "image.hpp"

using namespace v8;
using namespace node;
using namespace std;


extern "C" {


NAN_MODULE_INIT(init) {
	
	atexit(Image::deinit);
	
	Image::init(target);
	
}


NODE_MODULE(NODE_GYP_MODULE_NAME, init);


} // extern "C"
