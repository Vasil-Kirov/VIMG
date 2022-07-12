#include "scaling.h"

#define GetU32Pixel(Array, X, Y, W) Array[X + (Y * W)]


void ScaleImageRGBA(const u8 *Input, int InW, int InH, u8 *Output, int OutW, int OutH)
{
	u32 *InP = (u32 *)Input;
	u32 *OutP = (u32 *)Output;
	for(int Y = 0; Y < OutH; ++Y)
	{
		for(int X = 0; X < OutW; ++X)
		{
			float XNearest = NormalizeBetween(X, 0, OutW, 0, InW - 1);
			float YNearest = NormalizeBetween(Y, 0, OutH, 0, InH - 1);
			int X2;
			int Y2;
			int X1 = (int)XNearest;
			if(X1 == InW - 1)
				X2 = X1;
			else
				X2 = ((int)XNearest)+1;

			int Y1 = (int)YNearest;
			if(Y1 == InH - 1)
				Y2 = Y1;
			else
				Y2 = ((int)YNearest)+1;
			
			register RGBA Q1 = {GetU32Pixel(InP, X1, Y1, InW)};
			register RGBA Q2 = {GetU32Pixel(InP, X1, Y2, InW)};
			register RGBA Q3 = {GetU32Pixel(InP, X2, Y1, InW)};
			register RGBA Q4 = {GetU32Pixel(InP, X2, Y2, InW)};

			RGBA P1;
			P1.r = (X2-XNearest)*Q1.r + (XNearest-X1)*Q2.r;
			P1.g = (X2-XNearest)*Q1.g + (XNearest-X1)*Q2.g;
			P1.b = (X2-XNearest)*Q1.b + (XNearest-X1)*Q2.b;
			P1.a = (X2-XNearest)*Q1.a + (XNearest-X1)*Q2.a;

			RGBA P2;
			P2.r = (X2 - XNearest) * Q3.r + (XNearest - X1) * Q4.r;
			P2.g = (X2 - XNearest) * Q3.g + (XNearest - X1) * Q4.g;
			P2.b = (X2 - XNearest) * Q3.b + (XNearest - X1) * Q4.b;
			P2.a = (X2 - XNearest) * Q3.a + (XNearest - X1) * Q4.a;

			if(X1 == X2)
			{
				P1 = Q1;
				P2 = Q2;
			}

			RGBA P;
			P.r = (Y2 - YNearest) * P1.r + (YNearest - Y1) * P2.r;
			P.g = (Y2 - YNearest) * P1.g + (YNearest - Y1) * P2.g;
			P.b = (Y2 - YNearest) * P1.b + (YNearest - Y1) * P2.b;
			P.a = (Y2 - YNearest) * P1.a + (YNearest - Y1) * P2.a;

			OutP[X + (Y * OutW)] = P.Color;
		}
	}
}


void ScaleImageRGB(const u8 *Input, int InW, int InH, u8 *Output, int OutW, int OutH, int BytesPerPixel)
{
	u8 *OutP = Output;
	unsigned int InPitch = InW * BytesPerPixel;
	
	for(int Y = 0; Y < OutH; ++Y)
	{
		for(int X = 0; X < OutW; ++X)
		{
			float XNearest = NormalizeBetween(X, 0, OutW, 0, InW - 1);
			float YNearest = NormalizeBetween(Y, 0, OutH, 0, InH - 1);
			int X2;
			int Y2;
			int X1 = (int)XNearest;
			if(X1 == InW - 1)
				X2 = X1;
			else
				X2 = ((int)XNearest)+1;

			int Y1 = (int)YNearest;
			if(Y1 == InH - 1)
				Y2 = Y1;
			else
				Y2 = ((int)YNearest)+1;

			const u8 *Q1Ptr = Input + (X1 * BytesPerPixel) + (Y1 * InPitch);
			register RGBA Q1;
			Q1.r = *Q1Ptr++;
			Q1.g = *Q1Ptr++;
			Q1.b = *Q1Ptr++;
			Q1.a = 0xFF;
			const u8 *Q2Ptr = Input + (X1 * BytesPerPixel) + (Y2 * InPitch);
			register RGBA Q2;
			Q2.r = *Q2Ptr++;
			Q2.g = *Q2Ptr++;
			Q2.b = *Q2Ptr++;
			Q2.a = 0xFF;
			const u8 *Q3Ptr = Input + (X2 * BytesPerPixel) + (Y1 * InPitch);
			register RGBA Q3;
			Q3.r = *Q3Ptr++;
			Q3.g = *Q3Ptr++;
			Q3.b = *Q3Ptr++;
			Q3.a = 0xFF;
			const u8 *Q4Ptr = Input + (X2 * BytesPerPixel) + (Y2 * InPitch);
			register RGBA Q4;
			Q4.r = *Q4Ptr++;
			Q4.g = *Q4Ptr++;
			Q4.b = *Q4Ptr++;
			Q4.a = 0xFF;

			RGBA P1;
			P1.r = (X2-XNearest)*Q1.r + (XNearest-X1)*Q2.r;
			P1.g = (X2-XNearest)*Q1.g + (XNearest-X1)*Q2.g;
			P1.b = (X2-XNearest)*Q1.b + (XNearest-X1)*Q2.b;
			P1.a = (X2-XNearest)*Q1.a + (XNearest-X1)*Q2.a;

			RGBA P2;
			P2.r = (X2 - XNearest) * Q3.r + (XNearest - X1) * Q4.r;
			P2.g = (X2 - XNearest) * Q3.g + (XNearest - X1) * Q4.g;
			P2.b = (X2 - XNearest) * Q3.b + (XNearest - X1) * Q4.b;
			P2.a = (X2 - XNearest) * Q3.a + (XNearest - X1) * Q4.a;

			if(X1 == X2)
			{
				P1 = Q1;
				P2 = Q2;
			}

			RGBA P;
			P.r = (Y2 - YNearest) * P1.r + (YNearest - Y1) * P2.r;
			P.g = (Y2 - YNearest) * P1.g + (YNearest - Y1) * P2.g;
			P.b = (Y2 - YNearest) * P1.b + (YNearest - Y1) * P2.b;
			P.a = (Y2 - YNearest) * P1.a + (YNearest - Y1) * P2.a;

			*OutP++ = P.r;
			*OutP++ = P.g;
			*OutP++ = P.b;
		}
	}
}

void ScaleImageRGBNN(const u8 *Input, int InW, int InH, u8 *Output, int OutW, int OutH, int BytesPerPixel)
{
	u8 *OutP = Output;
	unsigned int InPitch = InW * BytesPerPixel;
	
	for(int Y = 0; Y < OutH; ++Y)
	{
		for(int X = 0; X < OutW; ++X)
		{
			float XNearest = NormalizeBetween(X, 0, OutW, 0, InW - 1);
			float YNearest = NormalizeBetween(Y, 0, OutH, 0, InH - 1);
  
			const u8 *PixelScan = Input + (int)XNearest * BytesPerPixel + ((int)YNearest * InPitch);
			
			*OutP++ = *PixelScan++;
			*OutP++ = *PixelScan++;
			*OutP++ = *PixelScan++;
		}
	}
}


