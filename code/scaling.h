#pragma once
#include "basic.h"


void ScaleImageRGBA(const u8 *Input, int InW, int InH, u8 *Output, int OutW, int OutH);

void ScaleImageRGB(const u8 *Input, int InW, int InH, u8 *Output, int OutW, int OutH, int BytesPerPixel);

void ScaleImageRGBNN(const u8 *Input, int InW, int InH, u8 *Output, int OutW, int OutH, int BytesPerPixel);