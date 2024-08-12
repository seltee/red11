#ifndef VERTEXSHADER_VSO_H
#define VERTEXSHADER_VSO_H

static const unsigned char VertexShader_vso[] = {
	0x00, 0x03, 0xfe, 0xff, 0xfe, 0xff, 0x43, 0x00, 0x43, 0x54, 0x41, 0x42,
	0x1c, 0x00, 0x00, 0x00, 0xd7, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xff,
	0x04, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0xd0, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0x00,
	0x10, 0x00, 0x42, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x94, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00,
	0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x08, 0x00, 0x03, 0x00, 0x22, 0x00, 0x9c, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00,
	0x04, 0x00, 0x12, 0x00, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x4c, 0x69, 0x67, 0x68, 0x74, 0x73, 0x53, 0x68, 0x61, 0x64, 0x6f, 0x77,
	0x4d, 0x61, 0x74, 0x72, 0x69, 0x63, 0x69, 0x65, 0x73, 0x00, 0xab, 0xab,
	0x03, 0x00, 0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x00, 0xab, 0xab,
	0x03, 0x00, 0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x49, 0x6e, 0x76,
	0x65, 0x72, 0x73, 0x65, 0x54, 0x72, 0x61, 0x6e, 0x73, 0x70, 0x6f, 0x73,
	0x65, 0x00, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x56, 0x69, 0x65, 0x77, 0x50,
	0x72, 0x6f, 0x6a, 0x00, 0x76, 0x73, 0x5f, 0x33, 0x5f, 0x30, 0x00, 0x4d,
	0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29,
	0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72,
	0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x39, 0x2e,
	0x32, 0x39, 0x2e, 0x39, 0x35, 0x32, 0x2e, 0x33, 0x31, 0x31, 0x31, 0x00,
	0x51, 0x00, 0x00, 0x05, 0x03, 0x00, 0x0f, 0xa0, 0x00, 0x00, 0x80, 0x3f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xbf,
	0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0x90,
	0x1f, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x80, 0x01, 0x00, 0x0f, 0x90,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80, 0x02, 0x00, 0x0f, 0x90,
	0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x02, 0x80, 0x01, 0x00, 0x07, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x01, 0x80, 0x02, 0x00, 0x07, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80, 0x03, 0x00, 0x03, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x03, 0x80, 0x04, 0x00, 0x07, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x04, 0x80, 0x05, 0x00, 0x07, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x05, 0x80, 0x06, 0x00, 0x07, 0xe0,
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x06, 0x80, 0x07, 0x00, 0x07, 0xe0,
	0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x24, 0x90,
	0x03, 0x00, 0x40, 0xa0, 0x03, 0x00, 0x15, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0xe4, 0x80, 0x04, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0xe0, 0x00, 0x00, 0xe4, 0x80,
	0x05, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x04, 0xe0,
	0x00, 0x00, 0xe4, 0x80, 0x06, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x08, 0xe0, 0x00, 0x00, 0xe4, 0x80, 0x07, 0x00, 0xe4, 0xa0,
	0x08, 0x00, 0x00, 0x03, 0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x90,
	0x08, 0x00, 0xe4, 0xa0, 0x08, 0x00, 0x00, 0x03, 0x01, 0x00, 0x02, 0x80,
	0x01, 0x00, 0xe4, 0x90, 0x09, 0x00, 0xe4, 0xa0, 0x08, 0x00, 0x00, 0x03,
	0x01, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x0a, 0x00, 0xe4, 0xa0,
	0x08, 0x00, 0x00, 0x03, 0x01, 0x00, 0x08, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x07, 0x00, 0x00, 0x02, 0x01, 0x00, 0x08, 0x80,
	0x01, 0x00, 0xff, 0x80, 0x05, 0x00, 0x00, 0x03, 0x02, 0x00, 0x07, 0xe0,
	0x01, 0x00, 0xff, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x09, 0x00, 0x00, 0x03,
	0x01, 0x00, 0x01, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0x00, 0x03, 0x01, 0x00, 0x02, 0x80, 0x00, 0x00, 0xe4, 0x80,
	0x01, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x01, 0x00, 0x04, 0x80,
	0x00, 0x00, 0xe4, 0x80, 0x02, 0x00, 0xe4, 0xa0, 0x01, 0x00, 0x00, 0x02,
	0x01, 0x00, 0x07, 0xe0, 0x01, 0x00, 0xe4, 0x80, 0x01, 0x00, 0x00, 0x02,
	0x03, 0x00, 0x03, 0xe0, 0x02, 0x00, 0xe4, 0x90, 0x01, 0x00, 0x00, 0x02,
	0x01, 0x00, 0x08, 0x80, 0x03, 0x00, 0x00, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x13, 0x00, 0xe4, 0xa0,
	0x06, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x80,
	0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x10, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x02, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x11, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x12, 0x00, 0xe4, 0xa0,
	0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x80,
	0x02, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x07, 0xe0,
	0x00, 0x00, 0xe4, 0x80, 0x03, 0x00, 0xce, 0xa0, 0x03, 0x00, 0xda, 0xa0,
	0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x17, 0x00, 0xe4, 0xa0, 0x06, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x01, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x14, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x15, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x16, 0x00, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04,
	0x05, 0x00, 0x07, 0xe0, 0x00, 0x00, 0xe4, 0x80, 0x03, 0x00, 0xce, 0xa0,
	0x03, 0x00, 0xda, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x1b, 0x00, 0xe4, 0xa0, 0x06, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x80, 0x09, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x18, 0x00, 0xe4, 0xa0,
	0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x02, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x19, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x04, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x1a, 0x00, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0xe4, 0x80,
	0x04, 0x00, 0x00, 0x04, 0x06, 0x00, 0x07, 0xe0, 0x00, 0x00, 0xe4, 0x80,
	0x03, 0x00, 0xce, 0xa0, 0x03, 0x00, 0xda, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x1f, 0x00, 0xe4, 0xa0,
	0x06, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x80,
	0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x01, 0x80, 0x01, 0x00, 0xe4, 0x80,
	0x1c, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03, 0x02, 0x00, 0x02, 0x80,
	0x01, 0x00, 0xe4, 0x80, 0x1d, 0x00, 0xe4, 0xa0, 0x09, 0x00, 0x00, 0x03,
	0x02, 0x00, 0x04, 0x80, 0x01, 0x00, 0xe4, 0x80, 0x1e, 0x00, 0xe4, 0xa0,
	0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x80,
	0x02, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04, 0x07, 0x00, 0x07, 0xe0,
	0x00, 0x00, 0xe4, 0x80, 0x03, 0x00, 0xce, 0xa0, 0x03, 0x00, 0xda, 0xa0,
	0xff, 0xff, 0x00, 0x00
};

#endif /* VERTEXSHADER_VSO_H */