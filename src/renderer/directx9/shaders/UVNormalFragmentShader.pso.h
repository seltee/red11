#ifndef UVNORMALFRAGMENTSHADER_PSO_H
#define UVNORMALFRAGMENTSHADER_PSO_H

static const unsigned char UVNormalFragmentShader_pso[] = {
	0x00, 0x03, 0xff, 0xff, 0xfe, 0xff, 0xbf, 0x00, 0x43, 0x54, 0x41, 0x42,
	0x1c, 0x00, 0x00, 0x00, 0xc7, 0x02, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff,
	0x0d, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0xc0, 0x02, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x02, 0x00, 0x0f, 0x00,
	0x01, 0x00, 0x3e, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3c, 0x01, 0x00, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x01, 0x00, 0x3a, 0x00,
	0x2c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x01, 0x00, 0x00,
	0x02, 0x00, 0x11, 0x00, 0x01, 0x00, 0x46, 0x00, 0x2c, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x5d, 0x01, 0x00, 0x00, 0x02, 0x00, 0x10, 0x00,
	0x01, 0x00, 0x42, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6b, 0x01, 0x00, 0x00, 0x02, 0x00, 0x14, 0x00, 0x20, 0x00, 0x52, 0x00,
	0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd0, 0x01, 0x00, 0x00,
	0x02, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x32, 0x00, 0x2c, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xde, 0x01, 0x00, 0x00, 0x02, 0x00, 0x0d, 0x00,
	0x01, 0x00, 0x36, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xec, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0x00,
	0x03, 0x00, 0x06, 0x00, 0x01, 0x00, 0x1a, 0x00, 0x20, 0x02, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x06, 0x00, 0x44, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x54, 0x02, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x00, 0x0e, 0x00,
	0x68, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x02, 0x00, 0x00,
	0x03, 0x00, 0x02, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x8c, 0x02, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x9c, 0x02, 0x00, 0x00, 0x03, 0x00, 0x05, 0x00,
	0x01, 0x00, 0x16, 0x00, 0xb0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x41, 0x6c, 0x62, 0x65, 0x64, 0x6f, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00,
	0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x41, 0x6d, 0x62, 0x69, 0x65, 0x6e, 0x74, 0x4c,
	0x69, 0x67, 0x68, 0x74, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x43, 0x61,
	0x6d, 0x65, 0x72, 0x61, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e,
	0x00, 0x45, 0x6d, 0x69, 0x73, 0x73, 0x69, 0x6f, 0x6e, 0x43, 0x6f, 0x6c,
	0x6f, 0x72, 0x00, 0x4c, 0x69, 0x67, 0x68, 0x74, 0x73, 0x00, 0x74, 0x79,
	0x70, 0x65, 0x00, 0xab, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x6f, 0x73, 0x69,
	0x74, 0x69, 0x6f, 0x6e, 0x00, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x00,
	0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0xab, 0xab, 0x72, 0x01, 0x00, 0x00,
	0x78, 0x01, 0x00, 0x00, 0x88, 0x01, 0x00, 0x00, 0x78, 0x01, 0x00, 0x00,
	0x91, 0x01, 0x00, 0x00, 0x78, 0x01, 0x00, 0x00, 0x98, 0x01, 0x00, 0x00,
	0x78, 0x01, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00,
	0x08, 0x00, 0x04, 0x00, 0xa0, 0x01, 0x00, 0x00, 0x4d, 0x61, 0x74, 0x65,
	0x72, 0x69, 0x61, 0x6c, 0x44, 0x61, 0x74, 0x61, 0x31, 0x00, 0x4d, 0x61,
	0x74, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x44, 0x61, 0x74, 0x61, 0x32, 0x00,
	0x61, 0x6c, 0x62, 0x65, 0x64, 0x6f, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d,
	0x70, 0x6c, 0x65, 0x72, 0x00, 0xab, 0xab, 0xab, 0x04, 0x00, 0x0c, 0x00,
	0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x61, 0x6f, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72,
	0x00, 0xab, 0xab, 0xab, 0x04, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x6d, 0x69, 0x73,
	0x73, 0x69, 0x6f, 0x6e, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c,
	0x65, 0x72, 0x00, 0xab, 0x04, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x65, 0x74, 0x61,
	0x6c, 0x6c, 0x69, 0x63, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c,
	0x65, 0x72, 0x00, 0xab, 0x04, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x6f, 0x72, 0x6d,
	0x61, 0x6c, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72,
	0x00, 0xab, 0xab, 0xab, 0x04, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x6f, 0x75, 0x67,
	0x68, 0x6e, 0x65, 0x73, 0x73, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70,
	0x6c, 0x65, 0x72, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x73, 0x5f, 0x33,
	0x5f, 0x30, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74,
	0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68,
	0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65,
	0x72, 0x20, 0x39, 0x2e, 0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33,
	0x31, 0x31, 0x31, 0x00, 0x51, 0x00, 0x00, 0x05, 0x00, 0x00, 0x0f, 0xa0,
	0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0x3f, 0x51, 0x00, 0x00, 0x05, 0x01, 0x00, 0x0f, 0xa0,
	0x17, 0xb7, 0xd1, 0xb8, 0x83, 0xf9, 0xa2, 0x3e, 0x00, 0x00, 0x00, 0x41,
	0x00, 0x40, 0x9c, 0x47, 0x51, 0x00, 0x00, 0x05, 0x02, 0x00, 0x0f, 0xa0,
	0x00, 0x00, 0x80, 0xc0, 0x00, 0x00, 0xa0, 0xc0, 0x00, 0x00, 0xc0, 0xc0,
	0x00, 0x00, 0xe0, 0xc0, 0x51, 0x00, 0x00, 0x05, 0x03, 0x00, 0x0f, 0xa0,
	0x8f, 0xc2, 0x75, 0x3f, 0x0a, 0xd7, 0x23, 0x3d, 0x55, 0x55, 0x55, 0x3f,
	0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x05, 0x04, 0x00, 0x0f, 0xa0,
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x00, 0xc0,
	0x00, 0x00, 0x40, 0xc0, 0x30, 0x00, 0x00, 0x05, 0x00, 0x00, 0x0f, 0xf0,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x02, 0x80,
	0x00, 0x00, 0x07, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x01, 0x80,
	0x01, 0x00, 0x07, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80,
	0x02, 0x00, 0x03, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x03, 0x80,
	0x03, 0x00, 0x07, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x04, 0x80,
	0x04, 0x00, 0x07, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x00, 0x08, 0x0f, 0xa0, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x01, 0x08, 0x0f, 0xa0, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x02, 0x08, 0x0f, 0xa0, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x03, 0x08, 0x0f, 0xa0, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x05, 0x08, 0x0f, 0xa0, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x06, 0x08, 0x0f, 0xa0, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x05, 0x80,
	0x00, 0x00, 0xe4, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x01, 0x00, 0x0f, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x0c, 0x00, 0xe4, 0xa0, 0x42, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0x90, 0x00, 0x08, 0xe4, 0xa0,
	0x05, 0x00, 0x00, 0x03, 0x02, 0x00, 0x07, 0x80, 0x02, 0x00, 0xe4, 0x80,
	0x0f, 0x00, 0xe4, 0xa0, 0x58, 0x00, 0x00, 0x04, 0x02, 0x00, 0x07, 0x80,
	0x01, 0x00, 0x00, 0x8c, 0x02, 0x00, 0xe4, 0x80, 0x0f, 0x00, 0xe4, 0xa0,
	0x42, 0x00, 0x00, 0x03, 0x03, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0x90,
	0x02, 0x08, 0xe4, 0xa0, 0x04, 0x00, 0x00, 0x04, 0x03, 0x00, 0x07, 0x80,
	0x03, 0x00, 0xe4, 0x80, 0x00, 0x00, 0x55, 0xa0, 0x00, 0x00, 0x00, 0xa0,
	0x58, 0x00, 0x00, 0x04, 0x03, 0x00, 0x07, 0x80, 0x01, 0x00, 0x55, 0x8c,
	0x03, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xfa, 0xa0, 0x02, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x80, 0x0d, 0x00, 0x55, 0xa0,
	0x42, 0x00, 0x00, 0x03, 0x04, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0x90,
	0x01, 0x08, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03, 0x04, 0x00, 0x07, 0x80,
	0x04, 0x00, 0xe4, 0x80, 0x10, 0x00, 0xe4, 0xa0, 0x58, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x0b, 0x80, 0x00, 0x00, 0x00, 0x8c, 0x04, 0x00, 0xa4, 0x80,
	0x10, 0x00, 0xa4, 0xa0, 0x42, 0x00, 0x00, 0x03, 0x04, 0x00, 0x0f, 0x80,
	0x02, 0x00, 0xe4, 0x90, 0x03, 0x08, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03,
	0x01, 0x00, 0x01, 0x80, 0x04, 0x00, 0x00, 0x80, 0x0d, 0x00, 0xff, 0xa0,
	0x42, 0x00, 0x00, 0x03, 0x04, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0x90,
	0x05, 0x08, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03, 0x01, 0x00, 0x02, 0x80,
	0x04, 0x00, 0x00, 0x80, 0x0d, 0x00, 0xaa, 0xa0, 0x58, 0x00, 0x00, 0x04,
	0x01, 0x00, 0x03, 0x80, 0x01, 0x00, 0xee, 0x8c, 0x01, 0x00, 0xe4, 0x80,
	0x0d, 0x00, 0xeb, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x01, 0x00, 0x04, 0x80,
	0x01, 0x00, 0x55, 0x80, 0x01, 0x00, 0x00, 0xa0, 0x23, 0x00, 0x00, 0x02,
	0x01, 0x00, 0x08, 0x80, 0x0d, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x03,
	0x04, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xe4, 0x90, 0x06, 0x08, 0xe4, 0xa0,
	0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80, 0x01, 0x00, 0xff, 0x81,
	0x00, 0x00, 0xff, 0xa0, 0x04, 0x00, 0x00, 0x80, 0x24, 0x00, 0x00, 0x02,
	0x04, 0x00, 0x07, 0x80, 0x03, 0x00, 0xe4, 0x90, 0x24, 0x00, 0x00, 0x02,
	0x05, 0x00, 0x07, 0x80, 0x04, 0x00, 0xe4, 0x90, 0x24, 0x00, 0x00, 0x02,
	0x06, 0x00, 0x07, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x05, 0x00, 0x00, 0x03,
	0x05, 0x00, 0x07, 0x80, 0x03, 0x00, 0x55, 0x81, 0x05, 0x00, 0xe4, 0x80,
	0x04, 0x00, 0x00, 0x04, 0x03, 0x00, 0x0b, 0x80, 0x03, 0x00, 0x00, 0x81,
	0x04, 0x00, 0xa4, 0x80, 0x05, 0x00, 0xa4, 0x80, 0x04, 0x00, 0x00, 0x04,
	0x03, 0x00, 0x07, 0x80, 0x03, 0x00, 0xaa, 0x80, 0x06, 0x00, 0xe4, 0x80,
	0x03, 0x00, 0xf4, 0x80, 0x24, 0x00, 0x00, 0x02, 0x04, 0x00, 0x07, 0x80,
	0x03, 0x00, 0xe4, 0x80, 0x02, 0x00, 0x00, 0x03, 0x03, 0x00, 0x07, 0x80,
	0x11, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xe4, 0x91, 0x08, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x08, 0x80, 0x03, 0x00, 0xe4, 0x80, 0x03, 0x00, 0xe4, 0x80,
	0x07, 0x00, 0x00, 0x02, 0x02, 0x00, 0x08, 0x80, 0x02, 0x00, 0xff, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x05, 0x00, 0x07, 0x80, 0x02, 0x00, 0xff, 0x80,
	0x03, 0x00, 0xe4, 0x80, 0x05, 0x00, 0x00, 0x03, 0x06, 0x00, 0x07, 0x80,
	0x02, 0x00, 0xe4, 0x80, 0x01, 0x00, 0x55, 0xa0, 0x05, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x07, 0x80, 0x02, 0x00, 0xe4, 0x80, 0x0e, 0x00, 0xe4, 0xa0,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0b, 0x80, 0x02, 0x00, 0xa4, 0x80,
	0x01, 0x00, 0xff, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x02, 0x00, 0x00, 0x03,
	0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0x00, 0x81, 0x00, 0x00, 0xff, 0xa0,
	0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x02, 0x80, 0x01, 0x00, 0x55, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x01, 0x00, 0x02, 0x80, 0x01, 0x00, 0x55, 0x80,
	0x01, 0x00, 0xaa, 0xa0, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x02, 0x80,
	0x01, 0x00, 0xaa, 0x80, 0x01, 0x00, 0x55, 0x80, 0x01, 0x00, 0xff, 0xa0,
	0x01, 0x00, 0x00, 0x02, 0x02, 0x00, 0x07, 0x80, 0x00, 0x00, 0xf4, 0x80,
	0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x80, 0x00, 0x00, 0xaa, 0xa0,
	0x26, 0x00, 0x00, 0x01, 0x00, 0x00, 0xe4, 0xf0, 0x02, 0x00, 0x00, 0x03,
	0x07, 0x00, 0x0f, 0x80, 0x01, 0x00, 0xaa, 0x80, 0x04, 0x00, 0xe4, 0xa0,
	0x02, 0x00, 0x00, 0x03, 0x08, 0x00, 0x0f, 0x80, 0x01, 0x00, 0xaa, 0x80,
	0x02, 0x00, 0xe4, 0xa0, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80,
	0x07, 0x00, 0x00, 0x8c, 0x14, 0x00, 0x00, 0xa0, 0x00, 0x00, 0xaa, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80, 0x07, 0x00, 0x55, 0x8c,
	0x18, 0x00, 0x00, 0xa0, 0x01, 0x00, 0xff, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x01, 0x00, 0x08, 0x80, 0x07, 0x00, 0xaa, 0x8c, 0x1c, 0x00, 0x00, 0xa0,
	0x01, 0x00, 0xff, 0x80, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80,
	0x07, 0x00, 0xff, 0x8c, 0x20, 0x00, 0x00, 0xa0, 0x01, 0x00, 0xff, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80, 0x08, 0x00, 0x00, 0x8c,
	0x24, 0x00, 0x00, 0xa0, 0x01, 0x00, 0xff, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x01, 0x00, 0x08, 0x80, 0x08, 0x00, 0x55, 0x8c, 0x28, 0x00, 0x00, 0xa0,
	0x01, 0x00, 0xff, 0x80, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80,
	0x08, 0x00, 0xaa, 0x8c, 0x2c, 0x00, 0x00, 0xa0, 0x01, 0x00, 0xff, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x08, 0x80, 0x08, 0x00, 0xff, 0x8c,
	0x30, 0x00, 0x00, 0xa0, 0x01, 0x00, 0xff, 0x80, 0x02, 0x00, 0x00, 0x03,
	0x01, 0x00, 0x08, 0x80, 0x01, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0xa0,
	0x58, 0x00, 0x00, 0x04, 0x09, 0x00, 0x07, 0x80, 0x07, 0x00, 0x00, 0x8c,
	0x16, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x09, 0x00, 0x07, 0x80, 0x07, 0x00, 0x55, 0x8c, 0x1a, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04, 0x09, 0x00, 0x07, 0x80,
	0x07, 0x00, 0xaa, 0x8c, 0x1e, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0xe4, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x09, 0x00, 0x07, 0x80, 0x07, 0x00, 0xff, 0x8c,
	0x22, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x09, 0x00, 0x07, 0x80, 0x08, 0x00, 0x00, 0x8c, 0x26, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04, 0x09, 0x00, 0x07, 0x80,
	0x08, 0x00, 0x55, 0x8c, 0x2a, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0xe4, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x09, 0x00, 0x07, 0x80, 0x08, 0x00, 0xaa, 0x8c,
	0x2e, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x09, 0x00, 0x07, 0x80, 0x08, 0x00, 0xff, 0x8c, 0x32, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04, 0x0a, 0x00, 0x07, 0x80,
	0x03, 0x00, 0xe4, 0x80, 0x02, 0x00, 0xff, 0x80, 0x09, 0x00, 0xe4, 0x81,
	0x24, 0x00, 0x00, 0x02, 0x0b, 0x00, 0x07, 0x80, 0x0a, 0x00, 0xe4, 0x80,
	0x08, 0x00, 0x00, 0x03, 0x03, 0x00, 0x08, 0x80, 0x0b, 0x00, 0xe4, 0x80,
	0x05, 0x00, 0xe4, 0x80, 0x02, 0x00, 0x00, 0x03, 0x04, 0x00, 0x08, 0x80,
	0x03, 0x00, 0xff, 0x81, 0x00, 0x00, 0xff, 0xa0, 0x58, 0x00, 0x00, 0x04,
	0x03, 0x00, 0x08, 0x80, 0x03, 0x00, 0xff, 0x80, 0x04, 0x00, 0xff, 0x80,
	0x00, 0x00, 0xff, 0xa0, 0x05, 0x00, 0x00, 0x03, 0x04, 0x00, 0x08, 0x80,
	0x03, 0x00, 0xff, 0x80, 0x03, 0x00, 0xff, 0x80, 0x05, 0x00, 0x00, 0x03,
	0x04, 0x00, 0x08, 0x80, 0x04, 0x00, 0xff, 0x80, 0x04, 0x00, 0xff, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x03, 0x00, 0x08, 0x80, 0x03, 0x00, 0xff, 0x80,
	0x04, 0x00, 0xff, 0x80, 0x04, 0x00, 0x00, 0x04, 0x03, 0x00, 0x08, 0x80,
	0x03, 0x00, 0xff, 0x80, 0x03, 0x00, 0x00, 0xa0, 0x03, 0x00, 0x55, 0xa0,
	0x08, 0x00, 0x00, 0x03, 0x04, 0x00, 0x08, 0x80, 0x04, 0x00, 0xe4, 0x80,
	0x09, 0x00, 0xe4, 0x81, 0x0b, 0x00, 0x00, 0x03, 0x05, 0x00, 0x08, 0x80,
	0x04, 0x00, 0xff, 0x80, 0x00, 0x00, 0xaa, 0xa0, 0x08, 0x00, 0x00, 0x03,
	0x04, 0x00, 0x08, 0x80, 0x04, 0x00, 0xe4, 0x80, 0x0b, 0x00, 0xe4, 0x80,
	0x0b, 0x00, 0x00, 0x03, 0x06, 0x00, 0x08, 0x80, 0x04, 0x00, 0xff, 0x80,
	0x00, 0x00, 0xaa, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x04, 0x00, 0x08, 0x80,
	0x03, 0x00, 0xff, 0x81, 0x00, 0x00, 0xff, 0xa0, 0x05, 0x00, 0x00, 0x03,
	0x04, 0x00, 0x08, 0x80, 0x01, 0x00, 0x00, 0x80, 0x04, 0x00, 0xff, 0x80,
	0x20, 0x00, 0x00, 0x03, 0x09, 0x00, 0x01, 0x80, 0x06, 0x00, 0xff, 0x80,
	0x01, 0x00, 0x55, 0x80, 0x05, 0x00, 0x00, 0x03, 0x03, 0x00, 0x08, 0x80,
	0x03, 0x00, 0xff, 0x80, 0x09, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x04,
	0x09, 0x00, 0x07, 0x80, 0x04, 0x00, 0xff, 0x80, 0x06, 0x00, 0xe4, 0x80,
	0x03, 0x00, 0xff, 0x80, 0x58, 0x00, 0x00, 0x04, 0x0a, 0x00, 0x07, 0x80,
	0x07, 0x00, 0x00, 0x8c, 0x17, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xaa, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x0a, 0x00, 0x07, 0x80, 0x07, 0x00, 0x55, 0x8c,
	0x1b, 0x00, 0xe4, 0xa0, 0x0a, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x07, 0x00, 0x07, 0x80, 0x07, 0x00, 0xaa, 0x8c, 0x1f, 0x00, 0xe4, 0xa0,
	0x0a, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04, 0x07, 0x00, 0x07, 0x80,
	0x07, 0x00, 0xff, 0x8c, 0x23, 0x00, 0xe4, 0xa0, 0x07, 0x00, 0xe4, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x07, 0x00, 0x07, 0x80, 0x08, 0x00, 0x00, 0x8c,
	0x27, 0x00, 0xe4, 0xa0, 0x07, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x07, 0x00, 0x07, 0x80, 0x08, 0x00, 0x55, 0x8c, 0x2b, 0x00, 0xe4, 0xa0,
	0x07, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04, 0x07, 0x00, 0x07, 0x80,
	0x08, 0x00, 0xaa, 0x8c, 0x2f, 0x00, 0xe4, 0xa0, 0x07, 0x00, 0xe4, 0x80,
	0x58, 0x00, 0x00, 0x04, 0x07, 0x00, 0x07, 0x80, 0x08, 0x00, 0xff, 0x8c,
	0x33, 0x00, 0xe4, 0xa0, 0x07, 0x00, 0xe4, 0x80, 0x05, 0x00, 0x00, 0x03,
	0x07, 0x00, 0x07, 0x80, 0x07, 0x00, 0xe4, 0x80, 0x09, 0x00, 0xe4, 0x80,
	0x04, 0x00, 0x00, 0x04, 0x07, 0x00, 0x07, 0x80, 0x07, 0x00, 0xe4, 0x80,
	0x05, 0x00, 0xff, 0x80, 0x02, 0x00, 0xe4, 0x80, 0x58, 0x00, 0x00, 0x04,
	0x02, 0x00, 0x07, 0x80, 0x01, 0x00, 0xff, 0x8c, 0x07, 0x00, 0xe4, 0x80,
	0x02, 0x00, 0xe4, 0x80, 0x02, 0x00, 0x00, 0x03, 0x01, 0x00, 0x04, 0x80,
	0x01, 0x00, 0xaa, 0x80, 0x00, 0x00, 0xff, 0xa0, 0x27, 0x00, 0x00, 0x00,
	0x0f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x80, 0x02, 0x00, 0x00, 0x80,
	0x0f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x02, 0x80, 0x02, 0x00, 0x55, 0x80,
	0x0f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x80, 0x02, 0x00, 0xaa, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0xe4, 0x80,
	0x03, 0x00, 0xaa, 0xa0, 0x0e, 0x00, 0x00, 0x02, 0x00, 0x08, 0x01, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x0e, 0x00, 0x00, 0x02, 0x00, 0x08, 0x02, 0x80,
	0x00, 0x00, 0x55, 0x80, 0x0e, 0x00, 0x00, 0x02, 0x00, 0x08, 0x04, 0x80,
	0x00, 0x00, 0xaa, 0x80, 0x01, 0x00, 0x00, 0x02, 0x00, 0x08, 0x08, 0x80,
	0x00, 0x00, 0xff, 0xa0, 0xff, 0xff, 0x00, 0x00
};

#endif /* UVNORMALFRAGMENTSHADER_PSO_H */
