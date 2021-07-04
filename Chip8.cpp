#include <stdio.h>
#include <time.h>
#include <chrono>

#include "Chip8.h"

#include "SDL_scancode.h"
#include "SDL_keyboard.h"

Chip8::Chip8()
{
	memset(ram, 0, sizeof(ram));
	memset(frame_buffer, 0, sizeof(frame_buffer));
	memset(V, 0, sizeof(V));
	
	const char* hex_font[16 * 5] = 
	{
	"****",   "  * ",   "****",   "****",   "*  *",   "****",   "****",   "****",   "****",   "****",   "****",   "*** ",   "****",   "*** ",   "****",   "****",
	"*  *",   " ** ",   "   *",   "   *",   "*  *",   "*   ",   "*   ",   "   *",   "*  *",   "*  *",   "*  *",   "*  *",   "*   ",   "*  *",   "*   ",   "*   ",
	"*  *",   "  * ",   "****",   "****",   "****",   "****",   "****",   "  * ",   "****",   "****",   "****",   "*** ",   "*   ",   "*  *",   "****",   "****",
	"*  *",   "  * ",   "*   ",   "   *",   "   *",   "   *",   "*  *",   " *  ",   "*  *",   "   *",   "*  *",   "*  *",   "*   ",   "*  *",   "*   ",   "*   ",
	"****",   " ***",   "****",   "****",   "   *",   "****",   "****",   " *  ",   "****",   "****",   "*  *",   "*** ",   "****",   "*** ",   "****",   "*   ",
	};

	for (char k = 0; k < 16; ++k)
	{
		for (char f = 0; f < 5; f++)
		{
			for (char i = 3; i >= 0; --i)
			{
				ram[k * 5 + f] |= hex_font[f * 16 + k][i] == '*' ? (1 << 7 - i) : 0;
			}
		}
	}
	
	for(int i=0;i<5;++i)
		std::cout << std::hex << (short)ram[i] << std::endl;
}

void Chip8::setKeyMap(const char* k)
{
	keys = k;
}

void Chip8::loadRom(const std::string& path)
{
	FILE* file = nullptr;
	fopen_s(&file, path.c_str(), "rb");

	if (!file)
	{
		std::cout << "Failed to open file" << path << '\n';
		return;
	}
	
	program_bytes = fread(ram + prg_start, 1, ram_bytes - prg_start, file);

	fclose(file);
}

void Chip8::update()
{	
	data = (((short)ram[pc]) << 8) | (0x00ff & ram[pc + 1]);

	using namespace std::chrono;

	static auto t1 = high_resolution_clock::now();

	shouldDraw = false;
	callInstruction();
	pc+=2;

	auto t2 = high_resolution_clock::now();

	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	if (time_span.count() >= 1.0 / 60.0)
	{
		t1 = high_resolution_clock::now();
		dt -= dt > 0 ? 1 : 0;
		st -= st > 0 ? 1 : 0;
	}
}

void Chip8::callInstruction()
{
	x = data >> 8 & 0xf;
	y = data >> 4 & 0xf;
	kk = data & 0xff;
	n = data & 0xf;

	nnn = data & 0xfff;
	
	switch (data >> 12 & 0x000f)
	{
	case 0x0:
	{
		switch (data)
		{
		case 0x00ee: i_00ee(); break;
		case 0x00e0: i_00e0(); break;
		}
	}
	break;
	case 0x1: pc = nnn - 2; break;
	case 0x2: i_2nnn();  break;
	case 0x3: pc += (V[x] == kk) * 2;  break;
	case 0x4: pc += (V[x] != kk) * 2;  break;
	case 0x5: pc += (V[x] == V[y]) * 2;  break;
	case 0x6: V[x] = kk;  break;
	case 0x7: V[x] += kk;  break;
	case 0x8:
	{
		switch (data & 0xf)
		{
		case 0x0: V[x] = V[y]; break;
		case 0x1: V[x] |= V[y]; break;
		case 0x2: V[x] &= V[y]; break;
		case 0x3: V[x] ^= V[y]; break;
		case 0x4: i_8xy4(); break;
		case 0x5: i_8xy5(); break;
		case 0x6: i_8xy6(); break;
		case 0x7: i_8xy7(); break;
		case 0xE: i_8xyE(); break;
		}
	}
	break;
	case 0x9: pc += (V[x] != V[y]) * 2;	break;
	case 0xA: I = nnn; break;
	case 0xB: pc = nnn + V[0] - 2; break;
	case 0xC: i_Cxkk(); break;
	case 0xD: i_Dxyn(); break;
	case 0xE:
	{
		switch (data & 0xff)
		{
		case 0x9E: i_Ex9E();  break;
		case 0xA1: i_ExA1();  break;
		}
	}
	break;
	case 0xF: 
	{
		switch (data & 0xff)
		{
		case 0x07: V[x] = dt; break;
		case 0x0A: i_Fx0A();  break;
		case 0x15: dt = V[x]; break;
		case 0x18: st = V[x]; break;
		case 0x1E: I += V[x]; break;
		case 0x29: I = V[x] * 5; break;
		case 0x33: i_Fx33(); break;
		case 0x55: i_Fx55();  break;
		case 0x65: i_Fx65(); break;
		}
	}
	break;
	}
}

inline void Chip8::i_00ee()
{
	pc = stack.top();
	stack.pop();
}

inline void Chip8::i_00e0()
{
	memset(frame_buffer, 0, sizeof(frame_buffer));
}

inline void Chip8::i_2nnn()
{
	stack.push(pc);
	pc = nnn - 2;
}

inline void Chip8::i_Cxkk()
{
	V[x] = char(rand() % 256) & kk;
}

inline void Chip8::i_Dxyn()
{
	shouldDraw = true;
	V[15] = 0;

	for (char f = 0; f < n; ++f)
	{
		for (char i = 0; i < 8; i++)
		{
			char d = (ram[I + f] >> (7 - i)) & 0x1;

			char posx = (V[x] + i) % buffer_width;
			char posy = (V[y] + f) % buffer_height;

			if (frame_buffer[posy * buffer_width + posx] && d)
				V[15] = 1;

			if (d)
				frame_buffer[posy * buffer_width + posx] ^= 0xff;
			else
				frame_buffer[posy * buffer_width + posx] ^= 0;
		}
	}
}

inline void Chip8::i_Ex9E()
{
	char kname = V[x] < 0xA ? '0' + V[x] : 'A' + V[x] - 10;

	char test[2] = { kname, '\0' };

	if (keys[SDL_GetScancodeFromName(test)])
		pc += 2;
}

inline void Chip8::i_ExA1()
{
	char kname = V[x] < 0xA ? '0' + V[x] : 'A' + V[x] - 10;

	char test[2] = { kname, '\0' };

	if (!keys[SDL_GetScancodeFromName(test)])
		pc += 2;
}

inline void Chip8::i_8xy4()
{
	if (V[x] + V[y] > 255)
		V[15] = 1;
	else
		V[15] = 0;

	V[x] += V[y];
}

inline void Chip8::i_8xy5()
{
	if(V[x] > V[y])
		V[15] = 1;
	else
		V[15] = 0;
	
	V[x] -= V[y];
}

inline void Chip8::i_8xy6()
{
	if(V[x] & 0x1)
		V[15] = 1;
	else
		V[15] = 0;
	
	V[x] /= 2;
}

inline void Chip8::i_8xy7()
{
	if (V[y] > V[x])
		V[15] = 1;
	else
		V[15] = 0;
	
	V[x] = V[y] - V[x];
}

inline void Chip8::i_8xyE()
{
	if(V[x] >> 7 & 0x1)
		V[15] = 1;
	else
		V[15] = 0;

	V[x] *= 2;
}

inline void Chip8::i_Fx0A()
{
	for (char i = 0; i < 16; ++i)
	{
		if (heldkeys[i])
		{
			V[x] = i;
			return;
		}
	}
	pc -= 2;
}

inline void Chip8::i_Fx33()
{
	unsigned char tx = V[x];
		
	ram[I + 0] = tx / 100;
	tx -= ram[I + 0] * 100;

	ram[I + 1] = tx / 10;
	tx -= ram[I + 1] * 10;

	ram[I + 2] = tx;
}

inline void Chip8::i_Fx55()
{
	memcpy(ram + I, V, x + 1);
}

inline void Chip8::i_Fx65()
{
	memcpy(V, ram + I, x + 1);
}