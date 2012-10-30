/**
 * @file str.hpp
 * @brief Provides string class with a small foot-print
 * @ingroup Utilities
 *
 * Version: 06192012    Initial
 */
#ifndef STR_HPP__
#define STR_HPP__



/**
 * Defining float support will bloat your code, but allow you to use floats.
 * Floats can be used like this:
 * @code
 *  str s;
 *  s = 1.23;
 *  if(s == "1.23")...
 * @endcode
 */
#define STR_SUPPORT_FLOAT  0




/**
 * Custom string class
 * @ingroup Utilities
 *
 * str class can provide many string manipulation functions while
 * maintaining internal memory and allocating more memory when needed.
 *
 * @code
 * str s;
 * s = "Hello World...";
 * s.trimEnd(".");
 * printf("%s", s()); // str becomes: "Hello World"
 *
 * s.erase("World");
 * printf("%s", s()); // str becomes: "Hello "
 * s += "World";
 * printf("%s", s()); // str back to: "Hello World"
 *
 * // Use String class as an integer:
 * str myInt;
 * myInt = 123;
 * myInt++; if(myInt == 124)    // true
 * myInt--; if(myInt == "123")  // true
 * int n = (int)myInt;          // Cast back from string to int, n = 123
 * @endcode
 *
 *
 * You can also printf to this str object:
 * @code
 *  str s;
 *  s.printf("Hello World %u\n", 123);
 * @endcode
 *
 *
 * Parsing (Tokenize) Example:
 * @code
 *      str s;
 *      s = "Hello,World.Parse Example!";
 *      puts(s.getToken(",", true)->c_str()); // Prints: Hello
 *      puts(s.getToken(".")->c_str());       // Prints: World
 *      puts(s.getToken()->c_str());          // Prints: Parse
 *      puts(s.getToken()->c_str());          // Prints Example!
 *      assert(0 == s.getToken());            // No more tokens -> NULL Pointer
 * @endcode
 * Note that the original str s is not destroyed during tokenize operations
 */
class str
{
    public:
        /**
         * @{ \name Static conversion functions from string to int and float
         *          @note To convert this string to int or float, use cast operator: int x = (int)myStr;
         */
        static int toInt(const char* pString);     ///< Static method to convert char* string to integer
        static int toInt(str& s)   { return str::toInt(s()); }
#if STR_SUPPORT_FLOAT
        static float toFloat(const char* pString); ///< Static method to convert char* string to float
        static int toFloat(str& s) { return str::toFloat(s()); }
#endif
        /** @} */



        /**
         * @{ \name Constructors (and destructor)
         */
        str();                      ///< Default constructor
        str(int capacity);          ///< Constructor with initial capacity
        str(const char* pString);   ///< Construct from char* pointer
        str(const str& s);          ///< Copy Constructor
        ~str();                     ///< Destructor
        /** @} */



        int getLen() const;      ///< @returns Number of characters in the string
        int getCapacity() const; ///< @returns the current allocated capacity of str
        void reserve(int n);     ///< reserves memory to hold n characters
        void clear();            ///< Clears the string

        void toLower(); ///< Make every alphabet character lowercase
        void toUpper(); ///< Make every alphabet character uppercase

        /**
         * Similar to printf, but will print data into the string
         * @returns The number of characters printed
         */
        int printf(const char* pFormat, ...);

        /**
         * @{ \name Insertion functions
         */
        void insertAtBeg(const char* pString);
        void insertAtBeg(const str& s) { insertAtBeg(s()); }
        void insertAtEnd(const char* pString);
        void insertAtEnd(const str& s) { insertAtEnd(s()); }
        void insertAt(const int index, const char* pString);
        void insertAt(const int index, const str& s) { insertAt(index, s()); }
        /** @} */



        /**
         * @{ \name Append functions
         */
        void append(const char* pString);           ///< Appends constant string pointer
        void append(const str& s) { append(s()); }  ///< Appends another str
        void append(int x);                         ///< Appends integer as characters
#if STR_SUPPORT_FLOAT
        void append(float x);                       ///< Appends float as characters
#endif
        void appendAsHex(unsigned int num);         ///< Appends as hexadecimal ie: DEADBEEF
        /** @} */



        /**
         * @{ \name Comparison functions
         * @see See also the overloaded == and != operators
         */
        bool compareTo(const char* pString) const;
        bool compareTo(const str& s) const { return compareTo(s.c_str()); }
        bool compareToIgnoreCase(const char* pString) const;
        bool compareToIgnoreCase(const str& s) const { return compareToIgnoreCase(s()); }
        /** @} */



        /**
         * @{ \name Get Word Index Functions
         */
        int getWordIndexIgnoreCase(const char* pString) const;
        int getWordIndexIgnoreCase(const str& s) const { return getWordIndexIgnoreCase(s()); }

        int firstIndexOf(const char* pString) const;
        int firstIndexOf(const str& s) const { return firstIndexOf(s()); }
        int lastIndexOf(const char* pString) const;
        int lastIndexOf(const str& s) const { return lastIndexOf(s()); }
        /** @} */



        /**
         * @{ \name Functions to check a contained string
         */
        bool contains(const char* pString) const;
        bool contains(const str& s) const { return contains(s()); }
        bool containsIgnoreCase(const char* pString) const;
        bool containsIgnoreCase(const str& s) const { return containsIgnoreCase(s()); }

        int countOf(const char* pString) const;
        int countOf(const str& s) const { return countOf(s()); }

        bool beginsWith(const char* pString) const;
        bool beginsWith(const str& s) const { return endsWith(s()); }
        bool beginsWithIgnoreCase(const char* pString) const;
        bool beginsWithIgnoreCase(const str& s) const { return endsWithIgnoreCase(s()); }

        /**
         * Checks if the string begins with a whole word.
         * If str = "Hello World", then str.beginsWithWholdWord("Hello") will be true
         * But if str = "HelloWorld" then this will be false because this function will
         * check whole word separated by space char unless the separator itself is provided.
         */
        bool beginsWithWholeWord(const char* pString, char seperator=' ') const;
        bool beginsWithWholeWordIgnoreCase(const char* pString, char seperator=' ') const;

        bool endsWith(const char* pString) const;
        bool endsWith(const str& s) const { return endsWith(s()); }
        bool endsWithIgnoreCase(const char* pString) const;
        bool endsWithIgnoreCase(const str& s) const { return endsWithIgnoreCase(s()); }
        /** @} */



        /**
         * @{ \name Erase Functions
         */
        bool erase(const char* pString);
        bool erase(const str& s) { return erase(s()); }
        void eraseFirst(int nChars);
        void eraseLast(int nChars);
        void eraseCharAt(int index);
        void eraseAllAfter(int index);
        void eraseAfter(int index, int nChars);
        void eraseAllSpecialChars(); ///< Erase all characters except alphabets and numerals
        /** @} */



        /**
         * @{ \name Trimming Functions to remove leading or trailing character sets
         * Example: str s("...Hello..;;''");
         * s.trimEnd(".;'"); --> s is now: "...Hello"
         */
        void trimStart(const char* pChars);
        void trimStart(const str& s) { trimStart(s()); }
        void trimEnd(const char* pChars);
        void trimEnd(const str& s) { trimEnd(s()); }
        /** @} */



        /**
         * @{ \name Find & Replace Functions
         */
        bool replaceFirst(const char* pFind, const char* pWith);
        bool replaceLast(const char* pFind, const char* pWith);
        int replaceAll(const char* pFind, const char* pWith);
        /** @} */



        /**
         * @{ \name Sub-string Functions
         * @note These sub-string functions will allocate an internal str object
         *       and each subString() function will return reference to this object
         *       to minimize dynamic memory allocation and avoid copy constructors.
         */
        const str& subString(int fromIndex);
        const str& subString(int fromIndex, int charCount);
        const str& subString(char fromFirstChar);
        const str& subString(const char* fromStr);
        const str& subString(char fromFirstChar, int charCount);
        const str& subString(const char* fromStr, int charCount);
        const str& subString(char fromFirstChar, char toLastChar);
        /** @} */

        /**
         * Tokenize function
         * @param pSplitter  The tokens that mark the end of the token to get.
         * @param restart    Restarts tokenize operation on contents of this str. This optional
         *                     parameter is not needed after restarting 1st tokenize operation.
         * @returns  Pointer to substring, or NULL if no more tokens remain
         * @note     Unlike strtok(), the contents of this str is not destroyed.
         * @warning  The returned substring pointer is temporary, and further calls to this function will
         *            re-use the same substr, so copy the  returned substring if it is to be used later.
         *
         * Example:
         * @code
         *   str s = "Hello,World tokentest";
         *   str* t1 = s.getToken(",", true); // t1 == "Hello"
         *   str* t2 = s.getToken(" ");       // t2 == "World"
         *   str* t3 = s.getToken("");        // t3 == "tokentest"
         * @endcode
         */
        const str* getToken(const char* pSplitter = " ", bool restart=false);



        /**
         * @{ \name Datatype detection Functions
         */
        bool isAllAlpha() const;     ///< @returns TRUE if the string is all alphabetical characters only
        bool isAlphaNumeric() const; ///< @returns TRUE if the string is all alpha or numerical characters only
#if STR_SUPPORT_FLOAT
        bool isFloat() const;        ///< @returns TRUE if the string is a floating point number
#endif
        bool isUint() const;         ///< @returns TRUE if the string is an unsigned number
        bool isInt() const;          ///< @returns TRUE if the string is a number
        /** @} */


        /**
         * @{ \name Encryption & Checksum Functions
         */
        void encrypt(const char* pPassword);  ///< Encrypts (garbles) the string with given password
        void decrypt(const char* pPassword);  ///< Encrypted string must be decrypted before further use
        int getEncryptedStringLength() const; ///< Gets the number of characters that are currently encrypted

        unsigned int checksum_Get(); ///< Get integer value of XOR checksum of this string
        void checksum_Append();      ///< Appends checksum characters: Ex: 123 becomes: 123:0A
        void checksum_Remove();      ///< Removes checksum characters: Ex: 123:0A becomes 123
        bool checksum_Verify();      ///< @returns TRUE if for example: actual checksum 0A matches calculated 0A of string "123"
        /** @} */


        /**
         * @{ \name Assignment Operators
         */
        void operator=(const char* pString);  ///< Assign a string: myCStr = "123";
        void operator=(int num);              ///< Assign an int : myCStr = 123;
#if STR_SUPPORT_FLOAT
        void operator=(float num);            ///< Assign a float: myCStr = 1.23;
#endif
        str& operator=(const str& rhs);       ///< Assign Operator for str a = str b
        /** @} */


        /**
         * @{ \name Increment & Decrement Operators if the string consist of integer value
         */
        void operator++();           ///< Pre-Increment if String is an integer
        void operator++(int unused); ///< Post-Increment if String is an integer
        void operator--();           ///< Pre-Decrement if String is an integer
        void operator--(int unused); ///< Post-Decrement if String is an integer
        /** @} */


        /**
         * Add/Subtract Operators:
         */
        void operator+=(const char singleChar); ///< Append char using += Operator
        void operator+=(const char* pString);   ///< Append String using += Operator
        void operator+=(int n);                 ///< Subtract Integer using += Operator if string is an integer value
        void operator-=(const char* pString);   ///< Remove all instances of pString using -= Operator
        void operator-=(int n);                 ///< Subtract Integer String using if string is an integer value
#if STR_SUPPORT_FLOAT
        void operator+=(float n);               ///< Subtract Float using += Operator if string is an float value
        void operator-=(float n);               ///< Subtract Float String using -= Operator if string is an float value
#endif
        /** @} */


        /**
         * @{ \name Equality Operators
         */
        bool operator==(const char* pString) const;
        bool operator==(const str& rhs) const;
        bool operator==(int) const;
        bool operator!=(const char* pString) const;
        bool operator!=(int) const;
        // Note: Floats cannot be compared using == operator, they require delta comparison
        /** @} */


        /**
         * @{ \name Comparison Operators to perform comparisons, such as:
         * str s;
         * s = "hello";  if(s < "helloo") ...
         * s = 123;      if(s < 1234) ...
         * s = 1.23;     if(s >= 1.22) ...
         */
        bool operator<(const char* pString) const;
        bool operator<(int) const;
        bool operator>(const char* pString) const;
        bool operator>(int) const;
        bool operator<=(const char* pString) const;
        bool operator<=(int) const;
        bool operator>=(const char* pString) const;
        bool operator>=(int) const;

#if STR_SUPPORT_FLOAT
        bool operator<(float) const;
        bool operator>=(float) const;
        bool operator>(float) const;
        bool operator<=(float) const;
#endif
        /** @} */


        const char* operator()() const;                   ///< () Operator (without a name): Ex: puts(myCStr());
        const char* c_str() const { return (*this)(); };  ///< Get c-string pointer (calls operator above)

#if STR_SUPPORT_FLOAT
        operator float() const;     ///< (float) Cast Operator: Ex: float x = (float)myCStr;
#endif
        operator int() const;       ///< (int) Cast Operator: Ex: int x = (int)myCStr;
        char& operator[](int pos);  ///< Index Operator to get and set value @ Index


    private:
        int mCapacity;              ///< Capacity of the memory of this string
        int mLenOfEncryptedChars;   ///< Length of characters that were originally encrypted
        char* mpStr;                ///< Pointer to the primary memory
        str* mpTempStr;             ///< Avoid construction of new object for substr functions
        char* mpTokenPtr;           ///< Used for getToken() to remember last token location
        static const int mInvalidIndex = -1;


        /// init() is called by constructors to initialize the string
        void init(int initialLength=16)
        {
            mCapacity = 0;
            mLenOfEncryptedChars = 0;
            mpStr = 0;
            mpTempStr = 0;
            mpTokenPtr = 0;

            reAllocateMem(initialLength);
        }

        /// Ensures that the string contains enough memory to store additional nChars characters
        void ensureMemoryToInsertNChars(const int nChars);

        /// reallocates memory for this string given the new size
        void reAllocateMem(const int size);

        /// copies to our string the string given by pString
        void copyFrom(const char* pString);

        /// Converts single hexadecimal to int, such as "A" to the value of 10
        int singleHexCharToInt(unsigned char theChar);

        /// Converts hexadecimal value of string to its integer equivalent (base 10)
        int hexStrDigitsToInt(char* pString);

        /// Encryps the string given the password and the length to encrypt
        void encrypt(const char* pPassword, const int lenToEncrypt);
};

#endif /* STR_HPP__ */
