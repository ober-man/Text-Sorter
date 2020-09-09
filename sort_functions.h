#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

const long MAXSYMB = 10e6; ///max number of symbols in file
const int MAXLINES = 10e6; ///max number of lines in file

/** Struct "line" contains pointer to the start of the line end it's length **/
struct line
{
    char* pointer;
    int len;
};

long CountSymbols(FILE *f);
int CountLines(char text[], int max);
char* FileToArray(FILE* file, long numsymb);
line* ArrayToStruct(char text[], int numline, long numsymb);
int Compar(const void* a, const void* b);
char* PunctOut(char* str);
int Min(int a, int b);
void StructToFile(line* text, int number, FILE* file);
int ComparEnd(const void* a, const void* b);
void ArrayToFile(char text[], long numsymb, FILE* file);

//-----------------------------------------------
//! Function "CountSymbols" count symbols in file
//!
//!@param [in] file File we are working with
//!
//!@return Number of symbols
//!
//-----------------------------------------------
long CountSymbols(FILE* file)
{
    /** Checking correctness of entry **/
    assert(file != NULL);

    /** Counting symbols **/
    fseek(file, 0, SEEK_END);
    long count = ftell(file);
    rewind(file);

    /** Checking emptiness **/
    if(count == 0)
    {
        printf("File is empty");
        exit(1);
    }
    /** Checking overfilled **/
    if(count > MAXSYMB)
    {
        printf("Too big file");
        exit(1);
    }

    return count;
}

//------------------------------------------------------------------------
//! Function "CountLines" count lines in array and change all '\n' to '\0'
//!
//!@param [in] text Array we are working with
//!@param [in] numsymb Number of symbols in array
//!
//!@return Number of lines
//!
//!@note Changing is needed for convenience in futher work
//------------------------------------------------------------------------
int CountLines(char text[], int numsymb)
{
    /** Checking correctness of entry **/
    assert(text != NULL);
    assert(isfinite(numsymb));
    assert(numsymb != 0);
    assert(numsymb <= MAXSYMB);

    /** Counting lines and changing **/
    int count = 1;
    for(int i = 0; (i < numsymb) && (text[i] != '\0'); ++i)
        if(text[i] == '\n')
        {
            text[i] = '\0';
            ++count;
        }

    /** Checking emptiness **/
    if((count <= 2) && (text[0] == '\0'))
    {
        printf("File is empty");
        exit(1);
    }
    /** Checking overfilled **/
    if(count > MAXLINES)
    {
        printf("Too many lines");
        exit(1);
    }
    return count;
}

//----------------------------------------------
//! Function "FileToArray" write file to array
//!
//!@param [in] file File we are working with
//!@param [in] numsymb Number of symbols in file
//!
//!@return Pointer to the start of the array
//!
//----------------------------------------------
char* FileToArray(FILE* file, long numsymb)
{
    /** Checking correctness of entry **/
    assert(file != NULL);
    assert(isfinite(numsymb));
    assert(numsymb != 0);
    assert(numsymb <= MAXSYMB);

    /** Creating array of symbols **/
    char* text = (char*)calloc(numsymb, sizeof(char));
    fread(text, sizeof(char), numsymb, file);
    return text;
}

//----------------------------------------------------------------------------------
//! Function "ArrayToStruct" write data from array of symbols to array of structures
//!
//!@param [in] text Array we are working with
//!@param [in] numline Number of lines in original file
//!@param [in] numsymb Number of symbols in original file
//!
//!@return Pointer to the start of the array of structures
//!
//----------------------------------------------------------------------------------
line* ArrayToStruct(char text[], int numline, long numsymb)
{
    /** Checking correctness of entry **/
    assert(text != NULL);
    assert(isfinite(numsymb));
    assert(isfinite(numline));
    assert(numsymb != 0);
    assert(numline != 0);
    assert(numsymb <= MAXSYMB);
    assert(numline <= MAXLINES);

    /** Creating array of structures **/
    line* str = (line*)calloc(numline, sizeof(line));
    str[0].pointer = text; /// beginning of array
    int length = 0;
    long j = 0;

    /** Filling the structures **/
    for(int i = 0; i < numline; ++i)
        for(j; j < numsymb; ++j)
        {
            if(text[j] != '\0')
            {
                ++length;
                continue;
            }
            str[i].len = length + 1;
            if((i + 1 < numline) && (j + 1 < numsymb)) /// the end is not reached
                str[i+1].pointer = &text[j+1];
            length = 0;
            ++j;
            break;
        }
    return str;
}

//-------------------------------------------------------------------------------------
//! Function "Compar" compares lines using the pointer from the structure
//!                                  and ignoring signes in the start and high-low case
//!
//!@param [in] a Pointer to a struct 1
//!@param [in] b Pointer to a struct 2
//!
//!@return  0, if line 1 = line 2
//!        <0, if line 1 < line 2
//!        >0, if line 1 > line 2
//-------------------------------------------------------------------------------------
int Compar(const void* a, const void* b)
{
    /** Checking correctness of entry **/
    assert(a != NULL);
    assert(b != NULL);

    line a2 = *(line *)a; /// turning
    line b2 = *(line *)b; /// to a structure

    char* str_a = a2.pointer; /// turning
    char* str_b = b2.pointer; /// to lines

    str_a = PunctOut(str_a); /// ignoring signes
    str_b = PunctOut(str_b); ///  in the start

    long min = Min(a2.len, b2.len);

    for(int i = 0; i < min; ++i)
    {
        if(tolower(str_a[i]) == tolower(str_b[i])) /// ignoring high-low case
            continue;
        else
            return tolower(str_a[i]) - tolower(str_b[i]);
    }
    return 0; /// if equality
}

//------------------------------------------------------------
//! Function "PunctOut" ignore signes in the start of the line
//!
//!@param [in] str Line we are working with
//!
//!@return Pointer to the end of the appearance of signes
//!
//------------------------------------------------------------
char* PunctOut(char* str)
{
    /** Checking correctness of entry **/
    assert(str != NULL);

    int i = 0;
    int len = strlen(str);
    for(i; i < len; ++i)
    {
        if(!isalpha(str[i]))
            continue;
        else
            break; /// when signes are over
    }
    return (str + i);
}

//------------------------------------------------------------
//! Function "Min" compare two integer numbers
//!
//!@param [in] a First number
//!@param [in] b Second number
//!
//!@return Least number
//!
//------------------------------------------------------------
int Min(int a, int b)
{
    return (a < b) ? a : b;
}

//---------------------------------------------------------------------
//! Function "StructToFile" write data from array of structures to file
//!
//!@param [in] text Structure we are working with
//!@param [in] number Number of elements in array of structures
//!@param [in] file Outgoing file
//!
//!
//---------------------------------------------------------------------
void StructToFile(line* text, int number, FILE* file)
{
    /** Checking correctness of entry **/
    assert(text != NULL);
    assert(isfinite(number));
    assert(number != 0);
    assert(file != NULL);

    /** Writing in file **/
    for(int i = 0; i < number; ++i)
    {
        if(text[i].len <= 2)
            continue;
        fputs(text[i].pointer, file);
        fputs("\n", file);
    }
    return;
}

//----------------------------------------------------------------------------------------
//! Function "ComparEnd" compares by rhymes lines using the pointer from the structure
//!                                     and ignoring signes in the start and high-low case
//!
//!@param [in] a Pointer to a struct 1
//!@param [in] b Pointer to a struct 2
//!
//!@return  0, if line 1 = line 2
//!        <0, if line 1 < line 2
//!        >0, if line 1 > line 2
//----------------------------------------------------------------------------------------
int ComparEnd(const void* a, const void* b)
{
    /** Checking correctness of entry **/
    assert(a != NULL);
    assert(b != NULL);

    line a2 = *(line *)a; /// turning
    line b2 = *(line *)b; /// to a structure

    char* str_a = a2.pointer; /// turning
    char* str_b = b2.pointer; /// to lines

    int len_a = strlen(str_a);
    int len_b = strlen(str_b);

    /** Ignored signes in the end **/
    for(int j = 0; ; ++j)
        if(isalpha(str_a[len_a-j]))
        {
            len_a -= j;
            break;
        }
    for(int j = 0; ; ++j)
        if(isalpha(str_b[len_b-j]))
        {
            len_b -= j;
            break;
        }

    /** Sorting by rhymes **/
    int min = Min(len_a, len_b);
    for(int i = 0; i < min; ++i)
    {
        if(tolower(str_a[len_a-i]) == tolower(str_b[len_b-i])) /// ignoring high-low case
            continue;
        else
            return tolower(str_a[len_a-i]) - tolower(str_b[len_b-i]);
    }
    return 0; /// if equality
}

//------------------------------------------------------
//! Function "ArrayToFile" write data from array to file
//!
//!@param [in] text Array we are working with
//!@param [in] numsymb Number of elements in array
//!@param [in] file Outgoing file
//!
//------------------------------------------------------
void ArrayToFile(char text[], long numsymb, FILE* file)
{
    /** Checking correctness of entry **/
    assert(text != NULL);
    assert(isfinite(numsymb));
    assert(file != NULL);
    assert(numsymb != 0);
    assert(numsymb <= MAXSYMB);

    /** Writing in file **/
    for(int i = 0; i < numsymb; ++i)
    {
        if(text[i] == '\0')
        {
            fputs("\n", file);
            continue;
        }
        fputc(text[i], file);
    }
    return;
}
