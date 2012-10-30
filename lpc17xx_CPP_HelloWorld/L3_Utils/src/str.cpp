#include "str.hpp"
#include <string.h> // memcpy, strcmp
#include <ctype.h>  // tolower/toupper
#include <stdlib.h> // realloc()
#include <stdio.h>  // sprintf
#include <stdlib.h> // atoi() atof()


int str::toInt(const char* pString)     {   return atoi(pString);   }
#if STR_SUPPORT_FLOAT
float str::toFloat(const char* pString) {   return atof(pString);   }
#endif

str::str()
{
    init();
}
str::str(int capacity)
{
    init(capacity);
}
str::str(const char* pString)
{
    init(strlen(pString)); // Init with enough memory allocation needed to store pString's length
    copyFrom(pString);
}
str::~str()
{
    //printf("Delete %u bytes @ %p\n", mCapacity, mpStr);
    if(0 != mpStr) {
        delete mpStr;
    }
}


int str::getLen() const
{
    return strlen(mpStr);
}
int str::getCapacity() const
{
    return mCapacity;
}
void str::reserve(int n)
{
    if(mCapacity < n)
    {
        reAllocateMem(n);
    }
}
void str::clear()
{
    *mpStr = '\0';
    //memset(mpStr, 0, mCapacity);
}


void str::toLower()
{
    char* pOurStr = mpStr;
    while(0 != *pOurStr) {
        *pOurStr = tolower(*pOurStr);
        pOurStr++;
    }
}
void str::toUpper()
{
    char* pOurStr = mpStr;
    while(0 != *pOurStr) {
        *pOurStr = toupper(*pOurStr);
        pOurStr++;
    }
}

#include <stdarg.h>
int str::printf(const char* pFormat, ...)
{
    va_list args;
    va_start(args, pFormat);

    /**
     * Try to print into allocated memory, and if the number of characters
     * is greater than capacity, reallocate more memory, and print again
     */
#if STR_SUPPORT_FLOAT
    int len = vsnprintf(mpStr, getCapacity(), pFormat, args);
#else
    int len = vsniprintf(mpStr, getCapacity(), pFormat, args);
#endif

    if(len <= getCapacity())
    {
        reserve(len);
#if STR_SUPPORT_FLOAT
        len = vsnprintf(mpStr, getCapacity(), pFormat, args);
#else
        len = vsniprintf(mpStr, getCapacity(), pFormat, args);
#endif
    }

    va_end(args);
    return len;
}

void str::insertAtBeg(const char* pString)
{
    insertAt(0, pString);
}
void str::insertAtEnd(const char* pString)
{
    ensureMemoryToInsertNChars(strlen(pString));
    strcat(mpStr, pString);
}
void str::insertAt(const int index, const char* pString)
{
    const int newLen = strlen(pString);
    ensureMemoryToInsertNChars(newLen);

    if(index >= 0 && index <= getLen() && newLen > 0)
    {
        // "Hello", insert at 2 "123" ==> "He123llo"
        void* destToMakeRoom = mpStr + index + newLen;
        void* srcToCopyLeftOverString = mpStr + index;
        const int lenToMove = getLen() - index + 1; // +1 for NULL Char
        memmove(destToMakeRoom, srcToCopyLeftOverString, lenToMove);

        memcpy(mpStr + index, pString, newLen);
    }
}


void str::append(const char* pString)
{
    insertAtEnd(pString);
}
void str::append(int x)
{
    char intValString[32];
    sprintf(intValString, "%i", x);
    append(intValString);
}
#if STR_SUPPORT_FLOAT
void str::append(float x)
{
    char floatValString[32];
    sprintf(floatValString, "%f", x);
    append(floatValString);
}
#endif
void str::appendAsHex(unsigned int num)
{
    char hexVal[32];
    sprintf(hexVal, "%02X", num);
    append(hexVal);
}


bool str::compareTo(const char* pString) const
{
    return (0 == strcmp(pString, mpStr));
}
bool str::compareToIgnoreCase(const char* pString) const
{
    return (0 == strcasecmp(mpStr, pString));
}
bool str::contains(const char* pString) const
{
    return firstIndexOf(pString) != mInvalidIndex;
}


int str::getWordIndexIgnoreCase(const char* pString) const
{
    int findIndex = mInvalidIndex;
    char* firstChar;

    if(0 != *pString && 0 != *mpStr)
    {
        for(char* pSearchLoc = mpStr; 0 != *pSearchLoc; pSearchLoc = firstChar + 1)
        {
            // First, find first char match ignoring the case.
            if(0 == (firstChar = strchr(pSearchLoc, tolower(*pString)))) {
                if(0 == (firstChar = strchr(pSearchLoc, toupper(*pString)))) {
                    findIndex = mInvalidIndex;
                    break;
                }
            }

            // First char matched, now match remaining string
            char* i2 = (char*)pString + 1;
            findIndex = firstChar - pSearchLoc;

            for(char* i1 = firstChar + 1; *i1 && *i2; i1++, i2++)
            {
                if(tolower(*i1) != tolower(*i2))
                {
                    findIndex = mInvalidIndex;
                    break;
                }
            }

            // If iterator2 reached null char, we matched entire string!
            if(0 == *i2) {
                break;
            }
        };
    }

    return findIndex;
}
bool str::containsIgnoreCase(const char* pString) const
{
    return getWordIndexIgnoreCase(pString) != mInvalidIndex;
}
int str::firstIndexOf(const char* pString) const
{
    char* pFindPtr = strstr(mpStr, pString);
    return (0 == pFindPtr) ? mInvalidIndex : (pFindPtr - mpStr);
}
int str::lastIndexOf(const char* pString) const
{
    const int lenOfStrToFind = strlen(pString);
    char* pFindPtr = strstr(mpStr, pString);
    char* pLastPtr = pFindPtr;

    while(0 != pFindPtr)
    {
        pLastPtr = pFindPtr;
        pFindPtr = strstr(pFindPtr + lenOfStrToFind, pString);
    }

    return (0 == pLastPtr) ? mInvalidIndex : (pLastPtr - mpStr);
}
int str::countOf(const char* pString) const
{
    const int lenOfStrToFind = strlen(pString);
    char* pFindPtr = strstr(mpStr, pString);
    int count = 0;

    while(0 != pFindPtr)
    {
        count++;
        pFindPtr = strstr(pFindPtr + lenOfStrToFind, pString);
    }

    return count;
}


bool str::beginsWith(const char* pString) const
{
    const int theirLen = strlen(pString);

    // If their length is greater, it can't match our string
    return (getLen() >= theirLen && (0 == strncmp(mpStr, pString, theirLen)));
}
bool str::beginsWithIgnoreCase(const char* pString) const
{
    const int theirLen = strlen(pString);

    // If their length is greater, it can't match our string
    return (getLen() >= theirLen && 0 == strncasecmp(pString, mpStr, theirLen));
}
bool str::beginsWithWholeWord(const char* pString, char seperator) const
{
    // After comparison, the char must be a space or null terminated
    const char c = mpStr[strlen(pString)];
    return beginsWith(pString) && (seperator == c || '\0' == c);
}
bool str::beginsWithWholeWordIgnoreCase(const char* pString, char seperator) const
{
    // After comparison, the char must be a space or null terminated
    const char c = mpStr[strlen(pString)];
    return beginsWithIgnoreCase(pString) && (seperator == c || '\0' == c);
}


bool str::endsWith(const char* pString) const
{
    bool decision = false;

    const int theirLen = strlen(pString);
    const int ourLen = getLen();
    if(ourLen >= theirLen)
    {
        decision = (0 == strcmp(mpStr+ourLen-theirLen, pString));
    }

    return decision;
}
bool str::endsWithIgnoreCase(const char* pString) const
{
    const int theirLen = strlen(pString);
    const int ourLen = getLen();
    bool decision = (theirLen <= ourLen);

    // If decision is not already false, make actual comparison:
    if(true == decision)
    {
        char* pTheirStr = (char*)pString;
        char* pOurStr = mpStr + ourLen - theirLen;

        return (0 == strcasecmp(pTheirStr, pOurStr));
    }

    return decision;
}


bool str::erase(const char* pString)
{
    const int findIndex = firstIndexOf(pString);
    if(mInvalidIndex != findIndex)
    {
        eraseAfter(findIndex, strlen(pString));
        return true;
    }
    return false;
}
void str::eraseFirst(int nChars)
{
    const int len = getLen();
    if(nChars > 0 && nChars <= len)
    {
        memmove(mpStr, mpStr + nChars, len - nChars + 1);
    }
}
void str::eraseLast(int nChars)
{
    const int len = getLen();
    if(nChars <= len)
    {
        mpStr[len-nChars] = '\0';
    }
}
void str::eraseCharAt(int index)
{
    eraseAfter(index, 1);
}
void str::eraseAllAfter(int index)
{
    if(index >= 0 && index < getLen())
    {
        mpStr[index] = '\0';
    }
}
void str::eraseAfter(int index, int nChars)
{
    const int ourLen = getLen();
    if(index >= 0 && index < ourLen && nChars > 0)
    {
        // "012345", index = 2, nChars = 6
        if(nChars > ourLen - index) {
            nChars = ourLen - index;
        }
        memmove(mpStr+index, mpStr+index+nChars, ourLen - index - nChars + 1);
    }
}
void str::eraseAllSpecialChars()
{
    // TODO: Optimize.
    for(int i = 0; i < getLen(); i++)
    {
        const char thisChar = mpStr[i];
        if(!isalnum(thisChar))
        {
            eraseCharAt(i);
        }
    }
}


void str::trimStart(const char* pChars)
{
    int numBegCharsToRemove = 0;
    const int ourLen = getLen();

    for(int i = 0; i < ourLen; i++)
    {
        const int len = strlen(pChars);
        bool trimDone = false;

        for(int j = 0 ; j < len; j++)
        {
            if(pChars[j] == mpStr[i]) {
                trimDone = true;
                numBegCharsToRemove++;
                break;
            }
        }

        if(!trimDone) {
            break;
        }
    }

    if(numBegCharsToRemove > 0) {
        eraseFirst(numBegCharsToRemove);
    }
}
void str::trimEnd(const char* pChars)
{
    const int trimLen = strlen(pChars);
    for(int i = getLen() - 1; i >= 0; i--)
    {
        bool trimDone = false;
        for(int j = 0 ; j < trimLen; j++)
        {
            if(pChars[j] == mpStr[i])
            {
                trimDone = true;
                mpStr[i] = '\0';
                break;
            }
        }

        // If no trim took place this iteration, break here
        if(!trimDone) {
            break;
        }
    }
}


bool str::replaceFirst(const char* pFind, const char* pWith)
{
    const int findIndex = firstIndexOf(pFind);
    if(findIndex != mInvalidIndex)
    {
        eraseAfter(findIndex, strlen(pFind));
        insertAt(findIndex, pWith);
        return true;
    }
    return false;
}
bool str::replaceLast(const char* pFind, const char* pWith)
{
    const int findIndex = lastIndexOf(pFind);
    if(findIndex != mInvalidIndex)
    {
        eraseAfter(findIndex, strlen(pFind));
        insertAt(findIndex, pWith);
        return true;
    }
    return false;
}
int str::replaceAll(const char* pFind, const char* pWith)
{
    int count = 0;
    int findIndex = mInvalidIndex;

    while(mInvalidIndex != (findIndex = firstIndexOf(pFind)))
    {
        eraseAfter(findIndex, strlen(pFind));
        insertAt(findIndex, pWith);
        ++count;
    }

    return count;
}


/********
 * All substring functions funnel down to this function which creates
 * the mpTempStr object that is returned as reference
 */
const str& str::subString(int fromIndex, int charCount)
{
    if(0 == mpTempStr) {
        mpTempStr = new str();
    }
    str& ref = *mpTempStr;

    ref.clear();
    ref.reserve(getLen());

    if(fromIndex >= 0 && fromIndex < getLen() && charCount > 0)
    {
        // Cap the charCount if it is greater than remaining length
        if(charCount > getLen() - fromIndex)
                charCount = getLen() - fromIndex;

        memcpy(ref.mpStr, mpStr+fromIndex, charCount);
        ref.mpStr[charCount] = '\0';
    }

    return ref;
}
// Calls function: str::subString(int fromIndex, int charCount) (above)
const str& str::subString(int fromIndex)
{
    return subString(fromIndex, getLen());
}
// Calls function: subString(int fromIndex)
const str& str::subString(const char* fromStr)
{
    return subString(firstIndexOf(fromStr));
}
// Calls function: subString(const char* fromStr) (above)
const str& str::subString(char fromFirstChar)
{
    const char str[] = {fromFirstChar, 0x00};
    return subString(str);
}
// Calls subString(int fromIndex, int charCount);
const str& str::subString(const char* fromStr, int charCount)
{
    return subString(firstIndexOf(fromStr), charCount);
}
// Calls function: subString(const char* fromStr, int charCount) (above)
const str& str::subString(char fromFirstChar, int charCount)
{
    const char str[] = {fromFirstChar, 0x00};
    return subString(str, charCount);
}
// Calls function: subString(int fromIndex, int charCount)
const str& str::subString(char fromFirstChar, char toLastChar)
{
    const char fi[] = {fromFirstChar, 0x00};
    const char li[] = {toLastChar, 0x00};
    const int firstIndex = firstIndexOf(fi);
    const int lastIndex  =  lastIndexOf(li);

    return subString(firstIndex, lastIndex - firstIndex + 1);
}

const str* str::getToken(const char* pSplitter, bool restart)
{
    if(restart || 0 == mpTokenPtr) {
        mpTokenPtr = mpStr;
    }

    str* pTokenStr = 0;
    if('\0' != *mpTokenPtr)
    {
        // Get char* pointer where we see any chars of pSplitter
        char* pFindPtr = strpbrk(mpTokenPtr, pSplitter);
        int indexOfFindPtr = mpTokenPtr-mpStr;
        int tokenLength = 0;

        if(0 == pFindPtr)
        {
            // No chars found, set token to the remaining string:
            tokenLength = strlen(mpTokenPtr);
            mpTokenPtr = mpStr + getLen();
        }
        else {
            // Length of token is from previous token ptr to the find pointer
            tokenLength = (pFindPtr - mpTokenPtr);

            // Set next token pointer to the pointer after the pFindPtr
            mpTokenPtr = pFindPtr+1;
        }
        pTokenStr = (str*)&subString(indexOfFindPtr, tokenLength);
    }

    return pTokenStr;
}

bool str::isAllAlpha() const
{
    const int ourLen = getLen();
    bool isAlpha = true;

    for(int i = 0; i < ourLen; i++)
    {
        if(! isalpha(mpStr[i]))
        {
            isAlpha = false;
            break;
        }
    }

    return isAlpha;
}
bool str::isAlphaNumeric() const
{
    const int ourLen = getLen();
    bool isAlphaNumero = true;

    for(int i = 0; i < ourLen; i++)
    {
        const char thisChar = mpStr[i];
        if(!isalnum(thisChar))
        {
            isAlphaNumero = false;
            break;
        }
    }

    return isAlphaNumero;
}

#if STR_SUPPORT_FLOAT
bool str::isFloat() const
{
    const int ourLen = getLen();
    bool strIsAFloat = countOf(".") <= 1;

    if(strIsAFloat)
    {
        for(int i = mpStr[0] == '-' ? 1 : 0; i < ourLen; i++)
        {
            const char thisChar = mpStr[i];
            if(thisChar != '.' && ! isdigit(thisChar))
            {
                strIsAFloat = false;
                break;
            }
        }
    }

    return strIsAFloat;
}
#endif

bool str::isUint() const
{
    const int ourLen = getLen();
    bool isAllDigits = true;

    for(int i = 0; i < ourLen; i++)
    {
        const char thisChar = mpStr[i];
        if(! isdigit(thisChar))
        {
            isAllDigits = false;
            break;
        }
    }

    return isAllDigits;
}
bool str::isInt() const
{
    const int ourLen = getLen();
    bool strIsAnInt = true;

    for(int i = mpStr[0] == '-' ? 1 : 0; i < ourLen; i++)
    {
        const char thisChar = mpStr[i];
        if(! isdigit(thisChar))
        {
            strIsAnInt = false;
            break;
        }
    }

    return strIsAnInt;
}

void str::encrypt(const char* pPassword)
{
    mLenOfEncryptedChars = getLen();
    encrypt(pPassword, mLenOfEncryptedChars);

}

void str::decrypt(const char* pPassword)
{
    // Decrypt is same thing as encrypt to get original string back
    encrypt(pPassword, mLenOfEncryptedChars);
}

int str::getEncryptedStringLength() const
{
    return mLenOfEncryptedChars;
}

unsigned int str::checksum_Get()
{
    int chkSum = 0;
    const int ourLen = getLen();
    for(int i = 0; i < ourLen; i++) {
        chkSum xor_eq mpStr[i];
    }
    return chkSum;
}

void str::checksum_Append()
{
    const unsigned int chksum = checksum_Get();
    append(":");
    appendAsHex(chksum);
}

void str::checksum_Remove()
{
    eraseAllAfter(lastIndexOf(":"));
}

bool str::checksum_Verify()
{
    bool checksumIsValid = false;
    const int checkSumIndex = lastIndexOf(":");

    if(mInvalidIndex != checkSumIndex)
    {
        mpStr[checkSumIndex] = '\0';
        const unsigned int actualChecksum = hexStrDigitsToInt(mpStr+checkSumIndex+1);
        const unsigned int expectedChecksum = checksum_Get();
        checksumIsValid = (actualChecksum == expectedChecksum);
        mpStr[checkSumIndex] = ':';
    }

    return checksumIsValid;
}



/*****************************************************************/
/******************* Operator Overloading ************************/
/*****************************************************************/
void str::operator=(const char* pString)
{
    copyFrom(pString);
}
void str::operator=(int num)
{
    char buff[32];
    sprintf(buff, "%i", num);
    copyFrom(buff);
}
#if STR_SUPPORT_FLOAT
void str::operator=(float num)
{
    char buff[32];
    sprintf(buff, "%f", num);
    copyFrom(buff);
}
#endif

void str::operator++()
{
    (*this) += 1; // Call += Operator Below
}
void str::operator++(int unused)
{
    (*this) += 1; // Call += Operator Below
}

void str::operator--()
{
    (*this) -= 1; // Call -= Operator Below
}
void str::operator--(int unused)
{
    (*this) -= 1; // Call -= Operator Below
}



void str::operator+=(const char* pString)
{
    append(pString);
}
void str::operator+=(const char singleChar)
{
    char s[] = {singleChar, '\0'};
    append(s);
}
void str::operator+=(int n)
{
    if(isInt())
    {
        (*this) = str::toInt(mpStr) + n; // Call Assignment Operator for int
    }
}
#if STR_SUPPORT_FLOAT
void str::operator+=(float n)
{
    if(isFloat())
    {
        (*this) = (float)str::toFloat(mpStr) + n; // Call Assignment Operator for float
    }
}
void str::operator-=(float n)
{
    if(isFloat())
    {
        (*this) = (float)str::toFloat(mpStr) - n; // Call Assignment Operator for float
    }
}
#endif
void str::operator-=(const char* pString)
{
    erase(pString);
}
void str::operator-=(int n)
{
    if(isInt())
    {
        (*this) = str::toInt(mpStr) - n; // Call Assignment Operator for int
    }
}
bool str::operator==(const char* pString) const
{
    return compareTo(pString);
}
bool str::operator==(const str& rhs) const
{
    return compareTo(rhs.c_str());
}
bool str::operator==(int n) const
{
    return str::toInt(mpStr) == n;
}
bool str::operator!=(const char* pString) const
{
    return !((*this) == pString); // Call ==Operator for char*
}
bool str::operator!=(int n) const
{
    return !((*this) == n); // Call ==Operator for int
}
/*
// Note: Floats cannot be compared using == operator, they require delta comparison
bool cString::operator==(float n)
{
    return cString::toFloat(mpStr) == n;
}
bool cString::operator!=(float n)
{
    return !((*this) == n); // Call ==Operator for float
}
*/

bool str::operator<(const char* pString) const { return strcmp(mpStr, pString) < 0; }
bool str::operator<(int x) const               { return (int)(*this) < x;           }
bool str::operator>(const char* pString) const { return strcmp(mpStr, pString) > 0; }
bool str::operator>(int x) const               { return (int)(*this) > x;           }
bool str::operator<=(const char* pString) const{ return (strcmp(mpStr, pString) < 0 || strcmp(mpStr, pString) == 0); }
bool str::operator<=(int x) const              { return (int)(*this) <= x;          }
bool str::operator>=(const char* pString) const{ return (strcmp(mpStr, pString) > 0 || strcmp(mpStr, pString) == 0); }
bool str::operator>=(int x) const              { return (int)(*this) >= x;          }

#if STR_SUPPORT_FLOAT
bool str::operator<(float x) const             { return (float)(*this) < x;         }
bool str::operator<=(float x) const            { return (float)(*this) <= x;        }
bool str::operator>=(float x) const            { return (float)(*this) >= x;        }
bool str::operator>(float x) const             { return (float)(*this) > x;         }
#endif

const char* str::operator()() const
{
    return mpStr;
}

str::operator int() const
{
    return str::toInt(mpStr);
}
#if STR_SUPPORT_FLOAT
str::operator float() const
{
    return str::toFloat(mpStr);
}
#endif

char& str::operator[](int pos)
{
    return pos < getLen() ? mpStr[pos] : mpStr[0];
}

str& str::operator=(const str& copy)
{
    // For the assignment operator, we need to copy string data memory into our memory
    // The rest of the member variables do not matter and they will auto-adjust
    if(this != &copy)
    {
        // Same as: this->copyFrom(copy.get());
        *this = copy();
    }
    return *this;
}

str::str(const str& copy)
{
    // Allocate enough memory first
    this->init(copy.getLen());

    // Simply use assignment operator code above
    *this = copy;
}




// Private
void str::ensureMemoryToInsertNChars(const int nChars)
{
    const int newLen = nChars;
    const int existingLen = getLen();
    const int requiredMem = newLen + existingLen;

    if(mCapacity < requiredMem)
    {
        // Allocate more so we don't end up doing this upon each character insertion
        reAllocateMem(requiredMem + 32);
    }
}

void str::reAllocateMem(const int size)
{
    mCapacity = size;
    if(0 == mpStr){
        mpStr = (char*)malloc(mCapacity+1); // +1 for NULL
        memset(mpStr, 0, mCapacity);
    }
    else {
        mpStr = (char*)realloc(mpStr, mCapacity+1);
    }
}

void str::copyFrom(const char* pString)
{
    const int strLen = strlen(pString);

    if(strLen > mCapacity) {
        reAllocateMem(strLen);
    }

    strcpy(mpStr, pString);
}

int str::singleHexCharToInt(unsigned char theChar)
{
    switch(theChar) {
        case 'a' ... 'f': theChar -= 'a' + 10; break;
        case 'A' ... 'F': theChar -= 'A' + 10; break;
        default: theChar -= '0'; break;
    }

    return theChar;
}
int str::hexStrDigitsToInt(char* pString)
{
    const char leftChar = pString[0];
    const char rightChar = pString[1];
    return (singleHexCharToInt(leftChar) << 4) + singleHexCharToInt(rightChar);
}

void str::encrypt(const char* pPassword, const int lenToEncrypt)
{
    const int passLen = strlen(pPassword);
    for(int i = 0; i < lenToEncrypt; i++) {
        for(int j = 0; j < passLen; j++) {
            mpStr[i] xor_eq pPassword[j];
        }
    }
}
