#define Light_RootSig \
    "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "DescriptorTable(SRV(t0, numDescriptors = 1) )," \
    "CBV(b0, space = 0), " \
    "CBV(b1, space = 0), " \
    "StaticSampler(s0," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "filter = FILTER_MIN_MAG_MIP_LINEAR)," \
    "StaticSampler(s1," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "filter = FILTER_MIN_MAG_MIP_POINT)"