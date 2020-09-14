// String Implementation
// IMPORTANT: Do not use any of the functions in the string C runtime library
// Example. Do not use strcpy, strcmp, etc. Implement your own

// IMPORTANT: See the MyString.h file for a description of
// what each method needs to do.

#include <stdio.h>
#include "MyString.h"

int
MyString::slength(const char *s) const
{
  // Add implementation here
        int count = 0;
        while (*s++)
        {
    count++;

        }
  return count;
}

// Initialize _s. Allocate memory for _s and copy s into _s
void
MyString::initialize(const char * s)
{
  // Add implementation here
  // Allocate memory for _s.
        _s = new char[slength(s)];
  // Copy s into _s
        int i;
        int length = slength(s) ;
        for ( i = 0; i < length; i++){
                _s[i] = s[i];
        }
        _s[i] = '\0';
}

// Create a MyString from a C string
MyString::MyString(const char * s)
{
  initialize(s);
}

// Create a MyString from a copy of another string
MyString::MyString(const MyString &s)
{
  initialize(s._s);
}

// Create a MyString with an empty string
MyString::MyString()
{
  _s = new char[1];
  *_s = 0;
}

MyString &
MyString::operator = (const MyString & other) {
        if (this != &other)
        {
                //return *this;


        //int size = slength(other);
    delete [] _s;

    // Initialize _s with the "other" object.
    initialize(other._s);

    // by convention, always return *this
    return *this;
  }
}

MyString
MyString::substring(int i, int n)
{
  // Add implementation here

  // Make sure that i is not beyond the end of string

  // Allocate memory for substring

  // Copy characters of substring
  MyString subSt;
  if (i > length()){
          return MyString();
  }
  char *_sub;
  _sub = new char[n + 1];
  for (int k = 0; k < n; k++){
          _sub[k] = _s[i + k];
  }
  _sub[n] = '\0';
  subSt = MyString(_sub);
  return subSt;
}

void
MyString::remove(int i, int n)
{
  // Add implementation here

  // If i is beyond the end of string return

  // If i+n is beyond the end trunc string

  // Remove characters
        int length = slength(_s);
        char *t = new char[length];
        int k = 0, j = 0;
        if (i > slength(_s)){
                return;
        }
        if ((i + n) > slength(_s)){
                substring(i, n);}
        while (_s[j] != '\0'){
                if ((j>= i)&& (j <= (n+i-1))) {
                        j++;
                        continue;}
                t[k] = _s[j];
                j++;
                k++;}
        t[k] = '\0';
    delete[] _s;
        _s = t;}

// Return true if strings "this" and s are equal
bool
MyString::operator == (const MyString & s)
{
  // Add implementation here
        int i = 0;
        while ((_s[i] == s._s[i]) && (_s[i] != '\0'&& s._s[i] != '\0'))
        {
                i++;
        }
return ((_s[i] - s._s[i]) == 0);
}


// Return true if strings "this" and s are not equal
bool
MyString::operator != (const MyString &s)
{
  // Add implementation here
        int i = 0;
        if (this->length() != s.length()) {
                return false;}
        for (i = 0; i < this->length(); i++) {
                if (this->_s[i] != s._s[i]) {
                        return true;}}
        return false;
}

// Return true if string "this" and s is less or equal
bool
MyString::operator <= (const MyString &s)
{
  // Add implementation here
        int i = 0;
        while ((_s[i] == s._s[i]) && (_s[i] != '\0'&& s._s[i] != '\0'))
        {
                i++;
        }
return ((_s[i] - s._s[i]) <= 0);
}

// Return true if string "this" is greater than s
bool
MyString::operator > (const MyString &s)
{
  // Add implementation here
        int i = 0;
        while ((_s[i] == s._s[i]) && (_s[i] != '\0'&& s._s[i] != '\0'))
        {
                i++;
        }
        return ((_s[i] - s._s[i]) > 0);
}

// Return character at position i.  Return '\0' if out of bounds.
char
MyString::operator [] (int i)
{
  // Add implementation here
        if (i > this->length()) {
                return '\0';
        }
        return this->_s[i];
}
// Return C representation of string
const char *
MyString::cStr()
{
  // Add implementation here
        return _s;
}

// Get string length of this string.
int
MyString::length() const
{
  // Add implementation here
  return slength(_s);
}

MyString::~MyString()
{
  // Add implementation here
  delete [] _s;
}

// Concatanate two strings (non member method)
MyString operator + (const MyString &s1, const MyString &s2)
{
  // Add implementation here

  // Allocate memory for the concatenated string

  // Add s1

  // Add s2

        int length = s1.length() + s2.length();
        int i = 0, j = 0, k = 0;
        char *str = new char[length + 1];
        for (i = 0; s1._s[i] != '\0'; i++) {
                str[i] = s1._s[i];}
        for (k = 0; s2._s[k] != '\0'; k++) {
                str[i] = s2._s[k];
                i++;    }
        str[i] = '\0';
        MyString s(str);
  return s;
}

char myStrcpy(char *dest, char*src){
        char *temp = dest;
        while (*src){
                *temp = *src;
                temp++;
                src++;
        }
        *(temp++) = 0;
}

