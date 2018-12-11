#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

struct file_header{
    char name[100];
    char mode[8];
    char owner[8];
    char group[8];
    char size[12];
};

void add(FILE* output_file, const char* file){
	if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
		return;
    
    struct stat info;
    if(stat(file, &info) < 0)    
        return;

    struct file_header header;
    memset(&header, 0, sizeof(struct file_header));
    snprintf(header.name, 100, "%s", file);
    snprintf(header.mode, 8, "%06o", info.st_mode);
    snprintf(header.owner, 8, "%6o", getpwuid(info.st_uid) != 0 ? info.st_uid : 0);
    snprintf(header.group, 8, "%6o", getgrgid(info.st_gid) != 0 ? info.st_gid : 0);
    snprintf(header.size, 12, "%011ld", info.st_size);

    fwrite(&header, sizeof(struct file_header), 1, output_file);

    FILE* input = fopen(file, "rb");
    if(input == NULL){
        fprintf( stderr, "Failed to open %s for reading\n", file);
        return;
    }
    while(!feof(input)) {
        char buffer[2000];
        size_t read = fread(buffer, 1, 2000, input);
        fwrite(buffer, 1, read, output_file);
    }

    fclose(input);
}

char output_name[25];
int num_files;

void pack(char filenames[][256]) {
	FILE *fout = fopen(output_name, "wb");
	for (int i = 0; i < num_files; ++i) {
		add(fout, filenames[i]);
	}
	fclose(fout);
}

void unpack() {
	FILE *fin = fopen(output_name, "rb");
	while (!feof(fin)) {
		struct file_header header;
		memset(&header, 0, sizeof(struct file_header));

		fread(header.name, sizeof(header.name), 1, fin);
    	fread(header.mode, sizeof(header.mode), 1, fin);
    	fread(header.owner, sizeof(header.owner), 1, fin);
    	fread(header.group, sizeof(header.group), 1, fin);
    	fread(header.size, sizeof(header.size), 1, fin);

    	int size;
    	uid_t id;
    	gid_t gid;
    	sscanf(header.size, "%d", &size);
    	sscanf(header.owner, "%u", &id);
    	sscanf(header.group, "%u", &gid);
    	mode_t mode = strtol(header.mode, NULL, 8);
		
		char buffer[2000];
		size_t read = fread(buffer, 1, size, fin);
		if (read == size) {
			FILE *fout = fopen(header.name, "wb");
			fwrite(buffer, 1, read, fout);
			chmod(header.name, mode);
			chown(header.name, id, gid);
			fclose(fout);
		} else
			break;
	}
	fclose(fin);
}

void list() {
	FILE *fin = fopen(output_name, "r");
	while (!feof(fin)) {
		int name_size;
		int file_size;
		fscanf(fin, "%d", &name_size);
		char file_name[256];
		memset(file_name, 0, sizeof(file_name));
		size_t read = fread(file_name, sizeof(char), name_size, fin);
		if (read == name_size) {
			fscanf(fin, "%d", &file_size);
		
			printf("%s\n", file_name);
			char buf[file_size];
			memset(buf, 0, file_size);
			fread(buf, 1, file_size, fin);
		}
	}
	fclose(fin);
}

int main(int argc, char **argv) {
	if (argc == 1) {
		return 1;
	}

	else if (argc > 2) {
		char* arg = argv[1];
		if (arg[0] == '-') {
			switch (arg[1]) {
				case 'c':
					strcpy(output_name, argv[2]);
					num_files = argc - 3;
					if (num_files > 0) {
						char files[num_files][256];
						for (int j = 3; j-3 < num_files; j ++) {
							strcpy(files[j - 3], argv[j]);
						}
						pack(files);
					}
					break;
				case 'x':
					strcpy(output_name, argv[2]);
					unpack();
					break;
				case 'l':
					strcpy(output_name, argv[2]);
					list();
					break;
				default:
					printf("Unknown flag: %s\n", arg);
					return 1;
			}
		} else {
			printf("Unknown flag: %s\n", arg);
				return 1;
		}
	}
}