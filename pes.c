#include "pes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"
#include "commit.h"
// ─── PROVIDED: Phase 5 Command Wrappers ─────────────────────────────────────

// Usage: 
//   pes branch          (lists branches)
//   pes branch <name>   (creates a branch)
//   pes branch -d <name>(deletes a branch)
void cmd_branch(int argc, char *argv[]) {
    if (argc == 2) {
        branch_list();
    } else if (argc == 3) {
        if (branch_create(argv[2]) == 0) {
            printf("Created branch '%s'\n", argv[2]);
        } else {
            fprintf(stderr, "error: failed to create branch '%s'\n", argv[2]);
        }
    } else if (argc == 4 && strcmp(argv[2], "-d") == 0) {
        if (branch_delete(argv[3]) == 0) {
            printf("Deleted branch '%s'\n", argv[3]);
        } else {
            fprintf(stderr, "error: failed to delete branch '%s'\n", argv[3]);
        }
    } else {
        fprintf(stderr, "Usage:\n  pes branch\n  pes branch <name>\n  pes branch -d <name>\n");
    }
}

// Usage: pes checkout <branch_or_commit>
void cmd_checkout(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: pes checkout <branch_or_commit>\n");
        return;
    }

    const char *target = argv[2];
    if (checkout(target) == 0) {
        printf("Switched to '%s'\n", target);
    } else {
        fprintf(stderr, "error: checkout failed. Do you have uncommitted changes?\n");
    }
}

// ─── PROVIDED: Command dispatch ─────────────────────────────────────────────

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: pes <command> [args]\n");
        fprintf(stderr, "\nCommands:\n");
        fprintf(stderr, "  init            Create a new PES repository\n");
        fprintf(stderr, "  add <file>...   Stage files for commit\n");
        fprintf(stderr, "  status          Show working directory status\n");
        fprintf(stderr, "  commit -m <msg> Create a commit from staged files\n");
        fprintf(stderr, "  log             Show commit history\n");
        fprintf(stderr, "  branch          List, create, or delete branches\n");
        fprintf(stderr, "  checkout <ref>  Switch branches or restore working tree\n");
        return 1;
    }

    const char *cmd = argv[1];

    if      (strcmp(cmd, "init") == 0)     cmd_init();
    else if (strcmp(cmd, "add") == 0)      cmd_add(argc, argv);
    else if (strcmp(cmd, "status") == 0)   cmd_status();
    else if (strcmp(cmd, "commit") == 0)   cmd_commit(argc, argv);
    else if (strcmp(cmd, "log") == 0)      cmd_log();
    else if (strcmp(cmd, "branch") == 0)   cmd_branch(argc, argv);
    else if (strcmp(cmd, "checkout") == 0) cmd_checkout(argc, argv);
    else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        fprintf(stderr, "Run 'pes' with no arguments for usage.\n");
        return 1;
    }

    return 0;
}
void cmd_init() {
    system("mkdir -p .pes/objects .pes/refs/heads");
    FILE *f = fopen(".pes/HEAD", "w");
    if (f) {
        fprintf(f, "ref: refs/heads/main\n");
        fclose(f);
    }
    printf("Initialized empty PES repository\n");
}

void cmd_add(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: pes add <file>\n");
        return;
    }

    Index index;
    index_load(&index);

    for (int i = 2; i < argc; i++)
        index_add(&index, argv[i]);
}

void cmd_status() {
    Index index;
    index_load(&index);
    index_status(&index);
}

void cmd_commit(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[2], "-m") != 0) {
        printf("Usage: pes commit -m \"message\"\n");
        return;
    }

    ObjectID id;
    if (commit_create(argv[3], &id) == 0)
        printf("Commit created successfully\n");
    else
        printf("Commit failed\n");
}

void show_commit(const ObjectID *id, const Commit *commit, void *ctx) {
    char hex[HASH_HEX_SIZE + 1];
    hash_to_hex(id, hex);

    printf("commit %s\n", hex);
    printf("Author: %s\n", commit->author);
    printf("Time: %lu\n", commit->timestamp);
    printf("Message: %s\n\n", commit->message);
}

void cmd_log() {
    commit_walk(show_commit, NULL);
}

void branch_list() { printf("branch list not implemented yet\n"); }
int branch_create(const char *name) { return 0; }
int branch_delete(const char *name) { return 0; }
int checkout(const char *target) { return 0; }
