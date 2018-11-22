#ifndef _BINARYREADSTREAM_H_
#define _BINARYREADSTREAM_H_

#define BINARY_PACKLEN_LEN_2 sizeof(unsigned int)
#define CHECKSUM_LEN 4
#include <cstdint>
#include <WinSock2.h>

#include <string>
using std::string;

//
void ErrorHandling(char *message);
//读取指定数量的字节,成功返回true,失败返回false,errNo中是错误码
bool readAssignedAmountBytes(SOCKET hSocket,char * buf, int size, DWORD & errNo);
//

//计算校验和
unsigned short checksum(const unsigned short *buffer, int size);
//压缩一个int
bool compress_(unsigned int i, char *buf, size_t &len);
//解压成int
bool uncompress_(char *buf, size_t len, unsigned int &i);



class BinaryReadStream // : public IReadStream
{
private:

    const char* const ptr;

    const size_t      len;

    const char*       cur;

    BinaryReadStream(const BinaryReadStream&);

    BinaryReadStream& operator=(const BinaryReadStream&);

public:

    BinaryReadStream(const char* ptr, size_t len);

    const char* GetData() const;

    size_t GetSize() const;

    bool IsEmpty() const;

    bool ReadString(string* str, size_t maxlen, size_t& outlen);

    bool ReadCString(char* str, size_t strlen, size_t& len);

    bool ReadCCString(const char** str, size_t maxlen, size_t& outlen);

    bool ReadInt32(int32_t& i);

    bool ReadInt64(int64_t& i);

    bool ReadShort(short& i);

    bool ReadChar(char& c);

    size_t ReadAll(char* szBuffer, size_t iLen) const;

    bool IsEnd() const;

    const char* GetCurrent() const{ return cur; }

public:

    bool ReadLength(size_t & len);

    bool ReadLengthWithoutOffset(size_t &headlen, size_t & outlen);

};

//д

class BinaryWriteStream
{

public:

    BinaryWriteStream(string* data);

    const char* GetData() const;

    size_t GetSize() const;

    bool WriteCString(const char* str, size_t len);

    bool WriteString(const string& str);

    bool WriteDouble(double value, bool isNULL = false);

    bool WriteInt64(int64_t value, bool isNULL = false);

    bool WriteInt32(int32_t i, bool isNULL = false);

    bool WriteShort(short i, bool isNULL = false);

    bool WriteChar(char c, bool isNULL = false);

    size_t GetCurrentPos() const;

    void Flush();

    void Clear();

private:

    string* m_data;

};

#endif
