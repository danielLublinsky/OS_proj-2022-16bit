#include "../includes/string.h"

//return the length of the param string
int strlen(char* str)
{
    const char* p = str;

    while (*p) ++p;

    return p - str;
}

void swap(char* p1, char* p2)
{
    char temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}


void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        swap((str + start), (str + end));
        start++;
        end--;
    }
}


int itoa(int num, char* str, int base){
	int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitly, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return 1;;
}

int atoi(char* str)
{
    int res = 0;

    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    return res;
}

int strcpy(char* destination, const char* source)
{
    char* ptr = destination;

    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }

    *destination = '\0';
    return 1;
}

int strncpy(char* dest, const char* src, uint_64 n)
{
    for(int i = 0; i< n; i++)
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';

    return 1;
}

int boolToChar(bool vIn, char* str)
{
    str = vIn ? "true" : "false";
} 

//num- intteger number to convert into an Hax
//str- string to store the hex value in
int hexToString(uint_64 num, char* str)
{
    char hexChars[] = "0123456789ABCDEF";

    uint_64 temp = num;
    int i = 0;
    if (num == 0) {
        str[0] = '0';
        i++;
    }
    while (num > 0) {
        num /= 16;
        str[i++] = hexChars[temp - (16 * num)];
        temp = num;  
    }
    str[i] = 'x';
    str[i + 1] = '0';
    //now reverse the char* before the return
    reverse(str, strlen(str));
    return 1;
}

void* memset(void* s, int c, uint_32 count) {
    char* xs = (char*)s;

    while (count--)
        *xs++ = c;

    return s;
}

void memcpy(void* destination, void* source, uint_64 num)
{
    if (num <= 8) {
        uint_8* valPtr = (uint_8*)source;
        for (uint_8* ptr = (uint_8*)destination; ptr < (uint_8*)((uint_64)destination + num); ptr++) {
            *ptr = *valPtr;
            valPtr++;
        }

        return;
    }

    uint_64 proceedingBytes = num % 8;
    uint_64 newnum = num - proceedingBytes;
    uint_64* srcptr = (uint_64*)source;

    for (uint_64* destptr = (uint_64*)destination; destptr < (uint_64*)((uint_64)destination + newnum); destptr++) {
        *destptr = *srcptr;
        srcptr++;
    }

    uint_8* srcptr8 = (uint_8*)((uint_64)source + newnum);
    for (uint_8* destptr8 = (uint_8*)((uint_64)destination + newnum); destptr8 < (uint_8*)((uint_64)destination + num); destptr8++)
    {
        *destptr8 = *srcptr8;
        srcptr8++;
    }
}

int strcmp(char* str1, char* str2)
{
    while ((*str1 != '\0' && *str2 != '\0') && *str1 == *str2)
    {
        str1++;
        str2++;
    }

    if (*str1 == *str2)
    {
        return 1; // strings are identical
    }
    
    return 0;
}

int strncmp(char* str1, char* str2, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (str1[i] != str2[i])  return 0;
    }
    return 1;
}

char* strtok(char* s, char d)
{
    // Stores the state of string
    static char* input = 0;
    //memset(input, 0, 100);

    // Initialize the input string
    if (s != 0)
        input = s;

    // Case for final token
    if (input == 0)
        return 0;

    // Stores the extracted string
    //char* result = (char*)malloc(strlen(s) + 1);
    static char* result;
    int i = 0;

    // Start extracting string and
    // store it in array
    for (; input[i] != '\0'; i++) {

        // If delimiter is not reached
        // then add the current character
        // to result[i]
        if (input[i] != d) {
            result[i] = input[i];
        }

        // Else store the string formed
        else {
            result[i] = '\0';
            input = input + i + 1;
            return result;
        }
    }

    // Case when loop ends
    result[i] = '\0';
    input = 0;

    // Return the resultant pointer
    // to the string
    return result;
}

int strCount(char* str, const char c)
{
    int len = strlen(str);
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == c)
            count++;
    }
    return count;
}

char* deleteSpaces(char* str)
{
    while (*str == ' ') str++;
    int i = strlen(str) - 1;
    while (str[i] == ' ') {
        str[i] = '\0';
        i--;
    }

    return str;
}

#include "../includes/vga_print.h"
char* my_strtok(char* s, char d)
{
    char* s_cpy = s;

    int countChars = 0;

    while (s_cpy[countChars] != '\0' && s_cpy[countChars] != d) {
        countChars++;
    }

    char* dest = (char*)malloc(10);
    strncpy(dest, s, countChars);


    return dest;
}

//returns the pointer to dest after the connection
char* append(char* dest, char* src)
{
    char* destPtr = dest;

    dest = dest + strlen(dest);
    strcpy(dest, src);

    return destPtr;
}

int strFind(char* str, char c)
{
    int countChars = 0;
    char* ptr = str;
    while (*ptr != c && *ptr != '\0') {
        countChars++;
        ptr++;
    }
    if (*ptr == c) {
        return countChars;
    }
    else {
        return -1;
    }
}

int isNumber(char* num)
{
    for (int i = 0; i < strlen(num) - 1; i++) {
        if (num[i] < '0' || num[i] > '9')           //number must be in this range
            return false;
    }

    return true;
}

char* strchr(const char* str, int c)
{
    while (*str != c && *str != '\0') {
        str++;
    }
    if (*str == c) {
        return str;
    }
    else {
        return 0;
    }
}