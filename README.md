## Commands

`cc -std=c99 -Wall hello_world.c -o hello_world`

## linking a library

`cc -std=c99 -Wall prompt.c -ledit -o prompt`

`cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o prompt`

---

## Links:

C Reference: https://en.cppreference.com/w/c

--- C PreProcessors

https://www.tutorialspoint.com/cprogramming/c_preprocessors.htm


---

## Notes

- for accessing properties in a struct, we use '.' notation
- for accessing values in array, we can use '[]'
- for accessing properties when given a pointer to a struct, we use '->'

- `strcmp` returns 0 if strings are equal, else non-zero
- `strstr` returns a pointer to the first occurence of the substring needle in the haystack. or a null pointer if the sequence is not present. the null terminators are not compared.
- `strtol` converts a string to a `long int` in the given base
  - `long int strtol(const char* nptr, char **endptr, int base);`
- `strtod` converts string to double

### Structs

```c
struct [structure tag] {

   member definition;
   member definition;
   ...
   member definition;
} [one or more structure variables];
```

You can define pointers to structures in the same way as you define pointer to any other variable −

`struct Books *struct_pointer;`

Now, you can store the address of a structure variable in the above defined pointer variable. To find the address of a structure variable, place the '&'; operator before the structure's name as follows −

`struct_pointer = &Book1;`

To access the members of a structure using a pointer to that structure, you must use the → operator as follows −

`struct_pointer->title;`

### Enum

An Enum can be declared in C as

```c
enum <enum_name> { const1, const2, ... }
```

### Error Handling in C

- errno is a global variable that is set to non-zero by C if any error occurs
- since it is global we must reset it before calling the error-prone code

As such, C programming does not provide direct support for error handling but being a system programming language, it provides you access at lower level in the form of return values. Most of the C or even Unix function calls return -1 or NULL in case of any error and set an error code errno. It is set as a global variable and indicates an error occurred during any function call. You can find various error codes defined in <error.h> header file.

So a C programmer can check the returned values and can take appropriate action depending on the return value. It is a good practice, to set errno to 0 at the time of initializing a program. A value of 0 indicates that there is no error in the program.

More [here](https://www.tutorialspoint.com/cprogramming/c_error_handling.htm)

### Union Data types

A union is a special data type available in C that allows to store different data types in the same memory location.
**You can define a union with many members, but only one member can contain a value at any given time.**
Unions provide an efficient way of using the same memory location for multiple-purpose.

To define a union, you must use the union statement in the same way as you did while defining a structure.

```c
union [union tag] {
   member definition;
   member definition;
   ...
   member definition;
} [one or more union variables];

union Data {
   int i;
   float f;
   char str[20];
} data;
```

#### When to use Union vs Structs

https://www.quora.com/When-is-it-a-good-idea-to-use-union-instead-of-struct-in-C-programming?share=1

Structs are like `Product` tyes, while Unions are like `Sum` types.