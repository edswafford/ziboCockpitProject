#pragma warning( push )
#pragma warning( disable : 4273 )
#pragma warning( disable : 26451 )
#pragma warning( disable : 26812 )
#pragma warning( disable : 26439 )
#pragma warning( disable : 26800 )
#pragma warning( disable : 26495 )

#include "xplane_mock.hpp"

static std::function<int (void*)> get_int_;
static std::function<float (void*)> get_float_;
static std::function<void (void*, float)> set_float_;
static std::function<double (void*)> get_double_;

static std::function<int(void*, int*, int, int)> get_vector_int_;
static std::function<int(void*, float*, int, int)> get_vector_float_;
static std::function<int(void*, void*, int, int)> get_vector_byte_;

static std::function<void* (const char*)> find_dataref_;
static std::function<XPLMDataTypeID (void*)> get_types_;


XplaneDataAccessMock::XplaneDataAccessMock()
{
	assert(!get_int_ && !get_float_ && !set_float_ && !get_double_ && !get_vector_int_ && !get_vector_float_ && !get_vector_byte_ && !find_dataref_ && !get_types_);
	get_int_ = [this](void* ref) { return XPLMGetDatai(ref); };
	get_float_ = [this](void* ref) { return XPLMGetDataf(ref); };
	set_float_ = [this](void* ref, float value) {XPLMSetDataf(ref, value);};
	get_double_ = [this](void* ref) { return XPLMGetDatad(ref); };

	get_vector_int_ = [this](void* ref, int* out_values, int in_offset, int in_max) { return XPLMGetDatavi(ref, out_values, in_offset, in_max); };
	get_vector_float_ = [this](void* ref, float* out_values, int in_offset, int in_max) { return XPLMGetDatavf(ref, out_values, in_offset, in_max); };
	get_vector_byte_ = [this](void* ref, void* out_values, int in_offset, int in_max) { return XPLMGetDatab(ref, out_values, in_offset, in_max); };

	find_dataref_ = [this](const char* str) {return XPLMFindDataRef(str); };
	get_types_ = [this](void* ref) {return XPLMGetDataRefTypes(ref); };
}

XplaneDataAccessMock::~XplaneDataAccessMock()
{
	get_int_ = {};
	get_float_ = {};
	set_float_ = {};
	get_double_ = {};

	get_vector_int_ = {};
	get_vector_float_ = {};
	get_vector_byte_ = {};

	find_dataref_ = {};
	get_types_ = {};
}

int XPLMGetDatai(void* ref)
{
	return get_int_(ref);
}
float XPLMGetDataf(void* ref)
{
	return get_float_(ref);
}
 void XPLMSetDataf(void* ref, float inValue)
{
	set_float_(ref, inValue);
}    

double XPLMGetDatad(void* ref)
{
	return get_double_(ref);
}

// Vectors
int XPLMGetDatavi(void* ref, int* out_values, int in_offset, int in_max)
{
	return get_vector_int_(ref, out_values, in_offset, in_max);
}
int XPLMGetDatavf(void* ref, float* out_values, int in_offset, int in_max)
{
	return get_vector_float_(ref, out_values, in_offset, in_max);
}
int XPLMGetDatab(void* ref, void* out_values, int in_offset, int in_max)
{
	return get_vector_byte_(ref, out_values, in_offset, in_max);
}




void* XPLMFindDataRef(const char* str)
{
	return find_dataref_(str);
}

XPLMDataTypeID XPLMGetDataRefTypes(void* ref)
{
	return get_types_(ref);
}
#pragma warning( pop )