#include "MyBinaryReadStream.h"
#include <assert.h>

//计算校验和
unsigned short checksum(const unsigned short *buffer, int size)
{
    unsigned int cksum = 0;
    while (size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if (size)
    {
        cksum += *(unsigned char*)buffer;
    }
    //将32位数转换成16
    while (cksum >> 16)
    {
        //高16位加低16位
        cksum = (cksum >> 16) + (cksum & 0xffff);
    }
    return (unsigned short)(~cksum);
}

bool CheckData(const char * data, int size)
{
	//校验和
	data += BINARY_PACKLEN_LEN_2;
	unsigned short checksum_len = *((unsigned short*)data);
	unsigned short checksum_len1;
	memcpy(&checksum_len1,data ,CHECKSUM_LEN);
	unsigned short tmp1 = *data;
	unsigned int tmp2 = *data;
	unsigned long long tmp3 = *data;
	data += CHECKSUM_LEN;


	unsigned short checksum_len2 = checksum((const unsigned short*)data, size - BINARY_PACKLEN_LEN_2 - CHECKSUM_LEN);
	if (checksum_len == checksum_len2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool compress_(unsigned int i, char *buf, size_t &len)
{
    len = 0;
    for (int a = 4; a >= 0; a--)
    {
        char c;
        c = i >> (a * 7) & 0x7f;
        if (c == 0x00 && len == 0)
            continue;
        if (a == 0)
            c &= 0x7f;      //标记最高位为0
        else
            c |= 0x80;      //标记最高位为1
        buf[len] = c;
        len++;
    }
    if (len == 0)
    {
        len++;
        buf[0] = 0;
    }
    //cout << "compress:" << i << endl;
    //cout << "compress len:" << len << endl;
    return true;
}
bool uncompress_(char *buf, size_t len, unsigned int &i)
{
    i = 0;
    for (int index = 0; index < (int)len; index++)
    {
        char c = *(buf + index);
        i = i << 7;
        c &= 0x7f;
        i |= c;
    }
    //cout << "uncompress:" << i << endl;
    return true;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

bool readAssignedAmountBytes(SOCKET hSocket, char * buf, int size, DWORD & errNo)
{
    int recvLen = 0;
    int recvCnt = 0;
    recvCnt += recvLen;
    buf += recvLen;
    while (recvCnt < size)
    {
        recvLen = recv(hSocket, buf, size - recvLen,0);
        if (recvLen == 0)
        {
            errNo = GetLastError();
            return false;
        }
        else if (recvLen < 0)
        {
            errNo = GetLastError();
            return false;
        }
        recvCnt += recvLen;
        buf += recvLen;

    }
    return true;
}

MyBinaryReadStream::MyBinaryReadStream(const char* ptr_, size_t len_)
: m_ptr(ptr_), m_len(len_), m_cur(ptr_)
{
    m_cur += BINARY_PACKLEN_LEN_2 + CHECKSUM_LEN;

}
bool MyBinaryReadStream::IsEmpty() const
{
    return m_len <= BINARY_PACKLEN_LEN_2;
}
size_t MyBinaryReadStream::GetSize() const
{
    return m_len;
}
bool MyBinaryReadStream::ReadCString(char* str, size_t strlen, /* out */ size_t& outlen)
{
    size_t fieldlen;
    size_t headlen;
    if (!ReadLengthWithoutOffset(headlen, fieldlen)) {
        return false;
    }
    // user buffer is not enough
    if (fieldlen > strlen) {
        return false;
    }
    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;    
    m_cur += headlen;
    if (m_cur + fieldlen > m_ptr + m_len)
    {
        outlen = 0;
        return false;
    }
    memcpy(str, m_cur, fieldlen);
    outlen = fieldlen;
    m_cur += outlen;
    return true;
}
bool MyBinaryReadStream::ReadString(string* str, size_t maxlen, size_t& outlen)
{
    size_t headlen;
    size_t fieldlen;
    if (!ReadLengthWithoutOffset(headlen, fieldlen)) {
        return false;
    }
    // user buffer is not enough
    if (maxlen != 0 && fieldlen > maxlen) {
        return false;
    }
    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;    
    m_cur += headlen;
    if (m_cur + fieldlen > m_ptr + m_len)
    {
        outlen = 0;
        return false;
    }
    str->assign(m_cur, fieldlen);
    outlen = fieldlen;
    m_cur += outlen;
    return true;
}
bool MyBinaryReadStream::ReadCCString(const char** str, size_t maxlen, size_t& outlen)
{
    size_t headlen;
    size_t fieldlen;
    if (!ReadLengthWithoutOffset(headlen, fieldlen)) {
        return false;
    }
    // user buffer is not enough
    if (maxlen != 0 && fieldlen > maxlen) {
        return false;
    }
    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;    
    m_cur += headlen;
    //memcpy(str, cur, fieldlen);
    if (m_cur + fieldlen > m_ptr + m_len)
    {
        outlen = 0;
        return false;
    }
    *str = m_cur;
    outlen = fieldlen;
    m_cur += outlen;
    return true;
}
bool MyBinaryReadStream::ReadInt32(int32_t& i)
{
    const int VALUE_SIZE = sizeof(int32_t);
    if (m_cur + VALUE_SIZE > m_ptr + m_len)
        return false;
    memcpy(&i, m_cur, VALUE_SIZE);
	//不用转字节序
    //i = ntohl(i);
    m_cur += VALUE_SIZE;
    return true;
}
bool MyBinaryReadStream::ReadInt64(int64_t& i)
{
    char int64str[128];
    size_t length;
    if (!ReadCString(int64str, 128, length))
        return false;
    i = atoll(int64str);
    return true;
}
bool MyBinaryReadStream::ReadShort(short& i)
{
    const int VALUE_SIZE = sizeof(short);
    if (m_cur + VALUE_SIZE > m_ptr + m_len) {
        return false;
    }
    memcpy(&i, m_cur, VALUE_SIZE);
	//不用转字节序
    //i = ntohs(i);
    m_cur += VALUE_SIZE;
    return true;
}
bool MyBinaryReadStream::ReadChar(char& c)
{
    const int VALUE_SIZE = sizeof(char);
    if (m_cur + VALUE_SIZE > m_ptr + m_len) {
        return false;
    }
    memcpy(&c, m_cur, VALUE_SIZE);
    m_cur += VALUE_SIZE;
    return true;
}
bool MyBinaryReadStream::ReadLength(size_t & outlen)
{
    size_t headlen;
    if (!ReadLengthWithoutOffset(headlen, outlen)) {
        return false;
    }
    //cur += BINARY_PACKLEN_LEN_2;
    m_cur += headlen;
    return true;
}
bool MyBinaryReadStream::ReadLengthWithoutOffset(size_t& headlen, size_t & outlen)
{
    headlen = 0;
    const char *temp = m_cur;
    char buf[5];
    for (size_t i = 0; i<sizeof(buf); i++)
    {
        memcpy(buf + i, temp, sizeof(char));
        temp++;
        headlen++;
        //if ((buf[i] >> 7 | 0x0) == 0x0)
        if ((buf[i] & 0x80) == 0x00)
            break;
    }
    if (m_cur + headlen > m_ptr + m_len)
        return false;
    unsigned int value;
    uncompress_(buf, headlen, value);
    outlen = value;
    /*if ( cur + BINARY_PACKLEN_LEN_2 > ptr + len ) {
    return false;
    }
    unsigned int tmp;
    memcpy(&tmp, cur, sizeof(tmp));
    outlen = ntohl(tmp);*/
    return true;
}
bool MyBinaryReadStream::IsEnd() const
{
    assert(m_cur <= m_ptr + m_len);
    return m_cur == m_ptr + m_len;
}
const char* MyBinaryReadStream::GetData() const
{
    return m_ptr;
}
size_t MyBinaryReadStream::ReadAll(char * szBuffer, size_t iLen) const
{
    size_t iRealLen = min(iLen, m_len);
    memcpy(szBuffer, m_ptr, iRealLen);
    return iRealLen;
}

//=================class BinaryWriteStream implementation============//
MyBinaryWriteStream::MyBinaryWriteStream(string *data) :
m_data(data)
{
    m_data->clear();
    char str[BINARY_PACKLEN_LEN_2 + CHECKSUM_LEN];
    m_data->append(str, sizeof(str));
}
bool MyBinaryWriteStream::WriteCString(const char* str, size_t len)
{
    char buf[5];
    size_t buflen;
    compress_(len, buf, buflen);
    m_data->append(buf, sizeof(char)*buflen);
    m_data->append(str, len);
    //unsigned int ulen = htonl(len);
    //m_data->append((char*)&ulen,sizeof(ulen));
    //m_data->append(str,len);
    return true;
}
bool MyBinaryWriteStream::WriteString(const string& str)
{
    return WriteCString(str.c_str(), str.length());
}
const char* MyBinaryWriteStream::GetData() const
{
    return m_data->data();
}
size_t MyBinaryWriteStream::GetSize() const
{
    return m_data->length();
}
bool MyBinaryWriteStream::WriteInt32(int32_t i, bool isNULL)
{
    m_data->append((char*)&i, sizeof(i));
    return true;
}
bool MyBinaryWriteStream::WriteInt64(int64_t value, bool isNULL)
{
    char int64str[128];
    if (isNULL == false)
    {
#ifndef _WIN32
        sprintf_s(int64str,sizeof(int64str), "%ld", value);
#else
        sprintf_s(int64str, sizeof(int64str), "%lld", value);
#endif
        WriteCString(int64str, strlen(int64str));
    }
    else
        WriteCString(int64str, 0);
    return true;
}
bool MyBinaryWriteStream::WriteShort(short i, bool isNULL)
{
    m_data->append((char*)&i, sizeof(i));
    return true;
}
bool MyBinaryWriteStream::WriteChar(char c, bool isNULL)
{
    (*m_data) += c;
    return true;
}
bool MyBinaryWriteStream::WriteDouble(double value, bool isNULL)
{
    char   doublestr[128];
    if (isNULL == false)
    {
        sprintf_s(doublestr,sizeof(doublestr), "%f", value);
        WriteCString(doublestr, strlen(doublestr));
    }
    else
        WriteCString(doublestr, 0);
    return true;
}
void MyBinaryWriteStream::Flush()
{
	//最开始填写长度
    char *ptr = &(*m_data)[0];
    unsigned int ulen = m_data->length();
    memcpy(ptr, &ulen, sizeof(ulen));

	//填写校验和
	unsigned short checksum_len = checksum((const unsigned short*)(ptr+BINARY_PACKLEN_LEN_2+CHECKSUM_LEN), ulen-BINARY_PACKLEN_LEN_2-CHECKSUM_LEN);

	memcpy(ptr + BINARY_PACKLEN_LEN_2, &checksum_len, CHECKSUM_LEN);

}
void MyBinaryWriteStream::Clear()
{
    m_data->clear();
    char str[BINARY_PACKLEN_LEN_2 + CHECKSUM_LEN];
    m_data->append(str, sizeof(str));
}

size_t MyBinaryWriteStream::GetCurrentPos() const
{
    return m_data->length();
}
