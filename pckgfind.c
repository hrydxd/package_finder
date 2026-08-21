#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_BOLD    "\033[1m"

//security for command injection etc.
int is_safe_query(const char *query) {
    for (int i = 0; query[i] != '\0'; i++) {
        if (!isalnum(query[i]) && 
            query[i] != '.' && query[i] != '-' && 
            query[i] != '_' && query[i] != '+') {
            return 0; // ilegal symbol
        }
    }
    return 1;
}

void search_packages(const char *query) {
    char command[512];
    char safe_query[256]; //safe method
    FILE *fp;
    char line[2048];
    int line_count = 0;
    
    strncpy(safe_query, query, sizeof(safe_query) - 1);
    safe_query[sizeof(safe_query) - 1] = '\0';
    
    if (!is_safe_query(safe_query)) {
        printf(COLOR_RED "Error: Query contains invalid characters\n" COLOR_RESET);
        printf(COLOR_CYAN "Only letters, numbers, ., -, _, + are allowed\n" COLOR_RESET);
        return;
    }

    int max_query_len = 100; 
    
    if (system("which apt-cache > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "apt-cache search --names-only %.*s | head -20", 
                 max_query_len, safe_query);
    }
    else if (system("which pacman > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "pacman -Ss %.*s | grep '^[^ ]*/%.*s' | head -20",
                 max_query_len, safe_query, max_query_len, safe_query);
    }
    else if (system("which dnf > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "dnf search --all --name %.*s | head -20",
                 max_query_len, safe_query);
    }
    else if (system("which zypper > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "zypper search --match-substrings %.*s | head -20",
                 max_query_len, safe_query);
    }
    else {
        printf(COLOR_RED "Not found any operated package manager\n" COLOR_RESET);
        return;
    }
    
    fp = popen(command, "r");
    if (fp == NULL) {
        printf(COLOR_RED "You use wrong command, try something else\n" COLOR_RESET);
        return;
    }
    
    printf(COLOR_BOLD COLOR_CYAN "Found packages for '%s':\n" COLOR_RESET, safe_query);
    printf(COLOR_YELLOW "----------------------------------------\n" COLOR_RESET);
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_count++;
        line[strcspn(line, "\n")] = 0;
        
        char *slash = strchr(line, '/');
        char *space = strchr(line, ' ');
        
        if (slash != NULL && space != NULL) {
            char repo[64], name[128], version[64];
            int repo_len = slash - line;
            int name_len = space - slash - 1;
            
            //security
            if (repo_len >= (int)sizeof(repo)) repo_len = sizeof(repo) - 1;
            if (name_len >= (int)sizeof(name)) name_len = sizeof(name) - 1;
            
            strncpy(repo, line, repo_len);
            repo[repo_len] = '\0';
            
            strncpy(name, slash + 1, name_len);
            name[name_len] = '\0';
            
            strncpy(version, space + 1, sizeof(version) - 1);
            version[sizeof(version) - 1] = '\0';
            
            printf(COLOR_BOLD COLOR_BLUE "%s" COLOR_RESET, name);
            printf(" " COLOR_GREEN "%s" COLOR_RESET, version);
            printf(" " COLOR_CYAN "(%s)" COLOR_RESET, repo);
        }
        else if (slash != NULL) {
            char repo[64], name[128];
            int repo_len = slash - line;
            
            if (repo_len >= (int)sizeof(repo)) repo_len = sizeof(repo) - 1;
            
            strncpy(repo, line, repo_len);
            repo[repo_len] = '\0';
            
            strncpy(name, slash + 1, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
            
            printf(COLOR_BOLD COLOR_BLUE "%s" COLOR_RESET, name);
            printf(" " COLOR_CYAN "(%s)" COLOR_RESET, repo);
        }
        else {
            printf(COLOR_BOLD COLOR_BLUE "%s" COLOR_RESET, line);
        }
        
        printf("\n");
    }
    
    printf(COLOR_YELLOW "----------------------------------------\n" COLOR_RESET);
    printf(COLOR_GREEN "Found %d packages\n" COLOR_RESET, line_count);
    
    pclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(COLOR_BOLD "Use: %s <name_package>\n" COLOR_RESET, argv[0]);
        printf(COLOR_CYAN "Example: %s rar\n" COLOR_RESET, argv[0]);
        return 1;
    }
    
    char query[256] = "";
    
    for (int i = 1; i < argc; i++) {
        size_t current_len = strlen(query);
        size_t arg_len = strlen(argv[i]);
        
        if (current_len + arg_len + 1 >= sizeof(query)) {
            printf(COLOR_RED "Error: Query too long\n" COLOR_RESET);
            return 1;
        }
        
        strncat(query, argv[i], sizeof(query) - current_len - 1);
        
        if (i < argc - 1) {
            if (strlen(query) + 1 >= sizeof(query)) {
                printf(COLOR_RED "Error: Query too long\n" COLOR_RESET);
                return 1;
            }
            strncat(query, " ", sizeof(query) - strlen(query) - 1);
        }
    }
    
    search_packages(query);
    
    return 0;
}