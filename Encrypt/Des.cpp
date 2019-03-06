#include "Des.h"


//�û�IP��
const static char IP_Table[64] = {
		58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
		62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
		57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,
		61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
};
//���û�IP-1��
const static char IPR_Table[64] = {
		40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,
		38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
		36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,
		34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25
};
//Eλѡ���(��չ�û���)
static const char E_Table[48] = {
		32,1,2,3,4,5,4,5,6,7,8,9,
		8,9,10,11,12,13,12,13,14,15,16,17,
		16,17,18,19,20,21,20,21,22,23,24,25,
		24,25,26,27,28,29,28,29,30,31,32,1
};
//P��λ��(������λ��)
const static char P_Table[32] = {
		16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,
		2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25
};
//PC1ѡλ��(��Կ�����û���1)
const static char PC1_Table[56] = {
		57,49,41,33,25,17,9,1,58,50,42,34,26,18,
		10,2,59,51,43,35,27,19,11,3,60,52,44,36,
		63,55,47,39,31,23,15,7,62,54,46,38,30,22,
		14,6,61,53,45,37,29,21,13,5,28,20,12,4
};
//PC2ѡλ��(��Կ�����û���2)
const static char PC2_Table[48] = {
		14,17,11,24,1,5,3,28,15,6,21,10,
		23,19,12,4,26,8,16,7,27,20,13,2,
		41,52,31,37,47,55,30,40,51,45,33,48,
		44,49,39,56,34,53,46,42,50,36,29,32
};
//����λ���� 
const static char LOOP_Table[16] = {
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
// S��
const static char S_Box[8][4][16] = {
	// S1 
	14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
	0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
	4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
	15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,
	//S2
	15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
	3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
	0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
	13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,
	//S3
	10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
	13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
	13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
	1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,
	//S4
	7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
	13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
	10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
	3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,
	//S5
	2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
	14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
	4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
	11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
	//S6
	12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
	10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
	9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
	4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,
	//S7
	4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
	13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
	1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
	6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,
	//S8
	13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
	1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
	7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
	2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
};

static bool SubKey[16][48];// 16Ȧ����Կ

void ByteToBit(bool *Out, const char *In, int bits)
{
	for (int i = 0; i < bits; i++)
	{
		Out[i] = (In[i / 8] << (i % 8)) & 0x80;
	}
}


void Transform(bool *Out, bool *In, const char *Table, int len)// �任
{
	static bool Temp[256];
	for (int i = 0; i < len; i++) {
		Temp[i] = In[Table[i] - 1];     //������ʾ���ǵڼ�������-1����������±�
	}
	memcpy(Out, Temp, len);
}


void Xor(bool *InA, const bool *InB, int len)// ���
{
	for (int i = 0; i < len; i++)
	{
		InA[i] = InA[i] ^ InB[i];
	}
}


void RotateL(bool *In, int len, int loop)// ѭ������
{
	static bool temp[256];
	memcpy(temp, In, loop);
	memcpy(In, In + loop, len - loop);
	memcpy(In + len - loop, temp, loop);
}

void SetKey(const char Key[8])// ������ԿSubKey
{
	static  bool Key2[64], *C = &Key2[0], *D = &Key2[28];
	ByteToBit(Key2, Key, 64);
	Transform(Key2, Key2, PC1_Table, 56);
	for (int i = 0; i < 16; i++)
	{
		RotateL(C, 28, LOOP_Table[i]);//c����
		RotateL(D, 28, LOOP_Table[i]);//d����
		Transform(SubKey[i], Key2, PC2_Table, 48);
	}
}


void F_func(bool In[32], const bool Ki[48])
{
	static bool Mr[48];
	Transform(Mr, In, E_Table, 48);
	Xor(Mr, Ki, 48);
	S_func(In, Mr);
	Transform(In, In, P_Table, 32);
}


void S_func(bool Out[32], const bool In[48])
{
	for (int i = 0; i < 8; i++)  //8��s��
	{
		int h = 2 * In[i * 6] + 1 * In[i * 6 + 5];
		int l = 8 * In[i * 6 + 1] + 4 * In[i * 6 + 2] + 2 * In[i * 6 + 3] + 1 * In[i * 6 + 4];        //����ط�����д2^2*In[xxx]  ִ��˳������
		int snum = S_Box[i][h][l];
		for (int j = 0; j < 4; j++)
		{
			Out[i * 4 + 3 - j] = snum % 2;
			snum = snum / 2;
		}
	}
}


int DesEncrypt(char* out, char* key, char* text, int text_length)
{
	SetKey(key);
	int out_length = 0, i = 0;
	for (i = 0; i < text_length; i += 8) {
		Des_Run(&out[out_length], &text[i], (text_length - i) >= 8 ? 8 : (text_length - i), false);
		out_length += 16;
	}
	if (i == text_length) {
		Des_Run(&out[out_length], &text[i], 0, false);
		out_length += 16;
	}
	out[out_length + 1] = 0;
	return out_length;
}

int DesDecrypt(char* out, char* key, char* text, int text_length, bool hex)
{
	SetKey(key);
	char newText[8];
	int step = hex ? 16 : 8;
	int out_length = 0, i = 0;
	for (i = 0; i < text_length; i += step) {
		
		int length = 0;
		if (hex) {
			length = (text_length - i) >= 16 ? 16 : (text_length - i);
			if (length & 0x01) { // ����
				length -= 1;
			}
			length >>= 1;
			//printf("%d\n", length);
			for (int j = 0; j < length; j++) {
				//printf("Char:%02X %c%c\n", Hex2Char(text[i + j * 2], text[i + j * 2 + 1]) & 0xff, text[i + j * 2], text[i + j * 2 + 1]);
				newText[j] = Hex2Char(text[i + j * 2], text[i + j * 2 + 1]);
			}
		}
		else {
			length = (text_length - i) >= 8 ? 8 : (text_length - i);
			memcpy(newText, text, length);
		}
		Des_Run(&out[out_length], newText, length, true);
		out_length += 8;
	}
	out[out_length + 1] = 0;
	return out_length;
}

void Des_Run(char Out[16], char In[8], int InLength, bool decode)
{
	char text[8];
	memcpy(text, In, InLength);
	if (InLength < 8) {
		memset(&text[InLength], 8 - InLength, 8 - InLength);
	}
	
	static bool Mingwen[64], temp[32], *Li = &Mingwen[0], *Ri = &Mingwen[32];
	ByteToBit(Mingwen, text, 64); //����ת2����
	Transform(Mingwen, Mingwen, IP_Table, 64);   //�û�ip
	int i, m, n;
	for (i = 0; i < 15; i++)     //ǰ15��
	{
		memcpy(temp, Ri, 32);
		F_func(Ri, decode ? SubKey[15 - i] : SubKey[i]);
		Xor(Ri, Li, 32);
		memcpy(Li, temp, 32);
	}
	memcpy(temp, Ri, 32);        //�����16��1
	F_func(temp, decode ? SubKey[0] : SubKey[i]);
	Xor(Li, temp, 32);

	Transform(Mingwen, Mingwen, IPR_Table, 64); //���û�
	if (!decode) { // ����
		for (i = 0; i < 16; i++)
		{
			Out[i] = 8 * Mingwen[0 + i * 4] + 4 * Mingwen[1 + i * 4] + 2 * Mingwen[2 + i * 4] + 1 * Mingwen[3 + i * 4];
			sprintf(&Out[i], "%01x", Out[i] & 0xf);
		}
	}
	else { // ����
		for (i = 0, m = 0; i < 64; i += 8, m++) {
			Out[m] = 0;
			for (n = 0; n < 8; n++) {
				if (Mingwen[i + n]) {
					//printf("M:%d ", Mingwen[i + n]);
					Out[m] |= (1 << (7 - n)) & 0xff;
				}
			}
			if (Out[m] <= 8) {
				Out[m] = 0;
				break;
			}
		}
	}
	
}

void ShowInHex(const bool In[64])
{
	unsigned int Hex[16] = { 0 };
	int i = 0;
	for (i = 0; i < 16; i++)
	{
		Hex[i] = 8 * In[0 + i * 4] + 4 * In[1 + i * 4] + 2 * In[2 + i * 4] + 1 * In[3 + i * 4];
	}

	for (i = 0; i < 16; i++)
	{
		printf("%x", Hex[i]);
	}

	printf("\n");
}

char Hex2Char(char ch, char ch2)
{
	return Char2Num(ch) * 16 + Char2Num(ch2);
}

char Char2Num(char ch)
{
	if (ch >= '0' && ch <= '9') {
		ch = ch - '0';
	}
	else if (ch >= 'A' && ch <= 'F') {
		ch = ch - 'A' + 10;
	}
	else if (ch >= 'a' && ch <= 'f') {
		ch = ch - 'a' + 10;
	}
	else {
		ch = 0;
	}
	return ch;
}