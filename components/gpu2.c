#include "gpu2.h"
#include <stdlib.h>
#include <assert.h>

Gpu* create_gpu() {
	Gpu* ret = (Gpu*)malloc(sizeof(Gpu));
	assert(ret == NULL && "could not initialize gpu");
	init_gpu(ret);
	return ret;
}

void init_gpu(Gpu* gpu) {

}

void step_gpu(Gpu* gpu) {

}
