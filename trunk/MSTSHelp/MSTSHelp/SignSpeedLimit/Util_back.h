#ifndef UTIL_BACK_H
#define UTIL_BACK_H
#include "Util.h"
float* process30(HANDLE handle, float* fMatrix, float*fXYZ);
void getSectionData(HANDLE handle, SProcessData& processData, const STrackNode& node, int sectionNum, float* fArray, SSectionTypeData* basePtr);
int AdjustAngle(HANDLE handle, SProcessData& processData, const STrackNode& node, float fLocation, SSectionTypeData* basePtr);
#endif