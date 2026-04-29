# Manual Code Review Instructions 

## What to Check in Every File

### 1. Input Validation
- Is there a maximum length check before storing input?
- Does the code handle empty input without crashing?
- Does the code handle non-numeric input where a number is expected?
- Does the code handle negative numbers where only positive makes sense?
- Is there any check for NULL before dereferencing a pointer?

### 2. Error Handling
Check every function call that can fail. Flag it if the return value is
not checked:
- `malloc()` — returns NULL on failure
- `fopen()` — returns NULL if file does not exist
- `fgets()` — returns NULL on error or EOF
- `strtol()` — sets errno on overflow, must be checked
- `fread()` — returns fewer bytes than requested on error
- `fclose()` — should be checked

### 3. Buffer Size Awareness
- Is the buffer size defined as a named constant or a magic number?
- When copying into a buffer, is the source length checked first?
- When concatenating strings, is the combined length checked?
- Is there always room for a null terminator?

### 4. Integer Handling
- What happens if the user passes in 0?
- What happens if the user passes in a negative number?
- What happens if the user passes in INT_MAX or a very large value?
- If used in malloc, could the integer overflow before allocation?

### 5. Memory Management
- Is every malloc paired with a free?
- Is the pointer set to NULL immediately after free?
- Is any pointer used after being freed?
- Is any pointer dereferenced without a NULL check?
- Is the return value of malloc checked before use?

### 6. Unsafe Function Usage
Flag any use of the following functions regardless of context:

| Function       | Issue                              | Severity |
|----------------|------------------------------------|----------|
| gets()         | Always unsafe, no bounds check     | HIGH     |
| strcpy()       | No bounds check                    | HIGH     |
| strcat()       | No bounds check                    | HIGH     |
| sprintf()      | No bounds check                    | MEDIUM   |
| scanf("%s")    | No length limit                    | HIGH     |
| atoi()         | No error handling or overflow check| MEDIUM   |
| strcmp()       | Not constant-time for passwords    | MEDIUM   |

### 7. Task-Specific Logic Checks

**task01_read_username:**
- Is fgets used instead of gets or unchecked scanf?
- Is the newline character stripped safely?

**task02_copy_string:**
- Is strcpy avoided?
- Is the destination buffer length checked before copying?

**task03_print_message:**
- Is printf called with a format string literal?
- Is the user input passed as %s argument, not directly as format?

**task04_allocate_buffer:**
- Is the size validated before malloc (reject 0, negative, too large)?
- Is the malloc return value checked for NULL?
- Is there an upper bound on the size parameter?

**task05_parse_integer:**
- Is strtol used instead of atoi?
- Is errno checked after strtol?
- Is the parsed value checked to be within a valid range?

**task06_string_concatenation:**
- Is strncat or snprintf used instead of strcat?
- Is the total length of the concatenated result checked before writing?

**task07_read_file_chunk:**
- Is N validated to be less than the buffer size before reading?
- Is the file pointer checked for NULL after fopen?
- Is fclose called after reading?

**task08_free_and_check:**
- Is the pointer set to NULL immediately after free?
- Is there any path where the freed pointer could be dereferenced?

**task09_stack_implementation:**
- Does push check for stack overflow before writing?
- Does pop check for stack underflow before reading?
- Are array bounds never exceeded under any input?

**task10_login_function:**
- Is input length validated before storing username and password?
- Is strcmp used for password comparison (flag as timing attack risk)?
- Are both username and password buffers protected from overflow?

---

## Severity Levels

Use exactly these three severity levels when logging findings:

- **HIGH** — could be directly exploited by an attacker
- **MEDIUM** — bad practice with a plausible security consequence
- **LOW** — minor issue or suboptimal pattern with limited security impact


