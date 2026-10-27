// Yusif Musayev 251ADB135
// https://github.com/musayevyusif/Programming_Languages_C_2026_Graded_Lab
// Compile with: gcc -O2 -Wall -Wextra -std=c17 -o calc calc.c -lm
/* 
Resources used:
- AI
- ARGUMENTS: https://www.geeksforgeeks.org/cpp/command-line-arguments-in-c-cpp/
- READING FILES: https://www.w3schools.com/c/c_files_read.php
*/


// ALL COMMENTS ARE WROTEN BY ME ONLY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> //said CHATGPT
#include <sys/stat.h> //said CHATGPT
#include <math.h>
#include <dirent.h>

typedef struct
{
    int is_float;
    long long int_value;
    double float_value;
} Value;


//==================================================================
char *read_line(FILE *input);
int check_blankline(const char *line);
int comment_checker(const char *line);
int parse_expression(const char **p, const char *line, long long line_start, Value *result, long long *error_pos);
int process_directory(const char *inputdir, const char *outdir);
int parse_term(const char **p, const char *line, long long line_start, Value *result, long long *error_pos);
int parse_primary(const char **p, const char *line, long long line_start, Value *result, long long *error_pos);
int parse_power(const char **p, const char *line, long long line_start, Value *result, long long *error_pos);



double value_to_double(Value v)
{
    if (v.is_float)
    {
        return v.float_value;
    }

    return (double)v.int_value;
}

//===================== SPACE SKIPPER FUNCTION =====================

//WHOLE SPACE SKIPPER MADE by MYSELF

void skip_spaces(const char **p)
{
    while (**p == ' ' || **p == '\t') //skips the spaces. 
    {
        (*p)++;
    }
}

//==================================================================
















//============================= ERROR CORRECTIONS. HELP from chatpg=============================



int process_file(const char *input_path, const char *outdir)
{
    FILE *input = fopen(input_path, "r");

    if (input == NULL)
    {
        fprintf(stderr, "ERROR: Missing input file or folder: %s\n", input_path);
        return 1;
    }

    const char *filename = strrchr(input_path, '/');

    if (filename == NULL)
    {
        filename = input_path;
    }
    else
    {
        filename++;
    }

    char base[300];
    snprintf(base, sizeof(base), "%s", filename);

    char *dot = strrchr(base, '.');

    if (dot != NULL && strcmp(dot, ".txt") == 0)
    {
        *dot = '\0';
    }

    char output_path[600];

    snprintf(
        output_path,
        sizeof(output_path),
        "%s/%s_yusif_musayev_251ADB135.txt",
        outdir,
        base
    );

    FILE *output = fopen(output_path, "w");

    if (output == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open output file\n");
        fclose(input);
        return 1;
    }

    long long file_pos = 1;
    long long error_pos = 0;
    int has_error = 0;

    char *line;

    while ((line = read_line(input)) != NULL)
    {
        if (check_blankline(line))
        {
            file_pos += strlen(line);
            free(line);
            continue;
        }

        if (comment_checker(line))
        {
            file_pos += strlen(line);
            free(line);
            continue;
        }

        const char *p = line;
        Value result;

        if (!parse_expression(
                &p,
                line,
                file_pos,
                &result,
                &error_pos))
        {
            has_error = 1;
            free(line);
            break;
        }

        skip_spaces(&p);

        if (*p != '\0' && *p != '\n')
        {
            error_pos = file_pos + (p - line);
            has_error = 1;
            free(line);
            break;
        }

        if (result.is_float)
        {
            fprintf(output, "%.15g\n", result.float_value);
        }
        else
        {
            fprintf(output, "%lld\n", result.int_value);
        }

        file_pos += strlen(line);
        free(line);
    }

    if (has_error)
    {
        fclose(output);

        output = fopen(output_path, "w");

        if (output == NULL)
        {
            fclose(input);
            return 1;
        }

        fprintf(output, "ERROR:%lld\n", error_pos);
    }

    fclose(input);
    fclose(output);

    return 0;
}

















int process_directory(const char *inputdir, const char *outdir)
{
    DIR *dir = opendir(inputdir);

    if (dir == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open input directory: %s\n", inputdir);
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        const char *name = entry->d_name;
        size_t len = strlen(name);

        if (len < 4)
        {
            continue;
        }

        if (strcmp(name + len - 4, ".txt") != 0)
        {
            continue;
        }

        char input_path[600];

        snprintf(
            input_path,
            sizeof(input_path),
            "%s/%s",
            inputdir,
            name
        );

        struct stat st;

        if (stat(input_path, &st) != 0)
        {
            continue;
        }

        if (!S_ISREG(st.st_mode))
        {
            continue;
        }

        if (process_file(input_path, outdir) != 0)
        {
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);

    return 0;
}









//==============================================================================================

// CHECK BLANKLINE is MADE WITH HALF HELP OF AI
int check_blankline(const char *line)
{
    for (int i = 0; line[i] != '\0'; i++)
    {

                                            //all blank line types:
        if (line[i] != ' ' &&
            line[i] != '\t' &&
            line[i] != '\n' &&
            line[i] != '\r')
        {
            return 0;
        }
    }

    return 1;
}









char *read_line(FILE *input)
{
    size_t size = 128;
    size_t len = 0;

    char *line = malloc(size);

    if (line == NULL)
    {
        return NULL;
    }

    int c;

    while ((c = fgetc(input)) != EOF)
    {
        if (len + 1 >= size)
        {
            size *= 2;

            char *temp = realloc(line, size);

            if (temp == NULL)
            {
                free(line);
                return NULL;
            }

            line = temp;
        }

        line[len++] = (char)c;

        if (c == '\n')
        {
            break;
        }
    }

    if (len == 0 && c == EOF)
    {
        free(line);
        return NULL;
    }

    line[len] = '\0';

    return line;
}





//======================== CHECK COMMENTS =========================

int comment_checker(const char *line)
{
    const char *p = line;

    while (*p == ' ' || *p == '\t')
    {
        p++;
    }

    if (*p == '#')
    {
        return 1;
    }

    return 0;
}

//===================== EXPRESSION PARSER ==========================

  int parse_expression(const char **p, const char *line, long long line_start, Value *result, long long *error_pos)
{
    if (!parse_term(p, line, line_start, result, error_pos)) //calling parse_tern
    {
        return 0;
    }

    while (1) //endless loop
    {
        skip_spaces(p); //skips spaces

        char op = **p;

        if (op != '+' && op != '-')  // if operation is not + or -, so there is nothing to do with parse_expression.
        {
            break;
        }

        (*p)++;

        Value value;

    if (!parse_term(p, line, line_start, &value, error_pos)) //calling parse term again for next pos
        {
            return 0;
        }

        if (op == '+')
{
    if (!result->is_float && !value.is_float)
    {
        result->int_value =
            result->int_value + value.int_value;
    }
    else
    {
        result->float_value =
            value_to_double(*result) + value_to_double(value);

        result->is_float = 1;
    }
}
else
{
    if (!result->is_float && !value.is_float)
    {
        result->int_value =
            result->int_value - value.int_value;
    }
    else
    {
        result->float_value =
            value_to_double(*result) - value_to_double(value);

        result->is_float = 1;
    }
}
    }

    return 1;

}
//=================================================================

//======================= PRIMARY PARSION =========================

int parse_primary(const char **p, const char *line, long long line_start, Value *result, long long *error_pos)
{
    skip_spaces(p); //skips spaces

if (**p >= '0' && **p <= '9')
{
    const char *start = *p;
    const char *q = *p;

    while (*q >= '0' && *q <= '9')
    {
        q++;
    }

    int has_dot = 0;

    if (*q == '.')
    {
        if (*(q + 1) >= '0' && *(q + 1) <= '9')
        {
            has_dot = 1;
            q++;

            while (*q >= '0' && *q <= '9')
            {
                q++;
            }
        }
    }

    if (has_dot)
    {
        char *end;
        double number = strtod(start, &end);

        result->is_float = 1;
        result->float_value = number;

        *p = q;
    }
    else
    {
        char *end;
        result->is_float = 0;
        result->int_value = strtoll(start, &end, 10);

        *p = end;
    }

    return 1;
}


    if (**p == '(') // if it is not number, check if it is ( or not
    {
        (*p)++;

        if (!parse_expression(p, line, line_start, result, error_pos))
        {
            return 0;
        }

        skip_spaces(p); //skips the spaces

        if (**p != ')') //check if closed the parantez
        {
            *error_pos = line_start + (*p - line); // it is error becaouse there is no ")"
            return 0;
        }

        (*p)++; //next positon

        return 1;
    }

    *error_pos = line_start + (*p - line);
    return 0;
}

//======================== POWER PARSER ===========================

int parse_power(const char **p, const char *line, long long line_start, Value *result, long long *error_pos)
{
     if (!parse_primary(p, line, line_start, result, error_pos))
    {
        return 0;
    }






    skip_spaces(p);

   if (**p == '*' && *(*p + 1) == '*')
{
    const char *operator_start = *p;

    (*p) += 2;

        skip_spaces(p);

        const char *exponent_start = *p;

        Value expo;

        if (!parse_power(p, line, line_start, &expo, error_pos))
        {
            return 0;
        }





        if (!expo.is_float && expo.int_value < 0)
        {
            *error_pos = line_start + (exponent_start - line);
            return 0;
        }





        if (!result->is_float && !expo.is_float)
        {
            long long base = result->int_value;
            long long exp = expo.int_value;
            long long power_result = 1;

            for (long long i = 0; i < exp; i++)
            {
                power_result *= base;
            }

            result->int_value = power_result;
            result->is_float = 0;
        }





        else
        {
            double base = value_to_double(*result);
            double exp = value_to_double(expo);

            double answer = pow(base, exp);



            
           if (!isfinite(answer))
{
    *error_pos = line_start + (operator_start - line);
    return 0;
}




            result->float_value = answer;
            result->is_float = 1;
        }




    }





    return 1;
}




//======================== TERRM PARSION ===========================

int parse_term(const char **p, const char *line, long long line_start, Value *result, long long *error_pos)
{

    if (!parse_power(p, line, line_start, result, error_pos)) //changed from parseprimary to parsepower
{
    return 0;
}




    while (1)                  //endless loop
    {
        skip_spaces(p);

        char op = **p;





        if (op != '*' && op != '/')
        {
            break; // if it s not * or / , parse_term is finished
        }





        (*p)++;

        skip_spaces(p);

        if (**p == '\0' || **p == '\n') // // if there is nothing after the last operation, example: "5 * ", it is our error.
        {
            *error_pos = line_start + (*p - line);
            return 0;
        }





        const char *operand_start = *p;

        Value value;

if (!parse_power( p, line, line_start, &value, error_pos)) //changed again from primary to power
{
    return 0;
}





      if (op == '*')
{
    if (!result->is_float && !value.is_float)
    {
        result->int_value = result->int_value * value.int_value;
    }





    else
    {
        result->float_value =
            value_to_double(*result) * value_to_double(value);

        result->is_float = 1;
    }





}





else
{
    double divisor = value_to_double(value);




    if (divisor == 0.0)
    {
        *error_pos = line_start + (operand_start - line);
        return 0;
    }




    if (!result->is_float && !value.is_float)
    {
        result->int_value = result->int_value / value.int_value;
    }



    else
    {
        result->float_value =
            value_to_double(*result) / divisor;

        result->is_float = 1;
    }




}



    }






    return 1;
}

//==================================================================











//=========================== MAIN ================================

int main(int argc, char *argv[])
{
    const char *input_file = NULL;
    const char *outdir = NULL;
    const char *inputdir = NULL;







    if (argc == 1)
    {
        fprintf(stderr,
                "Usage: calc [-d DIR | --dir DIR] "
                "[-o OUTDIR | --output-dir OUTDIR] [input.txt]\n");
        return 1;
    }







    for (int i = 1; i < argc; i++)
    {



        if (strcmp(argv[i], "-o") == 0 ||
            strcmp(argv[i], "--output-dir") == 0)
        {



            if (i + 1 >= argc)
            {
                fprintf(stderr, "ERROR: Missing output directory\n");
                return 1;
            }   



            outdir = argv[++i];
        }




        else if (strcmp(argv[i], "-d") == 0 ||
                 strcmp(argv[i], "--dir") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "ERROR: Missing input directory\n");
                return 1;
            }

            inputdir = argv[++i];
        }






        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "ERROR: Unknown option: %s\n", argv[i]);
            return 1;
        }





        else
        {
            input_file = argv[i];
        }




    }











    char default_outdir[600];

    if (outdir == NULL)
    {




        if (inputdir != NULL)
        {
            const char *dirname = strrchr(inputdir, '/');

            if (dirname == NULL)
            {
                dirname = inputdir;
            }
            else
            {
                dirname++;
            }

            snprintf(
                default_outdir,
                sizeof(default_outdir),
                "%s_yusif_musayev_251ADB135",
                dirname
            );
        }






        else if (input_file != NULL)
        {
            const char *filename = strrchr(input_file, '/');

            if (filename == NULL)
            {
                filename = input_file;
            }
            else
            {
                filename++;
            }

            char base[300];

            snprintf(base, sizeof(base), "%s", filename);

            char *dot = strrchr(base, '.');

            if (dot != NULL && strcmp(dot, ".txt") == 0)
            {
                *dot = '\0';
            }

            snprintf(
                default_outdir,
                sizeof(default_outdir),
                "%s_yusif_musayev_251ADB135",
                base
            );
        }




        else
        {
            fprintf(stderr, "ERROR: Missing input\n");
            return 1;
        }




        outdir = default_outdir;
    }










    if (mkdir(outdir, 0777) != 0 && errno != EEXIST)
    {
        fprintf(stderr, "ERROR: Cannot create output directory\n");
        return 1;
    }




    if (inputdir != NULL)
    {
        return process_directory(inputdir, outdir);
    }





    if (input_file == NULL)
    {
        fprintf(stderr, "ERROR: Missing input file\n");
        return 1;
    }





    return process_file(input_file, outdir);
}