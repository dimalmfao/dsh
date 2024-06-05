// shitty af dont run this

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <dirent.h>

#define BUFFERSIZE 100
#define GREEN "\033[0;32m"
#define NOCLR "\033[0m"
#define BLUE "\033[0;34m"

char *inbuild[] = { "help", "sysinfo" };

char *help =
" --------------------------------------- \n"
" Dima Shell, another shitty shell made by\n"
"               made for idiot, by idiots.\n"
" \n"
" Visit https://github.com/dimalmfao/dsh\n"
"                   for more information.\n"
" \n"
" -           Built in commands:        - \n"
" \n"
" help - all built in commands\n"
" sysinfo - system information\n"
" --------------------------------------- \n";

void error(const char *msg);
char **parse(char *str);
int build_fn(char **args);
int process_exec(char **args);
int execute(char **args);
char *read_line();
void get_info();
void tab_completion(char *input);

int main() {
    system("clear");

    while (1) {
        get_info();
        char *line = read_line();
        tab_completion(line);
        char **args = parse(line);
        execute(args);

        free(line);
        free(args);
    }

    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

char **parse(char *str) {
    int buffer = 64;
    int position = 0;
    char **tokens = malloc(buffer * sizeof(char*));
    if (!tokens) error("malloc error");

    char *token = strtok(str, " \t\r\n\a");
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= buffer) {
            buffer += 64;
            tokens = realloc(tokens, buffer * sizeof(char*));
            if (!tokens) error("realloc error");
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

int build_fn(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        char *dir = args[1];
        if (!dir) {
            char *home = getenv("HOME");
            if (!home) home = "/"; // Default to root if HOME not set
            dir = home;
        }
        if (chdir(dir) != 0) {
            perror("chdir error");
        }
        return 1;
    } else if (strcmp(args[0], "help") == 0) {
        printf("%s", help);
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "sysinfo") == 0) {
        struct utsname buf;
        if (uname(&buf) != 0) {
            perror("uname error");
            return 1;
        }
        printf("System Name = %s\n", buf.sysname);
        printf("Node Name = %s\n", buf.nodename);
        printf("Version = %s\n", buf.version);
        printf("Release = %s\n", buf.release);
        printf("Machine = %s\n", buf.machine);
        return 1;
    }
    return 0;
}

int process_exec(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork error");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return 1;
}

int execute(char **args) {
    if (!args[0]) return 1;

    for (int i = 0; i < 4; i++) {
        if (strcmp(args[0], inbuild[i]) == 0) {
            return build_fn(args);
        }
    }
    return process_exec(args);
}

char *read_line() {
    int buffer_size = BUFFERSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    if (!buffer) error("malloc error");

    int ch;
    while (1) {
        ch = getchar();
        if (ch == EOF || ch == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position++] = ch;
        }

        if (position >= buffer_size) {
            buffer_size += BUFFERSIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) error("realloc error");
        }
    }
}

void get_info() {
    char hostname[100];
    char username[100];
    char cwd[200];

    if (gethostname(hostname, sizeof(hostname)) != 0) error("gethostname error");
    if (getlogin_r(username, sizeof(username)) != 0) error("getlogin_r error");
    if (!getcwd(cwd, sizeof(cwd))) error("getcwd error");

    printf("%s%s@%s:%s%s%s%s%c ", GREEN, username, hostname, NOCLR, BLUE, cwd, NOCLR, '$');
}

void tab_completion(char *input) {
    DIR *dir;
    struct dirent *ent;
    char *directory;

    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, input) == ent->d_name) {
                printf("%s\n", ent->d_name);
            }
        }
        closedir(dir);
    }
}
