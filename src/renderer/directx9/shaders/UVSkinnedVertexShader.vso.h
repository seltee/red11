#ifndef UVSKINNEDVERTEXSHADER_VSO_H
#define UVSKINNEDVERTEXSHADER_VSO_H

static const unsigned char UVSkinnedVertexShader_vso[] = {
	0x00, 0x03, 0xfe, 0xff, 0xfe, 0xff, 0x2f, 0x00, 0x43, 0x54, 0x41, 0x42,
	0x1c, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xff,
	0x02, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x02, 0x00, 0x34, 0x00,
	0xd0, 0x00, 0xd2, 0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x64, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00,
	0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x6f, 0x6e, 0x65,
	0x4d, 0x61, 0x74, 0x72, 0x69, 0x63, 0x65, 0x73, 0x00, 0xab, 0xab, 0xab,
	0x03, 0x00, 0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0x34, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a,
	0x00, 0xab, 0xab, 0xab, 0x03, 0x00, 0x03, 0x00, 0x04, 0x00, 0x04, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x73, 0x5f, 0x33,
	0x5f, 0x30, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74,
	0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68,
	0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65,
	0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33,
	0x31, 0x31, 0x31, 0x00, 0x51, 0x00, 0x00, 0x05, 0x04, 0x00, 0x0f, 0xa0,
	0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x80,
	0x01, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80,
	0x02, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x80,
	0x03, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x80,
	0x04, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x0f, 0xe0, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x02, 0x80,
	0x01, 0x00, 0x07, 0xe0, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x01, 0x80,
	0x02, 0x00, 0x07, 0xe0, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80,
	0x03, 0x00, 0x03, 0xe0, 0x0c, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80,
	0x03, 0x00, 0xe4, 0x90, 0x03, 0x00, 0xe4, 0x91, 0x13, 0x00, 0x00, 0x02,
	0x01, 0x00, 0x0f, 0x80, 0x03, 0x00, 0xe4, 0x90, 0x02, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x0f, 0x80, 0x01, 0x00, 0xe4, 0x81, 0x03, 0x00, 0xe4, 0x90,
	0x0c, 0x00, 0x00, 0x03, 0x01, 0x00, 0x0f, 0x80, 0x01, 0x00, 0xe4, 0x81,
	0x01, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80,
	0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x02, 0x00, 0xe4, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0xe4, 0x80,
	0x04, 0x00, 0x00, 0xa0, 0x2e, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0f, 0xb0,
	0x00, 0x00, 0xe1, 0x80, 0x04, 0x00, 0x00, 0x04, 0x01, 0x00, 0x0f, 0x80,
	0x00, 0x00, 0x24, 0x90, 0x04, 0x00, 0x95, 0xa0, 0x04, 0x00, 0x6a, 0xa0,
	0x09, 0x00, 0x00, 0x04, 0x02, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x34, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x00, 0xb0, 0x09, 0x00, 0x00, 0x04,
	0x02, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x35, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0x00, 0xb0, 0x09, 0x00, 0x00, 0x04, 0x02, 0x00, 0x04, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x36, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x00, 0xb0,
	0x09, 0x00, 0x00, 0x04, 0x02, 0x00, 0x08, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x37, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x00, 0xb0, 0x05, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x55, 0x90,
	0x09, 0x00, 0x00, 0x04, 0x03, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x34, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x55, 0xb0, 0x09, 0x00, 0x00, 0x04,
	0x03, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x35, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0x55, 0xb0, 0x09, 0x00, 0x00, 0x04, 0x03, 0x00, 0x04, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x36, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x55, 0xb0,
	0x09, 0x00, 0x00, 0x04, 0x03, 0x00, 0x08, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x37, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x55, 0xb0, 0x04, 0x00, 0x00, 0x04,
	0x02, 0x00, 0x0f, 0x80, 0x04, 0x00, 0x00, 0x90, 0x03, 0x00, 0xe4, 0x80,
	0x02, 0x00, 0xe4, 0x80, 0x09, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x34, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0xb0,
	0x09, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x35, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0xb0, 0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x36, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0xaa, 0xb0, 0x09, 0x00, 0x00, 0x04, 0x00, 0x00, 0x08, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x37, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0xb0,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80, 0x04, 0x00, 0xaa, 0x90,
	0x00, 0x00, 0xe4, 0x80, 0x02, 0x00, 0xe4, 0x80, 0x09, 0x00, 0x00, 0x04,
	0x02, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x34, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0xff, 0xb0, 0x09, 0x00, 0x00, 0x04, 0x02, 0x00, 0x02, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x35, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xff, 0xb0,
	0x09, 0x00, 0x00, 0x04, 0x02, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x36, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xff, 0xb0, 0x09, 0x00, 0x00, 0x04,
	0x02, 0x00, 0x08, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x37, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0xff, 0xb0, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80,
	0x04, 0x00, 0xff, 0x90, 0x02, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0x80,
	0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0xe4, 0x80,
	0x00, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0xe0,
	0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x04, 0xe0, 0x00, 0x00, 0xe4, 0x80, 0x02, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x08, 0xe0, 0x00, 0x00, 0xe4, 0x80,
	0x03, 0x00, 0xe4, 0xa0, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x07, 0xe0,
	0x00, 0x00, 0xe4, 0x80, 0x08, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x80,
	0x01, 0x00, 0xe4, 0x90, 0x34, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x00, 0xb0,
	0x08, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x90,
	0x35, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x00, 0xb0, 0x08, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x36, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0x00, 0xb0, 0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x80,
	0x00, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x55, 0x90, 0x08, 0x00, 0x00, 0x04,
	0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x34, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0x55, 0xb0, 0x08, 0x00, 0x00, 0x04, 0x01, 0x00, 0x02, 0x80,
	0x01, 0x00, 0xe4, 0x90, 0x35, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x55, 0xb0,
	0x08, 0x00, 0x00, 0x04, 0x01, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x90,
	0x36, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0x55, 0xb0, 0x04, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x07, 0x80, 0x04, 0x00, 0x00, 0x90, 0x01, 0x00, 0xe4, 0x80,
	0x00, 0x00, 0xe4, 0x80, 0x08, 0x00, 0x00, 0x04, 0x01, 0x00, 0x01, 0x80,
	0x01, 0x00, 0xe4, 0x90, 0x34, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0xb0,
	0x08, 0x00, 0x00, 0x04, 0x01, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x90,
	0x35, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0xb0, 0x08, 0x00, 0x00, 0x04,
	0x01, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x36, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0xaa, 0xb0, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x07, 0x80,
	0x04, 0x00, 0xaa, 0x90, 0x01, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0x80,
	0x08, 0x00, 0x00, 0x04, 0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x90,
	0x34, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xff, 0xb0, 0x08, 0x00, 0x00, 0x04,
	0x01, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x35, 0x20, 0xe4, 0xa0,
	0x00, 0x00, 0xff, 0xb0, 0x08, 0x00, 0x00, 0x04, 0x01, 0x00, 0x04, 0x80,
	0x01, 0x00, 0xe4, 0x90, 0x36, 0x20, 0xe4, 0xa0, 0x00, 0x00, 0xff, 0xb0,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x07, 0x80, 0x04, 0x00, 0xff, 0x90,
	0x01, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x08, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0x80,
	0x07, 0x00, 0x00, 0x02, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0xff, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x02, 0x00, 0x07, 0xe0, 0x00, 0x00, 0xff, 0x80,
	0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0x00, 0x02, 0x03, 0x00, 0x03, 0xe0,
	0x02, 0x00, 0xe4, 0x90, 0xff, 0xff, 0x00, 0x00
};

#endif /* UVSKINNEDVERTEXSHADER_VSO_H */
