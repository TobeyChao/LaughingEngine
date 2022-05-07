#ifndef __COLOR_SPACE_UTILITY_HLSLI__
#define __COLOR_SPACE_UTILITY_HLSLI__

#define COLOR_FORMAT_LINEAR     0
#define COLOR_FORMAT_sRGB_FULL  1

// http://www.heronuke.com/archives/12305
float3 ApplySRGBCurve(float3 color)
{
    return color < 0.0031308 ? 12.92 * color : 1.055 * pow(abs(color), 1.0 / 2.4) - 0.055;
}

float3 ApplyDisplayProfile(float3 color, int displayFormat)
{
    switch (displayFormat)
    {
        case COLOR_FORMAT_LINEAR:
            return color;
        case COLOR_FORMAT_sRGB_FULL:
            return ApplySRGBCurve(color);
        default:
            return color;
    }
}

#endif // __COLOR_SPACE_UTILITY_HLSLI__