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