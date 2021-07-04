#include <string>
#include <iostream>
#include <stack>

#pragma once
class Chip8
{
private:
	static const short ram_bytes = 4096;
	static const short prg_start = 0x200;
public:
	static const int buffer_width = 64;
	static const int buffer_height = 32;
	static const int buffer_size = buffer_width * buffer_height;

	unsigned char frame_buffer[buffer_size];

	bool heldkeys[16];

private:
	//statics
	const char* keys;
	
	unsigned char ram[ram_bytes];
	unsigned char V[16];	
	
	int program_bytes;

	unsigned short I=0;
	unsigned short pc = prg_start;
	
	//temp data
	unsigned short data;
	unsigned char x,y,kk,n;
	unsigned short nnn;

	std::stack<unsigned short> stack;
public:
	unsigned char dt = 0, st = 0;

	bool shouldDraw = false;
	Chip8();

	inline void i_00ee();
	inline void i_00e0();

	inline void i_2nnn();

	inline void i_Cxkk();
	inline void i_Dxyn();

	inline void i_Ex9E();
	inline void i_ExA1();

	inline void i_8xy4();
	inline void i_8xy5();
	inline void i_8xy6();
	inline void i_8xy7();
	inline void i_8xyE();

	inline void i_Fx0A();
	inline void i_Fx33();
	inline void i_Fx55();
	inline void i_Fx65();

	void setKeyMap(const char* keys);
	void loadRom(const std::string& path);
	void update();

	void callInstruction();
};