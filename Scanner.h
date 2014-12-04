

#if !defined(COCO_SCANNER_H__)
#define COCO_SCANNER_H__

#include <iostream>
#include <map>
#include <sstream>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

// io.h and fcntl are used to ensure binary read from streams on windows
#if _MSC_VER >= 1300
#include <io.h>
#include <fcntl.h>
#endif

#define COCO_WCHAR_MAX 65535
#define COCO_MIN_BUFFER_LENGTH 1024
#define COCO_MAX_BUFFER_LENGTH (64*COCO_MIN_BUFFER_LENGTH)
#define COCO_CPP_NAMESPACE_SEPARATOR L':'

#if _MSC_VER >= 1400
#define coco_swprintf swprintf_s
#elif _MSC_VER >= 1300
#define coco_swprintf _snwprintf
#elif defined __MINGW32__
#define coco_swprintf _snwprintf
#else
// assume every other compiler knows swprintf
#define coco_swprintf swprintf
#endif



// string handling, wide character
class CocoUtil
{
public:

	// string handling, wide character

	static std::wstring coco_string_create(const std::wstring value) {
		return coco_string_create(value, 0);
	}

	static std::wstring coco_string_create(const std::wstring value, int startIndex) {
		int valueLen = 0;
		int len = 0;

		valueLen = value.length();
		len = valueLen - startIndex;

		return coco_string_create(value, startIndex, len);
	}

	static std::wstring coco_string_create(const std::wstring value, int startIndex, int length) {
		return value.substr(startIndex, length);
	}

	static std::wstring coco_string_create_upper(const std::wstring data) {
		int dataLen = data.length();

		std::wstring newData;
		newData.resize(dataLen);

		for (int i = 0; i <= dataLen; i++) {
			if ((L'a' <= data[i]) && (data[i] <= L'z')) {
				newData[i] = data[i] + (L'A' - L'a');
			}
			else { newData[i] = data[i]; }
		}

		return newData;
	}

	static std::wstring coco_string_create_lower(const std::wstring data) {
		int dataLen = data.length();
		return coco_string_create_lower(data, 0, dataLen);
	}

	static std::wstring coco_string_create_lower(const std::wstring data, int startIndex, int dataLen) {

		std::wstring newData;
		newData.resize(dataLen);

		for (int i = 0; i <= dataLen; i++) {
			wchar_t ch = data[startIndex + i];
			if ((L'A' <= ch) && (ch <= L'Z')) {
				newData[i] = ch - (L'A' - L'a');
			}
			else { newData[i] = ch; }
		}

		return newData;
	}

	static std::wstring coco_string_create_append(const std::wstring data1, const std::wstring data2) {
		return data1 + data2;
	}

	static std::wstring coco_string_create_append(const std::wstring target, const wchar_t appendix) {
		return target + appendix;
	}

	static void coco_string_delete(std::wstring &data) {
	}

	static int coco_string_length(const std::wstring data) {
		return data.length();
	}

	static bool coco_string_endswith(const std::wstring data, const std::wstring end) {
		int dataLen = data.length();
		int endLen = end.length();
		return (endLen <= dataLen) && (wcscmp(data.c_str() + dataLen - endLen, end.c_str()) == 0);
	}

	static int coco_string_indexof(const std::wstring data, const wchar_t value) {
		const wchar_t* chr = wcschr(data.c_str(), value);

		if (chr) { return (chr-data.c_str()); }
		return -1;
	}

	static int coco_string_lastindexof(const std::wstring data, const wchar_t value) {
		const wchar_t* chr = wcsrchr(data.c_str(), value);

		if (chr) { return (chr-data.c_str()); }
		return -1;
	}

	static void coco_string_merge(std::wstring &target, const std::wstring appendix) {
		target = target + appendix;
	}

	static bool coco_string_equal(const std::wstring data1, const std::wstring data2) {
		return wcscmp( data1.c_str(), data2.c_str() ) == 0;
	}

	static int coco_string_compareto(const std::wstring data1, const std::wstring data2) {
		return wcscmp(data1.c_str(), data2.c_str());
	}

	static int coco_string_hash(const std::wstring str) {
		int h = 0;

		const wchar_t* data = str.c_str();

		while (*data != 0) {
			h = (h * 7) ^ *data;
			++data;
		}
		if (h < 0) { h = -h; }
		return h;
	}

	// string handling, ascii character

	static std::wstring coco_string_create(const std::string value) {
		std::wstring data(value.begin(), value.end());
		return data;
	}

	static std::string coco_string_create_char(const std::wstring value) {
		std::string data(value.begin(), value.end());
		return data;
	}

	static void coco_string_delete(std::string &data) {
		data = "";
	}
};



class Token  
{
public:
	int kind;     // token kind
	int pos;      // token position in bytes in the source text (starting at 0)
	int charPos;  // token position in characters in the source text (starting at 0)
	int col;      // token column (starting at 1)
	int line;     // token line (starting at 1)
	std::wstring val; // token value
	std::shared_ptr<Token> next;  // ML 2005-03-11 Peek tokens are kept in linked list

	Token() : next() {
		kind = 0;
		pos  = 0;
		col  = 0;
		line = 0;
		charPos = 0;
	}

	~Token() {
	}
};

class Buffer {
// This Buffer supports the following cases:
// 1) seekable stream (file)
//    a) whole stream in buffer
//    b) part of stream in buffer
// 2) non seekable stream (network, console)
private:
	unsigned char *buf; // input buffer
	int bufCapacity;    // capacity of buf
	int bufStart;       // position of first byte in buffer relative to input stream
	int bufLen;         // length of buffer
	int fileLen;        // length of input stream (may change if the stream is no file)
	int bufPos;         // current position in buffer
	FILE* stream;       // input stream (seekable)
	bool isUserStream;  // was the stream opened by the user?
	
	// Read the next chunk of bytes from the stream, increases the buffer
	// if needed and updates the fields fileLen and bufLen.
	// Returns the number of bytes read.
	int ReadNextStreamChunk() {
		int free = bufCapacity - bufLen;
		if (free == 0) {
			// in the case of a growing input stream
			// we can neither seek in the stream, nor can we
			// foresee the maximum length, thus we must adapt
			// the buffer size on demand.
			bufCapacity = bufLen * 2;
			unsigned char *newBuf = new unsigned char[bufCapacity];
			memcpy(newBuf, buf, bufLen*sizeof(unsigned char));
			delete [] buf;
			buf = newBuf;
			free = bufLen;
		}
		int read = fread(buf + bufLen, sizeof(unsigned char), free, stream);
		if (read > 0) {
			fileLen = bufLen = (bufLen + read);
			return read;
		}
		// end of stream reached
		return 0;
	}

	bool CanSeek() {
		return (stream != NULL) && (ftell(stream) != -1);
	}
	
public:
	static const int EoF = COCO_WCHAR_MAX + 1;

	Buffer(FILE* s, bool isUserStream) {
	// ensure binary read on windows
	#if _MSC_VER >= 1300
		_setmode(_fileno(s), _O_BINARY);
	#endif
		stream = s; this->isUserStream = isUserStream;
		if (CanSeek()) {
			fseek(s, 0, SEEK_END);
			fileLen = ftell(s);
			fseek(s, 0, SEEK_SET);
			bufLen = (fileLen < COCO_MAX_BUFFER_LENGTH) ? fileLen : COCO_MAX_BUFFER_LENGTH;
			bufStart = INT_MAX; // nothing in the buffer so far
		} else {
			fileLen = bufLen = bufStart = 0;
		}
		bufCapacity = (bufLen>0) ? bufLen : COCO_MIN_BUFFER_LENGTH;
		buf = new unsigned char[bufCapacity];	
		if (fileLen > 0) SetPos(0);          // setup  buffer to position 0 (start)
		else bufPos = 0; // index 0 is already after the file, thus Pos = 0 is invalid
		if (bufLen == fileLen && CanSeek()) Close();
	}

	Buffer(Buffer *b) {
		buf = b->buf;
		bufCapacity = b->bufCapacity;
		b->buf = NULL;
		bufStart = b->bufStart;
		bufLen = b->bufLen;
		fileLen = b->fileLen;
		bufPos = b->bufPos;
		stream = b->stream;
		b->stream = NULL;
		isUserStream = b->isUserStream;
	}

	Buffer(const unsigned char* buf, int len) {
		this->buf = new unsigned char[len];
		memcpy(this->buf, buf, len*sizeof(unsigned char));
		bufStart = 0;
		bufCapacity = bufLen = len;
		fileLen = len;
		bufPos = 0;
		stream = NULL;
		isUserStream = true;
	}

	virtual ~Buffer() {
		Close(); 
		if (buf != NULL) {
			delete [] buf;
			buf = NULL;
		}
	}

	virtual void Close() {
		if (!isUserStream && stream != NULL) {
			fclose(stream);
			stream = NULL;
		}
	}

	virtual int Read() {
		if (bufPos < bufLen) {
			return buf[bufPos++];
		} else if (GetPos() < fileLen) {
			SetPos(GetPos()); // shift buffer start to Pos
			return buf[bufPos++];
		} else if ((stream != NULL) && !CanSeek() && (ReadNextStreamChunk() > 0)) {
			return buf[bufPos++];
		} else {
			return EoF;
		}
	}

	virtual int Peek() {
		int curPos = GetPos();
		int ch = Read();
		SetPos(curPos);
		return ch;
	}

	// beg .. begin, zero-based, inclusive, in byte
	// end .. end, zero-based, exclusive, in byte
	virtual std::wstring GetString(int beg, int end) {

		std::wstringstream ss;

		int oldPos = GetPos();
		SetPos(beg);
		while (GetPos() < end) {
			ss << Read();
		}
		SetPos(oldPos);
		return ss.str();
	}

	virtual int GetPos() {
		return bufPos + bufStart;
	}

	virtual void SetPos(int value) {
		if ((value >= fileLen) && (stream != NULL) && !CanSeek()) {
			// Wanted position is after buffer and the stream
			// is not seek-able e.g. network or console,
			// thus we have to read the stream manually till
			// the wanted position is in sight.
			while ((value >= fileLen) && (ReadNextStreamChunk() > 0));
		}

		if ((value < 0) || (value > fileLen)) {
			fwprintf(stderr, L"--- buffer out of bounds access, position: %d\n", value);
			exit(1);
		}

		if ((value >= bufStart) && (value < (bufStart + bufLen))) { // already in buffer
			bufPos = value - bufStart;
		} else if (stream != NULL) { // must be swapped in
			fseek(stream, value, SEEK_SET);
			bufLen = fread(buf, sizeof(unsigned char), bufCapacity, stream);
			bufStart = value; bufPos = 0;
		} else {
			bufPos = fileLen - bufStart; // make Pos return fileLen
		}
	}

};

class UTF8Buffer : public Buffer {
public:
	UTF8Buffer(Buffer *b) : Buffer(b) {};
	virtual int Read() {
		int ch;
		do {
			ch = Buffer::Read();
			// until we find a utf8 start (0xxxxxxx or 11xxxxxx)
		} while ((ch >= 128) && ((ch & 0xC0) != 0xC0) && (ch != EoF));
		if (ch < 128 || ch == EoF) {
			// nothing to do, first 127 chars are the same in ascii and utf8
			// 0xxxxxxx or end of file character
		} else if ((ch & 0xF0) == 0xF0) {
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			int c1 = ch & 0x07; ch = Buffer::Read();
			int c2 = ch & 0x3F; ch = Buffer::Read();
			int c3 = ch & 0x3F; ch = Buffer::Read();
			int c4 = ch & 0x3F;
			ch = (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
		} else if ((ch & 0xE0) == 0xE0) {
			// 1110xxxx 10xxxxxx 10xxxxxx
			int c1 = ch & 0x0F; ch = Buffer::Read();
			int c2 = ch & 0x3F; ch = Buffer::Read();
			int c3 = ch & 0x3F;
			ch = (((c1 << 6) | c2) << 6) | c3;
		} else if ((ch & 0xC0) == 0xC0) {
			// 110xxxxx 10xxxxxx
			int c1 = ch & 0x1F; ch = Buffer::Read();
			int c2 = ch & 0x3F;
			ch = (c1 << 6) | c2;
		}
		return ch;
	}
};

//-----------------------------------------------------------------------------------
// StartStates  -- maps characters to start states of tokens
//-----------------------------------------------------------------------------------
class StartStates {
private:
	class Elem {
	public:
		int key, val;
		Elem *next;
		Elem(int key, int val) { this->key = key; this->val = val; next = NULL; }
	};

	Elem **tab;

public:
	StartStates() { tab = new Elem*[128]; memset(tab, 0, 128 * sizeof(Elem*)); }
	virtual ~StartStates() {
		for (int i = 0; i < 128; ++i) {
			Elem *e = tab[i];
			while (e != NULL) {
				Elem *next = e->next;
				delete e;
				e = next;
			}
		}
		delete [] tab;
	}

	void set(int key, int val) {
		Elem *e = new Elem(key, val);
		int k = ((unsigned int) key) % 128;
		e->next = tab[k]; tab[k] = e;
	}

	int state(int key) {
		Elem *e = tab[((unsigned int) key) % 128];
		while (e != NULL && e->key != key) e = e->next;
		return e == NULL ? 0 : e->val;
	}
};

//-------------------------------------------------------------------------------------------
// KeywordMap  -- maps strings to integers (identifiers to keyword kinds)
//-------------------------------------------------------------------------------------------
class KeywordMap {
private:
	std::map<std::wstring, int> map;

public:
	KeywordMap() { }
	virtual ~KeywordMap() { }

	void set(const std::wstring key, int val) {
		map[key] = val;
	}

	int get(const std::wstring key, int defaultVal) const {
		auto val = map.find(key);
		if (map.find(key) != map.end()) {
			return val->second;
		}
		return defaultVal;
	}
};

class Scanner {
private:
	class Heap {
		std::shared_ptr<Token> token;
		std::wstring str;
		std::shared_ptr<Heap> next;
	};

	std::shared_ptr<Heap> heapTop;

	unsigned char EOL;
	int eofSym;
	int noSym;
	int maxT;
	StartStates start;
	KeywordMap keywords;

	std::shared_ptr<Token> t;         // current token
	std::wstring tval;    // text of current token
	int tlen;         // length of current token

	std::shared_ptr<Token> tokens;    // list of tokens already peeked (first token is a dummy)
	std::shared_ptr<Token> pt;        // current peek token

	int ch;           // current input character

	int pos;          // byte position of current character
	int charPos;      // position by unicode characters starting with 0
	int line;         // line number of current character
	int col;          // column number of current character
	int oldEols;      // EOLs that appeared in a comment;


	std::shared_ptr<Token> CreateToken() {
		return std::shared_ptr<Token>(new Token());
	}

	void AppendVal(std::shared_ptr<Token> t) {
		t->val = tval.substr(0, tlen);
	}


	void SetScannerBehindT() {
		buffer->SetPos(t->pos);
		NextCh();
		line = t->line; col = t->col; charPos = t->charPos;
		for (int i = 0; i < tlen; i++) NextCh();
	}

	void Init() {
		EOL    = '\n';
		eofSym = 0;
		maxT = 32;
	noSym = 32;
	int i;
	for (i = 65; i <= 90; ++i) start.set(i, 1);
	for (i = 97; i <= 122; ++i) start.set(i, 1);
	for (i = 48; i <= 57; ++i) start.set(i, 3);
	for (i = 10; i <= 10; ++i) start.set(i, 16);
	start.set(34, 6);
	start.set(60, 9);
	start.set(39, 11);
	start.set(45, 17);
	start.set(36, 25);
	start.set(91, 36);
	start.set(44, 37);
	start.set(93, 38);
	start.set(58, 39);
	start.set(123, 40);
	start.set(125, 41);
	start.set(40, 42);
	start.set(41, 43);
	start.set(46, 44);
	start.set(61, 49);
	start.set(33, 46);
	start.set(59, 48);
		start.set(Buffer::EoF, -1);
	keywords.set(L"true", 8);
	keywords.set(L"false", 9);
	keywords.set(L"and", 21);
	keywords.set(L"or", 22);
	keywords.set(L"SET", 23);
	keywords.set(L"MAKE", 25);
	keywords.set(L"RETURN", 26);
	keywords.set(L"if", 27);
	keywords.set(L"STACK", 28);
	keywords.set(L"MAPPING", 30);
	keywords.set(L"IMPORT", 31);


		pos = -1; line = 1; col = 0; charPos = -1;
		oldEols = 0;
		NextCh();
		if (ch == 0xEF) { // check optional byte order mark for UTF-8
			NextCh(); int ch1 = ch;
			NextCh(); int ch2 = ch;
			if (ch1 != 0xBB || ch2 != 0xBF) {
				fwprintf(stderr, L"Illegal byte order mark at start of file");
				exit(1);
			}
			Buffer *oldBuf = buffer;
			buffer = new UTF8Buffer(buffer); col = 0; charPos = -1;
			delete oldBuf; oldBuf = NULL;
			NextCh();
		}

	
		pt = tokens = CreateToken(); // first token is a dummy
	}

	void NextCh() {

		if (oldEols > 0) { ch = EOL; oldEols--; }
		else {
			pos = buffer->GetPos();
			// buffer reads unicode chars, if UTF8 has been detected
			ch = buffer->Read(); col++; charPos++;
			// replace isolated '\r' by '\n' in order to make
			// eol handling uniform across Windows, Unix and Mac
			if (ch == L'\r' && buffer->Peek() != L'\n') ch = EOL;
			if (ch == EOL) { line++; col = 0; }
		}
	
	}

	void AddCh() {
		if (tlen >= tval.length()) {
			tval.resize(tval.length() * 2 + 1);
		}
		
		if (ch != Buffer::EoF) {
			tval[tlen++] = ch;
			NextCh();
		}
	}



	std::shared_ptr<Token> NextToken() {

		while (ch == ' ' ||
				(ch >= 9 && ch <= 10) || ch == 13
		) NextCh();

		if ((ch == L'/' && Comment0()) || (ch == L'/' && Comment1())) return NextToken();
		int recKind = noSym;
		int recEnd = pos;
		t = CreateToken();
		t->pos = pos; t->col = col; t->line = line; t->charPos = charPos;
		int state = start.state(ch);
		tlen = 0; AddCh();

		switch (state) {
			case -1: { t->kind = eofSym; break; } // NextCh already done
			case 0: {
				case_0:
				if (recKind != noSym) {
					tlen = recEnd - t->pos;
					SetScannerBehindT();
				}
				t->kind = recKind; break;
			} // NextCh already done
			case 1:
			case_1:
			recEnd = pos; recKind = 1;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'A' && ch <= L'Z') || ch == L'_' || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_1;}
			else if (ch == L':') {AddCh(); goto case_2;}
			else {t->kind = 1; std::wstring literal = CocoUtil::coco_string_create(tval.c_str(), 0, tlen); t->kind = keywords.get(literal, t->kind); CocoUtil::coco_string_delete(literal); break;}
		case 2:
			case_2:
			if (ch == L':') {AddCh(); goto case_1;}
			else {goto case_0;}
		case 3:
			case_3:
			recEnd = pos; recKind = 2;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_3;}
			else if (ch == L'.') {AddCh(); goto case_4;}
			else {t->kind = 2; break;}
		case 4:
			case_4:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_5;}
			else {goto case_0;}
		case 5:
			case_5:
			recEnd = pos; recKind = 2;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_5;}
			else {t->kind = 2; break;}
		case 6:
			case_6:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'!') || (ch >= L'#' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_6;}
			else if (ch == L'"') {AddCh(); goto case_8;}
			else if (ch == 92) {AddCh(); goto case_7;}
			else {goto case_0;}
		case 7:
			case_7:
			if ((ch >= L' ' && ch <= L'~')) {AddCh(); goto case_6;}
			else {goto case_0;}
		case 8:
			case_8:
			{t->kind = 3; break;}
		case 9:
			case_9:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'!') || (ch >= L'#' && ch <= L'=') || (ch >= L'?' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_9;}
			else if (ch == L'>') {AddCh(); goto case_26;}
			else if (ch == 92) {AddCh(); goto case_10;}
			else {goto case_0;}
		case 10:
			case_10:
			if ((ch >= L' ' && ch <= L'~')) {AddCh(); goto case_9;}
			else {goto case_0;}
		case 11:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'&') || (ch >= L'(' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_12;}
			else if (ch == 92) {AddCh(); goto case_13;}
			else {goto case_0;}
		case 12:
			case_12:
			if (ch == 39) {AddCh(); goto case_15;}
			else {goto case_0;}
		case 13:
			case_13:
			if ((ch >= L' ' && ch <= L'~')) {AddCh(); goto case_14;}
			else {goto case_0;}
		case 14:
			case_14:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f')) {AddCh(); goto case_14;}
			else if (ch == 39) {AddCh(); goto case_15;}
			else {goto case_0;}
		case 15:
			case_15:
			{t->kind = 5; break;}
		case 16:
			{t->kind = 6; break;}
		case 17:
			if (ch == L'-') {AddCh(); goto case_18;}
			else {goto case_0;}
		case 18:
			case_18:
			if (ch == L'-') {AddCh(); goto case_19;}
			else {goto case_0;}
		case 19:
			case_19:
			if (ch == 10) {AddCh(); goto case_21;}
			else if (ch == 13) {AddCh(); goto case_20;}
			else {goto case_0;}
		case 20:
			case_20:
			if (ch == 10) {AddCh(); goto case_21;}
			else {goto case_0;}
		case 21:
			case_21:
			if (ch <= 65535) {AddCh(); goto case_22;}
			else {goto case_0;}
		case 22:
			case_22:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else {goto case_0;}
		case 23:
			case_23:
			recEnd = pos; recKind = 33;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_23;}
			else {t->kind = 33; break;}
		case 24:
			case_24:
			recEnd = pos; recKind = 34;
			if ((ch >= L'-' && ch <= L'.') || (ch >= L'0' && ch <= L':') || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_24;}
			else {t->kind = 34; break;}
		case 25:
			recEnd = pos; recKind = 33;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_23;}
			else if ((ch >= L'a' && ch <= L'z')) {AddCh(); goto case_29;}
			else {t->kind = 33; break;}
		case 26:
			case_26:
			recEnd = pos; recKind = 4;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'!') || (ch >= L'#' && ch <= L'=') || (ch >= L'?' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_9;}
			else if (ch == L'>') {AddCh(); goto case_26;}
			else if (ch == 92) {AddCh(); goto case_10;}
			else {t->kind = 4; break;}
		case 27:
			case_27:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L',') || (ch >= L'.' && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else if (ch == L'-') {AddCh(); goto case_30;}
			else {goto case_0;}
		case 28:
			case_28:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else {goto case_0;}
		case 29:
			case_29:
			recEnd = pos; recKind = 33;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_23;}
			else if ((ch >= L'a' && ch <= L'z')) {AddCh(); goto case_29;}
			else if (ch == L'=') {AddCh(); goto case_24;}
			else {t->kind = 33; break;}
		case 30:
			case_30:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L',') || (ch >= L'.' && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else if (ch == L'-') {AddCh(); goto case_31;}
			else {goto case_0;}
		case 31:
			case_31:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L',') || (ch >= L'.' && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else if (ch == L'-') {AddCh(); goto case_32;}
			else {goto case_0;}
		case 32:
			case_32:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_33;}
			else if (ch == 13) {AddCh(); goto case_34;}
			else {goto case_0;}
		case 33:
			case_33:
			recEnd = pos; recKind = 7;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L',') || (ch >= L'.' && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else if (ch == L'-') {AddCh(); goto case_30;}
			else {t->kind = 7; break;}
		case 34:
			case_34:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_35;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else {goto case_0;}
		case 35:
			case_35:
			recEnd = pos; recKind = 7;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L',') || (ch >= L'.' && ch <= 65535)) {AddCh(); goto case_22;}
			else if (ch == 10) {AddCh(); goto case_27;}
			else if (ch == 13) {AddCh(); goto case_28;}
			else if (ch == L'-') {AddCh(); goto case_30;}
			else {t->kind = 7; break;}
		case 36:
			{t->kind = 10; break;}
		case 37:
			{t->kind = 11; break;}
		case 38:
			{t->kind = 12; break;}
		case 39:
			{t->kind = 13; break;}
		case 40:
			{t->kind = 14; break;}
		case 41:
			{t->kind = 15; break;}
		case 42:
			{t->kind = 16; break;}
		case 43:
			{t->kind = 17; break;}
		case 44:
			{t->kind = 18; break;}
		case 45:
			case_45:
			{t->kind = 19; break;}
		case 46:
			if (ch == L'=') {AddCh(); goto case_47;}
			else {goto case_0;}
		case 47:
			case_47:
			{t->kind = 20; break;}
		case 48:
			{t->kind = 29; break;}
		case 49:
			recEnd = pos; recKind = 24;
			if (ch == L'=') {AddCh(); goto case_45;}
			else {t->kind = 24; break;}

		}


		AppendVal(t);
		return t;
	}

public:
	Buffer *buffer;   // scanner buffer

	Scanner(const unsigned char* buf, int len) {
		buffer = new Buffer(buf, len);
		Init();
	}

	Scanner(const std::wstring fileName) {
		FILE* stream;
		std::string chFileName = CocoUtil::coco_string_create_char(fileName);
		if ((stream = fopen(chFileName.c_str(), "rb")) == NULL) {
			fwprintf(stderr, L"--- Cannot open file %ls\n", fileName.c_str());
			exit(1);
		}
		CocoUtil::coco_string_delete(chFileName);
		buffer = new Buffer(stream, false);
		Init();
	}

	Scanner(FILE* s) {
		buffer = new Buffer(s, true);
		Init();
	}

	~Scanner() {
		delete buffer;
	}


	
bool Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'/') {
		NextCh();
		for(;;) {
			if (ch == 10) {
				level--;
				if (level == 0) { oldEols = line - line0; NextCh(); return true; }
				NextCh();
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}

bool Comment1() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'*') {
		NextCh();
		for(;;) {
			if (ch == L'*') {
				NextCh();
				if (ch == L'/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == L'/') {
				NextCh();
				if (ch == L'*') {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}




	// get the next token (possibly a token already seen during peeking)
	std::shared_ptr<Token> Scan() {

		if (!tokens->next) {
			pt = tokens = NextToken();
		} else {
			pt = tokens = tokens->next;
		}

		return tokens;
	}

	// peek for the next token, ignore pragmas
	std::shared_ptr<Token> Peek() {
		do {
			if (!pt->next) {
				pt->next = NextToken();
			}
			pt = pt->next;
		} while (pt->kind > maxT); // skip pragmas

		return pt;
	}

	// make sure that peeking starts at the current scan position
	void ResetPeek() {
		pt = tokens;
	}

}; // end Scanner



#endif

