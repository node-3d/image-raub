#include <cstdlib>

#include <event-emitter.hpp>

#include "image.hpp"


extern "C" {


NAN_MODULE_INIT(init) {
	
	EventEmitter::init(target);
	
	Image::init(target);
	
}


NODE_MODULE(image, init);


} // extern "C"
