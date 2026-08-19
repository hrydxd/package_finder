#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_BOLD    "\033[1m"

void search_packages(const char *query) {
    char command[512];
    FILE *fp;
    char line[2048];
    int line_count = 0;

    if (system("which apt-cache > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "apt-cache search --names-only %s | head -20", query);
    }
    else if (system("which pacman > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "pacman -Ssq %s | head -20", query);
    }
    else if (system("which dnf > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "dnf search --all --name %s | head -20", query);
    }
    else if (system("which zypper > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "zypper search --match-substrings %s | head -20", query);
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
    
    printf(COLOR_BOLD COLOR_CYAN "Found packages for '%s':\n" COLOR_RESET, query);
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
            int version_len = strlen(space + 1);
            
            strncpy(repo, line, repo_len);
            repo[repo_len] = '\0';
            
            strncpy(name, slash + 1, name_len);
            name[name_len] = '\0';
            
            strncpy(version, space + 1, version_len);
            version[version_len] = '\0';
            
            printf(COLOR_BOLD COLOR_BLUE "%s" COLOR_RESET, name);
            printf(" " COLOR_GREEN "%s" COLOR_RESET, version);
            printf(" " COLOR_CYAN "(%s)" COLOR_RESET, repo);
        }
        else if (slash != NULL) {
            char repo[64], name[128];
            int repo_len = slash - line;
            
            strncpy(repo, line, repo_len);
            repo[repo_len] = '\0';
            
            strcpy(name, slash + 1);
            
            printf(COLOR_BOLD COLOR_BLUE "%s" COLOR_RESET, name);
            printf(" " COLOR_CYAN "(%s)" COLOR_RESET, repo);
        }
        else if (space != NULL) {
            char name[128], rest[1024];
            int name_len = space - line;
            
            strncpy(name, line, name_len);
            name[name_len] = '\0';
            
            strcpy(rest, space + 1);
            
            printf(COLOR_BOLD COLOR_BLUE "%s" COLOR_RESET, name);
            printf(" %s", rest);
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
        printf(COLOR_CYAN "Example: %s libreoffice\n" COLOR_RESET, argv[0]);
        return 1;
    }
    
    char query[256] = "";
    for (int i = 1; i < argc; i++) {
        strcat(query, argv[i]);
        if (i < argc - 1) strcat(query, " ");
    }
    
    search_packages(query);
    
    return 0;
}