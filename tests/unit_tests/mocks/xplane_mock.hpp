#pragma once

#include "gmock/gmock.h"

#include "XPLMDataAccess.h"

struct XplaneDataAccessMock
{
	XplaneDataAccessMock();
	~XplaneDataAccessMock();

	MOCK_METHOD(int, XPLMGetDatai, (void*), (const));
	MOCK_METHOD(float, XPLMGetDataf, (void*), (const));
	MOCK_METHOD(double, XPLMGetDatad, (void*), (const));

	MOCK_METHOD(int, XPLMGetDatab, (void*, void*, int, int), (const));
	MOCK_METHOD(int, XPLMGetDatavi, (void*, int*, int, int), (const));
	MOCK_METHOD(int, XPLMGetDatavf, (void*, float*, int, int), (const));


	MOCK_METHOD(void*, XPLMFindDataRef, (const char*), (const));
	MOCK_METHOD(XPLMDataTypeID, XPLMGetDataRefTypes, (void*), (const));
};