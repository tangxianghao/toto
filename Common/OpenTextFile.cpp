#include "OpenTextFile.h"
#include "mystring.h"

bool OpenTextFile::Open(const char * file)
{
	m_File = fopen(file, "r");
	return m_File != nullptr;
}

void OpenTextFile::Close()
{
	if (m_File)
		fclose(m_File);
}

int OpenTextFile::GetLine(char* data, int maxCount)
{
	while (!feof(m_File)) {
		if (fgets(data, maxCount, m_File)) {
			//printf("����:%s %d\n", data, data[0]);
			if (data[0] == '#' || data[0] == ';') {
				return 0;
			}
			return strlen((data));
		}
		break;
	}
	return -1;
}

int OpenTextFile::GetCount()
{
	if (m_IsGetCount)
		return m_Count;

	rewind(m_File);
	char data[256];
	while (!feof(m_File)) {
		if (fgets(data, 256, m_File)) {
			if (data[0] != '#' && data[0] != ';') {
				m_Count++;
			}
		}
	}
	rewind(m_File);
	m_IsGetCount = true;
	return m_Count;
}

void OpenTextFile::ReWind()
{
	if (m_File)
		rewind(m_File);
}
