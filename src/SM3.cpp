#include "SM3.h"

void SM3::CF(const u8* block) {
	// DumpU32((u32*)block, 16);
    u32 W[68];
    u32 W_1[64];
    u32 A, B, C, D, E, F, G, H;
	u32 SS1, SS2, TT1, TT2;

    // 消息扩展
    for (int j = 0; j < 16; j++) {
		// W[j] = block[j * 4 + 0] << 24 | block[j * 4 + 1] << 16 | block[j * 4 + 2] << 8 | block[j * 4 + 3];
		W[j] = EndianSwitch(((u32*)block)[j]);
    }
	for (int j = 16; j < 68; j++) {
		W[j] = P_1(W[j - 16] ^ W[j - 9] ^ RoLU32(W[j - 3], 15)) ^ RoLU32(W[j - 13], 7) ^ W[j - 6];
    }
	for (int j = 0; j < 64; j++) {
		W_1[j] = W[j] ^ W[j + 4];
    }

    // 压缩
    A = this->buf[0];
	B = this->buf[1];
	C = this->buf[2];
	D = this->buf[3];
	E = this->buf[4];
	F = this->buf[5];
	G = this->buf[6];
	H = this->buf[7];
	for (int j = 0; j < 64; j++) {
		SS1 = RoLU32(((RoLU32(A, 12)) + E + (RoLU32(T(j), j))) & 0xFFFFFFFF, 7);
		SS2 = SS1 ^ (RoLU32(A, 12));
		TT1 = (FF(j, A, B, C) + D + SS2 + W_1[j]) & 0xFFFFFFFF;
		TT2 = (GG(j, E, F, G) + H + SS1 + W[j]) & 0xFFFFFFFF;
		D = C;
		C = RoLU32(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = RoLU32(F, 19);
		F = E;
		E = P_0(TT2);
	}

    this->buf[0] ^= A;
	this->buf[1] ^= B;
	this->buf[2] ^= C;
	this->buf[3] ^= D;
	this->buf[4] ^= E;
	this->buf[5] ^= F;
	this->buf[6] ^= G;
	this->buf[7] ^= H;
}

void SM3::Padding(const u8* block, int totalSize) {
	int left = totalSize % 64;
	u8 blockBuf[64] = {};
	u64 bitLength = totalSize * 8LL;
	memcpy(blockBuf, block, left);
	blockBuf[left] = 0x80;

	// 判断当前短块是否能放下，若放不下则新加一个块，先对当前块进行压缩，将bitLength放在下一个块中
	if (left >= 56) {
		CF(blockBuf);
		memset(blockBuf, 0, sizeof(blockBuf));
	}

	for (int i = 0; i < 8; i++) {
		blockBuf[56 + i] = (bitLength >> ((8 - 1 - i) * 8)) & 0xFF;
	}
	CF(blockBuf);
}

std::string SM3::Hash(const u8* arr, u32 length) {
	InitIV();

	for (u32 i = 0; i < length / 64; i++) {
		CF(arr + i * 64);
	}

	Padding(arr + (length / 64) * 64, length);

	return DumpHashStr();
}

std::string SM3::Hash(const std::string& path) {
	InitIV();

	std::ifstream src(path, std::ios::in | std::ios::binary);
	if (!src) {
		std::cerr << "No such file: " << path << std::endl;
		return "No such file: " + path;
	}
	src.seekg(0, std::ios::end);
	u64 size = src.tellg();
	src.seekg(0, std::ios::beg);

	u8 blockBuf[64] = {};
	for (u64 i = 0; i < size / 64; i++) {
		src.read((char*)blockBuf, 64);
		CF(blockBuf);
	}

	src.read((char*)blockBuf, 64);
	Padding(blockBuf, size);

	return DumpHashStr();
}
